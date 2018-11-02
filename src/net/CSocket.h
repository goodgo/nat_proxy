#ifndef LINUXNET_CSOCKET_H_
#define LINUXNET_CSOCKET_H_
#include "CDefine.h"
#include "CInclude.h"
#include "CClientData.h"

class CSocket {
public:
    static bool onCreate(CSocketData &socket,const uint32_t &uiProtol);
    static bool onCreateLocalUdp(CSocketData& udpsock);
    //static bool onBind(CSocketData &socket);
    static bool onBind(CSocketData &socket,const uint16_t usPort = 0, const char *szIp = NULL);
    static bool onBind(CSocketData &socket,const SOCKADDR_IN & localAddr);
    static bool onListen(const CSocketData &socket,int backlog = SOMAXCONN);
    static bool onAccept(const CSocketData &server,CSocketData &client);
    static bool onConnect(CSocketData &client,const uint16_t &usPort, const char *szIp);
    static bool onConnect(CSocketData &client,const SOCKADDR_IN & remoteAddr);
    static bool onReuseaddr(CSocketData &socket);
    static bool onReuseport(CSocketData &socket);
    static bool onClose(CSocketData &socket);
    static bool onReset(CSocketData &socket);
    static bool onSetNonBlocking(CSocketData &socket,bool flag);
    static bool onKeepAlive(CSocketData &socket);
    static bool onSetSockRecvbuff(CSocketData &socket);
    static bool onSetSocketTimeout(CSocketData &socket);

    static bool isSameAddr(const SOCKADDR_IN& lsock, const SOCKADDR_IN& rsock) { return isSameIp(lsock, rsock) && isSamePort(lsock, rsock); }
    static bool isSameIp(const SOCKADDR_IN& lsock, const SOCKADDR_IN& rsock) { return lsock.sin_addr.s_addr == rsock.sin_addr.s_addr; }
    static bool isSamePort(const SOCKADDR_IN& lsock, const SOCKADDR_IN& rsock) { return lsock.sin_port == rsock.sin_port; }
public:
   static
   bool onIsValid(const CSocketData &socket);
};

#endif /* LINUXNET_SOCKET_H_ */
