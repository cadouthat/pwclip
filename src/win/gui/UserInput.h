/*
Main UserInput implementation for managing prompt dialogs
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
	//Global
	static const char *class_name;
	static bool class_init;

	//Configuration
	const char *title;
	char *info_text;
	char *error_text;
	std::vector<UserInputField*> fields;

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
			case EN_CHANGE:
				for(int i = 0; i < fields.size(); i++)
				{
					if(fields[i]->owns((HWND)lParam))
					{
						if(fields[i]->editHandler) fields[i]->editHandler(this, i, fields[i]->editHandlerExtra);
						break;
					}
				}
				break;
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
				default:
					for(int i = 0; i < fields.size(); i++)
					{
						if(fields[i]->owns((HWND)lParam))
						{
							if(fields[i]->handler) fields[i]->handler(this, i);
							break;
						}
					}
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
	void addField(int type, const char *text, const void *valueIn)
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

		UserInputField *f = new UserInputField(type, text);
		f->init(hwnd_top, next_child_id, font);
		if(valueIn) f->value(valueIn);
		if(!fields.size()) f->focus();
		fields.push_back(f);

		UpdateLayout();
	}
	void onEdit(int i_field, UserInputFieldEditHandler cb, void *extra)
	{
		UserInputField *f = fields[i_field];
		f->editHandler = cb;
		f->editHandlerExtra = extra;
	}
	void setValue(int i_field, const void *valueIn)
	{
		UserInputField *f = fields[i_field];
		f->value(valueIn);
	}
	UserInputField *getField(int index)
	{
		return fields[index];
	}
};

const char *UserInput::class_name = "UserInputMain";
bool UserInput::class_init = false;
