/*
Manages popup menus
by: Connor Douthat
10/22/2015
*/
unsigned int NextMenuId()
{
	return menu_keys.size();
}
unsigned int AddMenuKey(char *key)
{
	unsigned int id = NextMenuId();
	menu_keys.push_back(key);
	return id;
}
HMENU EntryListMenu()
{
	MenuTree tree;
	sqlite3_stmt *stmt;
	if(SQLITE_OK == sqlite3_prepare_v2(db.loaded, "SELECT `key` FROM `entries` ORDER BY `key`", -1, &stmt, NULL))
	{
		while(sqlite3_step(stmt) == SQLITE_ROW)
		{
			//Duplicate key and append menu
			char *key = strdup((const char*)sqlite3_column_text(stmt, 0));
			tree.parse(key, TRAY_KEY + AddMenuKey(key));
		}
		sqlite3_finalize(stmt);
	}
	return tree.create();
}
HMENU DBHistoryMenu()
{
	HMENU hm = CreatePopupMenu();
	for(int i = 0; i < db.history.size(); i++)
	{
		AppendMenu(hm, MF_STRING, TRAY_SWITCH_DB + i, db.history[i]);
	}
	AppendMenu(hm, MF_STRING, TRAY_BROWSE_DB, "Other...");
	AppendMenu(hm, MF_STRING, TRAY_CLEAR_DB, "Clear List");
	CheckMenuRadioItem(hm, 0, 0, 0, MF_BYPOSITION);
	return hm;
}
bool MenuInit()
{
	//Create main menu container
	popup_menu = CreatePopupMenu();
	if(!popup_menu) return false;
	//Create database switcher menu
	HMENU dbMenu = DBHistoryMenu();
	//Create entry-based menus
	HMENU recallMenu = EntryListMenu();
	recall_menu_end = NextMenuId();
	HMENU encryptMenu = EntryListMenu();
	encrypt_menu_end = NextMenuId();
	HMENU removeMenu = EntryListMenu();
	remove_menu_end = NextMenuId();
	//Append top level menus
	AppendMenu(popup_menu, MF_STRING, TRAY_EXIT, "Exit");
	AppendMenu(popup_menu, MF_STRING, TRAY_EXPORT, "Raw Export");
	AppendMenu(popup_menu, MF_STRING | MF_POPUP, (UINT_PTR)dbMenu, "Change Vault");
	AppendMenu(popup_menu, MF_STRING | MF_POPUP, (UINT_PTR)removeMenu, "Delete");
	AppendMenu(popup_menu, MF_STRING | MF_POPUP, (UINT_PTR)encryptMenu, "Recrypt");
	AppendMenu(popup_menu, MF_STRING | MF_POPUP, (UINT_PTR)recallMenu, "Load to Clipboard");
	AppendMenu(popup_menu, MF_STRING, TRAY_SAVE, "Save from Clipboard");
	AppendMenu(popup_menu, MF_STRING, TRAY_GENERATE, "Randomize Clipboard");
	return true;
}
bool MenuCleanup()
{
	//Destroy main menu
	DestroyMenu(popup_menu);
	//Free menu keys
	for(int i = 0; i < menu_keys.size(); i++)
	{
		free(menu_keys[i]);
	}
	menu_keys.clear();
	return true;
}
bool MenuReload()
{
	MenuCleanup();
	return MenuInit();
}
