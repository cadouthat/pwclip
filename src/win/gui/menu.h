/*
Manages popup menus on windows
by: Connor Douthat
10/22/2015
*/
unsigned int NextMenuId()
{
	return menu_keys.size();
}
int AddMenuKey(const char *key)
{
	unsigned int id = NextMenuId();
	menu_keys.push_back(strdup(key));
	return TRAY_KEY + id;
}
HMENU EntryListMenu(const char *emptyText = NULL, const char *treeItemText = NULL)
{
	HMENU result = NULL;
	if(vaults.topOpen())
	{
		MenuTree tree(AddMenuKey);
		sqlite3_stmt *stmt;
		if(SQLITE_OK == sqlite3_prepare_v2(vaults.top()->db(), "SELECT `key` FROM `entries` WHERE `key`!='__meta__' ORDER BY `key`", -1, &stmt, NULL))
		{
			while(sqlite3_step(stmt) == SQLITE_ROW)
			{
				//Duplicate key and append menu
				const char *key = (const char*)sqlite3_column_text(stmt, 0);
				tree.parse(key);
			}
			sqlite3_finalize(stmt);
		}
		result = tree.create(treeItemText);
	}
	else result = CreatePopupMenu();
	if(emptyText && !GetMenuItemCount(result))
	{
		AppendMenu(result, MF_STRING | MF_GRAYED, (UINT_PTR)AddMenuKey(""), emptyText);
	}
	return result;
}
HMENU DBHistoryMenu(bool open)
{
	HMENU hm = CreatePopupMenu();
	for(int i = 0; i < vaults.history.size(); i++)
	{
		const char *name = fileNameInPath(vaults.history[i]->path());
		for(int j = 0; j < vaults.history.size(); j++)
		{
			if(j == i) continue;
			const char *name_compare = fileNameInPath(vaults.history[j]->path());
			if(!stricmp(name, name_compare))
			{
				name = vaults.history[i]->path();
				break;
			}
		}
		if(vaults.history[i]->isOpen() == open)
		{
			AppendMenu(hm, MF_STRING, TRAY_SWITCH_DB + i, name);
		}
	}
	if(!open) AppendMenu(hm, MF_STRING, TRAY_BROWSE_DB, "Browse...");
	//Indicate current vault if open
	if(open && vaults.topOpen()) CheckMenuRadioItem(hm, 0, 0, 0, MF_BYPOSITION);
	return hm;
}
bool MenuInit()
{
	UINT need_vault = vaults.topOpen() ? 0 : MF_GRAYED;
	//Create main menu container
	popup_menu = CreatePopupMenu();
	if(!popup_menu) return false;
	//Create vault switcher menu
	HMENU dbOpenMenu = DBHistoryMenu(false);
	HMENU dbSwitchMenu = DBHistoryMenu(true);
	//Build vault management menu
	HMENU vaultMenu = CreatePopupMenu();
	AppendMenu(vaultMenu, MF_STRING, TRAY_CREATE_DB, "Create Vault");
	AppendMenu(vaultMenu, MF_STRING | MF_POPUP, (UINT_PTR)dbOpenMenu, "Open Vault");
	AppendMenu(vaultMenu, MF_STRING | need_vault | MF_POPUP, (UINT_PTR)dbSwitchMenu, "Switch Vault");
	AppendMenu(vaultMenu, MF_STRING | need_vault | MF_POPUP, TRAY_CLOSE_DB, "Close All Vaults");
	//Create entry-based menus
	HMENU recallMenu = EntryListMenu("(no entries)");
	recall_menu_end = NextMenuId();
	HMENU removeMenu = EntryListMenu("(no entries)");
	remove_menu_end = NextMenuId();
	HMENU renameMenu = EntryListMenu("(no entries)", "(this tree)");
	rename_menu_end = NextMenuId();
	save_menu = EntryListMenu(NULL, "(new entry)");
	save_menu_end = NextMenuId();
	InsertMenu(save_menu, 0, MF_BYPOSITION | MF_STRING, TRAY_SAVE, "(new entry)");
	//Build edit menu
	HMENU editMenu = CreatePopupMenu();
	AppendMenu(editMenu, MF_STRING | need_vault | MF_POPUP, (UINT_PTR)renameMenu, "Rename");
	AppendMenu(editMenu, MF_STRING | need_vault | MF_POPUP, (UINT_PTR)removeMenu, "Delete Entry");
	AppendMenu(editMenu, MF_STRING | need_vault, TRAY_SET_MASTER, "Change Vault Password");
	AppendMenu(editMenu, MF_STRING, TRAY_CONFIG, "Preferences");
	//Append top level menus
	AppendMenu(popup_menu, MF_STRING, TRAY_EXIT, "Exit");
	AppendMenu(popup_menu, MF_STRING | MF_POPUP, (UINT_PTR)vaultMenu, "Vaults");
	AppendMenu(popup_menu, MF_STRING | need_vault, TRAY_EXPORT, "Export Raw Entries");
	AppendMenu(popup_menu, MF_STRING | MF_POPUP, (UINT_PTR)editMenu, "Edit");
	AppendMenu(popup_menu, MF_STRING | need_vault | MF_POPUP, (UINT_PTR)save_menu, "Save Clipboard as");
	AppendMenu(popup_menu, MF_STRING, TRAY_GENERATE, "Generate Password");
	AppendMenu(popup_menu, MF_STRING | need_vault | MF_POPUP, (UINT_PTR)recallMenu, "Load to Clipboard");
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
