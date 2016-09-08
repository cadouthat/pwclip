/*
Implementations for VaultEntry class
by: Connor Douthat
10/1/2015
*/
//Private
void VaultEntry::clearPlaintext()
{
	if(v_plaintext)
	{
		//Wipe plaintext before freeing
		memset(v_plaintext, 0, strlen(v_plaintext));
		free(v_plaintext);
		v_plaintext = NULL;
	}
	memset(iv_raw, 0, sizeof(iv_raw));
}
void VaultEntry::clearCiphertext()
{
	if(v_ciphertext)
	{
		free(v_ciphertext);
		v_ciphertext = NULL;
	}
	if(iv)
	{
		free(iv);
		iv = NULL;
	}
}
void VaultEntry::clear()
{
	clearPlaintext();
	clearCiphertext();
}
bool VaultEntry::encrypt(PasswordCipher *override_key)
{
	clearCiphertext();
	if(!v_plaintext) return false;
	//Encrypt plaintext from clipboard
	if(!override_key) override_key = vault->key();
	if(!override_key) return false;
	v_ciphertext = override_key->encrypt(v_plaintext, iv_raw);
	//Verify encryption success
	if(!v_ciphertext) return false;
	//Convert IV to hex encoding
	iv = bin2hex(iv_raw, sizeof(iv_raw));
	if(!iv)
	{
		clearCiphertext();
		ErrorBox("Failed to encode IV");
		fatal_flag = true;
		return false;
	}
	return true;
}

