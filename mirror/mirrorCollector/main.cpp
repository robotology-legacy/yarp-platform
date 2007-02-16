// ---------- headers

#include <iostream>
#include <conio.h>

#include <yarp/String.h>
#include <yarp/ThreadImpl.h>

#include <yarp/os/BinPortable.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/RateThread.h>

#include "CollectorCommands.h"

// ----------------- globals

using namespace std;
using namespace yarp::dev;

// what hardware is possibly attached to this setup?
typedef struct {
	YARPGrabber			grabber0;
	YARPGrabber			grabber1;
	YARPMagneticTracker	tracker0;
	YARPMagneticTracker	tracker1;
	YARPDataGlove		glove;
	PressureDriver		press;
	YARPGazeTracker		gt;
} collectorHardware;

// properties of the collector
mirrorCollectorProperty _property;

// ports
Port _cmdPort;
BufferedPort<BinPortable<collectorNumericalData> > _dataPort;
BufferedPort<collectorImage> _img0Port;
BufferedPort<collectorImage> _img1Port;

// flags
bool bQuit = false;
bool bStreaming = false;
bool bAwake = false;

// the hardware and its options
collectorHardware      _hardware;

// the data
collectorNumericalData _data;
collectorImage         _img0;
collectorImage         _img1;

// ---------- helpers

bool openPorts(void)
{

    char portName[256];

	ACE_OS::sprintf(portName, "/%s/cmd", _property.find("appName").asString().c_str());
    if ( _cmdPort.open(portName) == false ) return false;

	ACE_OS::sprintf(portName, "/%s/data", _property.find("appName").asString().c_str());
	if ( _dataPort.open(portName) == false ) return false;

	ACE_OS::sprintf(portName, "/%s/img0", _property.find("appName").asString().c_str());
	if ( _img0Port.open(portName) == false ) return false;

	ACE_OS::sprintf(portName, "/%s/img1", _property.find("appName").asString().c_str());
	if ( _img1Port.open(portName) == false ) return false;

	return true;

}

void closePorts(void)
{

    _img1Port.close();
    _img0Port.close();
    _dataPort.close();
    _cmdPort.close();

}

