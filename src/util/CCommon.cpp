#include "CCommon.h"

bool	CCommon::onSockAddr2String(const char *szIp,const char *szPort,CSTD_STR &szDst)
{
	if(!szIp || !szPort)
	{
		DEBUGINFO("invalid param!");
		return false;
	}
	SOCKADDR_IN inAddr;
	this->onSockNet2Sockaddr(szIp,szPort,inAddr);
	this->onSockAddr2String(inAddr,szDst);
	return true;
}
bool	CCommon::onSockAddr2String(const struct in_addr inaddr,CSTD_STR &szDst)
{
	static char szHost[32];
	strcpy(szHost,inet_ntoa(inaddr));
	szDst = szHost;
	return true;
}

bool	CCommon::onSockAddr2String(const struct sockaddr_in inaddr,CSTD_STR &szDst)
{
	static char szHost[64];
	if(inet_ntoa(inaddr.sin_addr))
	{
		sprintf(szHost,"%s:%d",inet_ntoa(inaddr.sin_addr),ntohs(inaddr.sin_port));
	}
	szDst = szHost;
	return true;
}
bool	CCommon::onSockAddr2String(const struct sockaddr_in inaddr,char *pdst)
{
	if(pdst && inet_ntoa(inaddr.sin_addr))
	{
		sprintf(pdst,"%s:%d",inet_ntoa(inaddr.sin_addr),ntohs(inaddr.sin_port));
		return true;
	}
	return false;
}

bool	CCommon::onSockNet2String(const char *szData,const int &nDataLen,CSTD_STR &szDst)
{
	if(!szData)
	{
		DEBUGINFO("param is error");
		return false;
	}
	if(nDataLen != 4 && nDataLen != 6)
	{
		DEBUGINFO("param is error");
		return false;
	}
	struct sockaddr_in inaddr;
	switch(nDataLen)
	{
	case 4:
		{
			memcpy(&inaddr.sin_addr,szData,nDataLen);
			this->onSockAddr2String(inaddr.sin_addr,szDst);
		}
		break;
	case 6:
		{
			memcpy(&inaddr.sin_port,szData+4,2);
			memcpy(&inaddr.sin_addr,szData,nDataLen);
			this->onSockAddr2String(inaddr,szDst);
		}
		break;
	default:
		return false;
	}
	return true;
}
bool	CCommon::onSockString2Addr(const char *szIp,const int &nPort,char *szAddr)
{
	if(!szIp || !szAddr)
	{
		DEBUGINFO("param is error");
		return false;
	}

	SOCKADDR_IN inaddr;
	inaddr.sin_addr.s_addr = inet_addr(szIp);
	inaddr.sin_port = htons(nPort);
	memcpy(szAddr,&inaddr.sin_addr.s_addr,4);
	memcpy(szAddr+4,&inaddr.sin_port,2);
	return true;
}
bool	CCommon::onSockAddr2Naddr(const SOCKADDR_IN &inaddr,char *szAddr)
{
	if(!szAddr)
	{
		DEBUGINFO("param is error");
		return false;
	}
	memcpy(szAddr,&inaddr.sin_addr.s_addr,4);
	memcpy(szAddr+4,&inaddr.sin_port,2);
	return true;
}
bool	CCommon::onSockNet2Sockaddr(const char *szNetAddr,SOCKADDR_IN &inaddr)
{
	if(!szNetAddr)
	{
		DEBUGINFO("param is error");
		return false;
	}
	memcpy(&inaddr.sin_addr.s_addr,szNetAddr,4);
	memcpy(&inaddr.sin_port,szNetAddr+4,2);
	return true;
}

bool	CCommon::onSockNet2Sockaddr(const char *szNetAddr,const int &nPort,SOCKADDR_IN &inaddr)
{
	if (!szNetAddr)
	{
		DEBUGINFO("param is error");
		return false;
	}
	memcpy(&inaddr.sin_addr.s_addr,szNetAddr,4);
	memcpy(&inaddr.sin_port,&nPort,2);
	return true;
}
bool	CCommon::onSockNet2Sockaddr(const uint32_t &uiNetAddr,const USHORT &usPort,SOCKADDR_IN &inaddr)
{
	inaddr.sin_addr.s_addr = uiNetAddr;
	inaddr.sin_port = usPort;
	return true;
}
bool	CCommon::onSockNet2Sockaddr(const char *szNetAddr,const char *szPort,SOCKADDR_IN &inaddr)
{
	if (!szNetAddr)
	{
		DEBUGINFO("param is error");
		return false;
	}
	memcpy(&inaddr.sin_addr.s_addr,szNetAddr,4);
	memcpy(&inaddr.sin_port,szPort,2);
	return true;
}

