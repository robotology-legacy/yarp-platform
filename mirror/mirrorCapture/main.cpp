// come scrivo un'immagine su file?

// investigare strani comportamenti di frequenze di streaming. i dati vanno piu`
// veloci se su macchine diverse, le immagini... il contrario!
// . come mandare le immagini di yarpdev su un altro network (i.e., 10.0.1.*) ?
// . mettere un ratethread nel picolodevicedriver?
// . provare fra atlas e macchina del setup babybot (cioe` su rete 43 e 10)

// ----------------------------------------------
// headers
// ----------------------------------------------

// general
#include <iostream>

// ACE
#include <ace/config.h>
#include <ace/OS.h>

// YARP
#include <yarp/os/Network.h> 
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/BinPortable.h>
#include <yarp/os/Value.h>
#include <yarp/os/Time.h>
#include <yarp/os/Property.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Stamp.h>
#include <yarp/sig/Image.h>

// GTK+
#include <gtk/gtk.h>

// local
#include "../mirrorCollector/CollectorCommands.h"

using namespace std;
using namespace yarp::os;

typedef yarp::sig::ImageOf<yarp::sig::PixelRgb> collectorImage;

// ----------------------------------------------
// properties of the application
// ----------------------------------------------

class _mirrorCaptureProperty : public Property {
public:
    _mirrorCaptureProperty() {
		fromString("\
           (appName mirrorCapture) (dataNetName default) (imgNetName Net1) \
           (useCamera0 0) (useCamera1 0) \
           (seqPrefix seq) (seqPath c:) (refreshFreq 2.0) \
           (propertyFileName c:\\\\work\\\\platform\\\\mirror\\\\mirrorCapture\\\\mirrorCapture.conf) \
        ");
	};
} _property;

// ----------------------------------------------
// globals
// ----------------------------------------------

// ports
Port _cmdPort;
BufferedPort<BinPortable<collectorNumericalData> > _dataPort;
BufferedPort<collectorImage> _img0Port;
BufferedPort<collectorImage> _img1Port;

// data & images, and their companions
collectorNumericalData _data;
collectorImage _img0, _img1;
Semaphore _dataSem, _img0Sem, _img1Sem;
Stamp _dataStamp, _img0Stamp, _img1Stamp;
double _dataFreq, _img0Freq, _img1Freq;
char statusBarMsg[50];

// is collector awake?
bool collector_awake = false;
// is collector in the streaming mode?
bool streaming = false;

// GTK interface objects
GtkWidget* numDataTextView;
GtkWidget* streamButton;
GtkWidget* camera0Image;
GdkPixbuf* camera0Buf;
GtkWidget* camera1Image;
GdkPixbuf* camera1Buf;
GtkWidget* wakeUpButton;
GtkWidget* statusBar;
GtkWidget* mainWindow;

// ----------------------------------------------
// helpers
// ----------------------------------------------

// send a command to the collector and expect OK; possibly, return to the
// caller the collector's answer.

bool sendCmd(collectorCommand cmd, Bottle* recd = 0)
{

    // if the second argument is omitted, the function will not care about it;
    // otherwise it will fill it with the reply from the collector

    Bottle sent, safeRecd;

    // if recd is null, ignore the response
    if ( recd == 0 ) {
        recd = &safeRecd;
    }

    // send command
    sent.addInt(cmd);
    _cmdPort.write(sent,*recd);

    if ( recd->get(0).asInt() == CCmdFailed ) {
        return false;
    }

    return true;

}

// read data from the ports and store them into the appropriate data structures

void readNumData()
{

    _dataSem.wait();
    _data = _dataPort.read()->content();
	_dataPort.getEnvelope(_dataStamp);
    _dataSem.post();

}

void readImg0()
{

    _img0Sem.wait();
    _img0 = *_img0Port.read();
    _img0Port.getEnvelope(_img0Stamp);
    _img0Sem.post();

}

void readImg1()
{

    _img1Sem.wait();
    _img1 = *_img1Port.read();
    _img1Port.getEnvelope(_img1Stamp);
    _img1Sem.post();

}

// convert an image to a GdkPixbuf

