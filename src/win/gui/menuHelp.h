/*
Menu building implementation for windows
by: Connor Douthat
10/29/2015
*/
void MenuExit(const char *key)
{
	DestroyWindow(hwnd_main);
}
void *CreateMenuContainer()
{
	return CreatePopupMenu();
}
int MapMenuItemId(MenuItemMeta *meta)
{
	int id = MENU_BASE_ID + menu_map.size();
	menu_map.push_back(meta);
	return id;
}
void AddMenuItem(void *menu, const char *text, MenuItemMeta *data, void *subItem, bool enabled, int atIndex)
{
	DWORD flags = MF_STRING;
	UINT_PTR id = 0;
	if(subItem)
	{
		flags |= MF_POPUP;
		id = (UINT_PTR)subItem;
	}
	else if(data)
	{
		id = MapMenuItemId(data);
	}
	else enabled = false;
	if(!enabled)
	{
		flags |= MF_GRAYED;
	}
	if(atIndex < 0)
	{
		InsertMenu((HMENU)menu, atIndex, MF_BYPOSITION | flags, id, text);
	}
	else
	{
		AppendMenu((HMENU)menu, flags, id, text);
	}
}
void CheckMenuItem(void *menu, int index)
{
	CheckMenuRadioItem((HMENU)menu, index, index, index, MF_BYPOSITION);
}
void DestroyMenuContainer(void *menu)
{
	DestroyMenu((HMENU)menu);
}
bool MenuReload()
{
	menu_map.clear();
	MenuCleanup();
	popup_menu = (HMENU)MenuInit();
	return true;
}
