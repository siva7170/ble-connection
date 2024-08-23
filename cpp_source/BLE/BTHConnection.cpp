#include "BTHConnection.h"

SOCKET btSocket;
SOCKADDR_BTH sockAddr;
int error = 0;

/*
     lastError 

     0 - No error
     1 - Create WSAStartup failure
     2 - Create socket failure
     3 - Create connect failure
     4 - Max attempts exceeded to reconnect
     5 - Socket error while sending data

*/
int lastError = 0;
int timeOutForReconnect=5000;
int noofAttempt=0;
const char* btaddr;
const char* btguid;
std::string connectionStatus="DISCONNECTED";
bool isDisconnectedRequest=false;

bool BTHConnection::InitializeBT() {
    WSADATA data;
    int startupError = WSAStartup(MAKEWORD(2, 2), &data);
    bool initializationSuccess = startupError == 0;
    if (initializationSuccess) {
        initializationSuccess = LOBYTE(data.wVersion) == 2 && HIBYTE(data.wVersion) == 2;
        if (!initializationSuccess) {
            FinalizeBT();
        }
    }

    return initializationSuccess;
}

bool BTHConnection::CreateSocket() {
    btSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
    if (btSocket == INVALID_SOCKET) {
        error = WSAGetLastError();
        std::cerr << "socket() failed, error: " << error;
        WSACleanup();
        return false;
    }
    return true;
}

void BTHConnection::SetBthInfo(const char* straddr, const char* guid, int timeoutForReconnect, int noOfAttempt) {
    btaddr = straddr;
    btguid = guid;

    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.addressFamily = AF_BTH;
    char* btguidCopy = new char[strlen(btguid) + 1];
    strcpy(btguidCopy, btguid);
    str2guid(btguidCopy, &sockAddr.serviceClassId);
    //sockAddr.serviceClassId= string2guid(guid);
    sockAddr.port = BT_PORT_ANY;
    char* btaddrCopy = new char[strlen(btaddr) + 1];
    strcpy(btaddrCopy, btaddr);
    str2ba(btaddrCopy, &sockAddr.btAddr);
    //sockAddr.btAddr = 0x9078B2CDCCE0;
    timeOutForReconnect = timeoutForReconnect;
    noofAttempt = noOfAttempt;
    delete[] btguidCopy;
    delete[] btaddrCopy;


    return;
}

