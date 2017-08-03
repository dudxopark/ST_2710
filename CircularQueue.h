#ifndef CIRCULAR_QUEUE_H

#define CIRCULAR_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include "Common.h" 

typedef U32 ElementType;

typedef struct tagCircularQueue
{
    int front;
    int rear;
    int count;
    int capacity;   // ���� ť�� �迭�� capacity + 1 ��ŭ �����ȴ�.
    ElementType* nodes;
} CircularQueue;

void CreateQueue(CircularQueue* queue, int capacity);
void DestroyQueue(CircularQueue* queue);

void ClearQueue(CircularQueue* queue);
void EnQueue(CircularQueue* queue, ElementType data);
bool DeQueue(CircularQueue* queue, ElementType* pData);
bool LookQueue(CircularQueue* queue, int nOffset, ElementType* pData);

int GetQueueSize(CircularQueue* queue);
int IsQueueEmpty(CircularQueue* queue);
int IsQueueFull(CircularQueue* queue);

#endif
