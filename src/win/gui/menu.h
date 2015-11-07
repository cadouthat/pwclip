/*
Manages popup menus on windows
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
	if(vaults.topOpen())
	{
		MenuTree tree;
		sqlite3_stmt *stmt;
		if(SQLITE_OK == sqlite3_prepare_v2(vaults.top()->db(), "SELECT `key` FROM `entries` WHERE `key`!='__meta__' ORDER BY `key`", -1, &stmt, NULL))
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
	else return CreatePopupMenu();
}
HMENU DBHistoryMenu()
{
	HMENU hm = CreatePopupMenu();
	for(int i = 0; i < vaults.history.size(); i++)
	{
		AppendMenu(hm, MF_STRING, TRAY_SWITCH_DB + i, vaults.history[i]->path());
	}
	AppendMenu(hm, MF_STRING, TRAY_BROWSE_DB, "Other...");
	//Indicate current vault if open
	if(vaults.topOpen()) CheckMenuRadioItem(hm, 0, 0, 0, MF_BYPOSITION);
	return hm;
}
bool MenuInit()
{
	//Create main menu container
	popup_menu = CreatePopupMenu();
	if(!popup_menu) return false;
	//Create vault switcher menu
	HMENU dbMenu = DBHistoryMenu();
	//Create entry-based menus
	HMENU recallMenu = EntryListMenu();
	recall_menu_end = NextMenuId();
	HMENU removeMenu = EntryListMenu();
	remove_menu_end = NextMenuId();
	//Append top level menus
	UINT need_vault = vaults.topOpen() ? 0 : MF_GRAYED;
	AppendMenu(popup_menu, MF_STRING, TRAY_EXIT, "Exit");
	AppendMenu(popup_menu, MF_STRING | need_vault | MF_POPUP, TRAY_CLOSE_DB, "Close All Vaults");
	AppendMenu(popup_menu, MF_STRING | MF_POPUP, (UINT_PTR)dbMenu, "Open Vault");
	AppendMenu(popup_menu, MF_STRING | need_vault, TRAY_EXPORT, "Raw Export");
	AppendMenu(popup_menu, MF_STRING | need_vault | MF_POPUP, TRAY_SET_MASTER, "Change Master Password");
	AppendMenu(popup_menu, MF_STRING | need_vault | MF_POPUP, (UINT_PTR)removeMenu, "Delete Entry");
	AppendMenu(popup_menu, MF_STRING | need_vault | MF_POPUP, (UINT_PTR)recallMenu, "Load to Clipboard");
	AppendMenu(popup_menu, MF_STRING | need_vault, TRAY_SAVE, "Save from Clipboard");
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