bool BTHConnection::Connect() {
    if (!InitializeBT()) {
        lastError = 1;
        return false;
    }

    if (!CreateSocket()) {
        lastError = 2;
        return false;
    }


    int attempt = 1;

    printf("btguid %s\n", btguid);
    printf("btaddr %s\n", btaddr);

    if (connect(btSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR) {
        error = WSAGetLastError();
        lastError = 3;
        std::cerr << "connect() failed, error: " << error;
        closesocket(btSocket);
        WSACleanup();

        return false;
    }
    connectionStatus="CONNECTED";
    lastError = 0;
    return true;
}

bool BTHConnection::ConnectToServer(const char* straddr, const char* guid) {
    SOCKADDR_BTH sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.addressFamily = AF_BTH;
    str2guid(guid, &sockAddr.serviceClassId);
    //sockAddr.serviceClassId= string2guid(guid);
    sockAddr.port = BT_PORT_ANY;
    str2ba(straddr, &sockAddr.btAddr);
    //sockAddr.btAddr = 0x9078B2CDCCE0;

    if (connect(btSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR) {
        error = WSAGetLastError();
        std::cerr << "connect() failed, error: " << error;
        closesocket(btSocket);
        WSACleanup();
        return false;
    }
    return true;
}

bool BTHConnection::IsConnected() {
    /*char buffer[1024];
    int result = recv(btSocket, buffer, sizeof(buffer), MSG_PEEK | MSG_);
    if (result == 0 || result == SOCKET_ERROR) {
        lastError = 2;
        return false;
    }
    else {
        lastError = 0;
        return true;
    }*/
    /*int r = recv(btSocket, NULL, 0, 0);
    if (r == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET) {
        return false;
    }
    else {
        return true;
    }*/
    // Check the connection status
    //int result;
    //int connectTime;
    //int size = sizeof(connectTime);
    //result = getsockopt(btSocket, SOL_SOCKET, SO_, (char*)&connectTime, &size);
    //if (result == SOCKET_ERROR) {
    //    // Handle error
    //}

    //if (connectTime >= 0) {
    //    // The socket is connected
    //}
    //else {
    //    // The socket is not connected
    //}
    //int error_code;
    //int error_code_size = sizeof(error_code);

    //if (getsockopt(btSocket, SOL_SOCKET, SO_ERROR, (char*)&error_code, &error_code_size) == SOCKET_ERROR) {
    //    std::cerr << "getsockopt(SO_ERROR) failed: " << WSAGetLastError() << std::endl;
    //    return false;
    //}
    //else {
    //    if (error_code != 0) {
    //        // An error occurred on the socket
    //        std::cerr << "Socket error: " << error_code << std::endl;
    //        return false;
    //    }
    //    else {
    //        // No error
    //         std::cout << "No error on the socket." << error_code << std::endl;
    //        return true;
    //    }
    //}
   /* int res = send(btSocket, NULL, 0, MSG_OOB);

    if (res == SOCKET_ERROR)
    {
        lastError = 5;
        std::cerr << "Send() failed with error code: " << WSAGetLastError() << std::endl;
        closesocket(btSocket);
        WSACleanup();
        return false;
    }
    else
    {
        lastError = 0;
        std::cout << "Send() is ok" << std::endl;
        std::cout << "Bytes sent: " << res << std::endl;
        return true;
    }*/
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(btSocket, &readfds);

    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    int selectResult = select(0, &readfds, NULL, NULL, &tv);
    if (selectResult > 0) {
        char buffer[1];
        int recvResult = recv(btSocket, buffer, 1, MSG_PEEK);
        if (recvResult == 0) {
            return false;
        }
    }
    else if (selectResult == SOCKET_ERROR) {
        return false;
    }
    return true;
}

bool BTHConnection::SendData(const char* msgToServer) {
    const char* sendbuf = msgToServer;
    int recvbuflen = DEFAULT_BUFLEN;
    char recvbuf[DEFAULT_BUFLEN] = "";

    int res = send(btSocket, sendbuf, (int)strlen(sendbuf), MSG_OOB);

    if (res == SOCKET_ERROR)
    {
        std::cerr << "Send() failed with error code: " << WSAGetLastError() << std::endl;
        closesocket(btSocket);
        WSACleanup();
        return false;
    }
    else
    {
        std::cout << "Send() is ok" << std::endl;
        std::cout << "Bytes sent: " << res << std::endl;
        return true;
    }
}

bool BTHConnection::SendDataToServer(const char* msgToServer) {
    bool isConnected = IsConnected();

    int attempt = 1;

    while (true) {
        if (attempt>5) {
            lastError = 4;
            return false;
            break;
        }
        if (!isConnected) {
            CloseSocket();
            FinalizeBT();

            if (Connect()){
                break;
            }
            else {
                attempt++;
                Sleep(5000);
            }
        }
        else 
        {
            break;
        }
    }


    const char* sendbuf = msgToServer;
    int recvbuflen = DEFAULT_BUFLEN;
    char recvbuf[DEFAULT_BUFLEN] = "";

    int res = send(btSocket, sendbuf, (int)strlen(sendbuf), MSG_OOB);

    if (res == SOCKET_ERROR)
    {
        lastError = 5;
        std::cerr << "Send() failed with error code: " << WSAGetLastError() << std::endl;
        closesocket(btSocket);
        WSACleanup();
        return false;
    }
    else
    {
        lastError = 0;
        std::cout << "Send() is ok" << std::endl;
        std::cout << "Bytes sent: " << res << std::endl;
        return true;
    }
}

bool BTHConnection::SuspendSendForReceiveData() {
   /* int res = shutdown(BTHConnection::btSocket, SD_SEND);
    if (res == SOCKET_ERROR)
    {
        std::cout << "Shutdown() failed with error code: " << WSAGetLastError() << std::endl;
        closesocket(BTHConnection::btSocket);
        WSACleanup();
        return false;
    }
    else
    {
        std::cout << "Shutdown() is working for receive data" << std::endl;
    }*/

    return true;
}

bool BTHConnection::ReceiveData() {
    /*int recvbuflen = DEFAULT_BUFLEN;
    char recvbuf[DEFAULT_BUFLEN] = "";
    int res = recv(BTHConnection::btSocket, recvbuf, recvbuflen, 0);
    std::cout << " " << res << " from sender" << std::endl;
    if (res > 0)
    {
        std::cout << " " << res << " Bytes received from sender" << std::endl;
    }
    else if (res == 0)
    {
        std::cout << "Client disconnected" << std::endl;
        return false;
    }
    else
    {
        std::cout << "recv() failed with code: " << WSAGetLastError() << std::endl;
        return false;
    }*/
    int res;
    int recvbuflen = DEFAULT_BUFLEN;
    char recvbuf[DEFAULT_BUFLEN] = "";
    do {
        res = recv(btSocket, recvbuf, recvbuflen, 0);
        if (res > 0) {
            printf("Bytes received: %d\n", res);
            std::cout << "Data: " << recvbuf << std::endl;
            
        }
        else if (res == 0)
            printf("Connection closed\n");
        else
            printf("recv failed: %d\n", WSAGetLastError());
    } while (res > 0);
    return true;
}

bool BTHConnection::Reconnect() {
    bool isConnected = IsConnected();
    if(connectionStatus=="CONNECTED" || connectionStatus=="RECONNECTING"){
        connectionStatus="RECONNECTING";
        int attempt = 1;

        while (true) {
            if(isDisconnectedRequest){
                CloseSocket();
                FinalizeBT();
                connectionStatus="DISCONNECTED";
                lastError = 4;
                return false;
                break;
            }
            if (noofAttempt!=0 && attempt > noofAttempt) {
                lastError = 4;
                return false;
                break;
            }
            if (!isConnected) {
                CloseSocket();
                FinalizeBT();

                if (Connect()) {
                    connectionStatus="RECONNECTED";
                    printf("Reconnected \n");
                    return true;
                    break;
                }
                else {
                    printf("Attempt %d\n",attempt);
                    attempt++;
                    Sleep(timeOutForReconnect);
                }
            }
            else
            {
                break;
            }
        }
    }
    

    return isConnected;
}

bool BTHConnection::ReceiveDataFromServer() {
    bool isConnected = IsConnected();

    int attempt = 1;

    while (true) {
        if(isDisconnectedRequest){
                lastError = 4;
                CloseSocket();
                FinalizeBT();
                connectionStatus="DISCONNECTED";
                return false;
                break;
        }
        if (attempt > 5) {
            lastError = 4;
            return false;
            break;
        }
        if (!isConnected) {
            CloseSocket();
            FinalizeBT();

            if (Connect()) {
                break;
            }
            else {
                attempt++;
                Sleep(5000);
            }
        }
        else
        {
            break;
        }
    }


    int res;
    int recvbuflen = DEFAULT_BUFLEN;
    char recvbuf[DEFAULT_BUFLEN] = "";
    do {
        res = recv(btSocket, recvbuf, recvbuflen, 0);
        if (res > 0) {
            lastError = 0;
            printf("Bytes received: %d\n", res);
            std::cout << "Data: " << recvbuf << std::endl;
        }
        else if (res == 0) {
            printf("Connection closed\n");
            if (Reconnect()) {
                res = 1;
            }
            else {
                lastError = 6;
            }
        }           
        else {
            printf("recv failed: %d\n", WSAGetLastError());
            if (Reconnect()) {
                res = 1;
            }
            else {
                lastError = 6;
            }
        }
           
    } while (res > 0);
    return true;
}

bool BTHConnection::CloseSocket() {
    closesocket(btSocket);
    return true;
}

void BTHConnection::FinalizeBT() {
    WSACleanup();
}

bool BTHConnection::Disconnect() {
    //connectionStatus="DISCONNECTED";
    // CloseSocket(); 
    // WSACleanup();
    isDisconnectedRequest=true;
    lastError = 0;
    return true;
}

int BTHConnection::GetLastError() {
    return lastError;
}

int BTHConnection::str2ba(const char* straddr, BTH_ADDR* btaddrObj)
{
    int i;
    unsigned int aaddr[6];
    BTH_ADDR tmpaddr = 0;

    if (sscanf_s(straddr, "%02x:%02x:%02x:%02x:%02x:%02x",
        &aaddr[0], &aaddr[1], &aaddr[2],
        &aaddr[3], &aaddr[4], &aaddr[5]) != 6)
        return 1;
    *btaddrObj = 0;
    for (i = 0; i < 6; i++) {
        tmpaddr = (BTH_ADDR)(aaddr[i] & 0xff);
        *btaddrObj = ((*btaddrObj) << 8) + tmpaddr;
    }
    return 0;
}

int BTHConnection::str2guid(const char* s, GUID* guidObj) {
    GUID guid = { 0 };  // Initialize with zeros

    // Parse the string using sscanf
    unsigned long p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10;
    int err = sscanf_s(s, "%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
        &p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10);

    if (err == 11) {
        guid.Data1 = p0;
        guid.Data2 = static_cast<unsigned short>(p1);
        guid.Data3 = static_cast<unsigned short>(p2);
        guid.Data4[0] = static_cast<unsigned char>(p3);
        guid.Data4[1] = static_cast<unsigned char>(p4);
        guid.Data4[2] = static_cast<unsigned char>(p5);
        guid.Data4[3] = static_cast<unsigned char>(p6);
        guid.Data4[4] = static_cast<unsigned char>(p7);
        guid.Data4[5] = static_cast<unsigned char>(p8);
        guid.Data4[6] = static_cast<unsigned char>(p9);
        guid.Data4[7] = static_cast<unsigned char>(p10);
        
        *guidObj = guid;
    }

    return 0;
}
