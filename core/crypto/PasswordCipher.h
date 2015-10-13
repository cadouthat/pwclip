/*
Symmetric cryptography with password-based key
by: Connor Douthat
10/4/2015
*/
#define CRYPTO_VERIFY_PREFIX "/Plaintext warning!/"

class PasswordCipher
{
	unsigned char key[CRYPTO_KEY_SIZE];

	char *raw_encrypt(const char *plain, unsigned char *iv_out)
	{
		//Generate IV
		if(!RAND_bytes(iv_out, CRYPTO_BLOCK_SIZE)) return NULL;
		char *enc = NULL;
		EVP_CIPHER_CTX *ctx;
		//Create and init cipher
		if(!(ctx = EVP_CIPHER_CTX_new())) return NULL;
		if(1 == EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv_out))
		{
			//Encrypt and finalize
			int plain_len = strlen(plain);
			int enc_raw_len = plain_len + CRYPTO_BLOCK_SIZE;
			int enc_raw_extra;
			unsigned char *enc_raw = (unsigned char*)malloc(enc_raw_len);
			if(1 == EVP_EncryptUpdate(ctx, enc_raw, &enc_raw_len, (const unsigned char*)plain, plain_len) &&
				1 == EVP_EncryptFinal_ex(ctx, enc_raw + enc_raw_len, &enc_raw_extra))
			{
				enc_raw_len += enc_raw_extra;
				//Copy result in hex encoding
				enc = bin2hex(enc_raw, enc_raw_len);
			}
			free(enc_raw);
		}
		EVP_CIPHER_CTX_free(ctx);
		return enc;
	}
	char *raw_decrypt(const char *enc, unsigned char *iv)
	{
		unsigned char *plain = NULL;
		EVP_CIPHER_CTX *ctx;
		//Create and init cipher
		if(!(ctx = EVP_CIPHER_CTX_new())) return NULL;
		if(1 == EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
		{
			//Decode hex to binary
			int enc_raw_len;
			unsigned char *enc_raw = hex2bin(enc, &enc_raw_len);
			if(enc_raw)
			{
				//Decrypt and finalize
				plain = (unsigned char*)malloc(enc_raw_len + CRYPTO_BLOCK_SIZE + 1);
				int plain_len, plain_extra;
				if(1 == EVP_DecryptUpdate(ctx, plain, &plain_len, enc_raw, enc_raw_len) &&
					1 == EVP_DecryptFinal_ex(ctx, plain + plain_len, &plain_extra))
				{
					plain_len += plain_extra;
					//Truncate result string
					plain[plain_len] = 0;
				}
				else
				{
					free(plain);
					plain = NULL;
				}
				free(enc_raw);
			}
		}
		EVP_CIPHER_CTX_free(ctx);
		return (char*)plain;
	}

public:
	PasswordCipher(const char *pass)
	{
		memset(key, 0, sizeof(key));
		//Generate key via PBKDF2-SHA256, with no salt (we need the same key every time)
		PKCS5_PBKDF2_HMAC(pass, -1, NULL, 0, CRYPTO_ITERATIONS, EVP_sha256(), sizeof(key), key);
	}
	~PasswordCipher()
	{
		//Wipe key
		memset(key, 0, sizeof(key));
	}
	char *encrypt(const char *plain, unsigned char *iv_out)
	{
		//Allocate space for prefixed string
		int prefix_len = strlen(CRYPTO_VERIFY_PREFIX);
		int plain_len = strlen(plain);
		char *prefixed = (char*)malloc(prefix_len + plain_len + 1);
		//Copy prefix and plaintext
		strcpy(prefixed, CRYPTO_VERIFY_PREFIX);
		strcat(prefixed, plain);
		//Encrypt prefixed string
		char *enc = raw_encrypt(prefixed, iv_out);
		//Wipe and free prefixed string
		memset(prefixed, 0, prefix_len + plain_len);
		free(prefixed);
		return enc;
	}
	char *decrypt(const char *enc, unsigned char *iv)
	{
		//Decrypt prefixed string
		char *prefixed = raw_decrypt(enc, iv);
		if(!prefixed) return NULL;
		//Verify correct prefix
		int prefix_len = strlen(CRYPTO_VERIFY_PREFIX);
		int plain_len = strlen(prefixed) - prefix_len;
		char *plain = NULL;
		if(plain_len > 0 && !memcmp(prefixed, CRYPTO_VERIFY_PREFIX, prefix_len))
		{
			//Allocate and extract plaintext
			plain = (char*)malloc(plain_len + 1);
			strcpy(plain, prefixed + prefix_len);
		}
		//Wipe and free prefixed string
		memset(prefixed, 0, prefix_len + plain_len);
		free(prefixed);
		return plain;
	}
};
