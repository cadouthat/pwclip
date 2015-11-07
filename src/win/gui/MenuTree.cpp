/*
Menu building implementation for windows
by: Connor Douthat
10/29/2015
*/
HMENU MenuTree::create()
{
	HMENU hm = CreatePopupMenu();
	for(int i = 0; i < nodes.size(); i++)
	{
		MenuTree *child = nodes[i];
		if(child->value)
		{
			AppendMenu(hm, MF_STRING, child->value, child->name);
		}
		else
		{
			AppendMenu(hm, MF_STRING | MF_POPUP, (UINT_PTR)child->create(), child->name);
		}
	}
	return hm;
}
