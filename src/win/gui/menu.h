/*
Manages popup menus on windows
by: Connor Douthat
10/22/2015
*/
unsigned int PeekMenuId()
{
	return menu_keys.size();
}
int AddMenuKey(const char *key)
{
	unsigned int id = PeekMenuId();
	menu_keys.push_back(strdup(key));
	return TRAY_ENTRY_KEY + id;
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
		result = BuildMenuTree(&tree, treeItemText);
	}
	else result = CreatePopupMenu();
	if(emptyText && !GetMenuItemCount(result))
	{
		AppendMenu(result, MF_STRING | MF_GRAYED, (UINT_PTR)AddMenuKey(""), emptyText);
	}
	return result;
}
HMENU DBHistoryMenu(int idBase, bool opened, bool includeGrayed)
{
	HMENU hm = CreatePopupMenu();
	int iEnabled = 0;
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
		bool isOpen = vaults.history[i]->isOpen();
		bool isEnabled = (isOpen == opened);
		if(isEnabled)
		{
			InsertMenu(hm, iEnabled++, MF_BYPOSITION | MF_STRING, idBase + i, name);
		}
		else if(includeGrayed)
		{
			AppendMenu(hm, MF_STRING | MF_GRAYED, idBase + i, name);
		}
	}
	return hm;
}
bool MenuInit()
{
	UINT need_vault = vaults.topOpen() ? 0 : MF_GRAYED;
	//Create main menu container
	popup_menu = CreatePopupMenu();
	if(!popup_menu) return false;
	//Create vault switcher menu
	HMENU dbOpenMenu = DBHistoryMenu(TRAY_SWITCH_DB, false, true);
	AppendMenu(dbOpenMenu, MF_STRING, TRAY_BROWSE_DB, "Browse...");
	HMENU dbSwitchMenu = DBHistoryMenu(TRAY_SWITCH_DB, true, false);
	if(vaults.topOpen()) CheckMenuRadioItem(dbSwitchMenu, 0, 0, 0, MF_BYPOSITION);
	HMENU dbExportMenu = DBHistoryMenu(TRAY_EXPORT_DB, true, false);
	HMENU dbCloseMenu = DBHistoryMenu(TRAY_CLOSE_DB, true, false);
	AppendMenu(dbCloseMenu, MF_STRING | MF_POPUP, TRAY_CLOSE_ALL, "(close all)");
	//Build vault management menu
	HMENU vaultMenu = CreatePopupMenu();
	AppendMenu(vaultMenu, MF_STRING, TRAY_CREATE_DB, "Create Vault");
	AppendMenu(vaultMenu, MF_STRING | MF_POPUP, (UINT_PTR)dbOpenMenu, "Open Vault");
	AppendMenu(vaultMenu, MF_STRING | need_vault | MF_POPUP, (UINT_PTR)dbSwitchMenu, "Switch Active Vault");
	AppendMenu(vaultMenu, MF_STRING | need_vault | MF_POPUP, (UINT_PTR)dbExportMenu, "Export Vault");
	if(GetMenuItemCount(dbCloseMenu) > 2)
	{
		AppendMenu(vaultMenu, MF_STRING | need_vault | MF_POPUP, (UINT_PTR)dbCloseMenu, "Close Vault");
	}
	else
	{
		AppendMenu(vaultMenu, MF_STRING | need_vault, TRAY_CLOSE_ALL, "Close Vault");
		DestroyMenu(dbCloseMenu);
	}
	//Create entry-based menus
	HMENU recallMenu = EntryListMenu("(no entries)");
	recall_menu_end = PeekMenuId();
	HMENU removeMenu = EntryListMenu("(no entries)");
	remove_menu_end = PeekMenuId();
	HMENU changeMenu = EntryListMenu("(no entries)", "(rename container)");
	change_menu_end = PeekMenuId();
	//Build edit menu
	HMENU editMenu = CreatePopupMenu();
	AppendMenu(editMenu, MF_STRING | need_vault | MF_POPUP, (UINT_PTR)changeMenu, "Change Entry");
	AppendMenu(editMenu, MF_STRING | need_vault | MF_POPUP, (UINT_PTR)removeMenu, "Delete Entry");
	AppendMenu(editMenu, MF_STRING | need_vault, TRAY_VAULT_PASS, "Change Vault Password");
	AppendMenu(editMenu, MF_STRING, TRAY_PREFERENCES, "Preferences");
	//Append top level menus
	AppendMenu(popup_menu, MF_STRING, TRAY_EXIT, "Exit");
	AppendMenu(popup_menu, MF_STRING | MF_POPUP, (UINT_PTR)vaultMenu, "Vaults");
	AppendMenu(popup_menu, MF_STRING | MF_POPUP, (UINT_PTR)editMenu, "Edit");
	AppendMenu(popup_menu, MF_STRING | need_vault, TRAY_CREATE_ENTRY, "Create Entry");
	AppendMenu(popup_menu, MF_STRING | need_vault | MF_POPUP, (UINT_PTR)recallMenu, "Recall Entry");
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
