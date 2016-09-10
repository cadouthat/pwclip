/*
Interaction for quick-recall
by: Connor Douthat
9/9/2016
*/
struct QUICK_RECALL_USERDATA
{
	std::vector<std::vector<char*>*> *options;
	int selected_vault;
	int selected_entry;
};

void QuickRecallOnEdit(void *prompt, int i_field, void *extra)
{
	QUICK_RECALL_USERDATA *userdata = (QUICK_RECALL_USERDATA*)extra;
	userdata->selected_vault = -1;

	char *search = UserInput_stringValue(prompt, 0);
	if(*search)
	{
		for(int i = 0; i < userdata->options->size(); i++)
		{
			for(int j = 0; j < userdata->options->at(i)->size(); j++)
			{
				char *key = userdata->options->at(i)->at(j);
				if(stristr(key, search))
				{
					UserInput_setInfo(prompt, key);
					userdata->selected_vault = i;
					userdata->selected_entry = j;
				}
			}
		}
	}
	free(search);

	if(userdata->selected_vault == -1)
	{
		UserInput_setInfo(prompt, "");
	}
}

void QuickRecallDialog()
{
	int hist = vaults.history.size();
	int total_peek = 0;
	std::vector<std::vector<char*>*> peek;
	for(int i = 0; i < hist; i++)
	{
		peek.push_back(new std::vector<char*>());
		vaults.history[i]->peek(peek.back());
		total_peek += peek.back()->size();
	}

	int selected_vault = -1;
	char *selected_key = NULL;

	if(total_peek)
	{
		QUICK_RECALL_USERDATA cb_data;
		memset(&cb_data, 0, sizeof(cb_data));
		cb_data.options = &peek;
		cb_data.selected_vault = -1;

		void *prompt = UserInput_new("Quick Recall");
		UserInput_addField(prompt, UIF_TEXT, "Start typing to search");
		UserInput_onEdit(prompt, 0, QuickRecallOnEdit, &cb_data);
		if(UserInput_get(prompt))
		{
			if(cb_data.selected_vault >= 0)
			{
				selected_key = strdup(peek[cb_data.selected_vault]->at(cb_data.selected_entry));
				bool isOpen = vaults.history[cb_data.selected_vault]->isOpen();
				if(!isOpen)
				{
					if(OpenVaultDialog(cb_data.selected_vault))
					{
						selected_vault = 0;
					}
				}
				else selected_vault = cb_data.selected_vault;
			}
		}
		UserInput_delete(prompt);
	}

	if(selected_vault >= 0 && selected_key)
	{
		VaultEntry entry(vaults.history[selected_vault], selected_key);
		RecallEntryDialog(&entry);
	}

	if(selected_key) free(selected_key);
	while(peek.size())
	{
		for(int j = 0; j < peek.back()->size(); j++)
		{
			free(peek.back()->at(j));
		}
		delete peek.back();
		peek.pop_back();
	}
}
