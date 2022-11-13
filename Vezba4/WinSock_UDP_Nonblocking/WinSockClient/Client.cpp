#include <winsock2.h>
#include <stdio.h>
#include <conio.h>

#define SERVER_PORT 15000
#define CLIENT_SLEEP_TIME 50
#define OUTGOING_BUFFER_SIZE 1024
#define ACCESS_BUFFER_SIZE 1024
// for demonstration purposes we will hard code
// local host ip adderss
#define SERVER_IP_ADDERESS "127.0.0.1"

// Initializes WinSock2 library
// Returns true if succeeded, false otherwise.
bool InitializeWindowsSockets();

struct Operation {
    char operand1;
    char sign;
    char operand2;
};

int main(int argc,char* argv[])
{
    // Server address
    sockaddr_in serverAddress;
    // size of sockaddr structure    
	int sockAddrLen = sizeof(struct sockaddr);
	// buffer we will use to store message
    char outgoingBuffer[OUTGOING_BUFFER_SIZE];
    // port used for communication with server
    int serverPort = SERVER_PORT;
	// variable used to store function return value
	int iResult;

    // Initialize windows sockets for this process
    InitializeWindowsSockets();

    // Initialize serverAddress structure
    memset((char*)&serverAddress,0,sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP_ADDERESS);
    serverAddress.sin_port = htons((u_short)serverPort);

	// create a socket
    SOCKET clientSocket = socket(AF_INET,      // IPv4 address famly
								 SOCK_DGRAM,   // datagram socket
								 IPPROTO_UDP); // UDP

    // check if socket creation succeeded
    if (clientSocket == INVALID_SOCKET)
    {
        printf("Creating socket failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Set socket to nonblocking mode
    unsigned long int nonBlockingMode = 1;
    iResult = ioctlsocket(clientSocket, FIONBIO, &nonBlockingMode);

    if (iResult == SOCKET_ERROR)
    {
        printf("ioctlsocket failed with error: %ld\n", WSAGetLastError());
        return 1;
    }

    printf("Enter message:\n");

    // Read string from user into outgoing buffer
    gets_s(outgoingBuffer, OUTGOING_BUFFER_SIZE);

    struct Operation* operation = (struct Operation *)outgoingBuffer;

    iResult = sendto(clientSocket,
        (const char *)operation,
        sizeof(operation),
        0,
        (LPSOCKADDR)&serverAddress,
        sockAddrLen);

    if (iResult == SOCKET_ERROR)
    {
        printf("sendto failed with error: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    printf("Message sent to server.\n");

    char accessBuffer[ACCESS_BUFFER_SIZE];
    memset(accessBuffer, 0, ACCESS_BUFFER_SIZE);

    // Initialize select parameters
    FD_SET set;
    timeval timeVal;

    FD_ZERO(&set);
    // Add socket we will wait to read from
    FD_SET(clientSocket, &set);

    // Set timeouts to zero since we want select to return
    // instantaneously
    timeVal.tv_sec = 0;
    timeVal.tv_usec = 0;

    iResult = select(0 /* ignored */, &set, NULL, NULL, &timeVal);

    // lets check if there was an error during select
    if (iResult == SOCKET_ERROR)
    {
        fprintf(stderr, "select failed with error: %ld\n", WSAGetLastError());
    }

    // now, lets check if there are any sockets ready
    if (iResult == 0)
    {
        // there are no ready sockets, sleep for a while and check again
        Sleep(CLIENT_SLEEP_TIME);
    }

    iResult = recvfrom(clientSocket,
        accessBuffer,
        ACCESS_BUFFER_SIZE,
        0,
        (LPSOCKADDR)&serverAddress,
        &sockAddrLen);

    if (iResult == SOCKET_ERROR)
    {
        printf("recvfrom failed with error: %d\n", WSAGetLastError());
    }

    printf("Received from server: %s\n", accessBuffer);

	_getch();

    iResult = closesocket(clientSocket);
    if (iResult == SOCKET_ERROR)
    {
        printf("closesocket failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    iResult = WSACleanup();
    if (iResult == SOCKET_ERROR)
    {
        printf("WSACleanup failed with error: %ld\n", WSAGetLastError());
        return 1;
    }

    return 0;
}

bool InitializeWindowsSockets()
{
    WSADATA wsaData;
	// Initialize windows sockets library for this process
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        return false;
    }
	return true;
}