bool wakeUpSensors(void)
{

	// try and connect all peripherals requested (look at _property)
	// if all of them fail to initialise, return failure; otherwise, ok, but
	// disable peripherals which did not initialise.

	bool atLeastOneIsOK = false;

 /*
	if (_property.useCamera0) {
		// Framegrabber Initialization
		cout << "Initialising camera #0... ";
		if ( _hardware.grabber0.open (0, _property.imgSizeX, _property.imgSizeY) == YARP_OK ) {
			cout <<  "done. W=" << _property.imgSizeX << ", H=" << _property.imgSizeY <<endl;
			atLeastOneIsOK = true;
		} else {
			cout <<  "failed." <<endl;
			_property.useCamera0 = false;
		}
	}

	if (_property.useCamera1) {
		// Framegrabber Initialization
		cout << "Initialising camera #1... ";
		if ( _hardware.grabber1.open (1, _property.imgSizeX, _property.imgSizeY) == YARP_OK ) {
			cout <<  "done. W=" << _property.imgSizeX << ", H=" << _property.imgSizeY <<endl;
			atLeastOneIsOK = true;
		} else {
			cout <<  "failed." <<endl;
			_property.useCamera1 = false;
		}
	}

	if (_property.useTracker0) {
		// Tracker Initialization
		cout << "Initialising tracker #0... ";
		FoBOpenParameters FoBparams;
//		FoBparams.nGroupID = 0;
		FoBparams.comPort = _property.tracker0ComPort;
		FoBparams.baudRate = _property.tracker0BaudRate;
		FoBparams.timeOut = _property.tracker0Timeout;
		FoBparams.measurementRate = _property.tracker0MeasRate;
		FoBparams.transOpMode = _property.tracker0TransOpMode;
		if ( _hardware.tracker0.open(FoBparams) == YARP_OK ) {
			cout <<  "done. On COM" << _property.tracker0ComPort << ", " << _property.tracker0BaudRate << " baud." << endl;
			atLeastOneIsOK = true;
		} else {
			cout <<  "failed." <<endl;
			_property.useTracker0 = false;
		}
	}
	
	if (_property.useTracker1) {
		// Tracker Initialization
		cout << "Initialising tracker #1... ";
		FoBOpenParameters FoBparams;
//		FoBparams.nGroupID = 0;
		FoBparams.comPort = _property.tracker1ComPort;
		FoBparams.baudRate = _property.tracker1BaudRate;
		FoBparams.timeOut = _property.tracker1Timeout;
		FoBparams.measurementRate = _property.tracker1MeasRate;
		FoBparams.transOpMode = _property.tracker1TransOpMode;
		if ( _hardware.tracker1.open(FoBparams) == YARP_OK ) {
			cout <<  "done. On COM" << _property.tracker1ComPort << ", " << _property.tracker1BaudRate << " baud." << endl;
			atLeastOneIsOK = true;
		} else {
			cout <<  "failed." <<endl;
			_property.useTracker1 = false;
		}
	}
	
	if (_property.useDataGlove) {
		// DataGlove Initialization
		cout << "Initialising DataGlove... ";
		if ( _hardware.glove.open (_property.gloveComPort, _property.gloveBaudRate) == YARP_OK ) {
			cout <<  "done. On COM" << _property.gloveComPort << ", " << _property.gloveBaudRate << " baud." << endl;
			atLeastOneIsOK = true;
		} else {
			cout <<  "failed." <<endl;
			_property.useDataGlove = false;
		}
	}
	
	if (_property.useGazeTracker) {
		// GT Initialization
		cout << "Initialising GazeTracker... ";
		E504OpenParameters params;
		params.baudRate = _property.GTBaudRate;
		params.comPort = _property.GTComPort;
		if ( _hardware.gt.open (params) == YARP_OK ) {
			cout <<  "done. On COM" << _property.GTComPort << ", " << _property.GTBaudRate << " baud." << endl;
			atLeastOneIsOK = true;
		} else {
			cout <<  "failed." <<endl;
			_property.useGazeTracker = false;
		}
	}*/

    if ( _property.find("usePresSens").asInt() ) {
        if ( _hardware.press.open () ) {
    		cout << "pressure sensors initialised";
			atLeastOneIsOK = true;
        } else {
    		cout << "pressure sensors failed to initialise";
			_property.put("usePresSens",0);
		}
	}

	return atLeastOneIsOK;

}

// shut down sensors

void shutDownSensors(void)
{

	if (_property.find("useCamera0").asInt()) {
		_hardware.grabber0.close ();
		cout << "camera #0 released." << endl;  
	}
	
	if (_property.find("useCamera1").asInt()) {
		_hardware.grabber1.close ();
		cout << "camera #1 released." << endl;  
	}
	
	if (_property.find("useTracker0").asInt()) {
		_hardware.tracker0.close ();
		cout << "tracker #0 released." << endl;  
	}

	if (_property.find("useTracker1").asInt()) {
		_hardware.tracker1.close ();
		cout << "tracker #1 released." << endl;  
	}

	if (_property.find("useDataGlove").asInt()) {
		_hardware.glove.close ();
		cout << "dataglove released." << endl;  
	}
	
	if (_property.find("useGazeTracker").asInt()) {
		_hardware.gt.close ();
		cout << "gaze tracker released." << endl;  
	}
	
	if (_property.find("usePresSens").asInt()) {
		_hardware.press.close ();
		cout << "pressure sensors released." << endl;  
	}

}

