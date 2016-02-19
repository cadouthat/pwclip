/*
MenuTree class for building nested popup menus
by: Connor Douthat
10/29/2015
*/

typedef void(*DoMenuAction)(const char*);

class MenuItemMeta
{
public:
	DoMenuAction action;
	char *key;

	MenuItemMeta(DoMenuAction actionIn, const char *keyIn = NULL)
	{
		action = actionIn;
		key = keyIn ? strdup(keyIn) : NULL;
	}
	~MenuItemMeta()
	{
		if(key) free(key);
	}
	void activate()
	{
		if(action) action(key);
	}
};

class MenuTree
{
	char *name;
	char *key;
	std::vector<MenuTree*> nodes;
	std::vector<MenuItemMeta*> meta_cache;

	MenuItemMeta *addMeta(DoMenuAction action, const char *key)
	{
		MenuItemMeta *meta = new MenuItemMeta(action, key);
		meta_cache.push_back(meta);
		return meta;
	}
	MenuTree *newChild(char *match_name, const char *key_in)
	{
		MenuTree *child = new MenuTree(match_name, key_in);
		nodes.push_back(child);
		return child;
	}
	MenuTree *treeChild(char *match_name, const char *key_in)
	{
		for(int i = 0; i < nodes.size(); i++)
		{
			if(nodes[i]->nodes.size() && !strcmp(nodes[i]->name, match_name))
			{
				//Return existing node
				return nodes[i];
			}
		}
		return newChild(match_name, key_in);
	}

public:

	MenuTree(const char *name_in = NULL, const char *key_in = NULL)
	{
		name = name_in ? strdup(name_in) : NULL;
		key = key_in ? strdup(key_in) : NULL;
	}
	~MenuTree()
	{
		if(name) free(name);
		if(key) free(key);
		for(int i = 0; i < nodes.size(); i++)
		{
			delete nodes[i];
		}
		for(int i = 0; i < meta_cache.size(); i++)
		{
			delete meta_cache[i];
		}
	}
	unsigned int count()
	{
		return nodes.size();
	}
	void parse(const char *str)
	{
		if(!str[0]) return;

		std::string cur_key = key ? key : "";

		int delim = 0;
		while(str[delim] && str[delim] != ':') delim++;

		char *name_tmp = (char*)malloc(delim + 1);
		strncpy(name_tmp, str, delim);
		name_tmp[delim] = 0;

		cur_key += name_tmp;

		if(str[delim])
		{
			cur_key += ":";
			treeChild(name_tmp, cur_key.c_str())->parse(str + delim + 1);
		}
		else newChild(name_tmp, cur_key.c_str());

		free(name_tmp);
	}
	void *build(DoMenuAction action, const char *treeItemText = NULL)
	{
		void *menu = CreateMenuContainer();
		for(int i = 0; i < nodes.size(); i++)
		{
			MenuTree *child = nodes[i];
			if(!child->nodes.size())
			{
				AddMenuItem(menu, child->name, addMeta(action, child->key));
			}
			else
			{
				void *sub = child->build(action, treeItemText);
				if(treeItemText)
				{
					AddMenuItem(sub, treeItemText, addMeta(action, child->key), NULL, true, 0);
				}
				AddMenuItem(menu, child->name, NULL, sub);
			}
		}
		return menu;
	}
};
