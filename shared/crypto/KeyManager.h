/*
Manages a set of PasswordCiphers
by: Connor Douthat
10/18/2015
*/

typedef PasswordCipher* (*PasswordSource)(bool, bool, const char*);

class KeyManager
{
	std::vector<PasswordCipher*> keys;

public:
	bool skip_encrypt_pass;
	PasswordSource get_pass;

	KeyManager()
	{
		skip_encrypt_pass = false;
		//Attempt a blank password by default
		keys.push_back(new PasswordCipher(""));
	}
	~KeyManager()
	{
		for(int i = 0; i < keys.size(); i++)
		{
			delete keys[i];
		}
	}
	char *encrypt(const char *plain, unsigned char *iv_out, const char *prompt_text = NULL)
	{
		//Always get explicit key for encryption
		if(!get_pass) return NULL;
		PasswordCipher *crypto = get_pass(true, skip_encrypt_pass, prompt_text);
		if(!crypto) return NULL;
		char *result = crypto->encrypt(plain, iv_out);
		delete crypto;
		return result;
	}
	char *decrypt(const char *enc, unsigned char *iv, const char *prompt_text = NULL)
	{
		char *result = NULL;
		//Attempt stored keys first
		for(int i = 0; i < keys.size(); i++)
		{
			if(result = keys[i]->decrypt(enc, iv))
			{
				return result;
			}
		}
		//Prompt for new password (if available)
		while(!result)
		{
			if(!get_pass) return NULL;
			PasswordCipher *crypto = get_pass(false, false, prompt_text);
			if(!crypto) return NULL;
			if(result = crypto->decrypt(enc, iv))
			{
				//Save successful key for later
				keys.push_back(crypto);
			}
			else
			{
				ErrorBox("Decryption failed, pease try again.");
				delete crypto;
			}
		}
		return result;
	}
};
