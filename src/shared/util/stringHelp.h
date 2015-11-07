/*
Simple C string utilities
by: Connor Douthat
10/31/2015
*/
#define _is_whitespace(c) ((c) == ' ' || (c) == '	' || (c) == '\r' || (c) == '\n')

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
