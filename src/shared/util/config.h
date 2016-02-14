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
	password_preview = true;

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
		if(!stricmp(line, "password_preview"))
		{
			password_preview = atoi(value) > 0;
		}
	}
	fclose(f);
	return true;
}
bool SaveConfig(const char *path)
{
	FILE *f = fopen(path, "w");
	if(!f) return false;
	fprintf(f, "clip_wipe_delay = %d\n", clip_wipe_delay);
	fprintf(f, "generate_length = %d\n", generate_length);
	fprintf(f, "generate_include_symbols = %d\n", generate_include_symbols ? 1 : 0);
	fprintf(f, "password_preview = %d\n", password_preview ? 1 : 0);
	fclose(f);
	return true;
}
