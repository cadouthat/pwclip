/*
Simple config file parsing
by: Connor Douthat
10/31/2015
*/
bool LoadConfig(const char *path)
{
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
	}
	fclose(f);
	return true;
}
