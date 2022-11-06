#include <stdio.h>
#include <Windows.h>
#include <conio.h>

#define RING_SIZE (16)
#define SLEEPING_TIME 400

struct RingBuffer {
    unsigned int tail;
    unsigned int head;
    unsigned char data[RING_SIZE];
};

HANDLE Empty;
HANDLE Full;
HANDLE FinishSignal;

RingBuffer Ring;

CRITICAL_SECTION BufferAccess;

char ringBuffGetChar(RingBuffer *apBuffer) {
    int index;
    index = apBuffer->head;
    apBuffer->head = (apBuffer->head + 1) % RING_SIZE;
    return apBuffer->data[index];
}

void ringBuffPutChar(RingBuffer *apBuffer, const char c) {
    apBuffer->data[apBuffer->tail] = c;
    apBuffer->tail = (apBuffer->tail + 1) % RING_SIZE;
}

DWORD WINAPI producer(LPVOID lpParam) {
    char c;
    const int semaphore_num = 2;
    HANDLE semaphores[semaphore_num] = { FinishSignal, Empty };

    while (WaitForMultipleObjects(semaphore_num, semaphores, FALSE, INFINITE) == WAIT_OBJECT_0 + 1) {
        c = _getch();

        if (c == 'q' || c == 'Q') {
            ReleaseSemaphore(FinishSignal, 2, NULL);
        }

        EnterCriticalSection(&BufferAccess);

        ringBuffPutChar(&Ring, c);

        LeaveCriticalSection(&BufferAccess);

        ReleaseSemaphore(Full, 1, NULL);
    }

    return 0;
}

DWORD WINAPI processor(LPVOID lpParam) {
    char c;
    const int semaphore_num = 2;
    HANDLE semaphores[semaphore_num] = { FinishSignal, Full };

    while (WaitForMultipleObjects(semaphore_num, semaphores, FALSE, INFINITE) == WAIT_OBJECT_0 + 1) {
        
        // Preuzimanje  iz kruznog bafera
        EnterCriticalSection(&BufferAccess);

        c = ringBuffGetChar(&Ring);

        LeaveCriticalSection(&BufferAccess);

        ReleaseSemaphore(Empty, 1, NULL);

        // Konverzija iz malog u veliko slovo
        c -= 32;
        Sleep(SLEEPING_TIME);

        // Upisivanje u kruzni bafer
        EnterCriticalSection(&BufferAccess);
        
        ringBuffPutChar(&Ring, c);

        LeaveCriticalSection(&BufferAccess);

        ReleaseSemaphore(Full, 1, NULL);
    }

    return 0;
}

DWORD WINAPI consumer(LPVOID lpParam) {
    char c;
    const int semaphore_num = 2;
    HANDLE semaphores[semaphore_num] = { FinishSignal, Full };

    while (WaitForMultipleObjects(semaphore_num, semaphores, FALSE, INFINITE) == WAIT_OBJECT_0 + 1) {
        EnterCriticalSection(&BufferAccess);

        c = ringBuffGetChar(&Ring);

        LeaveCriticalSection(&BufferAccess);

        printf("%c\n", c);

        ReleaseSemaphore(Empty, 1, NULL);

        Sleep(SLEEPING_TIME);
    }

    return 0;
}

int main() {

    HANDLE hProducer = NULL;
    HANDLE hProcessor = NULL;
    HANDLE hConsumer = NULL;

    DWORD ProducerID;
    DWORD ProcessorID;
    DWORD ConsumerID;

    Empty = CreateSemaphore(0, RING_SIZE, RING_SIZE, NULL);
    Full = CreateSemaphore(0, 0, RING_SIZE, NULL);
    FinishSignal = CreateSemaphore(0, 0, 2, NULL);

    if (Empty && Full && FinishSignal) {
        InitializeCriticalSection(&BufferAccess);

        hProducer = CreateThread(NULL, 0, &producer, (LPVOID)0, 0, &ProducerID);
        hProcessor = CreateThread(NULL, 0, &processor, (LPVOID)0, 0, &ProcessorID);
        hConsumer = CreateThread(NULL, 0, &consumer, (LPVOID)0, 0, &ConsumerID);
    
        if (!hProducer || !hProcessor || !hConsumer) {
            ReleaseSemaphore(FinishSignal, 2, NULL);
        }

        if (hProducer) {
            WaitForSingleObject(hProducer, INFINITE);
        }

        if (hProcessor) {
            WaitForSingleObject(hProcessor, INFINITE);
        }

        if (hConsumer) {
            WaitForSingleObject(hConsumer, INFINITE);
        }
    }

    if (hProducer != NULL) CloseHandle(hProducer);
    if (hProcessor != NULL) CloseHandle(hProcessor);
    if (hConsumer != NULL) CloseHandle(hConsumer);
    if (Empty) CloseHandle(Empty);
    if (Full) CloseHandle(Full);
    if (FinishSignal) CloseHandle(FinishSignal);

    DeleteCriticalSection(&BufferAccess);

    return 0;
}