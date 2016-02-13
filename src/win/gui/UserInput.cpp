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

enum { UIN_ID_INFO = 1, UIN_ID_ERROR, UIN_ID_NAME, UIN_ID_OLDPASS, UIN_ID_NEWPASS, UIN_ID_CONFIRM, UIN_ID_OKAY, UIN_ID_CANCEL };

class UserInput
{
	//Global
	static const char *class_name;
	static bool class_init;

	//Configuration
	int flags;
	const char *title;
	const char *info_text;
	const char *error_text;

	//Window state
	bool okay_flag;
	HFONT font;
	HWND hwnd_top;
	int title_height;
	std::vector<std::wstring*> *wStringsUsed;

	//Output
	char val_name[ENTRY_NAME_MAX + 1];
	char val_pass[PASSWORD_MAX + 1];

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
		int spacing_edit = 36;
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
		if(flags & UIF_NAME)
		{
			SetWindowPos(GetDlgItem(hwnd_top, UIN_ID_NAME), NULL, x, y, w, h, SWP_NOZORDER);
			y += spacing_edit;
		}
		if(flags & UIF_OLDPASS)
		{
			SetWindowPos(GetDlgItem(hwnd_top, UIN_ID_OLDPASS), NULL, x, y, w, h, SWP_NOZORDER);
			y += spacing_edit;
		}
		if(flags & UIF_NEWPASS)
		{
			SetWindowPos(GetDlgItem(hwnd_top, UIN_ID_NEWPASS), NULL, x, y, w, h, SWP_NOZORDER);
			y += spacing_edit;
			SetWindowPos(GetDlgItem(hwnd_top, UIN_ID_CONFIRM), NULL, x, y, w, h, SWP_NOZORDER);
			y += spacing_edit;
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
		bool isEdit = !stricmp(cn, "EDIT");
		HWND hwnd_sub = CreateWindowEx(ex, cn,
				(text && !isEdit) ? text : "",
				WS_CHILD | WS_VISIBLE | st,
				0, 0, 0, 0,
				hwnd_top, (HMENU)id, NULL, NULL);
		PostMessage(hwnd_sub, WM_SETFONT, (WPARAM)font, true);
		if(isEdit)
		{
			//Convert to unicode
			std::string tmp(text);
			std::wstring *wtext = new std::wstring(tmp.begin(), tmp.end());
			if(!wStringsUsed) wStringsUsed = new std::vector<std::wstring*>();
			wStringsUsed->push_back(wtext);
			//Set cue
			PostMessage(hwnd_sub, EM_SETCUEBANNER, (WPARAM)true, (LPARAM)wtext->c_str());
			//Set password mask
			if(id != UIN_ID_NAME)
			{
				PostMessage(hwnd_sub, EM_SETPASSWORDCHAR, 0x95, 0);
			}
		}
		return hwnd_sub;
	}
	HWND childStatic(int id, const char *text)
	{
		return child(id, text, "STATIC");
	}
	HWND childEdit(int id, const char *text, int max_len = PASSWORD_MAX)
	{
		HWND hwnd_sub = child(id, text, "EDIT", WS_TABSTOP, WS_EX_CLIENTEDGE);
		PostMessage(hwnd_sub, EM_SETLIMITTEXT, max_len, 0);
		return hwnd_sub;
	}
	HWND childCheckbox(int id, const char *text)
	{
		return child(id, text, "BUTTON", WS_TABSTOP | BS_AUTOCHECKBOX);
	}
	HWND childButton(int id, const char *text)
	{
		return child(id, text, "BUTTON", WS_TABSTOP | BS_PUSHBUTTON);
	}
	HWND childDefButton(int id, const char *text)
	{
		return child(id, text, "BUTTON", WS_TABSTOP | BS_DEFPUSHBUTTON);
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
			wnd_class.hIcon = LoadIcon(NULL, IDI_INFORMATION);
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
		childStatic(UIN_ID_INFO, info_text);
		childStatic(UIN_ID_ERROR, error_text);
		HWND firstInput = NULL;
		if(flags & UIF_NAME)
		{
			HWND child = childEdit(UIN_ID_NAME, "Entry Name", ENTRY_NAME_MAX);
			firstInput = firstInput ? firstInput : child;
		}
		if(flags & UIF_OLDPASS)
		{
			HWND child = childEdit(UIN_ID_OLDPASS, (flags & UIF_NEWPASS) ? "Current Master Password" : "Master Password");
			firstInput = firstInput ? firstInput : child;
		}
		if(flags & UIF_NEWPASS)
		{
			HWND child = childEdit(UIN_ID_NEWPASS, "New Master Password");
			firstInput = firstInput ? firstInput : child;
			childEdit(UIN_ID_CONFIRM, "Confirm Master Password");
		}
		childDefButton(UIN_ID_OKAY, "Okay");
		childButton(UIN_ID_CANCEL, "Cancel");
		//Finalize
		UpdateLayout();
		if(firstInput) SetFocus(firstInput);
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
	void getValue(int id, char *out, int max)
	{
		HWND hwnd = GetDlgItem(hwnd_top, id);
		if(!hwnd) return;
		const char *def_text = (const char*)GetWindowLong(hwnd, GWL_USERDATA);
		GetWindowText(hwnd, out, max);
		if(def_text && !strcmp(out, def_text))
		{
			*out = 0;
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
					okay_flag = true;
					if(okay_flag && (flags & UIF_NAME))
					{
						getValue(UIN_ID_NAME, val_name, sizeof(val_name));
						if(!val_name[0])
						{
							setError("Entry name required");
							okay_flag = false;
						}
					}
					if(okay_flag && (flags & UIF_OLDPASS))
					{
						getValue(UIN_ID_OLDPASS, val_pass, sizeof(val_pass));
					}
					if(okay_flag && (flags & UIF_NEWPASS))
					{
						getValue(UIN_ID_NEWPASS, val_pass, sizeof(val_pass));
						char tmp[PASSWORD_MAX + 1] = {0};
						getValue(UIN_ID_CONFIRM, tmp, sizeof(tmp));
						if(strcmp(tmp, val_pass))
						{
							memset(val_pass, 0, sizeof(val_pass));
							setError("Passwords do not match");
							okay_flag = false;
						}
						else if(!val_pass[0])
						{
							okay_flag = (IDYES == MessageBox(hwnd_top, "Master password is blank, encryption will not be secure. Are you sure you want to proceed?", "Confirm Empty Password", MB_YESNO));
						}
					}
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
	UserInput(int flags_in, const char *title_in)
	{
		memset(this, 0, sizeof(*this));
		flags = flags_in;
		title = title_in;
	}
	~UserInput()
	{
		if(hwnd_top) close();
		if(wStringsUsed)
		{
			for(int i = 0; i < wStringsUsed->size(); i++)
			{
				delete (*wStringsUsed)[i];
			}
			delete wStringsUsed;
		}
		memset(this, 0, sizeof(*this));
	}
	const char *name() { return val_name; }
	const char *pass() { return val_pass; }
	void setInfo(const char *text)
	{
		info_text = text;
		if(hwnd_top)
		{
			SetWindowText(GetDlgItem(hwnd_top, UIN_ID_INFO), text);
			UpdateLayout();
		}
	}
	void setError(const char *text)
	{
		error_text = text;
		if(hwnd_top)
		{
			SetWindowText(GetDlgItem(hwnd_top, UIN_ID_ERROR), text);
			UpdateLayout();
		}
	}
	bool get()
	{
		if(!hwnd_top)
		{
			//Open window for the first time
			if(!open())
			{
				ErrorBox("Unknown GUI error");
				return false;
			}
		}
		else
		{
			//Re-focus password input
			if(flags & UIF_OLDPASS) SetFocus(GetDlgItem(hwnd_top, UIN_ID_OLDPASS));
		}

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
};

const char *UserInput::class_name = "UserInputMain";
bool UserInput::class_init = false;

void *UserInput_new(int flags_in, const char *title_in)
{
	return new UserInput(flags_in, title_in);
}
void UserInput_delete(void *ui)
{
	delete (UserInput*)ui;
}
const char *UserInput_name(void *ui)
{
	return ((UserInput*)ui)->name();
}
const char *UserInput_pass(void *ui)
{
	return ((UserInput*)ui)->pass();
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
