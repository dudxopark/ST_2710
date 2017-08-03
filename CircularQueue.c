
#include "CircularQueue.h"
#include <stdio.h>
#include <stdlib.h>

/* 큐를 생성 */
void CreateQueue(CircularQueue* queue, int capacity)
{
    // 노드를 위한 메모리 할당. 순환 큐가 공백 상태인지 포화 상태인지
    // 구분하는 더미 노드가 필요하기 때문에 +1 만큼 생성.
    (queue)->nodes = (ElementType*) malloc(sizeof(ElementType) * (capacity + 1));

    // 초기화
    (queue)->capacity = capacity;
    
    (queue)->front = 0;
    (queue)->rear = 0;
    (queue)->count = 0;
}

/* 큐를 파괴 */
void DestroyQueue(CircularQueue* queue)
{
    free(queue->nodes);
}

// 초기화
void ClearQueue(CircularQueue* queue)
{
    // 초기화
    (queue)->front = 0;
    (queue)->rear = 0;
    (queue)->count = 0;
}

/* 삽입 */
void EnQueue(CircularQueue* queue, ElementType data)
{
	if(IsQueueFull(queue))
	{		
#if	1	// 꽉 찼으면 입력 안 받음
		TRACE("Queue is Full\n");
		return;
#elif 0	// 하나의 Element를 제거하고 삽입
		ElementType stDummy;
		DeQueue(queue, &stDummy);
		TRACE("A Element is DeQueued\n");
#else	// 전체를 제거하고 다시 삽입
		ClearQueue(queue);  
		TRACE("Queue is Cleared\n");
#endif
	}
	// rear에 데이터를 쓰고 rear + 1
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

/* 제거 */
bool DeQueue(CircularQueue* queue, ElementType* pData)
{
	if(IsQueueEmpty(queue))
		return FALSE;	

	// front에서 데이터를 읽고 front + 1
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

/* 내용 보기 */
bool LookQueue(CircularQueue* queue, int nOffset, ElementType* pData)
{
	int nIndex;
	if(IsQueueEmpty(queue))
		return FALSE;

	// front로부터 offset 만큼 떨어진 곳의 데이터를 읽음
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

/* 사이즈 반환 */
int GetQueueSize(CircularQueue* queue)
{
	return (queue)->count;
}

/* 완전히 비어있는지 */
int IsQueueEmpty(CircularQueue* queue)
{
    return ((queue)->count == 0);
}

/* 꽉 차 있는지 */
int IsQueueFull(CircularQueue* queue)
{
    return ((queue)->count == (queue)->capacity);
}
