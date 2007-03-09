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
           (imgSizeX 0) (imgSizeY 0) \
           (seqPrefix seq) (seqPath c:\\\\) \
	       (refreshFreq 40.0) \
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
// and associated semaphores
Semaphore cmdPortSema, dataPortSema, img0PortSema, img1PortSema;

// data & images
collectorNumericalData  _data;
collectorImage          _img0;
collectorImage          _img1;

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
GtkWidget* mainWindow;

// ----------------------------------------------
// helpers
// ----------------------------------------------

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

bool sendCmd(collectorCommand cmd, Bottle* recd = 0)
{

    // if the second argument is omitted, the function will not care about it;
    // otherwise it will fill it with the reply from the remote application

    Bottle sent, safeRecd;

    // if recd is null, fallback (the response is then ignored)
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

// ----------------------------------------------
// data streaming thread
// ----------------------------------------------

// expect data to be available continually as a stream and save it timely to the disc

class _acquireThread : public Thread {

    void run () {

        g_print ("acquisition thread starting....\n");

        while ( ! isStopping() ) {
            _now = Time::now();
            // read data (blocking read)
            _threadData = _dataPort.read()->content();
            // update status bar: what is the current streaming frequency?
            g_print("streaming at %3.2f Hz\r", 1/(_now-_prev));
            _prev = _now;
            // save data to disc
            g_print ("\
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
		}

        g_print ("acquisition thread stopping....\n");

    }

private:
    // data coming from the collector
    collectorNumericalData  _threadData;
    collectorImage          _threadImg0;
    collectorImage          _threadImg1;

    char _displayed[1000];
    
    double _now, _prev;

} acquireThread;

// -----------------------------
// GTK interface
// -----------------------------

// --------------------------- callbacks

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

gboolean refresh (void*)
{

    static char _displayed[1000];

    // send getData command
    if ( sendCmd(CCmdGetData) == false ) {
        g_print ("getData command failed\n");
        return (collector_awake?TRUE:FALSE);
    }

    // read numerical data and update textView
    _data = _dataPort.read()->content();
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
    GtkTextBuffer* buf = gtk_text_view_get_buffer((GtkTextView*)numDataTextView);
    gtk_text_buffer_set_text(buf, _displayed, -1);
    gtk_widget_queue_draw(numDataTextView);

    // read image 0 and update window
    if ( _property.find("useCamera0").asInt() == 1 ) {
        collectorImage *img0 = _img0Port.read();
	    if ( img0 != 0 ) {
            img2buf(img0,camera0Buf);
            gtk_widget_queue_draw (camera0Image);
        }
    }
    
    // read image 1 and update window
    if ( _property.find("useCamera1").asInt() == 1 ) {
        collectorImage *img1 = _img1Port.read();
	    if ( img1 != 0 ) {
            img2buf(img1,camera1Buf);
            gtk_widget_queue_draw (camera1Image);
        }
    }
    
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
        g_print ("activating buttons\n");
        gtk_widget_set_sensitive( (GtkWidget*)streamButton, TRUE);
        // change label
        gtk_button_set_label((GtkButton*)wakeUpButton, "Shut down");
        // install timeout for live data refreshing
        g_timeout_add (1000/_property.find("refreshFreq").asDouble(),refresh,0);
    } else {
        g_print ("user wants to shut down collector\n");
        g_print ("stopping pull thread\n");
        g_print ("asking collector to shut down\n");
        if ( sendCmd(CCmdShutDown) == false ) {
            g_print ("shut down command failed\n");
            return;
        }
        g_print ("shut down command succeeded\n");
        collector_awake = false;
        g_print ("deactivating buttons\n");
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
        // send startStreaming command; expect ok
        g_print ("asking collector to start streaming\n");
        if ( sendCmd(CCmdStartStreaming) == false ) {
            g_print ("startStreaming command failed\n");
            return;
        }
        g_print ("startStreaming command succeeded\n");
        // start push thread
        g_print ("starting push thread\n");
        acquireThread.start();
        // deactivate buttons
        g_print ("deactivating buttons\n");
        gtk_widget_set_sensitive( (GtkWidget*)wakeUpButton, FALSE);
        gtk_button_set_label((GtkButton*)button, "Stop");
        // set flag
        streaming = true;
    } else {
        g_print ("user wants to stop streaming\n");
        g_print ("stopping push thread\n");
        acquireThread.stop();
        g_print ("asking collector to stop streaming\n");
        if ( sendCmd(CCmdStopStreaming) == false ) {
            g_print ("stopStreaming command failed\n");
            return;
        }
        g_print ("stream successfully stopped\n");
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
    gtk_window_set_icon_name (GTK_WINDOW (mainWindow), "gtk-connect");

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (mainWindow), fixed);

    wakeUpButton = gtk_button_new_with_mnemonic ("Wake Up");
    gtk_widget_show (wakeUpButton);
    gtk_fixed_put (GTK_FIXED (fixed), wakeUpButton, 192, 0);
    gtk_widget_set_size_request (wakeUpButton, 112, 40);
    
    streamButton = gtk_button_new_with_mnemonic ("Stream");
    gtk_widget_show (streamButton);
    gtk_fixed_put (GTK_FIXED (fixed), streamButton, 312, 0);
    gtk_widget_set_size_request (streamButton, 112, 40);

    numDataTextView = gtk_text_view_new ();
    gtk_widget_show (numDataTextView);
    gtk_fixed_put (GTK_FIXED (fixed), numDataTextView, 192, 40);
    gtk_widget_set_size_request (numDataTextView, 232, 176);

	camera0Buf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 400, 400);
    camera0Image = gtk_image_new_from_pixbuf (camera0Buf);
    gtk_widget_show (camera0Image);
    gtk_fixed_put (GTK_FIXED (fixed), camera0Image, 0, 0);
    gtk_widget_set_size_request (camera0Image, 192, 216);

	camera1Buf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 400, 400);
    camera1Image = gtk_image_new_from_pixbuf (camera1Buf);
    gtk_widget_show (camera1Image);
    gtk_fixed_put (GTK_FIXED (fixed), camera1Image, 424, 0);
    gtk_widget_set_size_request (camera1Image, 192, 216);

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

    // bail out
    closePorts();
    Network::fini();

    return 0;

}