void img2buf(collectorImage* img, GdkPixbuf* buf)
{

	// every now and then we could get an empty image...
	if ( img->getRawImage()==0 ) return;

	// gather pixbuf buffer and dimensions
    guchar* dst_data = gdk_pixbuf_get_pixels(buf);
	unsigned int bufW = gdk_pixbuf_get_width(buf);
	unsigned int bufH = gdk_pixbuf_get_height(buf);

	// evaluate stretch factors (to adapt the image to the pixbufs)
    double xFact = (double)(img->width())/(double)bufW;
    double yFact = (double)(img->height())/(double)bufH;

	// how big is our pixbuf pixel?
    int pixSize = gdk_pixbuf_get_n_channels(buf)*gdk_pixbuf_get_bits_per_sample(buf)/8;

	// now copy the image to the pixbuf
    for (int i=0; i<bufH; ++i) {
        for (int j=0; j<bufW; ++j) {
			dst_data[i*bufW*pixSize+j*pixSize+2] = (*img)((int)(j*yFact),(int)(i*xFact)).r;
			dst_data[i*bufW*pixSize+j*pixSize+1] = (*img)((int)(j*yFact),(int)(i*xFact)).g;
			dst_data[i*bufW*pixSize+j*pixSize+0] = (*img)((int)(j*yFact),(int)(i*xFact)).b;
        }
    }

}

bool openPorts()
{

    char portName[256];

    ACE_OS::sprintf(portName, "/%s/data", _property.find("appName").asString().c_str());
    if ( _dataPort.open(portName) == false ) return false;

    ACE_OS::sprintf(portName, "/%s/img0", _property.find("appName").asString().c_str());
    if ( _img0Port.open(portName) == false ) return false;

    ACE_OS::sprintf(portName, "/%s/img1", _property.find("appName").asString().c_str());
    if ( _img1Port.open(portName) == false ) return false;

    ACE_OS::sprintf(portName, "/%s/cmd", _property.find("appName").asString().c_str());
    if ( _cmdPort.open(portName) == false ) return false;

    return true;

}

void closePorts()
{

    _cmdPort.close();
    _img1Port.close();
    _img0Port.close();
    _dataPort.close();

}

// ----------------------------------------------
// threads
// ----------------------------------------------

// data polling thread
// the thread polls the collector for new data (send getData / expect OK)
// and reads data from the ports, updating the related data structures.
// the polling frequency is specified in the conf file. notice that the
// same frequency is used to install the GTK interface update timer, but
// obviously there is no guarantee that the timeout and this thread be
// synchronised. this cannot be, as no thread other than the main is allowed
// to touch the GUI. and moreover, we don't really care about this synchronisation.

class _pollingThread : public RateThread {

public:

    _pollingThread() : RateThread(1000) {}

    bool threadInit() {
        setRate( (int) (1000/_property.find("refreshFreq").asDouble()) );
        g_print ("polling thread initialised at %3.2f Hz\n", _property.find("refreshFreq").asDouble());
        return true;
    }

    void run () {
        // poll collector
        if ( sendCmd(CCmdGetData) == false ) {
            g_print ("getData command failed\n");
        } else {
            // read data
            readNumData();
            if ( _property.find("useCamera0").asInt() == 1 ) readImg0();
            if ( _property.find("useCamera1").asInt() == 1 ) readImg1();
        }
    }

} pollingThread;

// streaming threads
// the threads expect data to be available continually as a stream; so they
// employ blocking read()s and their execution frequency is dictated by
// the stream itself, that is, by the collector application. they then
// update the data structures and save the data to the disc.
// we actually DO need three separate threads, since numerical data is
// basically independent of images grabbed off the cameras.

class _numDataStreamingThread : public Thread {
public:

    _numDataStreamingThread() :
	  _timeTop(0), _seqCount(1), _readFirstStamp(true) {}

    bool threadInit() {
        char outFileName[200];
        sprintf(outFileName, "%s\\%s.seq%02d.data.dat",
            _property.find("seqPath").asString().c_str(),
            _property.find("seqPrefix").asString().c_str(),
            _seqCount
            );
        _dataOutFile = fopen(outFileName,"w");
        if ( _dataOutFile == 0 ) {
            g_print ("could not open data out file for writing");
            return false;
        }
        return true;
    }

