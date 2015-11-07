/*
Interaction to export raw entries
by: Connor Douthat
10/24/2015
*/
void ExportDialog()
{
	//Make sure vault is open
	OpenVaultDialog(0);
	if(!vaults.topOpen()) return;

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
	if(SQLITE_OK == sqlite3_prepare_v2(vaults.top()->db(), "SELECT `key` FROM `entries` WHERE `key`!='__meta__' ORDER BY `key`", -1, &stmt, NULL))
	{
		while(sqlite3_step(stmt) == SQLITE_ROW)
		{
			//Load entry
			const char *key = (const char*)sqlite3_column_text(stmt, 0);
			VaultEntry entry(vaults.top(), key);
			//Verify decryption
			if(entry.decrypt() && entry.valuePlain())
			{
				//Write to output
				fprintf(f_out, "%s = '%s'\n", key, entry.valuePlain());
			}
			else
			{
				ErrorBox("Failed to decrypt '%s'", key);
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