//Public
VaultEntry::VaultEntry(Vault *vault_in, const char *pk_in)
{
	memset(this, 0, sizeof(*this));
	vault = vault_in;
	pk = strdup(pk_in);
}
VaultEntry::~VaultEntry()
{
	clear();
	if(pk) free(pk);
}
Vault *VaultEntry::getVault()
{
	return vault;
}
bool VaultEntry::exists()
{
	bool found = false;
	//Find existing value (if any)
	sqlite3_stmt *stmt;
	if(vault->db())
	{
		if(SQLITE_OK == sqlite3_prepare_v2(vault->db(), "SELECT `value`, `iv` FROM `entries` WHERE `key`=?", -1, &stmt, NULL))
		{
			if(SQLITE_OK == sqlite3_bind_text(stmt, 1, pk, -1, SQLITE_STATIC))
			{
				if(sqlite3_step(stmt) == SQLITE_ROW)
				{
					//Keep internal copy of value/iv
					clearCiphertext();
					v_ciphertext = strdup((const char*)sqlite3_column_text(stmt, 0));
					iv = strdup((const char*)sqlite3_column_text(stmt, 1));
					found = true;
				}
			}
			sqlite3_finalize(stmt);
		}
	}
	return found;
}
const char *VaultEntry::name()
{
	return pk;
}
const char *VaultEntry::ciphertext()
{
	return v_ciphertext;
}
const char *VaultEntry::ciphertext(const char *set)
{
	if(v_ciphertext)
	{
		memset(v_ciphertext, 0, strlen(v_ciphertext));
		free(v_ciphertext);
	}
	v_ciphertext = set ? strdup(set) : NULL;
	return v_ciphertext;
}
const char *VaultEntry::plaintext()
{
	return v_plaintext;
}
const char *VaultEntry::plaintext(const char *set)
{
	clearCiphertext();
	if(v_plaintext)
	{
		memset(v_plaintext, 0, strlen(v_plaintext));
		free(v_plaintext);
	}
	v_plaintext = set ? strdup(set) : NULL;
	return v_plaintext;
}
bool VaultEntry::fatal()
{
	bool prev = fatal_flag;
	fatal_flag = false;
	return prev;
}
bool VaultEntry::decrypt(PasswordCipher *override_key)
{
	//Existing plaintext can be reused
	if(v_plaintext) return true;
	//Must have existing value to decrypt
	if(!exists())
	{
		ErrorBox("Entry not found");
		fatal_flag = true;
		return false;
	}
	//Decode IV to binary
	hex2bin(iv, iv_raw, sizeof(iv_raw));
	//Decrypt value
	if(!override_key) override_key = vault->key();
	if(!override_key) return false;
	v_plaintext = override_key->decrypt(v_ciphertext, iv_raw);
	return (v_plaintext != NULL);
}
bool VaultEntry::save()
{
	//No implicit overwrite
	if(exists())
	{
		ErrorBox("Entry already exists");
		fatal_flag = true;
		return false;
	}
	if(!v_ciphertext && !encrypt())
	{
		ErrorBox("Failed to encrypt entry for saving");
		fatal_flag = true;
		return false;
	}
	clearPlaintext();
	//Attempt insert
	bool insert_ok = false;
	sqlite3_stmt *stmt;
	if(vault->db())
	{
		if(SQLITE_OK == sqlite3_prepare_v2(vault->db(), "INSERT INTO `entries` (`key`, `value`, `iv`) VALUES (?, ?, ?)", -1, &stmt, NULL))
		{
			if(SQLITE_OK == sqlite3_bind_text(stmt, 1, pk, -1, SQLITE_STATIC) &&
				SQLITE_OK == sqlite3_bind_text(stmt, 2, v_ciphertext, -1, SQLITE_STATIC) &&
				SQLITE_OK == sqlite3_bind_text(stmt, 3, iv ? iv : "", -1, SQLITE_STATIC))
			{
				insert_ok = (sqlite3_step(stmt) == SQLITE_DONE);
			}
			sqlite3_finalize(stmt);
		}
	}
	if(!insert_ok)
	{
		//Value not saved, clear internal value
		clear();
		ErrorBox("Unknown error creating entry");
		fatal_flag = true;
	}
	return insert_ok;
}
bool VaultEntry::reEncrypt(PasswordCipher *decrypt_key, PasswordCipher *encrypt_key)
{
	//Decrypt
	if(!decrypt(decrypt_key)) return false;
	//Displace original encrypted value for now
	char *v_ciphertext_old = v_ciphertext;
	char *iv_old = iv;
	v_ciphertext = NULL;
	iv = NULL;
	//Re-encrypt with appropriate prompt
	bool result = encrypt(encrypt_key);
	clearPlaintext();
	if(result)
	{
		result = false;
		//Attempt update
		sqlite3_stmt *stmt;
		if(vault->db())
		{
			if(SQLITE_OK == sqlite3_prepare_v2(vault->db(), "UPDATE `entries` SET `value`=?, `iv`=? WHERE `key`=?", -1, &stmt, NULL))
			{
				if(SQLITE_OK == sqlite3_bind_text(stmt, 1, v_ciphertext, -1, SQLITE_STATIC) &&
					SQLITE_OK == sqlite3_bind_text(stmt, 2, iv, -1, SQLITE_STATIC) &&
					SQLITE_OK == sqlite3_bind_text(stmt, 3, pk, -1, SQLITE_STATIC))
				{
					result = (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(vault->db()));
				}
				sqlite3_finalize(stmt);
			}
		}
		if(!result)
		{
			ErrorBox("Unknown error updating entry");
			fatal_flag = true;
		}
	}
	//Cleanup
	if(result)
	{
		//Free original value
		if(v_ciphertext_old) free(v_ciphertext_old);
		if(iv_old) free(iv_old);
	}
	else
	{
		//Rollback value
		clearCiphertext();
		v_ciphertext = v_ciphertext_old;
		iv = iv_old;
	}
	return result;
}
bool VaultEntry::remove()
{
	if(!exists())
	{
		ErrorBox("Entry not found");
		fatal_flag = true;
		return false;
	}
	//Attempt delete
	bool delete_ok = false;
	sqlite3_stmt *stmt;
	if(vault->db())
	{
		if(SQLITE_OK == sqlite3_prepare_v2(vault->db(), "DELETE FROM `entries` WHERE `key`=?", -1, &stmt, NULL))
		{
			if(SQLITE_OK == sqlite3_bind_text(stmt, 1, pk, -1, SQLITE_STATIC))
			{
				delete_ok = (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(vault->db()));
			}
			sqlite3_finalize(stmt);
		}
	}
	if(!delete_ok)
	{
		ErrorBox("Unknown error removing entry");
		fatal_flag = true;
	}
	else clear();
	return delete_ok;
}
