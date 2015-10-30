/*
Interaction to export raw entries
by: Connor Douthat
10/24/2015
*/
void ExportDialog()
{
	//Prompt for output path
	char out_path[256] = {0};
	if(!BrowseForOutput(out_path)) return;

	//Open output file
	FILE *f_out = fopen(out_path, "w");
	if(!f_out)
	{
		ErrorBox("Failed to open '%s'", out_path);
		return;
	}

	//Perform dump
	bool result = true;
	sqlite3_stmt *stmt;
	if(SQLITE_OK == sqlite3_prepare_v2(db.loaded, "SELECT `key` FROM `entries` ORDER BY `key`", -1, &stmt, NULL))
	{
		char prompt_text[512] = {0};
		UserInput prompt(UIF_OLDPASS, "Raw Export");
		while(sqlite3_step(stmt) == SQLITE_ROW)
		{
			const char *key = (const char*)sqlite3_column_text(stmt, 0);
			PWClipEntry entry(&crypto_keys, db.loaded, key);
			bool first_prompt = true;
			//Decrypt until success or fatal error
			while(!entry.decrypt() && !entry.fatal())
			{
				//User needs to know entry being decrypted
				snprintf(prompt_text, sizeof(prompt_text), "Current entry: '%s'", key);
				prompt.setInfo(prompt_text);
				//Failure feedback
				if(!first_prompt)
				{
					prompt.setError("Decryption failed, please try again.");
				}
				else first_prompt = false;
				//Prompt for new password
				if(prompt.get())
				{
					crypto_keys.nextDecrypt(new PasswordCipher(prompt.oldpass()));
				}
				else break;
			}
			//Write to output
			if(entry.valuePlain())
			{
				fprintf(f_out, "%s = '%s'\n", key, entry.valuePlain());
			}
			else
			{
				result = false;
				break;
			}
		}
		sqlite3_finalize(stmt);
	}

	//Cleanup
	if(result)
	{
		fclose(f_out);
		TrayBalloon("Export complete.");
	}
	else
	{
		//Attempt to wipe file (not guaranteed)
		long fs = ftell(f_out);
		fseek(f_out, 0, SEEK_SET);
		char buf[2048] = {0};
		long written = 0;
		while(written < fs) written += fwrite(buf, 1, sizeof(buf), f_out);
		//Close and delete
		fclose(f_out);
		remove(out_path);
	}
}
