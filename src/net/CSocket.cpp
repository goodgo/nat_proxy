#include "CSocket.h"
#include "CInclude.h"
#include "CDefine.h"
#include "CCommon.h"

bool CSocket::onCreate(CSocketData &socket,const uint32_t &uiProtol)
{
	int  flag=1,len= sizeof ( int );
	if(onIsValid(socket))
	{
		DEBUGINFO("socket[%d] has created before", socket._nFd);
		return false;
	}
    switch(uiProtol)
    {
    case IPPROTO_TCP:
    	if ((socket._nFd = ::socket(AF_INET, SOCK_STREAM, 0)) == -1)
    	{
    		DEBUGINFO("create tcp socket failed: %m");
    		return false;
    	}
    	break;
    case IPPROTO_UDP:
    	if ((socket._nFd = ::socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    	{
    		DEBUGINFO("create udp socket failed: %m");
    		return false;
    	}
    	break;
    default:
    	DEBUGINFO("invalid proto: %d", uiProtol);
    	return false;
    }
    setsockopt(socket._nFd, SOL_SOCKET, SO_REUSEPORT, &flag, len); //端口重用，不设置将在服务器重新启动后出现绑定失败
    setsockopt(socket._nFd, SOL_SOCKET, SO_REUSEADDR, &flag, len); //端口重用，不设置将在服务器重新启动后出现绑定失败
    socket._nOptions = SO_REUSEADDR | SO_REUSEPORT;
    socket._nProtol = uiProtol;
    return true;
}

bool CSocket::onBind(CSocketData &socket,const uint16_t usPort, const char *szIp)
{
	if (!onIsValid(socket))
	{
		DEBUGINFO("socket invalid");
		return false;
	}

	socket._localSockaddr.sin_family = AF_INET;
	socket._localSockaddr.sin_port = htons(usPort);
    if (szIp == NULL || strlen(szIp) <= 0)
    	socket._localSockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    else
    	socket._localSockaddr.sin_addr.s_addr = inet_addr(szIp);

    if (-1 == bind(socket._nFd, (const struct sockaddr *)&socket._localSockaddr, sizeof(socket._localSockaddr)))
    {
    	DEBUGINFO("%s bind failed: %m", (szIp == NULL || strlen(szIp) <= 0) ? "" : szIp);
    	return false;
    }
    //if (!(bind(socket._nFd, (const struct sockaddr *)&socket._localSockaddr, sizeof(socket._localSockaddr)) != -1,"bind failed",,DEBUGINFO)
    if(usPort == 0)
    {
    	uint32_t len = sizeof(socket._localSockaddr);
    	getsockname(socket._nFd,(struct sockaddr *)&socket._localSockaddr,&len);//get local host
    }
    return true;
}
bool CSocket::onListen(const CSocketData &socket,int nBlock)
{
	if (!onIsValid(socket))
	{
		DEBUGINFO("socket invald");
		return false;
	}
	if (-1 == listen(socket._nFd, nBlock))
	{
		DEBUGINFO("listen failed: %m");
		return false;
	}
	return true;
}
bool CSocket::onAccept(const CSocketData &server,CSocketData &client)
{
    if (!onIsValid(server))
    {
    	DEBUGINFO("socket is invalid");
    	return false;
    }

    socklen_t in_len = sizeof(SOCKADDR_IN);
	client._nFd = accept(server._nFd, (struct sockaddr *)&client._remoteSockAddr, (socklen_t *)&in_len);
    if (-1 == client._nFd)
    {
    	DEBUGINFO("accept faild: %m");
    	return false;
    }
    return true;
}
bool CSocket::onConnect(CSocketData &client,const SOCKADDR_IN & remoteAddr)
{
	if (!onIsValid(client))
	{
		DEBUGINFO("socket is invalid");
		return false;
	}

	client._remoteSockAddr = remoteAddr;
	client._remoteSockAddr.sin_family = AF_INET;
	if(-1 == connect(client._nFd, (const struct sockaddr *)&client._remoteSockAddr, sizeof(client._remoteSockAddr)))
	{
		CSTD_STR szRemote;
		gCCommon->onSockAddr2String(remoteAddr,szRemote);
		DEBUGINFO("connect %s failed: %m",szRemote.c_str());
		return false;
	}
	return true;
}
bool CSocket::onConnect(CSocketData &client,const uint16_t &usPort, const char *szIp)
{
	if (!onIsValid(client))
	{
		DEBUGINFO("socket is invalid");
		return false;
	}

	if (!szIp || usPort <= 0)
	{
		DEBUGINFO("param is error");
		return false;
	}

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(usPort);
    addr.sin_addr.s_addr = inet_addr(szIp);
    client._remoteSockAddr = addr;

    if(-1 == connect(client._nFd, (const struct sockaddr *)&addr, sizeof(addr)))
    {
    	if(errno != EINPROGRESS)
    	{
    		DEBUGINFO("connect %s.%d failed: %m",szIp,usPort);
    		return false;
    	}
    }
    return true;
}

bool CSocket::onSetNonBlocking(CSocketData &client,bool flag)
{
	if (!onIsValid(client))
	{
		DEBUGINFO("socket is invalid");
		return false;
	}

	int opt;
	if ((opt = fcntl(client._nFd, F_GETFL, 0)) == -1)
	{
		DEBUGINFO("fcntl is failed: %m");
		return false;
	}
	if (flag)
	   opt |= O_NONBLOCK;
	else
	   opt &= ~O_NONBLOCK;

	if (-1 == fcntl(client._nFd, F_SETFL, opt))
	{
		DEBUGINFO("fcntl is failed");
		return false;
	}
	client._nOptions = opt;
	return true;
}
bool CSocket::onReuseaddr(CSocketData &client)
{
	if (!onIsValid(client))
	{
		DEBUGINFO("socket is invalid");
		return false;
	}

    int on = 1;
    if (-1 == setsockopt(client._nFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)))
    {
    	DEBUGINFO("set reuse addr failed: %m");
    	return false;
    }
    return true;
}
bool CSocket::onReuseport(CSocketData &client)
{
	if (!onIsValid(client))
	{
		DEBUGINFO("socket is invalid");
		return false;
	}

    int on = 1;
    if (-1 == setsockopt(client._nFd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)))
    {
    	DEBUGINFO("set reuse addr failed: %m");
    	return false;
    }
    return true;
}

