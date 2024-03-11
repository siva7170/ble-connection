#include "BTHConnection.h"

SOCKET btSocket;
int error = 0;

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

bool BTHConnection::ConnectToServer(const char* straddr, const char* guid) {
    SOCKADDR_BTH sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.addressFamily = AF_BTH;
    str2guid(guid, &sockAddr.serviceClassId);
    //sockAddr.serviceClassId= string2guid(guid);
    sockAddr.port = BT_PORT_ANY;
    //str2ba(straddr, &sockAddr.btAddr);
    sockAddr.btAddr = 0x9078B2CDCCE0;

    if (connect(btSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR) {
        error = WSAGetLastError();
        std::cerr << "connect() failed, error: " << error;
        closesocket(btSocket);
        WSACleanup();
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

bool BTHConnection::CloseSocket() {
    closesocket(btSocket);
    return true;
}

void BTHConnection::FinalizeBT() {
    WSACleanup();
}

int BTHConnection::str2ba(const char* straddr, BTH_ADDR* btaddr)
{
    int i;
    unsigned int aaddr[6];
    BTH_ADDR tmpaddr = 0;

    if (sscanf_s(straddr, "%02x:%02x:%02x:%02x:%02x:%02x",
        &aaddr[0], &aaddr[1], &aaddr[2],
        &aaddr[3], &aaddr[4], &aaddr[5]) != 6)
        return 1;
    *btaddr = 0;
    for (i = 0; i < 6; i++) {
        tmpaddr = (BTH_ADDR)(aaddr[i] & 0xff);
        *btaddr = ((*btaddr) << 8) + tmpaddr;
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
