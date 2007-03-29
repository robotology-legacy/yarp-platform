var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("yarp connect /mirrorCapture/cmd  /mirrorCollector/cmd", 7, false);
WshShell.Run ("yarp connect /mirrorCollector/data /mirrorCapture/data ", 7, false);
WshShell.Run ("yarp connect /net=10/grabber/img0 /net=10/mirrorCapture/img0", 7, false);
WshShell.Run ("yarp connect /net=10/grabber/img1 /net=10/mirrorCapture/img1", 7, false);
