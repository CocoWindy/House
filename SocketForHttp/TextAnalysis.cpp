#include "TextAnalysis.h"


TextAnalysis::TextAnalysis(std::string *p)
	:pStr(p),cur(0)
{
}


TextAnalysis::~TextAnalysis(void)
{
	for(Node *p=head;p;)
	{
		Node *p2 = p;
		p = p->next;
		delete p2;
	}
}


int TextAnalysis::Analysis(char * sign)
{
	head = new Node;
	head->TAND.text = MPON.MP_ONNew();
	char *p = head->TAND.text;
	Node *c = head;
	int i,n=0;
	for(i=0;i<pStr->size();++i)
	{
		char a = (*pStr)[i];
		if(a == ' ' || a=='\r')
			continue;
		if(!CheckSign(a,sign))
		{
			*p = a;
			p = MPON.MP_ONNew();
		}
		else
		{
			if(a == '\n')
			{
				++n;
				if(n > 1)
					return i;
			}
			else
				n = 0;
			*p = '\0';
			c->TAND.sign = a;
			c->next = new Node;
			c = c->next;
			p = c->TAND.text = MPON.MP_ONNew();
		}
	}
	return -1;
}

bool TextAnalysis::CheckSign(char a,const char * sign)
{
	for(int i=0;sign[i];++i)
	{
		if(a == sign[i])
			return true;
	}
	return false;
}


TA_Node_Data* TextAnalysis::GetNextData(void)
{
	Node *p = cur;
	if(cur)
	{
		cur = cur->next;
		return &(p->TAND);
	}
	else
		return 0;
}


void TextAnalysis::InitializeGet(void)
{
	cur = head;
}
