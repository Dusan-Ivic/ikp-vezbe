#include <stdio.h>
#include <Windows.h>

CRITICAL_SECTION cs;

DWORD WINAPI print1(LPVOID lpParam) {
    char *c = (char *)lpParam;

    for (int i = 0; i < 100; i++) {
        while (*c != '1') {
            Sleep(1);
        }

        EnterCriticalSection(&cs);
        printf("%c", *c);
        *c = '2';
        LeaveCriticalSection(&cs);
    }

    return 0;
}

DWORD WINAPI print2(LPVOID lpParam) {
    char *c = (char *)lpParam;

    for (int i = 0; i < 100; i++) {
        while (*c != '2') {
            Sleep(1);
        }

        EnterCriticalSection(&cs);
        printf("%c", *c);
        *c = '3';
        LeaveCriticalSection(&cs);
    }

    return 0;
}

DWORD WINAPI print3(LPVOID lpParam) {
    char *c = (char *)lpParam;

    for (int i = 0; i < 100; i++) {
        while (*c != '3') {
            Sleep(1);
        }

        EnterCriticalSection(&cs);
        printf("%c", *c);
        *c = '1';
        LeaveCriticalSection(&cs);
    }

    return 0;
}

int main() {

    DWORD print1ID, print2ID, print3ID;
    HANDLE hPrint1, hPrint2, hPrint3;
    
    char c = '1';

    InitializeCriticalSection(&cs);

    hPrint1 = CreateThread(NULL, 0, &print1, &c, 0, &print1ID);
    hPrint2 = CreateThread(NULL, 0, &print2, &c, 0, &print2ID);
    hPrint3 = CreateThread(NULL, 0, &print3, &c, 0, &print3ID);

    int input = getchar();

    CloseHandle(hPrint1);
    CloseHandle(hPrint2);
    CloseHandle(hPrint3);

    DeleteCriticalSection(&cs);

    return 0;
}