#pragma once
#include "MemoryPool_OnlyNew.h"
#include <string>


struct TA_Node_Data
{
	char *text;
	char sign;
};


class TextAnalysis
{
private:
	struct Node
	{
		TA_Node_Data TAND;
		Node *next;
		Node()
			:next(0)
		{
		}
	};
	MemoryPool_OnlyNew<char> MPON;
	std::string *pStr;
	Node *head,*cur;
	bool CheckSign(char a,const char * sign);
public:
	TextAnalysis(std::string *);
	~TextAnalysis(void);
	int Analysis(char * sign);
	TA_Node_Data* GetNextData(void);
	void InitializeGet(void);
};

