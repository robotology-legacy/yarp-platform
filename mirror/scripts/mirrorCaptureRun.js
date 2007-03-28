var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("C:\\work\\yarp2\\bin\\release\\yarpdev --stamp --device picolo --name /grabber/img0 --unit 0 --width 384 --height 272", 7, false);
WshShell.Run ("C:\\work\\yarp2\\bin\\release\\yarpdev --stamp --device picolo --name /grabber/img1 --unit 1 --width 384 --height 272", 7, false);
WshShell.Run ("C:\\work\\platform\\mirror\\mirrorCollector\\release\\mirrorCollector", 7, false);
WshShell.Run ("C:\\work\\platform\\mirror\\mirrorCapture\\debug\\mirrorCapture", 1, false);
