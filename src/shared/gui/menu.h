/*
Generic menu structure
by: Connor Douthat
2/18/2016
*/
MenuItemMeta *AddGlobalMenuMeta(DoMenuAction action, const char *key = NULL)
{
	MenuItemMeta *meta = new MenuItemMeta(action, key);
	global_menu_cache.push_back(meta);
	return meta;
}
void *EntryListMenu(DoMenuAction action, const char *emptyText = NULL, const char *treeItemText = NULL)
{
	MenuTree *tree = new MenuTree();
	if(vaults.topOpen())
	{
		sqlite3_stmt *stmt;
		if(SQLITE_OK == sqlite3_prepare_v2(vaults.top()->db(), "SELECT `key` FROM `entries` WHERE `key`!='__meta__' ORDER BY `key`", -1, &stmt, NULL))
		{
			while(sqlite3_step(stmt) == SQLITE_ROW)
			{
				//Duplicate key and append menu
				const char *key = (const char*)sqlite3_column_text(stmt, 0);
				tree->parse(key);
			}
			sqlite3_finalize(stmt);
		}
	}
	if(tree->count())
	{
		dynamic_menus.push_back(tree);
		return tree->build(action, treeItemText);
	}
	else
	{
		delete tree;
		void *result = CreateMenuContainer();
		if(emptyText)
		{
			AddMenuItem(result, emptyText);
		}
		return result;
	}
}
void *VaultHistoryMenu(DoMenuAction action, bool opened, bool includeGrayed)
{
	void *menu = CreateMenuContainer();
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
			char istr[32] = {0};
			sprintf(istr, "%d", i);
			AddMenuItem(menu, name, AddGlobalMenuMeta(action, istr), NULL, true, iEnabled++);
		}
		else if(includeGrayed)
		{
			AddMenuItem(menu, name);
		}
	}
	return menu;
}
void *MenuInit()
{
	bool vault_enabled = vaults.topOpen();
	//Create main menu container
	main_menu = CreateMenuContainer();
	if(!main_menu) return NULL;
	//Create vault switcher menu
	void *dbOpenMenu = VaultHistoryMenu(MenuSwitchVault, false, true);
	AddMenuItem(dbOpenMenu, "Browse...", AddGlobalMenuMeta(MenuBrowseVault));
	void *dbSwitchMenu = VaultHistoryMenu(MenuSwitchVault, true, false);
	if(vaults.topOpen()) CheckMenuItem(dbSwitchMenu, 0);
	void *dbExportMenu = VaultHistoryMenu(MenuExportVault, true, false);
	void *dbCloseMenu = VaultHistoryMenu(MenuCloseVault, true, false);
	AddMenuItem(dbCloseMenu, "(close all)", AddGlobalMenuMeta(MenuCloseAllVaults));
	//Build vault management menu
	void *vaultMenu = CreateMenuContainer();
	AddMenuItem(vaultMenu, "Create Vault", AddGlobalMenuMeta(MenuCreateVault));
	AddMenuItem(vaultMenu, "Open Vault", NULL, dbOpenMenu);
	AddMenuItem(vaultMenu, "Switch Active Vault", NULL, dbSwitchMenu, vault_enabled);
	AddMenuItem(vaultMenu, "Export Vault", NULL, dbExportMenu, vault_enabled);
	if(vaults.totalOpen() > 1)
	{
		AddMenuItem(vaultMenu, "Close Vault", NULL, dbCloseMenu);
	}
	else
	{
		AddMenuItem(vaultMenu, "Close Vault", AddGlobalMenuMeta(MenuCloseAllVaults), NULL, vault_enabled);
		DestroyMenuContainer(dbCloseMenu);
	}
	//Create entry-based menus
	void *recallMenu = EntryListMenu(MenuRecallEntry, "(no entries)");
	void *removeMenu = EntryListMenu(MenuRemoveEntry, "(no entries)");
	void *changeMenu = EntryListMenu(MenuChangeEntry, "(no entries)", "(rename container)");
	//Build edit menu
	void *editMenu = CreateMenuContainer();
	AddMenuItem(editMenu, "Change Entry", NULL, changeMenu, vault_enabled);
	AddMenuItem(editMenu, "Delete Entry", NULL, removeMenu, vault_enabled);
	AddMenuItem(editMenu, "Change Vault Password", AddGlobalMenuMeta(MenuChangeVaultPass), NULL, vault_enabled);
	AddMenuItem(editMenu, "Preferences", AddGlobalMenuMeta(MenuPreferences));
	//Append top level menus
	AddMenuItem(main_menu, "Exit", AddGlobalMenuMeta(MenuExit));
	AddMenuItem(main_menu, "Vaults", NULL, vaultMenu);
	AddMenuItem(main_menu, "Edit", NULL, editMenu);
	AddMenuItem(main_menu, "Create Entry", AddGlobalMenuMeta(MenuCreateEntry), NULL, vault_enabled);
	AddMenuItem(main_menu, "Recall Entry", NULL, recallMenu, vault_enabled);
	return main_menu;
}
bool MenuCleanup()
{
	//Destroy main menu
	if(main_menu) DestroyMenuContainer(main_menu);
	//Destroy dynamic menus
	for(int i = 0; i < dynamic_menus.size(); i++)
	{
		delete dynamic_menus[i];
	}
	dynamic_menus.clear();
	//Destroy menu cache
	for(int i = 0; i < global_menu_cache.size(); i++)
	{
		delete global_menu_cache[i];
	}
	global_menu_cache.clear();
	return true;
}
