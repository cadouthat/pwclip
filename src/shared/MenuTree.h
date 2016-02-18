/*
MenuTree class for building nested popup menus
by: Connor Douthat
10/29/2015
*/

typedef int(*MenuTreeValueGenerator)(const char*);

class MenuTree
{
	MenuTreeValueGenerator genValue;

	MenuTree *treeChild(char *match_name, const char *key_in)
	{
		for(int i = 0; i < nodes.size(); i++)
		{
			if(!nodes[i]->value && !strcmp(nodes[i]->name, match_name))
			{
				//Free name without owner
				free(match_name);
				//Return existing node
				return nodes[i];
			}
		}
		MenuTree *child = new MenuTree(genValue, match_name);
		child->treeValue = genValue(key_in);
		nodes.push_back(child);
		return child;
	}
	MenuTree *valueChild(char *match_name, const char *key_in)
	{
		MenuTree *child = new MenuTree(genValue, match_name);
		child->value = genValue(key_in);
		nodes.push_back(child);
		return child;
	}

public:

	char *name;
	int value;
	int treeValue;
	std::vector<MenuTree*> nodes;

	MenuTree(MenuTreeValueGenerator genValueIn, const char *name_in = NULL)
	{
		name = strdup(name_in);
		genValue = genValueIn;
		value = 0;
		treeValue = 0;
	}
	~MenuTree()
	{
		if(name) free(name);
		for(int i = 0; i < nodes.size(); i++)
		{
			delete nodes[i];
		}
	}
	void parse(const char *str, const char *parent_key = NULL)
	{
		if(!str[0]) return;

		std::string cur_key;
		if(parent_key) cur_key = parent_key;

		int delim = 0;
		while(str[delim] && str[delim] != ':') delim++;

		char *name_tmp = (char*)malloc(delim + 1);
		strncpy(name_tmp, str, delim);
		name_tmp[delim] = 0;

		cur_key += name_tmp;

		if(str[delim])
		{
			cur_key += ":";
			MenuTree *child = treeChild(name_tmp, cur_key.c_str());
			child->parse(str + delim + 1, cur_key.c_str());
		}
		else valueChild(name_tmp, cur_key.c_str());
	}
};
