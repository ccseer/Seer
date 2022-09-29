#ifndef SEER_IPC_H
#define SEER_IPC_H

#define SEER_CLASS_NAME L"SeerWindowClass"

// sent from Seer
#define SEER_REQUEST_PATH 4000
// send back to Seer
#define SEER_RESPONSE_PATH 4001

// send to Seer directly without request
// the minimum call interval for SEER_INVOKE_* should be 200ms
#define SEER_INVOKE_W32 5000
#define SEER_INVOKE_W32_SEP 5001
#define SEER_INVOKE_QT 5002
#define SEER_INVOKE_QT_SEP 5003

// Seer can have multiple windows(main + separated) at the same time
// this is checking for visibility of main window
#define SEER_IS_VISIBLE 5004
#define SEER_IS_VISIBLE_TRUE 1
#define SEER_IS_VISIBLE_FALSE 0

// Hide main window if it is visible
#define SEER_HIDE 5005

//////////////////////////////////////////
// json file
// file should be located at  %USERPROFILE%/Documents/Seer/explorers/
#define SEER_EXPLORER_FOLDER "explorers"
// required key
#define SEER_JSON_KEY_CLASSNAME "classname"
// optional
#define SEER_JSON_KEY_WINDOWTEXT "windowtext"
#define SEER_JSON_KEY_APPNAME "appname"

#endif  // SEER_IPC_H
