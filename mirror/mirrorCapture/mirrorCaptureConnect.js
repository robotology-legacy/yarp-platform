var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("yarp connect /mirrorCapture/cmd  /mirrorCollector/cmd", 7, false);
WshShell.Run ("yarp connect /mirrorCollector/data /mirrorCapture/data ", 7, false);
WshShell.Run ("yarp connect /mirrorCollector/img0 /mirrorCapture/img0", 7, false);
WshShell.Run ("yarp connect /mirrorCollector/img1 /mirrorCapture/img1", 7, false);
