// ---------- headers

#include <iostream>
#include <conio.h>

#include <yarp/os/BinPortable.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/Stamp.h>

#include "CollectorCommands.h"

// ----------------- globals

using namespace std;
using namespace yarp::dev;

// what hardware is possibly attached to this setup?
typedef struct {
	FoBDeviceDriver			tracker0;
	FoBDeviceDriver			tracker1;
	CyberGloveDeviceDriver	glove;
	E504DeviceDriver		gt;
	JoyPresDeviceDriver		press;
} collectorHardware;

// properties of the collector
mirrorCollectorProperty _property;

// ports
Port _cmdPort;
BufferedPort<BinPortable<collectorNumericalData> > _dataPort;

Stamp stamp(0,0.0);

// flags
bool bQuit = false;
bool bStreaming = false;
bool bAwake = false;

// the hardware and its options
collectorHardware      _hardware;

// the data
collectorNumericalData _data;

// ---------- helpers

bool openPorts(void)
{

    char portName[256];

	ACE_OS::sprintf(portName, "/%s/cmd", _property.find("appName").asString().c_str());
    if ( _cmdPort.open(portName) == false ) return false;

	ACE_OS::sprintf(portName, "/%s/data", _property.find("appName").asString().c_str());
	if ( _dataPort.open(portName) == false ) return false;

	return true;

}

void closePorts(void)
{

    _dataPort.close();
    _cmdPort.close();

}

bool wakeUpSensors(void)
{

	// try and connect all peripherals requested (look at _property)
	// if all of them fail to initialise, return failure; otherwise, ok, but
	// disable peripherals which did not initialise.

	bool atLeastOneIsOK = false;

	if ( _property.find("useTracker0").asInt() ) {
		// Tracker Initialization
		cout << "Initialising tracker #0... ";
		yarp::os::Property tracker0Property;
		tracker0Property.put("comPort", _property.find("tracker0ComPort"));
		tracker0Property.put("baudRate", _property.find("tracker0BaudRate"));
		tracker0Property.put("timeOut", _property.find("tracker0Timeout"));
		tracker0Property.put("measurementRate", _property.find("tracker0MeasRate"));
		tracker0Property.put("transOpMode", _property.find("tracker0TransOpMode"));
		if ( _hardware.tracker0.open(tracker0Property)  ) {
			cout <<  "done. On COM" << tracker0Property.find("comPort").asInt() << ", " << tracker0Property.find("baudRate").asInt() << " baud." << endl;
			atLeastOneIsOK = true;
		} else {
			cout <<  "failed." <<endl;
			_property.put("useTracker0", 0);
		}
	}

	if ( _property.find("useTracker1").asInt() ) {
		// Tracker Initialization
		cout << "Initialising tracker #1... ";
		yarp::os::Property tracker1Property;
		tracker1Property.put("comPort", _property.find("tracker1ComPort"));
		tracker1Property.put("baudRate", _property.find("tracker1BaudRate"));
		tracker1Property.put("timeOut", _property.find("tracker1Timeout"));
		tracker1Property.put("measurementRate", _property.find("tracker1MeasRate"));
		tracker1Property.put("transOpMode", _property.find("tracker1TransOpMode"));
		if ( _hardware.tracker1.open(tracker1Property)  ) {
			cout <<  "done. On COM" << tracker1Property.find("comPort").asInt() << ", " << tracker1Property.find("baudRate").asInt() << " baud." << endl;
			atLeastOneIsOK = true;
		} else {
			cout <<  "failed." <<endl;
			_property.put("useTracker1", 0);
		}
	}

	if ( _property.find("useDataGlove").asInt() ) {
		// DataGlove Initialization
		cout << "Initialising DataGlove... ";
		yarp::os::Property GloveProperty;
		GloveProperty.put("comPort", _property.find("gloveComPort"));
		GloveProperty.put("baudRate", _property.find("gloveBaudRate"));
		if ( _hardware.glove.open (GloveProperty) ) {
			cout <<  "done. On COM" << GloveProperty.find("comPort").asInt() << ", " << GloveProperty.find("baudRate").asInt() << " baud." << endl;
			atLeastOneIsOK = true;
		} else {
			cout <<  "failed." <<endl;
			_property.put("useDataGlove",0);
		}
	}
	
	if ( _property.find("useGazeTracker").asInt() ) {
		// GT Initialization
		cout << "Initialising GazeTracker... ";
		yarp::os::Property GazeProperty;
		GazeProperty.put("comPort", _property.find("GTBaudRate"));
		GazeProperty.put("baudRate", _property.find("GTComPort"));
		if ( _hardware.gt.open (GazeProperty) ) {
			cout <<  "done. On COM" << GazeProperty.find("comPort").asInt() << ", " << GazeProperty.find("baudRate").asInt() << " baud." << endl;
			atLeastOneIsOK = true;
		} else {
			cout <<  "failed." <<endl;
			_property.put("useGazeTracker",0);
		}
	}	

    if ( _property.find("usePresSens").asInt() ) {
		yarp::os::Property PresSensProperty;
        if ( _hardware.press.open (PresSensProperty) ) {
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

  	// acquire and send numerical data
	if (_property.find("useTracker0").asInt())    _hardware.tracker0.getData(&_data.tracker0Data);
	if (_property.find("useTracker1").asInt())    _hardware.tracker1.getData(&_data.tracker1Data);
	if (_property.find("useDataGlove").asInt())   _hardware.glove.getData(&_data.gloveData);
	if (_property.find("usePresSens").asInt())    _hardware.press.getData(&_data.pressureData);
	if (_property.find("useGazeTracker").asInt()) _hardware.gt.getData(&_data.GTData);

	stamp.update();
	_dataPort.setEnvelope(stamp);
    BinPortable<collectorNumericalData>& portableData = _dataPort.prepare();
	portableData.content() = _data;
    _dataPort.write();

}

// ---------- streaming thread

class _streamingThread : public RateThread {
public:

    _streamingThread() : RateThread(1000) {}

    virtual bool threadInit (void) {
		// put those sensors which do have a streaming mode into streaming mode
		if (_property.find("useTracker0").asInt()) _hardware.tracker0.startStreaming();
		if (_property.find("useTracker1").asInt()) _hardware.tracker1.startStreaming();
		if (_property.find("useDataGlove").asInt()) _hardware.glove.startStreaming();
		// set correct stremaing rate
        setRate((int)(1000/_property.find("streamFreq").asDouble()));
        cout << "streaming thread runs each "  << getRate() << "msecs." << endl;
        return true;
    }

    virtual void run (void) {
		acquireAndSend();
	}

    virtual void threadRelease (void) {
		// stop streaming mode for some sensors
		if (_property.find("useDataGlove").asInt()) _hardware.glove.stopStreaming();
		if (_property.find("useTracker1").asInt()) _hardware.tracker1.stopStreaming();
		if (_property.find("useTracker0").asInt()) _hardware.tracker0.stopStreaming();
    }

} streamingThread;

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
					cout << "\nwakeUp succeeded" << endl;
            		cmdPortAnswer.addString(_property.toString().c_str());
					cout << "sent collector properties" << endl;
				} else {
					cmdPortAnswer.addInt(CCmdFailed);
					cout << "\nwakeUp failed" << endl;
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
				streamingThread.start();
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
				streamingThread.stop();
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
	    			streamingThread.stop();
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
		Time::delay(0.5);

    }

	cout << "Bailing out." << endl;
	closePorts();
	
    return 0;

}