bool	CCommon::onSockNet2SockString(const char *szNetAddr,const char *szPort,CSTD_STR &szDst)
{
	SOCKADDR_IN inaddr;
	memset(&inaddr, 0, sizeof(SOCKADDR_IN));
	this->onSockNet2Sockaddr(szNetAddr, szPort,inaddr);
	onSockAddr2String(inaddr,szDst);
	return true;
}

bool	CCommon::onDisplay(const char *szBuff,const int &nBuffLen,const char* szFile,const char*szFunction,const int &nLine)
{
	char szbuff[BUFFMAXLEN];
	char value[5];
	int i = 0,nMaxBuffLen;
	nMaxBuffLen = nBuffLen;
	if(nBuffLen >= 2048)
		nMaxBuffLen = 2048-1;
	bzero(szbuff,sizeof(szbuff));
	//szbuff[0]='\n';
	//szbuff[1]='\r';
	for(i = 0 ; i < nMaxBuffLen;i++)
	{
		bzero(value,sizeof(value));
		sprintf(value,"%02X-",szBuff[i]&0xff);
		strcat(szbuff,value);
	}
	DEBUGINFO("%s:%d ->[len:%d][%s]",szFunction,nLine,nMaxBuffLen,szbuff);

	if(0)
	{
		std::ofstream outfile("/usr/local/log/errorlog/snd_buff.log", std::ios_base::out | std::ios_base::app);
		outfile << szbuff << std::endl;
		outfile.flush();
		outfile.close();
	}
	return true;
}

char *CCommon::onErrstr(const char *info)
{
	static char tmpStr[128];
	if(!info)
	{
		return (char *)info;
	}
	if(strlen(info) >= sizeof(tmpStr))
	{
		return (char *)info;
	}
	sprintf(tmpStr,"%s errno %d -> errorstr %m",info,errno);
	return tmpStr;
}
void	CCommon::onSleep(int times,int timeus)
{
	struct timeval time;
	time.tv_sec = times;
	time.tv_usec = timeus;
	select(0,NULL,NULL,NULL,&time);
}