bool CSocket::onClose(CSocketData &client)
{
	if (!onIsValid(client))
	{
		DEBUGINFO("socket is invalid");
		return false;
	}

    close(client._nFd);
    client._nFd = -1;
    return false;
}
bool CSocket::onReset(CSocketData &client)
{
	return onClose(client);
}
bool CSocket::onIsValid(const CSocketData &socket)
{
	return socket._nFd > 2;
}
bool CSocket::onSetSockRecvbuff(CSocketData &socket)
{
	if(!onIsValid(socket))
	{
		DEBUGINFO("socket is invalid");
		return false;
	}
	int nRecvBuf = 8 * 1024 * 1024;       //设置成8M
	setsockopt(socket._nFd,SOL_SOCKET, SO_RCVBUF, (const char *)&nRecvBuf,sizeof(nRecvBuf));
	return true;
}
bool CSocket::onSetSocketTimeout(CSocketData &socket)
{
	struct timeval timeout = {1,0};
	setsockopt(socket._nFd ,SOL_SOCKET,SO_SNDTIMEO,(char *)&timeout,sizeof(struct timeval));
	setsockopt(socket._nFd ,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(struct timeval));
	return true;
}

bool	CSocket::onCreateLocalUdp(CSocketData& udpsock)
{
	if(!onCreate(udpsock,IPPROTO_UDP))
	{
		return false;
	}
	if(!onSetSocketTimeout(udpsock))
	{
		DEBUGINFO("set udp socket time out failed.");
		return false;
	}
	if(!onSetSockRecvbuff(udpsock))
	{
		DEBUGINFO("set udp socket recv buffer failed.");
		return false;
	}

	if(!onBind(udpsock, 0))
	{
		//param._cRes = CErrorCode::ERROR_SYSTEM_TOO_OPEN;
		return false;
	}
	char pdst[128];
	gCCommon->onSockAddr2String(udpsock._localSockaddr,pdst);
	DEBUGINFO("create udp channel: %s", pdst);
	return true;
}
