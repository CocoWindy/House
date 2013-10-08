#pragma once
#include <cstdlib>
template<typename T>
class MemoryPool_OnlyNew
{
private:
	static const unsigned int defaultLength = 10240;
	unsigned int addLength;
	T* pMP;
	int length;
	int nowNumber;
public:
	MemoryPool_OnlyNew(unsigned int initialLength = defaultLength,unsigned int aL = 1024);
	~MemoryPool_OnlyNew(void);
	T* MP_ONNew(int L = 1);
	void Arrange(void);
};


template<typename T>
MemoryPool_OnlyNew<T>::MemoryPool_OnlyNew(unsigned int initialLength,unsigned int aL)
	: length(initialLength), nowNumber(0), addLength(aL)
{
	pMP = (T*)malloc(sizeof(T)*initialLength);
}


template<typename T>
MemoryPool_OnlyNew<T>::~MemoryPool_OnlyNew(void)
{
	free((void*)pMP);
}


template<typename T>
T* MemoryPool_OnlyNew<T>::MP_ONNew(int L)
{
	if(nowNumber+L<length-1)
	{
		T* p = pMP + nowNumber;
		nowNumber += L;
		return p;
	}
	else
	{
		pMP = (T*)realloc((void *)pMP,sizeof(T)*(nowNumber+L+addLength));
		if(pMP)
		{
			T* p = pMP + nowNumber;
			nowNumber += L;
			return p;
		}
		else
			return 0;
	}
}


template<typename T>
void MemoryPool_OnlyNew<T>::Arrange(void)
{
	pMP = (T*)realloc((void *)pMP,sizeof(T)*(length+L+addLength));
}