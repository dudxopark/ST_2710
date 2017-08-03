
#include "CircularQueue.h"
#include <stdio.h>
#include <stdlib.h>

/* ť�� ���� */
void CreateQueue(CircularQueue* queue, int capacity)
{
    // ��带 ���� �޸� �Ҵ�. ��ȯ ť�� ���� �������� ��ȭ ��������
    // �����ϴ� ���� ��尡 �ʿ��ϱ� ������ +1 ��ŭ ����.
    (queue)->nodes = (ElementType*) malloc(sizeof(ElementType) * (capacity + 1));

    // �ʱ�ȭ
    (queue)->capacity = capacity;
    
    (queue)->front = 0;
    (queue)->rear = 0;
    (queue)->count = 0;
}

/* ť�� �ı� */
void DestroyQueue(CircularQueue* queue)
{
    free(queue->nodes);
}

// �ʱ�ȭ
void ClearQueue(CircularQueue* queue)
{
    // �ʱ�ȭ
    (queue)->front = 0;
    (queue)->rear = 0;
    (queue)->count = 0;
}

/* ���� */
void EnQueue(CircularQueue* queue, ElementType data)
{
	if(IsQueueFull(queue))
	{		
#if	1	// �� á���� �Է� �� ����
		TRACE("Queue is Full\n");
		return;
#elif 0	// �ϳ��� Element�� �����ϰ� ����
		ElementType stDummy;
		DeQueue(queue, &stDummy);
		TRACE("A Element is DeQueued\n");
#else	// ��ü�� �����ϰ� �ٽ� ����
		ClearQueue(queue);  
		TRACE("Queue is Cleared\n");
#endif
	}
	// rear�� �����͸� ���� rear + 1
	queue->nodes[queue->rear] = data;
#if 0	
	queue->rear = (queue->rear + 1) % queue->capacity;
#else
	queue->rear = (queue->rear + 1);
	if(queue->rear >= queue->capacity)
		queue->rear -= queue->capacity;
#endif
	queue->count++;
}

/* ���� */
bool DeQueue(CircularQueue* queue, ElementType* pData)
{
	if(IsQueueEmpty(queue))
		return FALSE;	

	// front���� �����͸� �а� front + 1
	*pData = queue->nodes[queue->front];
#if 0	
	queue->front = (queue->front + 1) % queue->capacity;
#else
	queue->front = (queue->front + 1);
	if(queue->front >= queue->capacity)
		queue->front -= queue->capacity;
#endif
	queue->count--;
	return TRUE;
}

/* ���� ���� */
bool LookQueue(CircularQueue* queue, int nOffset, ElementType* pData)
{
	int nIndex;
	if(IsQueueEmpty(queue))
		return FALSE;

	// front�κ��� offset ��ŭ ������ ���� �����͸� ����
#if 0
	nIndex = (queue->front + nOffset) % queue->capacity;
#else
	nIndex = (queue->front + nOffset);
	if(nIndex >= queue->capacity)
		nIndex -= queue->capacity;
#endif

	*pData = queue->nodes[nIndex];	
	return TRUE;
}

/* ������ ��ȯ */
int GetQueueSize(CircularQueue* queue)
{
	return (queue)->count;
}

/* ������ ����ִ��� */
int IsQueueEmpty(CircularQueue* queue)
{
    return ((queue)->count == 0);
}

/* �� �� �ִ��� */
int IsQueueFull(CircularQueue* queue)
{
    return ((queue)->count == (queue)->capacity);
}
