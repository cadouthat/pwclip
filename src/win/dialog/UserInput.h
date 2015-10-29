/*
Helper class for mixed user input
by: Connor Douthat
10/28/2015
*/
#define UIF_GENERATE 1
#define UIF_NAME 2
#define UIF_OLDPASS 4
#define UIF_NEWPASS 8

class UserInput
{
	//Configuration
	int flags;
	const char *title;
	const char *info_text;
	const char *error_text;

	//Window state
	bool window_open;

	//Output
	bool val_generate;
	char val_name[ENTRY_NAME_MAX + 1];
	char val_oldpass[PASSWORD_MAX + 1];
	char val_newpass[PASSWORD_MAX + 1];

	bool open()
	{
		//
		window_open = true;
		return true;
	}
	void close()
	{
		//
		window_open = false;
	}

public:
	UserInput(int flags_in, const char *title_in)
	{
		memset(this, 0, sizeof(*this));
		flags = flags_in;
		title = title_in;
	}
	~UserInput()
	{
		if(window_open) close();
		memset(this, 0, sizeof(*this));
	}
	bool generate() { return val_generate; }
	const char *name() { return val_name; }
	const char *oldpass() { return val_oldpass; }
	const char *newpass() { return val_newpass; }
	void setInfo(const char *text)
	{
		info_text = text;
		if(window_open)
		{
			//update
		}
	}
	void setError(const char *text)
	{
		error_text = text;
		if(window_open)
		{
			//update
		}
	}
	bool get()
	{
		if(!window_open)
		{
			//Open window for the first time
			if(!open()) return false;
		}
		else
		{
			//Re-focus password input
			if(flags & UIF_OLDPASS)
			{
				//
			}
		}

		//message loop

		return true;
	}
};