bool	CCommon::onSockAddrEqual(const SOCKADDR_IN &inaddr,const char *szIp,const char *szPort)
{
	if (!szIp || !szPort)
	{
		DEBUGINFO("param is error");
		return false;
	}

	SOCKADDR_IN inaddr2;
	memcpy(&inaddr2.sin_addr.s_addr,szIp,4);
	memcpy(&inaddr2.sin_port,szPort,2);
	if (!this->onSockAddrEqual(inaddr,inaddr2))
	{
		return false;
	}
	return true;
}
bool	CCommon::onSockAddrEqual(const SOCKADDR_IN &inaddr,const SOCKADDR_IN &inaddr2)
{
	bool b = true;
	b = b && (inaddr.sin_addr.s_addr == inaddr2.sin_addr.s_addr);
	b = b && (inaddr.sin_port == inaddr2.sin_port);
	return b;
}
bool	CCommon::onProtolString(const int &nProtol,CSTD_STR &szProtolString)
{
	switch(nProtol)
	{
	case IPPROTO_UDP:szProtolString= "udp";break;
	case IPPROTO_TCP:szProtolString = "tcp";break;
	case IPPROTO_ICMP:szProtolString = "icmp";break;
	default:
		szProtolString = "undef protol";
		return false;
	}
	return true;
}
bool	CCommon::onChar2Byte(const char &cData,char &cHigh,char &cLow)
{
	if (!cData != 0);
	cHigh = (cData & 0xf0)/0x10;
	cLow = (cData & 0x0f)%0x10;
	return true;
}
bool	CCommon::onChkLocalIp(const SOCKADDR_IN &inAddr)
{
	struct ifaddrs * ifAddrStruct = NULL;
	void * tmpAddrPtr = NULL;
	if (0 != getifaddrs(&ifAddrStruct))
	{
		DEBUGINFO("getifaddrs failed!");
		return false;
	}

	char addressBuffer[INET_ADDRSTRLEN];
	bool bProcess = false;
	struct ifaddrs *pif = ifAddrStruct;
	CSTD_STR szAddr,szLocalAddr;
	SOCKADDR_IN tmpAddr = inAddr;

	tmpAddr.sin_port = 0;
	gCCommon->onSockAddr2String(tmpAddr,szAddr);

	while (ifAddrStruct != NULL)
	{
		if(ifAddrStruct->ifa_addr)
		{
			if (ifAddrStruct->ifa_addr->sa_family == AF_INET // check it is IP4
						&& strcmp(ifAddrStruct->ifa_name, "lo") != 0)
					{
						// is a valid IP4 Address
						bzero(addressBuffer,sizeof(char)*INET_ADDRSTRLEN);
						tmpAddrPtr = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
						inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
						szLocalAddr = addressBuffer;
						szLocalAddr += ":0";
						if(szLocalAddr == szAddr)
						{
							bProcess = true;
							break;
						}
					}
		}
		ifAddrStruct = ifAddrStruct->ifa_next;
	}
	freeifaddrs(pif);
	return bProcess;
	return true;
}
bool	CCommon::onLoadLocalIp(std::vector<CSTD_STR>&iplist)
{
	struct ifaddrs * ifAddrStruct = NULL;
	if (0 != getifaddrs(&ifAddrStruct))
	{
		DEBUGINFO("getifaddrs failed!");
		return false;
	}

	void * tmpAddrPtr = NULL;
	struct ifaddrs *pif = ifAddrStruct;
	char addressBuffer[INET_ADDRSTRLEN]={0};
	CSTD_STR szLocalIp;
	iplist.clear();
	while (ifAddrStruct != NULL)
	{
		if(ifAddrStruct->ifa_addr)
		{
			if (ifAddrStruct->ifa_addr->sa_family == AF_INET // check it is IP4
						&& strcmp(ifAddrStruct->ifa_name, "lo") != 0)
					{
						// is a valid IP4 Address
						bzero(addressBuffer,sizeof(char)*INET_ADDRSTRLEN);
						tmpAddrPtr = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
						inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
						szLocalIp = addressBuffer;
						iplist.push_back(szLocalIp);
						//DEBUGINFO("local ip -> %s",szLocalIp.c_str());
					}
		}
		ifAddrStruct = ifAddrStruct->ifa_next;
	}

	freeifaddrs(pif);
	return true;
}
bool	CCommon::onIsIp(const char *szIp)
{

	if (!szIp);
	int a,b,c,d;
	int count = 0, flag = 0;

	for(size_t i = 0; i < strlen(szIp); i++)
	{
		if(szIp[i] == '.')
		{
			count++;
			continue;
		}
		else if(szIp[i]<'0' || szIp[i] > '9')
		{
			flag = 1;
		}
	}
	if(count != 3 || flag == 1)
	{
		return false;
	}
	if (4==sscanf(szIp,"%d.%d.%d.%d",&a,&b,&c,&d))
	{
		if (0<=a && a<=255
		 && 0<=b && b<=255
		 && 0<=c && c<=255
		 && 0<=d && d<=255)
		{
			return true;
		}
	}
	else
	{
		return false;
	}
	return true;
}
bool CCommon::onIsInnerIp(const char *szIp)
{
	if (!szIp)
	{
		return false;
	}
	/*A:*/
	uint32_t A1 = ntohl( inet_addr("10.0.0.0") );
	uint32_t A2 = ntohl( inet_addr("10.255.255.255") );
	/*B:*/
	uint32_t B1 = ntohl( inet_addr("172.16.0.0") );
	uint32_t B2 = ntohl( inet_addr("172.31.255.255") );
	/*C:*/
	uint32_t C1 = ntohl( inet_addr("192.168.0.0") );
	uint32_t C2 = ntohl( inet_addr("192.168.255.255") );
	/*127:*/
	uint32_t X = ntohl( inet_addr(szIp) );

	CSTD_STR csIp = szIp;
	if(csIp.find("127.0.0.1") != CSTD_STR::npos)
	{
		return false;
	}
	else if ( (X > A1 && X < A2) ||
			(X > B1 && X < B2) ||
			(X > C1 && X < C2)  )
	{
		return false;
	}
	return true;
}
uint32_t	CCommon::onGetMs()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec*1000 + tv.tv_usec/1000)+100;
}
bool	CCommon::onGetClientPrefixBuff(const void *pclient,char *szFixbuff)
{
	if (!pclient || !szFixbuff)
	{
		DEBUGINFO("param is null");
		return false;
	}
//	const CClientData *param = (const CClientData *)pclient;
//	if(param->_userData._pszUserId)
//	{
//		strcat(szFixbuff,(char*)param->_userData._pszUserId);
//		strcat(szFixbuff," ");
//	}
//	if(param->_userData._pszGameId)
//	{
//		strcat(szFixbuff,(char*)param->_userData._pszGameId);
//		strcat(szFixbuff," ");
//	}
	return true;
}
const char *CCommon::onGetProtocoString(const uint32_t &uiProtocol)
{
	switch(uiProtocol)
	{
	case IPPROTO_UDP:return "udp";
	case IPPROTO_TCP:return "tcp";
	case IPPROTO_ICMP:return "icmp";
	default :
		return "undefine protol";
	}
}
bool	CCommon::onDaemon()
{
 	if (signal(SIGINT, SIG_IGN) == SIG_ERR) return false;
	if (signal(SIGHUP, SIG_IGN) == SIG_ERR) return false;
	if (signal(SIGQUIT, SIG_IGN) == SIG_ERR) return false;
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) return false;
	if (signal(SIGTTOU, SIG_IGN) == SIG_ERR) return false;
	if (signal(SIGTTIN, SIG_IGN) == SIG_ERR) return false;


    int  fd;
    switch (fork())
    {
    case -1:
        DEBUGINFO( "fork() failed");
        return false;
    case 0:
        break;

    default:
    	exit(9);
    }
    /*if (setsid() == -1)
    {
		DEBUGINFO("setsid() failed");
		break;
    }

	umask(0);*/
	fd = open("/dev/null", O_RDWR);
	if (fd == -1)
	{
	   DEBUGINFO("open(\"/dev/null\") failed");
	   return false;
	}

	if (dup2(fd, STDIN_FILENO) == -1)
	{
	   DEBUGINFO("dup2(STDIN) failed");
	   return false;
	}

	if (dup2(fd, STDOUT_FILENO) == -1)
	{
	   DEBUGINFO("dup2(STDOUT) failed");
	   return false;
	}

	#if 0
	if (dup2(fd, STDERR_FILENO) == -1) {
	   ngx_log_error(NGX_LOG_EMERG, log, ngx_errno, "dup2(STDERR) failed");
	   return NGX_ERROR;
	}
	#endif

	if (fd > STDERR_FILENO)
	{
	   if (close(fd) == -1)
	   {
		  DEBUGINFO("dup2(STDOUT) failed");
		  return false;
	   }
	}
	return true;
}


#ifdef ENABLE_NET_SPEED_TEST

CNetSpeedCounter::CNetSpeedCounter(char dir, const char* msg, const char* userid)
: _bytes(0)
, _dir(dir?"下":"上")
{
	bzero(_msg, 32);
	if (msg)
		memcpy(_msg, msg, strlen(msg) < 32 ? strlen(msg) : 32);

	bzero(_userid, 32);
	if (userid)
		memcpy(_userid, userid, strlen(userid) < 32 ? strlen(userid) : 32);

	gettimeofday(&_tvStart, NULL);
	gettimeofday(&_tvEnd, NULL);
}

void CNetSpeedCounter::Counter(uint32_t bytes)
{
	_bytes += bytes;

	gettimeofday(&_tvEnd, NULL);
	if (_tvEnd.tv_sec - _tvStart.tv_sec > 5)
	{
		DEBUGINFO("[%s][%s]The rate of %s: %.2f KB/s", _dir, _userid, _msg, (_bytes/1024.0)/(_tvEnd.tv_sec - _tvStart.tv_sec));
		Reset();
	}
}
void CNetSpeedCounter::Reset()
{
	_bytes = 0;
	gettimeofday(&_tvStart, NULL);
}

#endif
