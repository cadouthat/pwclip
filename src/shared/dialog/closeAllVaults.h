/*
Interaction to close all vaults
by: Connor Douthat
2/16/2016
*/
void CloseAllVaultsDialog()
{
	int closed = vaults.close();
	if(closed > 0)
	{
		MenuReload();
		if(closed > 1) TrayBalloon("Vaults closed.");
		else TrayBalloon("Vault closed.");
	}
}
