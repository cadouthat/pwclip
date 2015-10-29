/*
Helper class for mixed user input
by: Connor Douthat
10/28/2015
*/
#define UIF_GENERATE 1
#define UIF_NAME 2
#define UIF_OLDPASS 4
#define UIF_NEWPASS 8

#define UIN_WIDTH 400
#define UIN_HEIGHT 200
#define UIN_PAD_X 30
#define UIN_PAD_Y 20
#define UIN_WIDTH_IN (UIN_WIDTH - UIN_PAD_X * 2)
#define UIN_LINE_HEIGHT 24
#define UIN_BUTTON_WIDTH 80
#define UIN_BUTTON_HEIGHT 26

enum { UIN_ID_NAME = 1, UIN_ID_OLDPASS, UIN_ID_NEWPASS, UIN_ID_CONFIRM, UIN_ID_OKAY, UIN_ID_CANCEL };

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
	const char *label_name;
	const char *label_oldpass;
	const char *label_newpass;
	const char *label_confirm;
	bool okay_flag;
	HFONT font;
	HWND hwnd_top,
		hwnd_info,
		hwnd_error,
		hwnd_generate,
		hwnd_name,
		hwnd_oldpass,
		hwnd_newpass,
		hwnd_confirm,
		hwnd_okay,
		hwnd_cancel;

	//Output
	bool val_generate;
	char val_name[ENTRY_NAME_MAX + 1];
	char val_oldpass[PASSWORD_MAX + 1];
	char val_newpass[PASSWORD_MAX + 1];

	void UpdateLayout()
	{
		RECT cr;
		GetClientRect(hwnd_top, &cr);
		int x = cr.left + (cr.right - cr.left - UIN_WIDTH_IN) / 2;
		int y = UIN_PAD_Y - 10;
		int spacing = UIN_LINE_HEIGHT * 3 / 2;
		if(hwnd_info && info_text && *info_text)
		{
			SetWindowPos(hwnd_info, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			y += spacing;
		}
		if(hwnd_error && error_text && *error_text)
		{
			SetWindowPos(hwnd_error, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			y += spacing;
		}
		if(hwnd_generate)
		{
			SetWindowPos(hwnd_generate, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			y += spacing;
		}
		if(hwnd_name)
		{
			SetWindowPos(hwnd_name, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			y += spacing;
		}
		if(hwnd_oldpass)
		{
			SetWindowPos(hwnd_oldpass, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			y += spacing;
		}
		if(hwnd_newpass)
		{
			SetWindowPos(hwnd_newpass, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			y += spacing;
		}
		if(hwnd_confirm)
		{
			SetWindowPos(hwnd_confirm, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			y += spacing;
		}
		int btm_width = UIN_BUTTON_WIDTH * 2 + UIN_PAD_X;
		x = (UIN_WIDTH - btm_width) / 2;
		SetWindowPos(hwnd_okay, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		x = UIN_WIDTH - x - UIN_BUTTON_WIDTH;
		SetWindowPos(hwnd_cancel, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		y += UIN_BUTTON_HEIGHT + UIN_PAD_Y;
		int ex_height = UIN_HEIGHT - (cr.bottom - cr.top);
		SetWindowPos(hwnd_top, NULL, 0, 0, UIN_WIDTH, y + ex_height, SWP_NOMOVE | SWP_NOZORDER);
		InvalidateRect(hwnd_top, NULL, true);
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
		font = CreateFont(15, 0,
			0, 0,
			FW_NORMAL,
			false,
			false,
			false,
			ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			FF_DONTCARE,
			"Arial");
		//Create main window
		hwnd_top = CreateWindowEx(0, class_name,
			title,
			WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT,
			UIN_WIDTH, UIN_HEIGHT,
			HWND_DESKTOP, NULL, hModule, NULL);
		if(!hwnd_top) return false;
		SendMessage(hwnd_top, WM_SETFONT, (WPARAM)font, true);
		SetWindowLongPtr(hwnd_top, GWL_USERDATA, (long)this);
		//Create controls
		hwnd_info = CreateWindowEx(0, "STATIC",
			info_text,
			WS_CHILD | WS_VISIBLE,
			0, 0,
			UIN_WIDTH_IN, UIN_LINE_HEIGHT,
			hwnd_top, NULL, NULL, NULL);
		SendMessage(hwnd_info, WM_SETFONT, (WPARAM)font, true);
		hwnd_error = CreateWindowEx(0, "STATIC",
			error_text,
			WS_CHILD | WS_VISIBLE,
			0, 0,
			UIN_WIDTH_IN, UIN_LINE_HEIGHT,
			hwnd_top, NULL, NULL, NULL);
		SendMessage(hwnd_error, WM_SETFONT, (WPARAM)font, true);
		if(flags & UIF_GENERATE)
		{
			hwnd_generate = CreateWindowEx(0, "BUTTON",
				"Generate random value (ignores current clipboard)",
				WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_AUTOCHECKBOX,
				0, 0,
				UIN_WIDTH_IN, UIN_LINE_HEIGHT,
				hwnd_top, NULL, NULL, NULL);
			SendMessage(hwnd_generate, WM_SETFONT, (WPARAM)font, true);
		}
		if(flags & UIF_NAME)
		{
			label_name = "Entry Name";
			hwnd_name = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT",
				label_name,
				WS_CHILD | WS_TABSTOP | WS_VISIBLE,
				0, 0,
				UIN_WIDTH_IN, UIN_LINE_HEIGHT,
				hwnd_top, (HMENU)UIN_ID_NAME, NULL, NULL);
			SendMessage(hwnd_name, WM_SETFONT, (WPARAM)font, true);
		}
		if(flags & UIF_OLDPASS)
		{
			label_oldpass = (flags & UIF_NEWPASS) ? "Current Password" : "Encryption Password";
			hwnd_oldpass = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT",
				label_oldpass,
				WS_CHILD | WS_TABSTOP | WS_VISIBLE,
				0, 0,
				UIN_WIDTH_IN, UIN_LINE_HEIGHT,
				hwnd_top, (HMENU)UIN_ID_OLDPASS, NULL, NULL);
			SendMessage(hwnd_oldpass, WM_SETFONT, (WPARAM)font, true);
		}
		if(flags & UIF_NEWPASS)
		{
			label_newpass = (flags & UIF_OLDPASS) ? "New Password" : "Encryption Password";
			hwnd_newpass = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT",
				label_newpass,
				WS_CHILD | WS_TABSTOP | WS_VISIBLE,
				0, 0,
				UIN_WIDTH_IN, UIN_LINE_HEIGHT,
				hwnd_top, (HMENU)UIN_ID_NEWPASS, NULL, NULL);
			SendMessage(hwnd_newpass, WM_SETFONT, (WPARAM)font, true);
			label_confirm = "Confirm Password";
			hwnd_confirm = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT",
				label_confirm,
				WS_CHILD | WS_TABSTOP | WS_VISIBLE,
				0, 0,
				UIN_WIDTH_IN, UIN_LINE_HEIGHT,
				hwnd_top, (HMENU)UIN_ID_CONFIRM, NULL, NULL);
			SendMessage(hwnd_confirm, WM_SETFONT, (WPARAM)font, true);
		}
		hwnd_okay = CreateWindowEx(0, "BUTTON",
			"Okay",
			WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,
			0, 0,
			UIN_BUTTON_WIDTH, UIN_BUTTON_HEIGHT,
			hwnd_top, (HMENU)UIN_ID_OKAY, NULL, NULL);
			SendMessage(hwnd_okay, WM_SETFONT, (WPARAM)font, true);
		hwnd_cancel = CreateWindowEx(0, "BUTTON",
			"Cancel",
			WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_PUSHBUTTON,
			0, 0,
			UIN_BUTTON_WIDTH, UIN_BUTTON_HEIGHT,
			hwnd_top, (HMENU)UIN_ID_CANCEL, NULL, NULL);
			SendMessage(hwnd_cancel, WM_SETFONT, (WPARAM)font, true);
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
	const char *defaultLabel(int id)
	{
		switch(id)
		{
		case UIN_ID_NAME:
			return label_name;
		case UIN_ID_OLDPASS:
			return label_oldpass;
		case UIN_ID_NEWPASS:
			return label_newpass;
		case UIN_ID_CONFIRM:
			return label_confirm;
		default:
			return "";
		}
	}
	void focus(int id, HWND hwnd)
	{
		char text[PASSWORD_MAX + ENTRY_NAME_MAX];
		GetWindowText(hwnd, text, sizeof(text));
		if(!strcmp(text, defaultLabel(id))) SetWindowText(hwnd, "");
		if(id != UIN_ID_NAME) SendMessage(hwnd, EM_SETPASSWORDCHAR, '#', 0);
	}
	void unfocus(int id, HWND hwnd)
	{
		char text[PASSWORD_MAX + ENTRY_NAME_MAX];
		GetWindowText(hwnd, text, sizeof(text));
		if(!text[0])
		{
			SendMessage(hwnd, EM_SETPASSWORDCHAR, 0, 0);
			SetWindowText(hwnd, defaultLabel(id));
		}
	}
	LRESULT MessageProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch(msg)
		{
		case WM_COMMAND:
			switch(HIWORD(wParam))
			{
			case EN_SETFOCUS:
				focus(LOWORD(wParam), (HWND)lParam);
				break;
			case EN_KILLFOCUS:
				unfocus(LOWORD(wParam), (HWND)lParam);
				break;
			}
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		return 0;
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
		memset(this, 0, sizeof(*this));
	}
	bool generate() { return val_generate; }
	const char *name() { return val_name; }
	const char *oldpass() { return val_oldpass; }
	const char *newpass() { return val_newpass; }
	void setInfo(const char *text)
	{
		info_text = text;
		if(hwnd_info)
		{
			SetWindowText(hwnd_info, text);
			UpdateLayout();
		}
	}
	void setError(const char *text)
	{
		error_text = text;
		if(hwnd_error)
		{
			SetWindowText(hwnd_error, text);
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
			if(hwnd_oldpass) SetFocus(hwnd_oldpass);
		}

		//Message loop
		MSG msg;
		okay_flag = false;
		while(!okay_flag && GetMessage(&msg, hwnd_top, 0, 0) > 0)
		{
			//Process message
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return okay_flag;
	}
	static LRESULT CALLBACK StaticMessageProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		UserInput *obj = (UserInput*)GetWindowLong(hwnd, GWL_USERDATA);
		if(obj) return obj->MessageProc(hwnd, msg, wParam, lParam);
		else return DefWindowProc(hwnd, msg, wParam, lParam);
	}
};

const char *UserInput::class_name = "UserInputMain";
bool UserInput::class_init = false;
