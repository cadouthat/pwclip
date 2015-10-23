/*
Manages popup menus
by: Connor Douthat
10/22/2015
*/
bool MenuInit()
{
	popupMenu = CreatePopupMenu();
	if(!popupMenu) return false;
    if(!AppendMenu(popupMenu, MF_STRING, TRAY_EXIT, "Exit")) return false;
    return true;
}
bool MenuCleanup()
{
	return DestroyMenu(popupMenu);
}
