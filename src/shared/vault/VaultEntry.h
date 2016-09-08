/*
VaultEntry class which acts on a password vault entry
by: Connor Douthat
10/1/2015
*/
class Vault;

class VaultEntry
{
	Vault *vault;
	bool fatal_flag;
	char *pk;
	char *v_ciphertext;
	char *iv;
	char *v_plaintext;
	unsigned char iv_raw[CRYPTO_BLOCK_SIZE];

	void clearPlaintext();
	void clearCiphertext();
	void clear();
	bool encrypt(PasswordCipher *override_key = NULL);

public:
	VaultEntry(Vault *vault_in, const char *pk_in);
	~VaultEntry();
	Vault *getVault();
	bool exists();
	const char *name();
	const char *ciphertext();
	const char *ciphertext(const char *set);
	const char *plaintext();
	const char *plaintext(const char *set);
	bool fatal();
	bool decrypt(PasswordCipher *override_key = NULL);
	bool save();
	bool reEncrypt(PasswordCipher *decrypt_key, PasswordCipher *encrypt_key);
	bool remove();
};
