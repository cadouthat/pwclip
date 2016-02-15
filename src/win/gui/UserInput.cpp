/*
User input prompt (windows implementations)
by: Connor Douthat
10/28/2015
*/
#define UIN_WIDTH 400
#define UIN_DEF_HEIGHT 200
#define UIN_PAD_X 30
#define UIN_PAD_Y 12
#define UIN_BUTTON_WIDTH 80
#define UIN_BUTTON_HEIGHT 26

enum { UIN_ID_INFO = 1, UIN_ID_ERROR, UIN_ID_OKAY, UIN_ID_CANCEL, UIN_ID_DYNAMIC };

class UserInput
{
public:
	class Field
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
		Field(int type_in, const char *label_in)
		{
			memset(this, 0, sizeof(*this));
			type = type_in;
			label = label_in;
		}
		~Field()
		{
			if(input_cue) delete input_cue;
			if(extra_cue) delete extra_cue;
		}
		void init(HWND owner, int &next_id, HFONT font)
		{
			hwnd_owner = owner;
			bool isButton = (type == UIF_TOGGLE);
			DWORD styleEx = isButton ? 0 : WS_EX_CLIENTEDGE;
			const char *classname = isButton ? "BUTTON" : "EDIT";
			const char *winText = isButton ? label : "";
			DWORD style = isButton ? BS_AUTOCHECKBOX : 0;
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
				break;
			case UIF_NEWPASS:
				SetWindowPos(hwnd_extra, NULL, x, y_next, w, h, SWP_NOZORDER);
				y_next += h + spacing;
				//Continue to default
			default:
				SetWindowPos(hwnd_input, NULL, x, y, w, h, SWP_NOZORDER);
			}
			return y_next;
		}
		void focus()
		{
			SetFocus(hwnd_input);
			if(type != UIF_TOGGLE)
			{
				int len = GetWindowTextLength(hwnd_input);
				PostMessage(hwnd_input, EM_SETSEL, len, len);
			}
		}
		const char *error()
		{
			if(!hwnd_input) return NULL;
			char istr[32] = {0};
			switch(type)
			{
			case UIF_TEXT:
				GetWindowText(hwnd_input, istr, sizeof(istr));
				if(!istr[0]) return "Please enter a value";
				break;
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
				if(!is_match) return "Passwords do not match";
				if(is_empty)
				{
					//Warning for blank passwords
					if(IDYES != MessageBox(hwnd_owner, "Password is blank, encryption will not be secure. Are you sure you want to proceed?", "Confirm Empty Password", MB_YESNO))
					{
						return "";
					}
				}
				break;
			}
			case UIF_UINT:
				if(GetWindowTextLength(hwnd_input) < sizeof(istr))
				{
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
		void value(void *valueIn)
		{
			if(!hwnd_input) return;
			char istr[32] = {0};
			switch(type)
			{
			case UIF_TEXT:
				SetWindowText(hwnd_input, (const char*)valueIn);
				break;
			case UIF_UINT:
				sprintf(istr, "%d", *((int*)valueIn));
				SetWindowText(hwnd_input, istr);
				break;
			case UIF_TOGGLE:
				bool toggled = *((bool*)valueIn);
				PostMessage(hwnd_input, BM_SETCHECK, toggled ? BST_CHECKED : BST_UNCHECKED, 0);
				break;
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

private:
	//Global
	static const char *class_name;
	static bool class_init;

	//Configuration
	const char *title;
	char *info_text;
	char *error_text;
	std::vector<Field*> fields;

	//Window state
	bool okay_flag;
	HFONT font;
	HWND hwnd_top;
	int next_child_id;
	int title_height;

	void UpdateLayout()
	{
		RECT cr;
		GetClientRect(hwnd_top, &cr);
		if(!title_height) title_height = UIN_DEF_HEIGHT - (cr.bottom - cr.top);
		int w = (UIN_WIDTH - UIN_PAD_X * 2);
		int h = 24;
		int x = cr.left + (cr.right - cr.left - w) / 2;
		int y = UIN_PAD_Y;
		int spacing_static = 26;
		int spacing_edit = 12;
		if(info_text && *info_text)
		{
			SetWindowPos(GetDlgItem(hwnd_top, UIN_ID_INFO), NULL, x, y, w, h, SWP_NOZORDER);
			y += spacing_static;
		}
		if(error_text && *error_text)
		{
			SetWindowPos(GetDlgItem(hwnd_top, UIN_ID_ERROR), NULL, x, y, w, h, SWP_NOZORDER);
			y += spacing_static;
		}
		for(int i = 0; i < fields.size(); i++)
		{
			y = fields[i]->position(x, y, w, spacing_edit);
		}
		w = UIN_BUTTON_WIDTH;
		h = UIN_BUTTON_HEIGHT;
		x = cr.right - UIN_PAD_X - UIN_BUTTON_WIDTH * 2 - 10;
		SetWindowPos(GetDlgItem(hwnd_top, UIN_ID_OKAY), NULL, x, y, w, h, SWP_NOZORDER);
		x += UIN_BUTTON_WIDTH + 10;
		SetWindowPos(GetDlgItem(hwnd_top, UIN_ID_CANCEL), NULL, x, y, w, h, SWP_NOZORDER);
		y += UIN_BUTTON_HEIGHT + UIN_PAD_Y;
		SetWindowPos(hwnd_top, NULL, 0, 0, UIN_WIDTH, y + title_height, SWP_NOMOVE | SWP_NOZORDER);
		InvalidateRect(hwnd_top, NULL, true);
	}
	HWND child(int id, const char *text, const char *cn, DWORD st = 0, DWORD ex = 0)
	{
		HWND hwnd_sub = CreateWindowEx(ex, cn,
				text,
				WS_CHILD | WS_VISIBLE | st,
				0, 0, 0, 0,
				hwnd_top, (HMENU)id, NULL, NULL);
		PostMessage(hwnd_sub, WM_SETFONT, (WPARAM)font, true);
		return hwnd_sub;
	}
	bool open()
	{
		HMODULE hModule = GetModuleHandle(NULL);
		if(!class_init)
		{
			//Register class for notifications
			WNDCLASSEX wnd_class;
			ZeroMemory(&wnd_class, sizeof(wnd_class));
			wnd_class.cbSize = sizeof(wnd_class);
			wnd_class.lpfnWndProc = UserInput::StaticMessageProc;
			wnd_class.hInstance = hModule;
			wnd_class.hIcon = LoadIcon(hModule, "MAINICON");
			wnd_class.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
			wnd_class.lpszClassName = class_name;
			if(!RegisterClassEx(&wnd_class)) return false;
			class_init = true;
		}
		//Load font
		font = CreateFont(16, 0,
			0, 0,
			FW_NORMAL,
			false,
			false,
			false,
			ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			PROOF_QUALITY,
			FF_DONTCARE,
			"Arial");
		if(!font) return false;
		//Create main window
		hwnd_top = CreateWindowEx(0, class_name,
			title,
			WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT,
			UIN_WIDTH, UIN_DEF_HEIGHT,
			hwnd_main, NULL, hModule, NULL);
		if(!hwnd_top) return false;
		title_height = 0;
		PostMessage(hwnd_top, WM_SETFONT, (WPARAM)font, true);
		SetWindowLongPtr(hwnd_top, GWL_USERDATA, (long)this);
		//Create controls
		child(UIN_ID_INFO, info_text, "STATIC");
		child(UIN_ID_ERROR, error_text, "STATIC");
		child(UIN_ID_OKAY, "Okay", "BUTTON", WS_TABSTOP | BS_DEFPUSHBUTTON);
		child(UIN_ID_CANCEL, "Cancel", "BUTTON", WS_TABSTOP | BS_PUSHBUTTON);
		//Finalize
		UpdateLayout();
		return true;
	}
	void close()
	{
		if(hwnd_top)
		{
			DestroyWindow(hwnd_top);
			hwnd_top = NULL;
		}
		if(font)
		{
			DeleteObject(font);
			font = NULL;
		}
	}
	LRESULT MessageProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch(msg)
		{
		case WM_COMMAND:
			switch(HIWORD(wParam))
			{
			case BN_CLICKED:
				switch(LOWORD(wParam))
				{
				case UIN_ID_OKAY:
					int i_error;
					const char *error;
					error = NULL;
					for(i_error = 0; i_error < fields.size(); i_error++)
					{
						error = fields[i_error]->error();
						if(error) break;
					}
					if(error)
					{
						okay_flag = false;
						if(*error) setError(error, i_error);
					}
					else okay_flag = true;
					break;
				case UIN_ID_CANCEL:
				case IDCANCEL:
					DestroyWindow(hwnd_top);
					break;
				}
				break;
			}
			break;
		case WM_CTLCOLORSTATIC:
		{
			HWND hwnd_sub = (HWND)lParam;
			HDC hdc = (HDC)wParam;
			HBRUSH hbr = GetSysColorBrush(COLOR_BTNFACE);
			LOGBRUSH lb;
			GetObject(hbr, sizeof(lb), &lb);
			if(hwnd_sub == GetDlgItem(hwnd_top, UIN_ID_ERROR))
			{
				SetTextColor(hdc, RGB(240, 50, 50));
			}
			SetBkColor(hdc, lb.lbColor);
			return (LRESULT)hbr;
			break;
		}
		case DM_GETDEFID:
			return MAKELPARAM(UIN_ID_OKAY, DC_HASDEFID);
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		return 0;
	}
	static LRESULT CALLBACK StaticMessageProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		UserInput *obj = (UserInput*)GetWindowLong(hwnd, GWL_USERDATA);
		if(obj) return obj->MessageProc(hwnd, msg, wParam, lParam);
		else return DefWindowProc(hwnd, msg, wParam, lParam);
	}

public:
	UserInput(const char *title_in)
	{
		title = title_in;
		info_text = NULL;
		error_text = NULL;
		okay_flag = false;
		font = NULL;
		hwnd_top = NULL;
		next_child_id = UIN_ID_DYNAMIC;
		title_height = 0;
	}
	~UserInput()
	{
		for(int i = 0; i < fields.size(); i++)
		{
			delete fields[i];
		}
		if(info_text) free(info_text);
		if(error_text) free(error_text);
		if(hwnd_top) close();
		memset(this, 0, sizeof(*this));
	}
	void setInfo(const char *text)
	{
		if(info_text) free(info_text);
		info_text = strdup(text);
		if(hwnd_top)
		{
			SetWindowText(GetDlgItem(hwnd_top, UIN_ID_INFO), text);
			UpdateLayout();
		}
	}
	void setError(const char *text, int for_field = 0)
	{
		if(error_text) free(error_text);
		error_text = strdup(text);
		if(hwnd_top)
		{
			SetWindowText(GetDlgItem(hwnd_top, UIN_ID_ERROR), text);
			UpdateLayout();
		}
		if(for_field < fields.size())
		{
			fields[for_field]->focus();
		}
	}
	bool get()
	{
		if(!hwnd_top) return false;

		//Message loop
		MSG msg;
		okay_flag = false;
		while(!okay_flag && GetMessage(&msg, hwnd_top, 0, 0) > 0)
		{
			//Process message
			if(!IsDialogMessage(hwnd_top, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		return okay_flag;
	}
	void addField(int type, const char *text, void *valueIn)
	{
		if(!hwnd_top)
		{
			//Open window for the first time
			if(!open())
			{
				ErrorBox("Unknown GUI error");
				return;
			}
		}

		Field *f = new Field(type, text);
		f->init(hwnd_top, next_child_id, font);
		if(valueIn) f->value(valueIn);
		if(!fields.size()) f->focus();
		fields.push_back(f);

		UpdateLayout();
	}
	Field *getField(int index)
	{
		return fields[index];
	}
};

const char *UserInput::class_name = "UserInputMain";
bool UserInput::class_init = false;

void *UserInput_new(const char *title_in)
{
	return new UserInput(title_in);
}
void UserInput_delete(void *ui)
{
	delete (UserInput*)ui;
}
void UserInput_addField(void *ui, int type, const char *text, void *valueIn)
{
	((UserInput*)ui)->addField(type, text, valueIn);
}
void UserInput_setInfo(void *ui, const char *text)
{
	((UserInput*)ui)->setInfo(text);
}
void UserInput_setError(void *ui, const char *text)
{
	((UserInput*)ui)->setError(text);
}
bool UserInput_get(void *ui)
{
	return ((UserInput*)ui)->get();
}
char *UserInput_stringValue(void *ui, int index)
{
	UserInput::Field *f = ((UserInput*)ui)->getField(index);
	return f->stringValue();
}
bool UserInput_boolValue(void *ui, int index)
{
	UserInput::Field *f = ((UserInput*)ui)->getField(index);
	return f->boolValue();
}
int UserInput_uintValue(void *ui, int index)
{
	UserInput::Field *f = ((UserInput*)ui)->getField(index);
	return f->uintValue();
}
