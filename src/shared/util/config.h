/*
Simple config file parsing
by: Connor Douthat
10/31/2015
*/
bool LoadConfig(const char *path)
{
	//Default values
	clip_wipe_delay = 10;
	generate_length = 16;
	generate_include_symbols = false;
	generate_echo = true;

	FILE *f = fopen(path, "r");
	if(!f) return false;
	char *value, line[256] = {0};
	while(fgets(line, sizeof(line), f))
	{
		trim(line);
		//Skip comments and section declarations
		if(*line == '#' || *line == '[') continue;
		//Isolate key/value
		if(!(value = strchr(line, '='))) continue;
		*(value++) = 0;
		trim(line);
		trim(value);
		//Process
		if(!stricmp(line, "clip_wipe_delay"))
		{
			clip_wipe_delay = atoi(value);
		}
		if(!stricmp(line, "generate_length"))
		{
			int iValue = atoi(value);
			if(iValue > 0) generate_length = iValue;
		}
		if(!stricmp(line, "generate_include_symbols"))
		{
			generate_include_symbols = atoi(value) > 0;
		}
		if(!stricmp(line, "generate_echo"))
		{
			generate_echo = atoi(value) > 0;
		}
	}
	fclose(f);
	return true;
}