    void run () {
        g_print ("numerical data streaming thread starting....\n");
        while ( ! isStopping() ) {
            // read data
            readNumData();
			if ( _readFirstStamp ) {
				_firstStamp = _dataStamp.getTime();
				_readFirstStamp = false;
			}
            // update status bar: what is the actual streaming frequency?
            _tick[ ((++_timeTop==10)?(_timeTop=0):_timeTop) ] = Time::now() - _prev;
            _prev = Time::now();
            _dataFreq = 1.0/((
                _tick[0]+_tick[1]+_tick[2]+_tick[3]+_tick[4]+
                _tick[5]+_tick[6]+_tick[7]+_tick[8]+_tick[9]
                )/10.0);
            // save data to disc
            fprintf(_dataOutFile, "%3.3g \
%g %g %g %g %g %g \
%g %g %g %g %g %g \
%1d %g %g \
%3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d \
%5d %5d %5d %5d\n",
		        _dataStamp.getTime()-_firstStamp,
			    _data.tracker0Data.x, _data.tracker0Data.y, _data.tracker0Data.z, _data.tracker0Data.azimuth,_data.tracker0Data.elevation, _data.tracker0Data.roll,
			    _data.tracker1Data.x, _data.tracker1Data.y, _data.tracker1Data.z, _data.tracker1Data.azimuth,_data.tracker1Data.elevation, _data.tracker1Data.roll,
			    _data.GTData.valid, _data.GTData.pupilX, _data.GTData.pupilY,
			    _data.gloveData.thumb[0], _data.gloveData.thumb[1], _data.gloveData.thumb[2],
			    _data.gloveData.index[0], _data.gloveData.index[1], _data.gloveData.index[2],
			    _data.gloveData.middle[0], _data.gloveData.middle[1], _data.gloveData.middle[2],
			    _data.gloveData.ring[0], _data.gloveData.ring[1], _data.gloveData.ring[2],
			    _data.gloveData.pinkie[0], _data.gloveData.pinkie[1], _data.gloveData.pinkie[2],
			    _data.gloveData.abduction[0], _data.gloveData.abduction[1], _data.gloveData.abduction[2], _data.gloveData.abduction[3],
			    _data.gloveData.palmArch, _data.gloveData.wristPitch, _data.gloveData.wristYaw,
                _data.pressureData.channelA, _data.pressureData.channelB, _data.pressureData.channelC, _data.pressureData.channelD
			);
		}
        g_print ("numerical data streaming thread stopping....\n");
    }

    void threadRelease() {
        fclose(_dataOutFile);
		_readFirstStamp = true;
		_seqCount++;
    }

private:
    FILE* _dataOutFile;
    int _seqCount;
    double _tick[10], _prev;
    int _timeTop;
	double _firstStamp;
	bool _readFirstStamp;
} numDataStreamingThread;

class _img0StreamingThread : public Thread {
public:

    _img0StreamingThread() : _timeTop(0), _seqCount(1), _readFirstStamp(true) {}

    bool threadInit() {
        char outFileName[200];
        sprintf(outFileName, "%s\\%s.seq%02d.img0.dat",
            _property.find("seqPath").asString().c_str(),
            _property.find("seqPrefix").asString().c_str(),
            _seqCount
            );
        _dataOutFile = fopen(outFileName,"w");
        if ( _dataOutFile == 0 ) {
            g_print ("could not open image 0 table out file for writing");
            return false;
        }
        return true;
    }

    void run () {
        g_print ("image 0 streaming thread starting....\n");
		unsigned int imgCount = 1;
        while ( ! isStopping() ) {
            // read data
            readImg0();
			if ( _readFirstStamp ) {
				_firstStamp = _img0Stamp.getTime();
				_readFirstStamp = false;
			}
            // update status bar: what is the current streaming frequency?
            _tick[ ((++_timeTop==10)?(_timeTop=0):_timeTop) ] = Time::now() - _prev;
            _prev = Time::now();
            _img0Freq = 1.0/((
                _tick[0]+_tick[1]+_tick[2]+_tick[3]+_tick[4]+
                _tick[5]+_tick[6]+_tick[7]+_tick[8]+_tick[9]
                )/10.0);
            // save data to disc
            char outFileName[200];
            sprintf(outFileName, "%s\\%s.seq%02d.%04d.img0.png",
                _property.find("seqPath").asString().c_str(),
                _property.find("seqPrefix").asString().c_str(),
                _seqCount, imgCount++
                );
//            _img0.save(outFileName);
            fprintf(_dataOutFile, "%3.3g %s\n", _img0Stamp.getTime()-_firstStamp, outFileName);
		}
        g_print ("image 0 streaming thread stopping....\n");
    }

    void threadRelease() {
        fclose(_dataOutFile);
		_readFirstStamp = true;
		_seqCount++;
    }

private:
    FILE* _dataOutFile;
    int _seqCount;
    double _tick[10], _prev;
    int _timeTop;
	double _firstStamp;
	bool _readFirstStamp;
} img0StreamingThread;

