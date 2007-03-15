/*

    - aggiustare live telecamere
    - scrittura su disco dati in streaming, con uso delle timestamp

*/

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

// GTK+
#include <gtk/gtk.h>

// local
#include "../mirrorCollector/CollectorCommands.h"

using namespace std;
using namespace yarp::os;

// ----------------------------------------------
// properties of the application
// ----------------------------------------------

class _mirrorCaptureProperty : public Property {
public:
    _mirrorCaptureProperty() {
		fromString("\
           (appName mirrorCapture) (dataNetName default) (imgNetName Net1) \
           (useCamera0 0) (useCamera1 0) (useTracker0 0) (useTracker1 0) \
           (useGazeTracker 0) (useDataGlove 0) (usePresSens 0) \
           (seqPrefix seq) (seqPath c:\\\\) \
	       (refreshFreq 2.0) \
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

// data & images, and their semaphores
collectorNumericalData _data;
collectorImage _img0, _img1;
Semaphore _dataSem, _img0Sem, _img1Sem;
Stamp _dataStamp, _img0Stamp, _img1Stamp;

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

// read data from the ports and store them to the appropriate data structures

void readNumData()
{

    _dataSem.wait();
    _data = _dataPort.read()->content();
    _dataPort.getEnvelope(_dataStamp);
    _dataSem.post();

}

void readImg0()
{

    if ( _property.find("useCamera0").asInt() == 1 ) {
        _img0Sem.wait();
        _img0 = *_img0Port.read();
        _img0Port.getEnvelope(_img0Stamp);
        _img0Sem.post();
    }

}

void readImg1()
{

    if ( _property.find("useCamera1").asInt() == 1 ) {
        _img1Sem.wait();
        _img1 = *_img1Port.read();
        _img1Port.getEnvelope(_img1Stamp);
        _img1Sem.post();
    }

}

// convert an image to a GdkPixbuf

void img2buf(collectorImage* img, GdkPixbuf* buf)
{

    guchar* dst_data = gdk_pixbuf_get_pixels(buf);
    unsigned int rowstride = gdk_pixbuf_get_rowstride (buf);
    unsigned int n_channels = gdk_pixbuf_get_n_channels (buf);

	char* src_data = (char*) img->getRawImage();
    unsigned int width = img->width();
    unsigned int height = img->height();
	unsigned int src_line_size = img->getRowSize();

	unsigned int dst_size_in_memory = rowstride * height;

	guchar *p_dst;
	char *p_src;

    if ( src_line_size == rowstride ) {
        ACE_OS::memcpy(dst_data, src_data, dst_size_in_memory);
    } else {
        for (int i=0; i < (int)height; i++) {
            p_dst = dst_data + i * rowstride;
            p_src = src_data + i * src_line_size;
            ACE_OS::memcpy(p_dst, p_src, (n_channels*width));
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
            // read all data in a single strike
            readNumData();
            readImg0();
            readImg1();
        }
    }

} pollingThread;

// streaming threads
// the threads expect data to be available continually as a stream,
// update the data structures and save the data to the disc

class _numDataStreamingThread : public Thread {
public:

    _numDataStreamingThread() : _timeTop(0) {}

    void run () {
        g_print ("numerical data streaming thread starting....\n");
        while ( ! isStopping() ) {
            // read data
            readNumData();
            // update status bar: what is the current streaming frequency?
            _tick[ ((++_timeTop==10)?(_timeTop=0):_timeTop) ] = Time::now() - _prev;
            _prev = Time::now();
            g_print("streaming at %3.2f Hz\r", 1.0/((
                _tick[0]+_tick[1]+_tick[2]+_tick[3]+_tick[4]+
                _tick[5]+_tick[6]+_tick[7]+_tick[8]+_tick[9]
                )/10.0) );
            // save data to disc
            // ..................
		}
        g_print ("numerical data streaming thread stopping....\n");
    }

private:
    double _tick[10], _prev;
    int _timeTop;
} numDataStreamingThread;

class _img0StreamingThread : public Thread {
public:

    _img0StreamingThread() : _timeTop(0) {}

    void run () {
        g_print ("image 0 streaming thread starting....\n");
        while ( ! isStopping() ) {
            // read data
            readImg0();
            // update status bar: what is the current streaming frequency?
            _tick[ ((++_timeTop==10)?(_timeTop=0):_timeTop) ] = Time::now() - _prev;
            _prev = Time::now();
            g_print("streaming at %3.2f Hz\r", 1.0/((
                _tick[0]+_tick[1]+_tick[2]+_tick[3]+_tick[4]+
                _tick[5]+_tick[6]+_tick[7]+_tick[8]+_tick[9]
                )/10.0) );
            // save data to disc
            // ..................
		}
        g_print ("image 0 streaming thread stopping....\n");
    }

private:
    double _tick[10], _prev;
    int _timeTop;
} img0StreamingThread;

class _img1StreamingThread : public Thread {
public:

    _img1StreamingThread() : _timeTop(0) {}

    void run () {
        g_print ("image 1 streaming thread starting....\n");
        while ( ! isStopping() ) {
            // read data
            readImg1();
            // update status bar: what is the current streaming frequency?
            _tick[ ((++_timeTop==10)?(_timeTop=0):_timeTop) ] = Time::now() - _prev;
            _prev = Time::now();
            g_print("streaming at %3.2f Hz\r", 1.0/((
                _tick[0]+_tick[1]+_tick[2]+_tick[3]+_tick[4]+
                _tick[5]+_tick[6]+_tick[7]+_tick[8]+_tick[9]
                )/10.0) );
            // save data to disc
            // ..................
		}
        g_print ("image 1 streaming thread stopping....\n");
    }

private:
    double _tick[10], _prev;
    int _timeTop;
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
_data.pressureData.fakeDatum[2], 0, 0, 0,
0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
0, 0.0, 0.0,
0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0,
0, 0, 0,
0, 0, 0, 0, 0, 0, 0
/*		    _data.pressureData.channelA, _data.pressureData.channelB, _data.pressureData.channelC, _data.pressureData.channelD,
			_data.tracker0Data.x, _data.tracker0Data.y, _data.tracker0Data.z, _data.tracker0Data.azimuth,_data.tracker0Data.elevation, _data.tracker0Data.roll,
			_data.tracker1Data.x, _data.tracker1Data.y, _data.tracker1Data.z, _data.tracker1Data.azimuth,_data.tracker1Data.elevation, _data.tracker1Data.roll,
			_data.GTData.valid, _data.GTData.pupilX, _data.GTData.pupilY,
			_data.gloveData.thumb[0], _data.gloveData.thumb[1], _data.gloveData.thumb[2],
			_data.gloveData.index[0], _data.gloveData.index[1], _data.gloveData.index[2],
			_data.gloveData.middle[0], _data.gloveData.middle[1], _data.gloveData.middle[2],
			_data.gloveData.ring[0], _data.gloveData.ring[1], _data.gloveData.ring[2],
			_data.gloveData.pinkie[0], _data.gloveData.pinkie[1], _data.gloveData.pinkie[2],
			_data.gloveData.abduction[0], _data.gloveData.abduction[1], _data.gloveData.abduction[2], _data.gloveData.abduction[3],
			_data.gloveData.palmArch, _data.gloveData.wristPitch, _data.gloveData.wristYaw*/
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

    printf("data: %3.2f, img0: %3.2f, img1: %3.2f\r", _dataStamp.getTime(), _img0Stamp.getTime(), _img1Stamp.getTime());

    return (collector_awake?TRUE:FALSE);

}

void on_wakeUpButton_clicked (GtkButton* button, gpointer user_data)
{
    
    g_print ("wakeUpButton clicked\n");

    if ( ! collector_awake ) {
        // user wants to wake up collector!
        g_print ("user wants to wake up collector\n");
        // send "wakeup" command; expect "succeeded"
        g_print ("asking collector to wake up\n");
        Bottle recd;
        if ( sendCmd(CCmdWakeUp,&recd) == false ) {
            g_print ("wake up command failed\n");
            return;
        }
        g_print ("wake up command succeeded\n");
        // must get collector properties
        if ( recd.size() != 2 ) {
            g_print ("collector did not send any property. using defaults.\n");
        } else {
            // read properties of the hardware
            _mirrorCollectorProperty _mcollProperty;
            _mcollProperty.fromString(recd.get(1).asString().c_str());
            _property.put("useCamera0",_mcollProperty.find("useCamera0").asInt());
            _property.put("useCamera1",_mcollProperty.find("useCamera1").asInt());
            _property.put("useTracker0",_mcollProperty.find("useTracker0").asInt());
            _property.put("useTracker1",_mcollProperty.find("useTracker1").asInt());
            _property.put("useGazeTracker",_mcollProperty.find("useGazeTracker").asInt());
            _property.put("useDataGlove",_mcollProperty.find("useDataGlove").asInt());
            _property.put("usePresSens",_mcollProperty.find("usePresSens").asInt());
            _property.put("imgSizeX",_mcollProperty.find("imgSizeX").asInt());
            _property.put("imgSizeY",_mcollProperty.find("imgSizeY").asInt());
            g_print ("received collector properties:\n");
            cout << _property.toString().c_str() << endl;
        }
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
        // user wants to stream
        g_print ("user wants to stream\n");
        // stop polling thread
        g_print ("stopping polling thread\n");
        pollingThread.stop();
        // send startStreaming command; expect ok
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
        img0StreamingThread.start();
        img1StreamingThread.start();
        // deactivate buttons
        g_print ("deactivating wake up button\n");
        gtk_widget_set_sensitive( (GtkWidget*)wakeUpButton, FALSE);
        gtk_button_set_label((GtkButton*)button, "Stop");
        // set flag
        streaming = true;
    } else {
        g_print ("user wants to stop streaming\n");
        g_print ("stopping streaming thread\n");
        img1StreamingThread.stop();
        img0StreamingThread.stop();
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
    gtk_fixed_put (GTK_FIXED (fixed), numDataTextView, 192, 40);
    gtk_widget_set_size_request (numDataTextView, 232, 176);
    gtk_container_set_border_width (GTK_CONTAINER (numDataTextView), 1);
    gtk_text_view_set_editable (GTK_TEXT_VIEW (numDataTextView), FALSE);
    gtk_text_view_set_accepts_tab (GTK_TEXT_VIEW (numDataTextView), FALSE);
    gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (numDataTextView), FALSE);

    streamButton = gtk_button_new_with_mnemonic ("Stream");
    gtk_widget_show (streamButton);
    gtk_fixed_put (GTK_FIXED (fixed), streamButton, 312, 0);
    gtk_widget_set_size_request (streamButton, 112, 40);
    gtk_widget_set_sensitive (streamButton, FALSE);

    wakeUpButton = gtk_button_new_with_mnemonic ("Wake Up");
    gtk_widget_show (wakeUpButton);
    gtk_fixed_put (GTK_FIXED (fixed), wakeUpButton, 192, 0);
    gtk_widget_set_size_request (wakeUpButton, 112, 40);
    
	camera0Buf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 400, 400);
    camera0Image = gtk_image_new_from_pixbuf (camera0Buf);
    gtk_widget_show (camera0Image);
    gtk_fixed_put (GTK_FIXED (fixed), camera0Image, 0, 0);
    gtk_widget_set_size_request (camera0Image, 192, 240);

    statusBar = gtk_statusbar_new ();
    gtk_widget_show (statusBar);
    gtk_fixed_put (GTK_FIXED (fixed), statusBar, 192, 216);
    gtk_widget_set_size_request (statusBar, 232, 24);
    gtk_container_set_border_width (GTK_CONTAINER (statusBar), 1);
    gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR (statusBar), FALSE);

	camera1Buf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 400, 400);
    camera1Image = gtk_image_new_from_pixbuf (camera1Buf);
    gtk_widget_show (camera1Image);
    gtk_fixed_put (GTK_FIXED (fixed), camera1Image, 424, 0);
    gtk_widget_set_size_request (camera1Image, 192, 240);

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

    // possibly, stop the threads
    if ( numDataStreamingThread.isRunning() ) numDataStreamingThread.stop();
    if ( img0StreamingThread.isRunning() ) img0StreamingThread.stop();
    if ( img1StreamingThread.isRunning() ) img1StreamingThread.stop();
    if ( pollingThread.isRunning() ) pollingThread.stop();

    // bail out
    closePorts();
    Network::fini();

    return 0;

}
