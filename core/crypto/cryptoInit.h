/*
Acquire user password and start crypto
by: Connor Douthat
10/4/2015
*/
PasswordCipher *CryptoInit()
{
	//Silent password input
	char pass[PASSWORD_MAX + 1] = {0};
	printf("Password: ");
	for(int i = 0; i < PASSWORD_MAX; i++)
	{
		char c = getch();
		if(c == '\r' || c == '\n') break;
		pass[i] = c;
	}
	printf("\n");
	//Create cipher
	PasswordCipher *cipher = new PasswordCipher(pass);
	//Wipe password
	memset(pass, 0, sizeof(pass));
	return cipher;
}
