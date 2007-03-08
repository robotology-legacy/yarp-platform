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
GtkWidget *liveData;
GtkWidget *streamButton;
GtkWidget *camera0Button;
GtkWidget *camera1Button;
GtkWidget *wakeUpButton;
GtkWidget *mainWindow;

// ----------------------------------------------
// helpers
// ----------------------------------------------

bool sendCmd(collectorCommand cmd, Bottle* recd = 0)
{

    // if the second argument is omitted, the function will not care about it;
    // otherwise it will fill it with the reply from the remote application

    Bottle sent, safeRecd;

    if ( recd == 0 ) {
        recd = &safeRecd;
    }

    sent.addInt(cmd);

    cmdPortSema.wait();
    _cmdPort.write(sent,*recd);
    cmdPortSema.post();

    if ( recd->get(0).asInt() == CCmdFailed ) {
        return false;
    }

    return true;

}

void readData()
{

    // safely read data off the data and images ports
    dataPortSema.wait();
    dataPortSema.post();

}

// ----------------------------------------------
// data streaming thread
// ----------------------------------------------

// expect data to be available continually as a stream and save it timely to the disc

class _acquireThread : public Thread {

    void run () {

        g_print ("acquisition thread stopping....\n");

        while ( ! isStopping() ) {
            _now = Time::now();
            // read data (blocking read)
            _threadData = _dataPort.read()->content();
            // update status bar: what is the current streaming frequency?
            g_print("streaming at %3.2f Hz\r", 1/(_now-_prev));
            _prev = _now;
            // save data to disc
            g_print ("Tr#0: %.2d %.2d %.2d %.2d %.2d %.2d\r",
                _threadData.tracker0Data.fakeDatum[0], _threadData.tracker0Data.fakeDatum[1],
                _threadData.tracker0Data.fakeDatum[2], _threadData.tracker0Data.fakeDatum[3],
                _threadData.tracker0Data.fakeDatum[4], _threadData.tracker0Data.fakeDatum[5]);
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

gboolean refreshLiveData (void*)
{

    static char _displayed[1000];

    // send getData command
    if ( sendCmd(CCmdGetData) == false ) {
        g_print ("getData command failed\n");
        return (collector_awake?TRUE:FALSE);
    }

    // read, pack and display data
    _data = _dataPort.read()->content();
    sprintf(_displayed, "Tracker #0: %3.2d %3.2d %3.2d %3.2d %3.2d %3.2d\nTracker #1: %3.2d %3.2d %3.2d %3.2d %3.2d %3.2d\n",
        _data.tracker0Data.fakeDatum[0], _data.tracker0Data.fakeDatum[1], _data.tracker0Data.fakeDatum[2], _data.tracker0Data.fakeDatum[3], _data.tracker0Data.fakeDatum[4], _data.tracker0Data.fakeDatum[5],
        _data.tracker1Data.fakeDatum[0], _data.tracker1Data.fakeDatum[1], _data.tracker1Data.fakeDatum[2], _data.tracker1Data.fakeDatum[3], _data.tracker1Data.fakeDatum[4], _data.tracker1Data.fakeDatum[5]);
    GtkTextBuffer* buf = gtk_text_view_get_buffer((GtkTextView*)liveData);
    gtk_text_buffer_set_text(buf, _displayed, -1);
    gtk_widget_queue_draw(liveData);

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
        gtk_widget_set_sensitive( (GtkWidget*)camera0Button, TRUE);
        gtk_widget_set_sensitive( (GtkWidget*)camera1Button, TRUE);
        // change label
        gtk_button_set_label((GtkButton*)wakeUpButton, "Shut down");
        // install timeout for live data refreshing
        g_timeout_add (1000/_property.find("refreshFreq").asDouble(),refreshLiveData,0);
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
        gtk_widget_set_sensitive( (GtkWidget*)camera0Button, FALSE);
        gtk_widget_set_sensitive( (GtkWidget*)camera1Button, FALSE);
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
        gtk_widget_set_sensitive( (GtkWidget*)camera0Button, FALSE);
        gtk_widget_set_sensitive( (GtkWidget*)camera1Button, FALSE);
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
        gtk_widget_set_sensitive( (GtkWidget*)camera0Button, TRUE);
        gtk_widget_set_sensitive( (GtkWidget*)camera1Button, TRUE);
        gtk_button_set_label((GtkButton*)button, "Stream");
    }

}

void on_camera0Button_clicked (GtkButton* button, gpointer user_data)
{

}

void on_camera1Button_clicked (GtkButton* button, gpointer user_data)
{

}

// --------------------------- interface

void create_interface (void)
{

    GtkWidget *myFixed;
    GtkWidget *frame1;
    GtkWidget *alignment1;
    GtkWidget *label1;
    GtkTooltips *tooltips;
  
    tooltips = gtk_tooltips_new ();

    mainWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (mainWindow), "Mirror Capture");
    gtk_window_set_icon_name (GTK_WINDOW (mainWindow), "gtk-connect");

    myFixed = gtk_fixed_new ();
    gtk_widget_show (myFixed);
    gtk_container_add (GTK_CONTAINER (mainWindow), myFixed);

    frame1 = gtk_frame_new (NULL);
    gtk_widget_show (frame1);
    gtk_fixed_put (GTK_FIXED (myFixed), frame1, 0, 44);
    gtk_widget_set_size_request (frame1, 400, 225);
    gtk_frame_set_shadow_type (GTK_FRAME (frame1), GTK_SHADOW_NONE);

    alignment1 = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_show (alignment1);
    gtk_container_add (GTK_CONTAINER (frame1), alignment1);
    gtk_alignment_set_padding (GTK_ALIGNMENT (alignment1), 0, 0, 12, 0);

    liveData = gtk_text_view_new ();
    gtk_widget_show (liveData);
    gtk_container_add (GTK_CONTAINER (alignment1), liveData);
    GTK_WIDGET_UNSET_FLAGS (liveData, GTK_CAN_FOCUS);

    label1 = gtk_label_new ("<b>Numerical data:</b>");
    gtk_widget_show (label1);
    gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);
    gtk_label_set_use_markup (GTK_LABEL (label1), TRUE);

