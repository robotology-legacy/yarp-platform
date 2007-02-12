// fake data from the hardware

typedef struct FakeDataStruct {
	FakeDataStruct() { clean(); }
	void clean() {
	    fakeDatum[0] = 0;
	    fakeDatum[1] = 1;
	    fakeDatum[2] = 2;
	    fakeDatum[3] = 3;
	    fakeDatum[4] = 4;
	    fakeDatum[5] = 5;
	};
	int fakeDatum[6];
} FakeData;

class TrackerData :		public FakeData {};
class GazeTrackerData : public FakeData {};
class DataGloveData :	public FakeData {};
class PresSensData :	public FakeData {};

// fake hardware

typedef struct FakeDriverStruct {
	void startStreaming(void) {};
	void stopStreaming(void) {};
	void getData(FakeData* f) {
        f->fakeDatum[0]++; f->fakeDatum[2]+=2; f->fakeDatum[4]+=3; 
        f->fakeDatum[1]--; f->fakeDatum[3]-=2; f->fakeDatum[5]-=3; 
    };
	void waitOnNewFrame(void) {};
    void acquireBuffer(unsigned char* b) {};
	void uninitialize(void) {};
} FakeDriver;

class YARPGrabber :			public FakeDriver {};
class YARPMagneticTracker : public FakeDriver {};
class YARPDataGlove :		public FakeDriver {};
class YARPGazeTracker :		public FakeDriver {};
class YARPPresSens :		public FakeDriver {};
