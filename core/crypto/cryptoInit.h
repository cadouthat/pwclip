/*
Acquire user password and start crypto
by: Connor Douthat
10/4/2015
*/
PasswordCipher *CryptoInit()
{
	//Silent password input
	char pass[PASSWORD_MAX + 1] = {0};
	do
	{
		printf("Encryption password: ");
		for(int i = 0; i < PASSWORD_MAX; i++)
		{
			char c = getch();
			if(c == '\r' || c == '\n') break;
			pass[i] = c;
		}
		printf("\n");
		if(!pass[0]) printf("You must enter a password.");
	}while(!pass[0]);
	//Create cipher
	PasswordCipher *cipher = new PasswordCipher(pass);
	//Wipe password
	memset(pass, 0, sizeof(pass));
	return cipher;
}
