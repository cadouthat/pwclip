/*
Constant definitions
by: Connor Douthat
10/3/2015
*/
#define APPDATA_NAME "pwclip"
#define REG_RUN_KEY "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"

#define CRYPTO_KEY_SIZE 32
#define CRYPTO_SALT_SIZE 16
#define CRYPTO_BLOCK_SIZE 16
#define CRYPTO_ITERATIONS 1500

#define MAX_DB_HIST 10

#define SCHEMA_SQL "CREATE TABLE `entries` (`key` VARCHAR(50) PRIMARY KEY, `value` TEXT NOT NULL, `iv` TEXT NOT NULL)"

#define FILE_TYPE_DB 1
#define FILE_TYPE_TXT 2
