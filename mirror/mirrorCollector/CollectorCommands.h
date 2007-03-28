#ifndef __CollectorCommandsh__
#define __CollectorCommandsh__

using namespace yarp::os;

// yarp drivers

#include <yarp/FoBDeviceDriver.h>
#include <yarp/CyberGloveDeviceDriver.h>
#include <yarp/E504DeviceDriver.h>
#include <yarp/JoyPresDeviceDriver.h>

// commands exchanged with the client applications

typedef enum {
    CCmdFailed         = 0,
    CCmdSucceeded      = 1,
    CCmdWakeUp         = 2,
    CCmdShutDown       = 3,
    CCmdGetData        = 4,
    CCmdStartStreaming = 5,
    CCmdStopStreaming  = 6,
} collectorCommand;

// numerical data sent over the network

typedef struct _collectorNumericalData {
    
	_collectorNumericalData() { clean(); };

    void clean() {
        tracker0Data.clean();
        tracker1Data.clean();
        GTData.clean();
        gloveData.clean();
        pressureData.clean();
    };
    
	FoBData			tracker0Data, tracker1Data;
    E504Data		GTData;
    CyberGloveData	gloveData;
    JoyPresData		pressureData;

} collectorNumericalData;

#endif
