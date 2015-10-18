/*
Dumping tool for pwclip (pwclipdump)
by: Connor Douthat
10/15/2015
*/
#include "../core/sysIncludes.h"
#include "includes.h"

int main(int argc, char **argv)
{
	//Requires output file at minimum
	if(argc <= 1)
	{
		printf("%s\n", USAGE_TEXT);
		return 1;
	}

	//Setup
	if(!ProcessArguments(argc, argv)) return 1;
	if(FileExists(argv[1]))
	{
		//Disable overwite to prevent accidentally erasing database
		printf("File already exists (safety feature): '%s'\n", argv[1]);
		return 1;
	}
	FILE *f_out = fopen(argv[1], "w");
	if(!f_out)
	{
		printf("Failed to open '%s'\n", argv[1]);
		return 1;
	}
	if(!OpenDB()) return 1;

	//Perform dump
	sqlite3_stmt *stmt;
	if(SQLITE_OK == sqlite3_prepare_v2(db, "SELECT `key`, `value`, `iv` FROM `entries` ORDER BY `key`", -1, &stmt, NULL))
	{
		KeyManager *crypto = new KeyManager();
		while(sqlite3_step(stmt) == SQLITE_ROW)
		{
			const char *key = (const char*)sqlite3_column_text(stmt, 0);
			const char *value = (const char*)sqlite3_column_text(stmt, 1);
			const char *iv = (const char*)sqlite3_column_text(stmt, 2);
			//Decode IV to binary
			unsigned char iv_raw[CRYPTO_BLOCK_SIZE];
			hex2bin(iv, iv_raw, sizeof(iv_raw));
			//Determine password and decrypt
			printf("Processing entry '%s'\n", key);
			char *value_plain = crypto->decrypt(value, iv_raw);
			if(!value_plain) break;
			//Write to output
			fprintf(f_out, "%s = '%s'\n", key, value_plain);
			//Wipe and free plaintext
			memset(value_plain, 0, strlen(value_plain));
			free(value_plain);
		}
		sqlite3_finalize(stmt);
		//Cleanup crypto (destroy keys)
		delete crypto;
	}

	//Cleanup
	CloseDB();
	fclose(f_out);
	return 0;
}