void acquireAndSend(void)
{

	// acquire images

/*				//Mattia le immagini le faccio dopo
	if (_property.useCamera0) {
		unsigned char *buffer = NULL;
		_hardware.grabber0.waitOnNewFrame();
		_hardware.grabber0.acquireBuffer(&buffer);
		memcpy((unsigned char *)_img0.GetRawBuffer(), buffer, _property.imgSizeX * _property.imgSizeY * 3);
		_hardware.grabber0.releaseBuffer();

		_img0_outport.Content().Refer(_img0);		//controllare Content, Refer, Write
		_img0_outport.Write();						//img verrà cambiato, inviare immagine su porta,
													//_img0_outport è la porta,
													//metti img0 sulla porta _img0_outport
	}

	if (_property.useCamera1) {
		unsigned char *buffer = NULL;
		_hardware.grabber1.waitOnNewFrame();
		_hardware.grabber1.acquireBuffer(&buffer);
		memcpy((unsigned char *)_img1.GetRawBuffer(), buffer, _property.imgSizeX * _property.imgSizeY * 3);
		_hardware.grabber1.releaseBuffer();

		_img1_outport.Content().Refer(_img1);
		_img1_outport.Write();
	}
*/

  	// acquire numerical data
	if (_property.find("useTracker0").asInt())    _hardware.tracker0.getData(&_data.tracker0Data);
	if (_property.find("useTracker1").asInt())    _hardware.tracker1.getData(&_data.tracker1Data);
	if (_property.find("useGazeTracker").asInt()) _hardware.gt.getData(&_data.GTData);
	if (_property.find("useDataGlove").asInt())   _hardware.glove.getData(&_data.gloveData);
	if (_property.find("usePresSens").asInt())    _hardware.press.getData(&_data.pressureData);

    // send numerical data
    BinPortable<collectorNumericalData>& portableData = _dataPort.prepare();
	portableData.content() = _data;
    _dataPort.write();

}

// ---------- streaming thread

class streamingThread : public RateThread {
public:

    streamingThread() : RateThread(1000) {}

    virtual bool threadInit (void) {
		// put those sensors which do have a streaming mode into streaming mode
		if (_property.find("useTracker0").asInt()) _hardware.tracker0.startStreaming();
		if (_property.find("useTracker1").asInt()) _hardware.tracker1.startStreaming();
		if (_property.find("useDataGlove").asInt()) _hardware.glove.startStreaming();

        setRate((int)(1000/_property.find("streamFreq").asDouble()));
        cout << "streaming thread runs each "  << getRate() << "msecs." << endl;

        return true;
    }

    virtual void run (void) {
    	// acquire and send data
		acquireAndSend();
	}

    virtual void threadRelease (void) {
		// stop streaming mode for some sensors
		if (_property.find("useDataGlove").asInt()) _hardware.glove.stopStreaming();
		if (_property.find("useTracker1").asInt()) _hardware.tracker1.stopStreaming();
		if (_property.find("useTracker0").asInt()) _hardware.tracker0.stopStreaming();
    }

} stream;

// ------------ command port callback

class _commandPortCallback : public PortReader{
	
