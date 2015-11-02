/*
Constant definitions
by: Connor Douthat
10/3/2015
*/
#define APPDATA_NAME "pwclip"

#define CRYPTO_KEY_SIZE 32
#define CRYPTO_BLOCK_SIZE 16
#define CRYPTO_ITERATIONS 1500

#define PASSWORD_MAX 255
#define ENTRY_NAME_MAX 50

#define MAX_DB_HIST 10

#define GEN_PASS_SIZE 16

#define SCHEMA_SQL "CREATE TABLE `entries` (`key` VARCHAR(50) PRIMARY KEY, `value` TEXT NOT NULL, `iv` TEXT NOT NULL)"
