/*
Menu callbacks and placeholders
by: Connor Douthat
2/18/2016
*/
void MenuExit(const char *key);

void MenuCreateVault(const char *key)
{
	char db_path[256] = {0};
	if(BrowseForOutput(FILE_TYPE_DB, db_path))
	{
		OpenVaultDialog(0, db_path);
	}
}
void MenuBrowseVault(const char *key)
{
	char db_path[256] = {0};
	if(BrowseForInput(FILE_TYPE_DB, db_path))
	{
		OpenVaultDialog(0, db_path);
	}
}
void MenuSwitchVault(const char *key)
{
	OpenVaultDialog(atoi(key));
}
void MenuExportVault(const char *key)
{
	ExportVaultDialog(atoi(key));
}
void MenuChangeVaultPass(const char *key)
{
	ChangeVaultPassDialog();
}
void MenuCloseVault(const char *key)
{
	CloseVaultDialog(atoi(key));
}
void MenuCloseAllVaults(const char *key)
{
	CloseAllVaultsDialog();
}
void MenuCreateEntry(const char *key)
{
	CreateEntryDialog();
}
void MenuRecallEntry(const char *key)
{
	if(!vaults.topOpen()) return;
	VaultEntry entry(vaults.top(), key);
	RecallEntryDialog(&entry);
}
void MenuRemoveEntry(const char *key)
{
	if(!vaults.topOpen()) return;
	VaultEntry entry(vaults.top(), key);
	RemoveEntryDialog(&entry);
}
void MenuChangeEntry(const char *key)
{
	if(!vaults.topOpen()) return;
	VaultEntry entry(vaults.top(), key);
	ChangeEntryDialog(&entry);
}
void MenuPreferences(const char *key)
{
	PreferencesDialog();
}
