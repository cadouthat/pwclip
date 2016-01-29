/*
Cryptography helper functions
by: Connor Douthat
10/4/2015
*/
char *bin2hex(const unsigned char *raw, int len)
{
	char *hex = (char*)malloc(len * 2 + 1);
	hex[len * 2] = 0;
	for(int i = 0; i < len; i++)
	{
		sprintf(hex + i * 2, "%02hhX", raw[i]);
	}
	return hex;
}
int hex2bin(const char *hex, unsigned char *raw_out, int raw_max)
{
	int hex_len = strlen(hex);
	if(!hex_len || hex_len % 2) return 0;
	int raw_len = hex_len / 2;
	if(raw_len > raw_max) raw_len = raw_max;
	char sub[3] = {0};
	unsigned int x;
	for(int i = 0; i < raw_len; i++)
	{
		memcpy(sub, hex + i * 2, 2);
		sscanf(sub, "%X", &x);
		raw_out[i] = (x & 0xFF);
	}
	return raw_len;
}
unsigned char *hex2bin(const char *hex, int *len_out)
{
	int raw_len = strlen(hex) / 2;
	if(!raw_len) return NULL;
	unsigned char *raw = (unsigned char*)malloc(raw_len);
	*len_out = hex2bin(hex, raw, raw_len);
	return raw;
}
bool RandText(char *out, int len)
{
	const char *char_set = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
	int char_set_size = strlen(char_set);
	unsigned char *u_out = (unsigned char*)out;
	if(!RAND_bytes(u_out, len)) return false;
	for(int i = 0; i < len; i++)
	{
		out[i] = char_set[u_out[i] % char_set_size];
	}
	return true;
}
