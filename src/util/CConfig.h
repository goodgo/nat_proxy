#ifndef SRC_UTILITY_CCONFIG_H_
#define SRC_UTILITY_CCONFIG_H_
#include "CDefine.h"
#include "CInclude.h"
class CConfigTunData
{
	__CLASS_ADD_MEMBER__(private,int,nMtu);
	__CLASS_ADD_MEMBER__(private,int,nTxqueuelen);
	__CLASS_ADD_MEMBER__(private,CSTD_STR,szDstIpAddr);
	__CLASS_ADD_MEMBER__(private,CSTD_STR,szIpAddr);
	__CLASS_ADD_MEMBER__(private,CSTD_STR,szBrocastAddr);
	__CLASS_ADD_MEMBER__(private,CSTD_STR,szNetMaskAddr);
	__CLASS_ADD_MEMBER__(private,CSTD_STR,szName);
	__CLASS_ADD_MEMBER__(private,CSTD_STR,szLocalIp);
	__CLASS_ADD_MEMBER__(private,CSTD_STR,szRouteFiles);
};

class CConfigTimeData
{
	__CLASS_ADD_MEMBER__(private,TIMEVAL,readTunTimeMs);
	__CLASS_ADD_MEMBER__(private,TIMEVAL,socket5OutTimes);
	__CLASS_ADD_MEMBER__(private,TIMEVAL,socketoutTimes);
	__CLASS_ADD_MEMBER__(private,TIMEVAL,socketsendTimes);
	__CLASS_ADD_MEMBER__(private,uint32_t,uiIcmpActiveTimes);
	__CLASS_ADD_MEMBER__(private,uint32_t,uiLinkTimeouts);

};
class CConfigServerData
{
	__CLASS_ADD_MEMBER__(private,bool,bDemon);
	__CLASS_ADD_MEMBER__(private,bool,bDebug);
	__CLASS_ADD_MEMBER__(private,bool,bTimeToExit);
	__CLASS_ADD_MEMBER__(private,uint32_t,uiMaxClientCount);
	__CLASS_ADD_MEMBER__(private,uint32_t,uiMaxRecvBuffSize);
	__CLASS_ADD_MEMBER__(private,uint32_t,uiNoticePort);
	__CLASS_ADD_MEMBER__(public,int,uiShmID);
	__CLASS_ADD_MEMBER__(private,CSTD_STR,szLocalPort);
	__CLASS_ADD_MEMBER__(private,bool,bStd);
	__CLASS_ADD_MEMBER__(private,bool,bChkWhite);
	__CLASS_ADD_MEMBER__(private,CSTD_STR,szVpcAddr);
public:
	std::vector<CSTD_STR> _localips;
};
class CConfigOpensslData
{
	__CLASS_ADD_MEMBER__(private,CSTD_STR,szMd5Key1);
	__CLASS_ADD_MEMBER__(private,CSTD_STR,szMd5Key2);
};
class CConfigSocket5Data
{
	__CLASS_ADD_MEMBER__(private,CSTD_STR,szNodeIp);
	__CLASS_ADD_MEMBER__(private,CSTD_STR,szNodePort);
	__CLASS_ADD_MEMBER__(private,CSTD_STR,szLocalIp);
};

class CConfig : public util::SSingleton<CConfig>
{
public:
	bool	onInit(int argc,char *argv[]);
	bool	onRelease();
public:
	bool	onParase(int argc,char *argv[]);
	bool	onParaseTimeData(const char *szFilePath);
	bool	onParaseSocket5Data(const char *szFilePath);
	bool	onParaseServerData(const char *szFilePath);
public:
	bool	onHelper();
public:
	__CLASS_ADD_MEMBER__(public,CConfigServerData,serverData);
	__CLASS_ADD_MEMBER__(private,CConfigTimeData,timeData);
	__CLASS_ADD_MEMBER__(private,CConfigOpensslData,opensslData);
	__CLASS_ADD_MEMBER__(private,CConfigSocket5Data,socket5Data);
	__CLASS_ADD_MEMBER__(private,CSTD_STR,szFilePath);
};
#define gCConfig (CConfig::Instance())
#endif /* SRC_UTILITY_CCONFIG_H_ */
