/*
GUI related constant values
by: Connor Douthat
10/22/2015
*/
#define TRAY_TOOLTIP "pwclip"
#define ERROR_TITLE "Error - pwclip"

#define MSG_TRAYICON (WM_USER + 1)

#define TRAY_EXIT 1
#define TRAY_PREFERENCES 2
#define TRAY_CREATE_ENTRY 3
#define TRAY_VAULT_PASS 4
#define TRAY_CLOSE_ALL 5
#define TRAY_BROWSE_DB 6
#define TRAY_CREATE_DB 7
#define TRAY_SWITCH_DB 8
#define TRAY_EXPORT_DB (TRAY_SWITCH_DB + MAX_DB_HIST + 1)
#define TRAY_CLOSE_DB (TRAY_EXPORT_DB + MAX_DB_HIST + 1)
#define TRAY_ENTRY_KEY (TRAY_CLOSE_DB + MAX_DB_HIST + 1)

#define TIMER_WIPE 1
#define TIMER_UPDATE_TRAY 2

#define TRAY_STATE_NORMAL 1
#define TRAY_STATE_WIPE 2
