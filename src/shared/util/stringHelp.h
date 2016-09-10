/*
Simple C string utilities
by: Connor Douthat
10/31/2015
*/
#define _is_whitespace(c) ((c) == ' ' || (c) == '	' || (c) == '\r' || (c) == '\n')

const char *stristr(const char *haystack, const char *needle)
{
	if(*needle == 0) return haystack;
	while(*haystack)
	{
		const char *haytmp = haystack;
		const char *neetmp = needle;
		while(tolower(*haytmp) == tolower(*neetmp))
		{
			haytmp++;
			neetmp++;
			if(*neetmp == 0) return haystack;
			if(*haytmp == 0) return NULL;
		}
		haystack++;
	}
	return NULL;
}

void trim(char *buf)
{
	//Locate first and last non-whitespace
	int first_data = -1;
	int last_data = -1;
	for(int i = 0; *(buf + i); i++)
	{
		if(!_is_whitespace(*(buf + i)))
		{
			if(first_data < 0) first_data = i;
			last_data = i;
		}
	}
	//Overwrite leading whitespace
	int len = 0;
	if(last_data >= 0)
	{
		len = last_data - first_data + 1;
		memcpy(buf, buf + first_data, len);
	}
	//Truncate the rest
	buf[len] = 0;
}

void stringPreview(const char *str, char *out, int out_size)
{
	int len = strlen(str);
	if(len < out_size)
	{
		strcpy(out, str);
	}
	else
	{
		strncpy(out, str, out_size - 4);
		out[out_size - 4] = 0;
		strcat(out, "...");
	}
	//Strip out endlines
	int out_len = strlen(out);
	int i_write = 0;
	for(int i = 0; i < out_len; i++)
	{
		if(out[i] != '\r' && out[i] != '\n')
		{
			out[i_write++] = out[i];
		}
	}
	out[i_write] = 0;
}
