/*
Interaction for quick-recall
by: Connor Douthat
9/9/2016
*/
void QuickRecallDialog()
{
	int hist = vaults.history.size();
	if(!hist) return;
	std::vector<std::vector<char*>*> peek;
	for(int i = 0; i < hist; i++)
	{
		peek.push_back(new std::vector<char*>());
		vaults.history[i]->peek(peek.back());
	}

	//

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
