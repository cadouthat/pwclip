/*
Interaction to generate a password
by: Connor Douthat
2/13/2016
*/
void GenerateDialog()
{
	//Refresh config
	LoadConfig(config_path);

	char *pass = (char*)malloc(generate_length + 1);
	if(!RandText(pass, generate_length, generate_include_symbols))
	{
		ErrorBox("Failed to generate password");
		free(pass);
		return;
	}
	pass[generate_length] = 0;
	if(!SetClipboardText(pass))
	{
		ErrorBox("Failed to set clipboard text");
		free(pass);
		return;
	}
	if(generate_echo)
	{
		char *message = (char*)malloc(generate_length + 64);
		sprintf(message, "Copied %s to clipboard", pass);
		TrayBalloon(message);
		memset(message, 0, strlen(message));
		free(message);
	}
	memset(pass, 0, strlen(pass));
	free(pass);
}
