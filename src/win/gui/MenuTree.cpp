/*
Menu building implementation for windows
by: Connor Douthat
10/29/2015
*/
HMENU MenuTree::create(const char *treeItemText)
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
			HMENU sub = child->create(treeItemText);
			if(child->treeValue && treeItemText)
			{
				InsertMenu(sub, 0, MF_BYPOSITION | MF_STRING, child->treeValue, treeItemText);
			}
			AppendMenu(hm, MF_STRING | MF_POPUP, (UINT_PTR)sub, child->name);
		}
	}
	return hm;
}
