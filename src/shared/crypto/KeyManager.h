/*
Manages a set of PasswordCiphers
by: Connor Douthat
10/18/2015
*/

class KeyManager
{
	PasswordCipher *next_encrypt_key;
	PasswordCipher *next_decrypt_key;
	std::vector<PasswordCipher*> keys;

public:
	KeyManager()
	{
		next_encrypt_key = NULL;
		next_decrypt_key = NULL;
		//Attempt a blank password by default
		keys.push_back(new PasswordCipher(""));
	}
	~KeyManager()
	{
		if(next_encrypt_key)
		{
			delete next_encrypt_key;
			next_encrypt_key = NULL;
		}
		if(next_decrypt_key)
		{
			delete next_decrypt_key;
			next_decrypt_key = NULL;
		}
		for(int i = 0; i < keys.size(); i++)
		{
			delete keys[i];
		}
	}
	void nextEncrypt(PasswordCipher *next_in)
	{
		if(next_encrypt_key) delete next_encrypt_key;
		next_encrypt_key = next_in;
	}
	void nextDecrypt(PasswordCipher *next_in)
	{
		if(next_decrypt_key) delete next_decrypt_key;
		next_decrypt_key = next_in;
	}
	char *encrypt(const char *plain, unsigned char *iv_out)
	{
		//Always get explicit key for encryption
		if(!next_encrypt_key) return NULL;
		char *result = next_encrypt_key->encrypt(plain, iv_out);
		if(result) keys.push_back(next_encrypt_key);
		else delete next_encrypt_key;
		next_encrypt_key = NULL;
		return result;
	}
	char *decrypt(const char *enc, unsigned char *iv)
	{
		char *result = NULL;
		//Attempt queued key (if present)
		if(next_decrypt_key)
		{
			if(result = next_decrypt_key->decrypt(enc, iv))
			{
				//Save successful key for later
				keys.push_back(next_decrypt_key);
			}
			else
			{
				//Destroy failed key
				delete next_decrypt_key;
			}
			next_decrypt_key = NULL;
		}
		else
		{
			//Fall back to stored keys
			for(int i = 0; i < keys.size(); i++)
			{
				if(result = keys[i]->decrypt(enc, iv)) break;
			}
		}
		return result;
	}
};
