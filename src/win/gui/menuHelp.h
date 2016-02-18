/*
Menu building implementation for windows
by: Connor Douthat
10/29/2015
*/
HMENU BuildMenuTree(MenuTree *tree, const char *treeItemText)
{
	HMENU hm = CreatePopupMenu();
	for(int i = 0; i < tree->nodes.size(); i++)
	{
		MenuTree *child = tree->nodes[i];
		if(child->value)
		{
			AppendMenu(hm, MF_STRING, child->value, child->name);
		}
		else
		{
			HMENU sub = BuildMenuTree(child, treeItemText);
			if(child->treeValue && treeItemText)
			{
				InsertMenu(sub, 0, MF_BYPOSITION | MF_STRING, child->treeValue, treeItemText);
			}
			AppendMenu(hm, MF_STRING | MF_POPUP, (UINT_PTR)sub, child->name);
		}
	}
	return hm;
}
