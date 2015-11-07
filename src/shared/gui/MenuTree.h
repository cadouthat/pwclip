/*
MenuTree class for building nested popup menus
by: Connor Douthat
10/29/2015
*/
class MenuTree
{
	char name[256];
	int value;
	std::vector<MenuTree*> nodes;

	MenuTree *treeChild(char *match_name)
	{
		for(int i = 0; i < nodes.size(); i++)
		{
			if(!nodes[i]->value && !strcmp(nodes[i]->name, match_name))
			{
				return nodes[i];
			}
		}
		MenuTree *child = new MenuTree();
		strcpy(child->name, match_name);
		nodes.push_back(child);
		return child;
	}
	MenuTree *valueChild(char *match_name, int value_in)
	{
		for(int i = 0; i < nodes.size(); i++)
		{
			if(nodes[i]->value && !strcmp(nodes[i]->name, match_name))
			{
				nodes[i]->value = value_in;
				return nodes[i];
			}
		}
		MenuTree *child = new MenuTree();
		strcpy(child->name, match_name);
		child->value = value_in;
		nodes.push_back(child);
		return child;
	}

public:

	MenuTree()
	{
		memset(name, 0, sizeof(name));
		value = 0;
	}
	~MenuTree()
	{
		for(int i = 0; i < nodes.size(); i++)
		{
			delete nodes[i];
		}
	}
	void parse(char *str, int value_in)
	{
		if(!str[0]) return;
		int delim = 0;
		while(str[delim] && str[delim] != ':') delim++;
		int name_len = sizeof(name) - 1;
		if(delim < name_len) name_len = delim;
		char name_tmp[256] = {0};
		strncpy(name_tmp, str, name_len);
		if(str[delim])
		{
			MenuTree *child = treeChild(name_tmp);
			child->parse(str + delim + 1, value_in);
		}
		else valueChild(name_tmp, value_in);
	}
#ifdef __WIN32__
	HMENU create();
#endif
};