class _img1StreamingThread : public Thread {
public:

    _img1StreamingThread() : _timeTop(0), _seqCount(1), _readFirstStamp(true) {}

    bool threadInit() {
        char outFileName[200];
        sprintf(outFileName, "%s\\%s.seq%02d.img1.dat",
            _property.find("seqPath").asString().c_str(),
            _property.find("seqPrefix").asString().c_str(),
            _seqCount
            );
        _dataOutFile = fopen(outFileName,"w");
        if ( _dataOutFile == 0 ) {
            g_print ("could not open image 1 table out file for writing");
            return false;
        }
        return true;
    }

    void run () {
        g_print ("image 1 streaming thread starting....\n");
		unsigned int imgCount = 1;
        while ( ! isStopping() ) {
            // read data
            readImg1();
			if ( _readFirstStamp ) {
				_firstStamp = _img1Stamp.getTime();
				_readFirstStamp = false;
			}
            // update status bar: what is the current streaming frequency?
            _tick[ ((++_timeTop==10)?(_timeTop=0):_timeTop) ] = Time::now() - _prev;
            _prev = Time::now();
            _img1Freq = 1.0/((
                _tick[0]+_tick[1]+_tick[2]+_tick[3]+_tick[4]+
                _tick[5]+_tick[6]+_tick[7]+_tick[8]+_tick[9]
                )/10.0);
            // save data to disc
            char outFileName[200];
            sprintf(outFileName, "%s\\%s.seq%02d.%04d.img1.png",
                _property.find("seqPath").asString().c_str(),
                _property.find("seqPrefix").asString().c_str(),
                _seqCount, imgCount++
                );
//            _img1.save(outFileName);
            fprintf(_dataOutFile, "%3.3g %s\n", _img1Stamp.getTime()-_firstStamp, outFileName);
		}
        g_print ("image 1 streaming thread stopping....\n");
    }

    void threadRelease() {
        fclose(_dataOutFile);
		_readFirstStamp = true;
		_seqCount++;
    }

private:
    FILE* _dataOutFile;
    int _seqCount;
    double _tick[10], _prev;
    int _timeTop;
	double _firstStamp;
	bool _readFirstStamp;
} img1StreamingThread;

// -----------------------------
// GTK interface
// -----------------------------

// --------------------------- callbacks

// timeout callback: every now and then (refreshFreq), update the GTK+ display

gboolean refreshData (void*)
{

    static char _displayed[1000];

    // update textView using numerical data
    _dataSem.wait();
    sprintf(_displayed, "\
Pressure sensors %5d %5d %5d %5d\n\
Tracker0 %3.2f %3.2f %3.2f %3.2f %3.2f %3.2f\n\
Tracker1 %3.2f %3.2f %3.2f %3.2f %3.2f %3.2f\n\
Gaze %1d %3.2f %3.2f\n\
Glove:\n\
%3d %3d %3d  %3d %3d %3d\n\
%3d %3d %3d  %3d %3d %3d\n\
%3d %3d %3d\n\
%3d %3d %3d %3d - %3d %3d %3d",
		    _data.pressureData.channelA, _data.pressureData.channelB, _data.pressureData.channelC, _data.pressureData.channelD,
			_data.tracker0Data.x, _data.tracker0Data.y, _data.tracker0Data.z, _data.tracker0Data.azimuth,_data.tracker0Data.elevation, _data.tracker0Data.roll,
			_data.tracker1Data.x, _data.tracker1Data.y, _data.tracker1Data.z, _data.tracker1Data.azimuth,_data.tracker1Data.elevation, _data.tracker1Data.roll,
			_data.GTData.valid, _data.GTData.pupilX, _data.GTData.pupilY,
			_data.gloveData.thumb[0], _data.gloveData.thumb[1], _data.gloveData.thumb[2],
			_data.gloveData.index[0], _data.gloveData.index[1], _data.gloveData.index[2],
			_data.gloveData.middle[0], _data.gloveData.middle[1], _data.gloveData.middle[2],
			_data.gloveData.ring[0], _data.gloveData.ring[1], _data.gloveData.ring[2],
			_data.gloveData.pinkie[0], _data.gloveData.pinkie[1], _data.gloveData.pinkie[2],
			_data.gloveData.abduction[0], _data.gloveData.abduction[1], _data.gloveData.abduction[2], _data.gloveData.abduction[3],
			_data.gloveData.palmArch, _data.gloveData.wristPitch, _data.gloveData.wristYaw
			);
    _dataSem.post();
    GtkTextBuffer* buf = gtk_text_view_get_buffer((GtkTextView*)numDataTextView);
    gtk_text_buffer_set_text(buf, _displayed, -1);
    gtk_widget_queue_draw(numDataTextView);

    // update camera windows
    _img0Sem.wait();
    img2buf(&_img0,camera0Buf);
    _img0Sem.post();
    gtk_widget_queue_draw (camera0Image);
    _img1Sem.wait();
    img2buf(&_img1,camera1Buf);
    _img1Sem.post();
    gtk_widget_queue_draw (camera1Image);

    // update status bar
    sprintf(statusBarMsg, "data %3.2f, cam0 %3.2f, cam1 %3.2f", _dataFreq, _img0Freq, _img1Freq);
    gtk_statusbar_push ((GtkStatusbar*)statusBar,
                        gtk_statusbar_get_context_id((GtkStatusbar*)statusBar,"msg"),
                        statusBarMsg);
    gtk_widget_queue_draw (statusBar);

    return (collector_awake?TRUE:FALSE);

}

