/*
GUI related constant values
by: Connor Douthat
10/22/2015
*/
#define TRAY_TOOLTIP "pwclip"
#define ERROR_TITLE "Error - pwclip"

#define MSG_TRAYICON (WM_USER + 1)

#define TRAY_EXIT 1
#define TRAY_EXPORT 2
#define TRAY_SET_MASTER 3
#define TRAY_GENERATE 4
#define TRAY_SAVE 5
#define TRAY_CLOSE_DB 6
#define TRAY_BROWSE_DB 7
#define TRAY_CREATE_DB 8
#define TRAY_CONFIG 9
#define TRAY_SWITCH_DB 10
#define TRAY_KEY (TRAY_SWITCH_DB + MAX_DB_HIST + 1)

#define TIMER_WIPE 1
#define TIMER_UPDATE_TRAY 2

#define TRAY_STATE_NORMAL 1
#define TRAY_STATE_WIPE 2
