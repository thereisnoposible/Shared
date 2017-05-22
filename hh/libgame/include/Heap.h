#include "Timer.h"

#include<cstdlib>

#define DefaultSize 1000000

using namespace std;


class MinHeap
{
public:
	MinHeap(int sz = DefaultSize)
	{
		maxHeapSize = (sz > DefaultSize) ? sz : DefaultSize;
		heap = new Timer*[maxHeapSize];
		if (heap == NULL)
		{
			exit(1);
		}

		currentSize = 0;
	}

	virtual ~MinHeap()
	{
		for (int32 i = 0; i < currentSize; i++)
		{
			delete heap[i];
			heap[i] = NULL;
		}

		//É¾³ýÊý×é¿Õ¼ä
		delete[] heap;

		heap = NULL;
	}

	bool Insert(Timer* x)
	{
		if (currentSize == maxHeapSize)
		{
			return false;
		}
		heap[currentSize] = x;
		siftUp(currentSize);
		currentSize++;

		return true;
	}

	bool RemoveMin()
	{
		if (currentSize == 0)
		{
			return false;
		}

		heap[0] = heap[currentSize - 1];
		currentSize--;
		siftDown(0, currentSize - 1);

		return true;
	}

	bool FindMin(Timer*& x)
	{
		if (currentSize == 0)
		{
			return false;
		}

		x = heap[0];

		return true;
	}

	bool IsEmpty()
	{
		return currentSize == 0; 
	}
	
	bool IsFull()
	{
		return currentSize == maxHeapSize; 
	}

	void MakeEmpty()
	{
		currentSize = 0; 
	}

    int GetCurrSize()
    {
        return currentSize;
    }
private:
	Timer** heap;

	int currentSize;
	int maxHeapSize;
	void siftDown(int start, int m)
	{
		int i = start;
		int j = 2 * i + 1;
		Timer* temp = heap[i];
		while (j <= m)
		{
			if (j<m && heap[j]->getRelativeTime() >heap[j + 1]->getRelativeTime()) j++;
			if (heap[j]->getRelativeTime() >= temp->getRelativeTime()) break;
			else
			{
				heap[i] = heap[j];
				i = j;
				j = 2 * j + 1;
			}
		}
		heap[i] = temp;
	}

	void siftUp(int start)
	{
		int j = start;
		int i = (j - 1) / 2;
		Timer* temp = heap[j];
		while (j > 0)
		{
			if (temp->getRelativeTime() >= heap[i]->getRelativeTime()) break;
			else
			{
				heap[j] = heap[i];
				j = i;
				i = (i - 1) / 2;
			}
		}
		heap[j] = temp;
	}
};