void on_wakeUpButton_clicked (GtkButton* button, gpointer user_data)
{
    
    g_print ("wakeUpButton clicked\n");

    if ( ! collector_awake ) {
        // send "wakeup" command; expect "succeeded"
        g_print ("asking collector to wake up\n");
        if ( sendCmd(CCmdWakeUp) == false ) {
            g_print ("wake up command failed\n");
            return;
        }
        g_print ("wake up command succeeded\n");
        // set flag
        collector_awake = true;
        // activate buttons
        g_print ("activating stream button\n");
        gtk_widget_set_sensitive( (GtkWidget*)streamButton, TRUE);
        // change label
        gtk_button_set_label((GtkButton*)wakeUpButton, "Shut down");
        // install timeout for data refreshing
        g_timeout_add (1000/_property.find("refreshFreq").asDouble(),refreshData,0);
        // start polling thread
        g_print ("starting polling thread\n");
        pollingThread.start();
    } else {
        g_print ("stopping polling thread\n");
        pollingThread.stop();
        g_print ("user wants to shut down collector\n");
        g_print ("asking collector to shut down\n");
        if ( sendCmd(CCmdShutDown) == false ) {
            g_print ("shut down command failed\n");
            return;
        }
        g_print ("shut down command succeeded\n");
        collector_awake = false;
        g_print ("deactivating stream button\n");
        gtk_widget_set_sensitive( (GtkWidget*)streamButton, FALSE);
        gtk_button_set_label((GtkButton*)wakeUpButton, "Wake up");
    }

}

void on_streamButton_clicked (GtkButton* button, gpointer user_data)
{

    g_print ("streamButton clicked\n");

    if ( ! streaming ) {
        // stop polling thread
        g_print ("stopping polling thread\n");
        pollingThread.stop();
        // send startStreaming command
        g_print ("asking collector to start streaming\n");
        if ( sendCmd(CCmdStartStreaming) == false ) {
            g_print ("startStreaming command failed\n");
            g_print ("restarting polling thread\n");
            pollingThread.start();
            return;
        }
        g_print ("startStreaming command succeeded\n");
        // start streaming thread
        g_print ("starting streaming threads\n");
        numDataStreamingThread.start();
        if ( _property.find("useCamera0").asInt() == 1 ) img0StreamingThread.start();
        if ( _property.find("useCamera1").asInt() == 1 ) img1StreamingThread.start();
        // deactivate buttons
        g_print ("deactivating wake up button\n");
        gtk_widget_set_sensitive( (GtkWidget*)wakeUpButton, FALSE);
        gtk_button_set_label((GtkButton*)button, "Stop");
        // set flag
        streaming = true;
    } else {
        g_print ("user wants to stop streaming\n");
        g_print ("stopping streaming threads\n");
        if ( _property.find("useCamera1").asInt() == 1 ) img1StreamingThread.stop();
        if ( _property.find("useCamera0").asInt() == 1 ) img0StreamingThread.stop();
        numDataStreamingThread.stop();
        g_print ("asking collector to stop streaming\n");
        if ( sendCmd(CCmdStopStreaming) == false ) {
            g_print ("stopStreaming command failed\n");
            return;
        }
        g_print ("stream successfully stopped\n");
        g_print ("starting polling thread\n");
        pollingThread.start();
        streaming = false;
        g_print ("re-activating buttons\n");
        gtk_widget_set_sensitive( (GtkWidget*)wakeUpButton, TRUE);
        gtk_button_set_label((GtkButton*)button, "Stream");
    }

}