	virtual bool read(ConnectionReader& cmdPortConnection) {

        Bottle cmdPortCommand, cmdPortAnswer;
        cmdPortCommand.read(cmdPortConnection);
	    collectorCommand command = (collectorCommand)cmdPortCommand.get(0).asInt();
  
		switch( command ) {

		case CCmdWakeUp:
			// wake up sensors
			cout << "got wakeUp command" << endl;
			if ( ! bAwake ) {
				if ( wakeUpSensors() ) {
					bAwake = true;
					cmdPortAnswer.addInt(CCmdSucceeded);
					cout << "wakeUp succeeded" << endl;
            		cmdPortAnswer.addString(_property.toString().c_str());
					cout << "sent collector properties" << endl;
				} else {
					cmdPortAnswer.addInt(CCmdFailed);
					cout << "wakeUp failed" << endl;
				}
			} else {
				cout << "sensors are already awake" << endl;
			}
			break;

		case CCmdShutDown:
			// shut down sensors
			cout << "got shutDown command" << endl;
			if ( bAwake ) {
				shutDownSensors();
				cmdPortAnswer.addInt(CCmdSucceeded);
				bAwake = false;
				cout << "shutDown succeeded." << endl;
			} else {
				cmdPortAnswer.addInt(CCmdFailed);
				cout << "sensors are already shut down" << endl;
			}
			break;

		case CCmdGetData:
			// gather data (need to be connected, obviously)
			if ( bAwake ) {
				cmdPortAnswer.addInt(CCmdSucceeded);
				acquireAndSend();
			} else {
				cmdPortAnswer.addInt(CCmdFailed);
				cout << "getdata failed - wake up sensors first" << endl;
			}
			break;

		case CCmdStartStreaming:
			// start streaming thread
			cout << "got startStreaming command" << endl;
			if ( bAwake && !bStreaming ) {
				stream.start();
				bStreaming = true;
				cmdPortAnswer.addInt(CCmdSucceeded);
				cout << "startStreaming succeeded - now streaming" << endl;
			} else {
				cmdPortAnswer.addInt(CCmdFailed);
				cout << "startStreaming failed (must be awake and not streaming)" << endl;
			}
			break;

		case CCmdStopStreaming:
			// start streaming thread
			cout << "got stopStreaming command" << endl;
			if ( bAwake && bStreaming ) {
				stream.stop();
				bStreaming = false;
				cmdPortAnswer.addInt(CCmdSucceeded);
				cout << "stopStreaming succeeded - no longer streaming" << endl;
			} else {
				cmdPortAnswer.addInt(CCmdFailed);
				cout << "stopStreaming failed (must be awake and streaming)" << endl;
			}
			break;

		default:
			// what the hell happened on that port?
			cout << "received unknown command on command port." << endl;
			break;

		} // switch

        // send the answer
        ConnectionWriter *cmdPortWriter = cmdPortConnection.getWriter();
    	if ( cmdPortWriter != NULL ) {
		    cmdPortAnswer.write(*cmdPortWriter);
        }

        return true;
	}

} commandPortCallback;

// ---------- main

int main (int argc, char *argv[])
{

	cout.precision(3); 
	cout.setf(ios::fixed);

    // load options from the configuration file
    _property.fromConfigFile(_property.find("propertyFileName").asString().c_str(),false);

//  _img0.Resize (_property.imgSizeX, _property.imgSizeY);
//  _img1.Resize (_property.imgSizeX, _property.imgSizeY);

	Network::init();

    if ( ! openPorts() ) {
    	cout << "could not open ports. bailing out." << endl;
        return 0;
    }
	_cmdPort.setReader(commandPortCallback);

	cout << "now waiting for commands." << endl;

	// ------------ check keyboard commands

	while ( true ) {

		if ( _kbhit() ) {
			switch ( _getch() ) {
			case 'r':
				// RESET: if streaming, stop it, then if connected, disconnect sensors
				if ( bStreaming ) {
	    			stream.stop();
					bStreaming = false;
					cout << "No longer streaming." << endl;
				}
				if ( bAwake ) {
					shutDownSensors();
					bAwake = false;
					cout << "Disconnected." << endl;
				}
				break;
			
            case 'q':
				// QUIT: unregister ports and bail out
				if ( bAwake ) {
					cout << "Reset me first ('r')." << endl;
				} else {
					bAwake = false;
					bQuit = true;
					cout << "Quitting." << endl;
					closePorts();
					return 0;
				}
				break;

            default:
				// unrecognised keyboard command
				cout << "Unrecognised keyboard command." << endl;
				cout << "'r' (stop streaming and) disconnect sensors" << endl;
				cout << "'q' quit" << endl;
				cout << endl;
				break;
			}
		}

		// in order not to suck the CPU up, sleep .1 seconds at every cycle. this does not
		// affect the actual streaming frequency, which is set in the streaming thread.
		Time::delay(0.1);

    }

	cout << "Bailing out." << endl;
	closePorts();
	
    return 0;

}