    streamButton = gtk_button_new_with_mnemonic ("Stream");
    gtk_widget_show (streamButton);
    gtk_fixed_put (GTK_FIXED (myFixed), streamButton, 104, 4);
    gtk_widget_set_size_request (streamButton, 96, 40);
    gtk_widget_set_sensitive (streamButton, FALSE);
    gtk_tooltips_set_tip (tooltips, streamButton, "Start streaming data to disc", NULL);

    camera0Button = gtk_button_new_with_mnemonic ("Camera #0");
    gtk_widget_show (camera0Button);
    gtk_fixed_put (GTK_FIXED (myFixed), camera0Button, 204, 4);
    gtk_widget_set_size_request (camera0Button, 96, 40);
    gtk_widget_set_sensitive (camera0Button, FALSE);
    gtk_tooltips_set_tip (tooltips, camera0Button, "Show camera #0 live", NULL);

    camera1Button = gtk_button_new_with_mnemonic ("Camera #1");
    gtk_widget_show (camera1Button);
    gtk_fixed_put (GTK_FIXED (myFixed), camera1Button, 304, 4);
    gtk_widget_set_size_request (camera1Button, 96, 40);
    gtk_widget_set_sensitive (camera1Button, FALSE);
    gtk_tooltips_set_tip (tooltips, camera1Button, "Show camera #1 live", NULL);

    wakeUpButton = gtk_button_new_with_mnemonic ("Wake up");
    gtk_widget_show (wakeUpButton);
    gtk_fixed_put (GTK_FIXED (myFixed), wakeUpButton, 4, 4);
    gtk_widget_set_size_request (wakeUpButton, 96, 40);
    gtk_tooltips_set_tip (tooltips, wakeUpButton, "Wake up the Mirror Collecor", NULL);

    g_signal_connect ((gpointer) wakeUpButton, "clicked", G_CALLBACK (on_wakeUpButton_clicked), NULL);
    g_signal_connect ((gpointer) streamButton, "clicked", G_CALLBACK (on_streamButton_clicked), NULL);
    g_signal_connect ((gpointer) camera0Button, "clicked", G_CALLBACK (on_camera0Button_clicked), NULL);
    g_signal_connect ((gpointer) camera1Button, "clicked", G_CALLBACK (on_camera1Button_clicked), NULL);

    gtk_widget_show (mainWindow);
    g_signal_connect ((gpointer) mainWindow, "destroy", G_CALLBACK(gtk_main_quit), NULL);

}

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