// --------------------------- interface

void create_interface (void)
{

    // this is, more or less, GLADE-generated code

    GtkWidget *fixed;

    mainWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (mainWindow), "Mirror Capture");
    gtk_window_set_resizable (GTK_WINDOW (mainWindow), FALSE);
    gtk_window_set_icon_name (GTK_WINDOW (mainWindow), "gtk-connect");

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (mainWindow), fixed);

    numDataTextView = gtk_text_view_new ();
    gtk_widget_show (numDataTextView);
    gtk_fixed_put (GTK_FIXED (fixed), numDataTextView, 0, 40);
    gtk_widget_set_size_request (numDataTextView, 232, 208);
    gtk_container_set_border_width (GTK_CONTAINER (numDataTextView), 1);
    gtk_text_view_set_editable (GTK_TEXT_VIEW (numDataTextView), FALSE);
    gtk_text_view_set_accepts_tab (GTK_TEXT_VIEW (numDataTextView), FALSE);
    gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (numDataTextView), FALSE);

    streamButton = gtk_button_new_with_mnemonic ("Stream");
    gtk_widget_show (streamButton);
    gtk_fixed_put (GTK_FIXED (fixed), streamButton, 120, 0);
    gtk_widget_set_size_request (streamButton, 112, 40);
    gtk_widget_set_sensitive (streamButton, FALSE);

    wakeUpButton = gtk_button_new_with_mnemonic ("Wake Up");
    gtk_widget_show (wakeUpButton);
    gtk_fixed_put (GTK_FIXED (fixed), wakeUpButton, 0, 0);
    gtk_widget_set_size_request (wakeUpButton, 112, 40);
    
	camera0Buf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 192, 136);
    camera0Image = gtk_image_new_from_pixbuf (camera0Buf);
    gtk_widget_show (camera0Image);
    gtk_fixed_put (GTK_FIXED (fixed), camera0Image, 232, 0);
    gtk_widget_set_size_request (camera0Image, 192, 136);
    gtk_widget_set_sensitive (camera0Image, TRUE);

	camera1Buf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 192, 136);
    camera1Image = gtk_image_new_from_pixbuf (camera1Buf);
    gtk_widget_show (camera1Image);
    gtk_fixed_put (GTK_FIXED (fixed), camera1Image, 232, 136);
    gtk_widget_set_size_request (camera1Image, 192, 136);
    gtk_widget_set_sensitive (camera1Image, TRUE);

    statusBar = gtk_statusbar_new ();
    gtk_widget_show (statusBar);
    gtk_fixed_put (GTK_FIXED (fixed), statusBar, 0, 248);
    gtk_widget_set_size_request (statusBar, 232, 24);
    gtk_container_set_border_width (GTK_CONTAINER (statusBar), 1);
    gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR (statusBar), FALSE);

    g_signal_connect ((gpointer) wakeUpButton, "clicked", G_CALLBACK (on_wakeUpButton_clicked), NULL);
    g_signal_connect ((gpointer) streamButton, "clicked", G_CALLBACK (on_streamButton_clicked), NULL);

    gtk_widget_show (mainWindow);
    g_signal_connect ((gpointer) mainWindow, "destroy", G_CALLBACK(gtk_main_quit), NULL);

}

// ----------------------------------------------
// main
// ----------------------------------------------

int main( int argc, char *argv[] )
{

    // initialise network
    Network::init();

    // read config file (override defaults, possibly)
    _property.fromConfigFile(_property.find("propertyFileName").asString().c_str(),false);

    // open ports
    if ( openPorts() == false ) {
        return 1;
    }

    // initialise GTK & create interface
    gtk_init (&argc, &argv);
    create_interface ();

    // idle and wait for events
    gtk_main ();

    // possibly, stop the threads if they are hanging (i.e., the user pressed the
    // KILL button without stopping streaming or shutting down
    if ( numDataStreamingThread.isRunning() ) numDataStreamingThread.stop();
    if ( img0StreamingThread.isRunning() ) img0StreamingThread.stop();
    if ( img1StreamingThread.isRunning() ) img1StreamingThread.stop();
    if ( pollingThread.isRunning() ) pollingThread.stop();

    // bail out
    closePorts();
    Network::fini();

    return 0;

}
