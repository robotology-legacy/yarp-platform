var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("yarpdev --stamp --device picolo --name /grabber/img0 --unit 0 --width 384 --height 272", 7, false);
WshShell.Run ("yarpdev --stamp --device picolo --name /grabber/img1 --unit 1 --width 384 --height 272", 7, false);
WshShell.Run ("C:\\work\\platform\\mirror\\mirrorCollector\\Debug\\mirrorCollector", 7, false);
//WshShell.Run ("C:\\work\\platform\\mirror\\mirrorCapture\\Debug\\mirrorCapture", 1, false);
