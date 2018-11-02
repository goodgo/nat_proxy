#ifndef SRC_UTILITY_CCOMMON_H_
#define SRC_UTILITY_CCOMMON_H_
#include "CInclude.h"
#include "CDefine.h"

class CCommon:public util::SSingleton<CCommon>
{
public:
	char*	onErrstr(const char *);
public:
	void	onSleep(int times,int timeus);
public:
	bool	onDisplay(const char *szBuff,const int &nLen,const char* szFile,const char*szFunction,const int &nLine);
public:
	bool	onSockAddr2String(const char *szIp,const char *szPort,CSTD_STR &szDst);
	bool	onSockAddr2String(const struct in_addr inaddr,CSTD_STR &szDst);
	bool	onSockAddr2String(const struct sockaddr_in inaddr,CSTD_STR &szDst);
	bool	onSockAddr2String(const struct sockaddr_in inaddr,char *pdst);
	bool	onSockNet2String(const char *szData,const int &nDataLen,CSTD_STR &szDst);
	bool	onSockString2Addr(const char *szIp,const int &nPort,char *szAddr);
	bool	onSockAddr2Naddr(const SOCKADDR_IN &inaddr,char *szAddr);
	bool	onSockNet2Sockaddr(const char *szNetAddr,SOCKADDR_IN &inaddr);
	bool	onSockNet2Sockaddr(const char *szNetAddr,const int &nPort,SOCKADDR_IN &inaddr);
	bool	onSockNet2Sockaddr(const char *szNetAddr,const char *szPort,SOCKADDR_IN &inaddr);
	bool	onSockNet2Sockaddr(const uint32_t &szNetAddr,const USHORT &szPort,SOCKADDR_IN &inaddr);
	bool	onSockNet2SockString(const char *szNetAddr,const char *szPort,CSTD_STR &szDst);
public:
	bool	onSockAddrEqual(const SOCKADDR_IN &inaddr,const char *szIp,const char *szPort);
	bool	onSockAddrEqual(const SOCKADDR_IN &inaddr,const SOCKADDR_IN &inaddr2);
	bool	onProtolString(const int &nProtol,CSTD_STR &szProtolString);
public:
	bool	onChar2Byte(const char &cData,char &cHigh,char &cLow);
public:
	bool	onLoadLocalIp(std::vector<CSTD_STR>&iplist);
	bool	onIsInnerIp(const char *szIp);
	bool	onIsIp(const char *szIp);
public:
	bool	onChkLocalIp(const SOCKADDR_IN &inAddr);
public:
	bool	onDaemon();
public:
	uint32_t	onGetMs();
public:
	bool	onGetClientPrefixBuff(const void *pclient,char *szFixbuff);
public:
	const char *onGetProtocoString(const uint32_t &uiProtocol);
};

#define gCCommon (CCommon::Instance())
char *ERRSTR(const char *data,...);




//#define ENABLE_NET_SPEED_TEST
#ifdef ENABLE_NET_SPEED_TEST

class CNetSpeedCounter
{
public:
	CNetSpeedCounter(char dir, const char* msg, const char* userid);
	void Counter(uint32_t bytes);
	void Reset();

public:
	uint64_t	_bytes;
	struct timeval _tvStart;
	struct timeval _tvEnd;
	char 	_msg[32];
	const char*	_dir;
	char	_userid[32];
};
#endif



#endif /* SRC_UTILITY_CCOMMON_H_ */
