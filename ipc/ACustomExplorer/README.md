### Steps

1. create a new file to store your _classname_
   - This code is written in [MainWindow::initSeer](https://github.com/ccseer/Seer/blob/master/ipc/ACustomExplorer/mainwindow.cpp#L38)
   - Seer will check whether the folder exists every time it starts, and create a new folder if it does not exist.
2. `Seer`: All json files in this path will be loaded after Seer is started. When the user presses the space, Seer gets the handle to match the classname through GetForegroundWindow. If it matches your program, it will send a WM_COPYDATA message to the handle, and the value of dwData is SEER_REQUEST_PATH .
3. `Your program`: After your program receives WM_COPYDATA, it matches the SEER_REQUEST_PATH of dwData, and sends a message back as soon as possible with the full path of the selected file.
   - This code is written in [MainWindow::onCopyDataMsg](https://github.com/ccseer/Seer/blob/master/ipc/ACustomExplorer/mainwindow.cpp#L140)
4. `Seer`: When the Seer receives the message, it previews the file and the process ends.
   - The logic code is all Win32, which has nothing to do with the Qt code of the UI, even if there is no Qt framework, it will not affect the reading.

### Screenshot

![example](https://user-images.githubusercontent.com/15963166/185766336-6b67a98b-ea2a-4647-aca8-c6437c877d09.png)
