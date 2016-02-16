/*
Implementations for VaultEntry class
by: Connor Douthat
10/1/2015
*/
//Private
void VaultEntry::clearPlaintext()
{
	if(value_plain)
	{
		//Wipe plaintext before freeing
		memset(value_plain, 0, strlen(value_plain));
		free(value_plain);
		value_plain = NULL;
	}
	memset(iv_raw, 0, sizeof(iv_raw));
}
void VaultEntry::clearValue()
{
	if(value)
	{
		free(value);
		value = NULL;
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
	clearValue();
}
bool VaultEntry::encrypt(PasswordCipher *override_key)
{
	clearValue();
	//Encrypt plaintext from clipboard
	if(!override_key) override_key = vault->key();
	if(!override_key) return false;
	value = override_key->encrypt(value_plain, iv_raw);
	//Verify encryption success
	if(!value) return false;
	//Convert IV to hex encoding
	iv = bin2hex(iv_raw, sizeof(iv_raw));
	if(!iv)
	{
		clearValue();
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
					value = strdup((const char*)sqlite3_column_text(stmt, 0));
					iv = strdup((const char*)sqlite3_column_text(stmt, 1));
				}
			}
			sqlite3_finalize(stmt);
		}
	}
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
	return (value && iv);
}
const char *VaultEntry::name()
{
	return pk;
}
const char *VaultEntry::valuePlain()
{
	return value_plain;
}
const char *VaultEntry::valuePlain(const char *set)
{
	if(value_plain)
	{
		memset(value_plain, 0, strlen(value_plain));
		free(value_plain);
	}
	value_plain = set ? strdup(set) : NULL;
	return value_plain;
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
	if(value_plain) return true;
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
	value_plain = override_key->decrypt(value, iv_raw);
	return (value_plain != NULL);
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
	if(!value_plain) return false;
	if(!encrypt()) return false;
	clearPlaintext();
	//Attempt insert
	bool insert_ok = false;
	sqlite3_stmt *stmt;
	if(vault->db())
	{
		if(SQLITE_OK == sqlite3_prepare_v2(vault->db(), "INSERT INTO `entries` (`key`, `value`, `iv`) VALUES (?, ?, ?)", -1, &stmt, NULL))
		{
			if(SQLITE_OK == sqlite3_bind_text(stmt, 1, pk, -1, SQLITE_STATIC) &&
				SQLITE_OK == sqlite3_bind_text(stmt, 2, value, -1, SQLITE_STATIC) &&
				SQLITE_OK == sqlite3_bind_text(stmt, 3, iv, -1, SQLITE_STATIC))
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
	char *value_old = value;
	char *iv_old = iv;
	value = NULL;
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
				if(SQLITE_OK == sqlite3_bind_text(stmt, 1, value, -1, SQLITE_STATIC) &&
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
		if(value_old) free(value_old);
		if(iv_old) free(iv_old);
	}
	else
	{
		//Rollback value
		clearValue();
		value = value_old;
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
