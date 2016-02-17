/*
Interaction to close specific vault
by: Connor Douthat
2/16/2016
*/
void CloseVaultDialog(int i)
{
	vaults.close(i);
	if(i == 0 && vaults.topOpen())
	{
		//Active vault changed
		char message[512] = {0};
		sprintf(message, "Vault closed. Active vault changed to '%s'.", fileNameInPath(vaults.top()->path()));
		TrayBalloon(message);
	}
	else
	{
		TrayBalloon("Vault closed.");
	}
	MenuReload();
}
