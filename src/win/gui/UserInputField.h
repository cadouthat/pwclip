/*
Field-specific operations for UserInput
by: Connor Douthat
2/18/2016
*/
typedef void(*UserInputFieldHandler)(void *ui, int i_field);

class UserInputField
{
	int type;
	const char *label;
	HWND hwnd_owner;
	HWND hwnd_input;
	HWND hwnd_extra;
	std::wstring *input_cue;
	std::wstring *extra_cue;

	char *getText(HWND hwnd)
	{
		int size = GetWindowTextLength(hwnd) + 1;
		char *str = (char*)malloc(size);
		GetWindowText(hwnd, str, size);
		return str;
	}

public:
	UserInputFieldHandler handler;

	UserInputField(int type_in, const char *label_in)
	{
		memset(this, 0, sizeof(*this));
		type = type_in;
		label = label_in;
	}
	~UserInputField()
	{
		if(input_cue) delete input_cue;
		if(extra_cue) delete extra_cue;
	}
	void init(HWND owner, int &next_id, HFONT font)
	{
		hwnd_owner = owner;
		bool isButton = (type == UIF_TOGGLE || type == UIF_BUTTON);
		DWORD styleEx = isButton ? 0 : WS_EX_CLIENTEDGE;
		const char *classname = isButton ? "BUTTON" : "EDIT";
		const char *winText = isButton ? label : "";
		DWORD style = 0;
		if(type == UIF_TOGGLE) style |= BS_AUTOCHECKBOX;
		//Window creation
		hwnd_input = CreateWindowEx(styleEx, classname, winText,
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | style,
			0, 0, 0, 0,
			owner, (HMENU)next_id++, NULL, NULL);
		PostMessage(hwnd_input, WM_SETFONT, (WPARAM)font, true);
		switch(type)
		{
		case UIF_NEWPASS:
			hwnd_extra = CreateWindowEx(styleEx, classname, winText,
				WS_CHILD | WS_VISIBLE | WS_TABSTOP | style,
				0, 0, 0, 0,
				owner, (HMENU)next_id++, NULL, NULL);
			PostMessage(hwnd_extra, WM_SETFONT, (WPARAM)font, true);
			break;
		case UIF_UINT:
			hwnd_extra = CreateWindowEx(0, "STATIC", label,
				WS_CHILD | WS_VISIBLE,
				0, 0, 0, 0,
				owner, (HMENU)next_id++, NULL, NULL);
			PostMessage(hwnd_extra, WM_SETFONT, (WPARAM)font, true);
			break;
		}
		//Cue text
		if(type == UIF_TEXT || type == UIF_OLDPASS || type == UIF_NEWPASS)
		{
			//Convert to unicode
			std::string tmp(label);
			if(input_cue) delete input_cue;
			input_cue = new std::wstring(tmp.begin(), tmp.end());
			//Set cue
			PostMessage(hwnd_input, EM_SETCUEBANNER, (WPARAM)true, (LPARAM)input_cue->c_str());
			if(type == UIF_NEWPASS)
			{
				tmp = std::string("Confirm ") + label;
				if(extra_cue) delete extra_cue;
				extra_cue = new std::wstring(tmp.begin(), tmp.end());
				PostMessage(hwnd_extra, EM_SETCUEBANNER, (WPARAM)true, (LPARAM)extra_cue->c_str());
			}
		}
		//Password masks
		if(type == UIF_OLDPASS || type == UIF_NEWPASS)
		{
			PostMessage(hwnd_input, EM_SETPASSWORDCHAR, 0x95, 0);
			if(type == UIF_NEWPASS)
			{
				PostMessage(hwnd_extra, EM_SETPASSWORDCHAR, 0x95, 0);
			}
		}
	}
	int position(int x, int y, int w, int spacing)
	{
		int w_small = 40;
		int h = 24;
		int y_next = y + h + spacing;
		switch(type)
		{
		case UIF_UINT:
			SetWindowPos(hwnd_extra, NULL, x, y, w - w_small - 20, h,  SWP_NOZORDER);
			SetWindowPos(hwnd_input, NULL, x + w - w_small, y, w_small, h,  SWP_NOZORDER);
			return y_next;
		case UIF_BUTTON:
			w /= 2;
			break;
		case UIF_NEWPASS:
			SetWindowPos(hwnd_extra, NULL, x, y_next, w, h, SWP_NOZORDER);
			y_next += h + spacing;
			break;
		}
		SetWindowPos(hwnd_input, NULL, x, y, w, h, SWP_NOZORDER);
		return y_next;
	}
	bool owns(HWND hwnd)
	{
		if(hwnd)
		{
			if(hwnd == hwnd_input) return true;
			if(hwnd == hwnd_extra) return true;
		}
		return false;
	}
	void focus()
	{
		SetFocus(hwnd_input);
	}
	const char *error()
	{
		if(!hwnd_input) return NULL;
		char istr[32] = {0};
		switch(type)
		{
		case UIF_NEWPASS:
		{
			if(!hwnd_extra) return NULL;
			//Get values
			char *val_input = getText(hwnd_input);
			char *val_extra = getText(hwnd_extra);
			//Compare
			bool is_empty = !(*val_input);
			bool is_match = !strcmp(val_input, val_extra);
			//Cleanup values
			memset(val_input, 0, strlen(val_input));
			memset(val_extra, 0, strlen(val_extra));
			free(val_input);
			free(val_extra);
			//Passwords must match
			if(is_empty) return "Please enter a value";
			if(!is_match) return "Passwords do not match";
			break;
		}
		case UIF_UINT:
			if(GetWindowTextLength(hwnd_input) < sizeof(istr))
			{
				memset(istr, 0, sizeof(istr));
				GetWindowText(hwnd_input, istr, sizeof(istr));
				for(int i = 0; i < strlen(istr); i++)
				{
					if(istr[i] < '0' || istr[i] > '9')
					{
						istr[0] = 0;
						break;
					}
				}
			}
			if(!istr[0]) return "Invalid number";
			break;
		}
		return NULL;
	}
	void value(const void *valueIn)
	{
		if(!hwnd_input) return;
		const char *valueInStr = valueIn ? (const char*)valueIn : "";
		char istr[32] = {0};
		switch(type)
		{
		case UIF_NEWPASS:
			SetWindowText(hwnd_extra, valueInStr);
		case UIF_OLDPASS:
		case UIF_TEXT:
			SetWindowText(hwnd_input, valueInStr);
			break;
		case UIF_UINT:
			sprintf(istr, "%d", *((int*)valueIn));
			SetWindowText(hwnd_input, istr);
			break;
		case UIF_TOGGLE:
			PostMessage(hwnd_input, BM_SETCHECK, *((bool*)valueIn) ? BST_CHECKED : BST_UNCHECKED, 0);
			break;
		case UIF_BUTTON:
			handler = (UserInputFieldHandler)valueIn;
			break;
		}
		//For text-based inputs, move cursor to the end
		if(type != UIF_TOGGLE && type != UIF_BUTTON)
		{
			int len = GetWindowTextLength(hwnd_input);
			PostMessage(hwnd_input, EM_SETSEL, len, len);
		}
	}
	char *stringValue()
	{
		if(!hwnd_input) return NULL;
		return getText(hwnd_input);
	}
	int uintValue()
	{
		if(!hwnd_input) return 0;
		char str[32] = {0};
		GetWindowText(hwnd_input, str, sizeof(str));
		int i = atoi(str);
		if(i < 0) i = 0;
		return i;
	}
	bool boolValue()
	{
		if(!hwnd_input) return false;
		return (SendMessage(hwnd_input, BM_GETCHECK, 0, 0) == BST_CHECKED);
	}
};
