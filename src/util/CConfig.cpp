#include "CConfig.h"
#include "CInclude.h"
#include "CCommon.h"
#define BLACK_USEID_PATH		"/tmp/3proxy.userblack.ry"
#define WHITE_USEID_PATH		"/tmp/3proxy.userwhite.ry"

bool	CConfig::onInit(int argc,char *argv[])
{
	if (!this->onParase(argc,argv))
	{
		DEBUGINFO("parase failed");
	}
	return true;
}

bool	CConfig::onRelease()
{
	return true;
}

bool	CConfig::onParase(int argc,char *argv[])
{
	if (argc <= 2 || !argv)
	{
		DEBUGINFO("argc is <= 0 ,plz check");
		return false;
	}

	int index = 1;

	while(index < argc)
	{
		switch(argv[index][1])
		{
		case 'f'://conf file
			if(index + 1 < argc)
			{
				this->onParaseTimeData(argv[index+1]);
				this->onParaseSocket5Data(argv[index+1]);
				this->onParaseServerData(argv[index+1]);
				this->_szFilePath = argv[index+1];
				index ++;
			}
			break;
		case 'd':
		case 'D':
			{
				if(strcmp(argv[index]+1,"debug") == 0 )
				{
					this->_serverData.onSetbDebug(true);
				}
				else if(strcmp(argv[index]+1,"daemon") == 0)
				{
					this->_serverData.onSetbDemon(true);
				}
			}
			break;
		case 'h':
		case 'H':
		{
			this->onHelper();
		}
		break;
		default:
			break;
		}
		index++;
	}
	return true;
}

bool	CConfig::onParaseTimeData(const char *szFilePath)
{
	if (!szFilePath)
	{
		DEBUGINFO("conf file param is null ");
		return false;
	}
	if (access(szFilePath,F_OK) != 0)
	{
		DEBUGINFO("szFilePath can't find in disk,plz check ");
		return false;
	}

	CIniFile iniFile;
	TIMEVAL timeval;
	int readTimeMs = 0;
	if (!iniFile.Load(szFilePath))
	{
		DEBUGINFO("open conf failed,plz check");
		return true;
	}

	readTimeMs = iniFile.GetIntValue("socket5timeMs","time",30000);
	MS2TIMEVAL(readTimeMs,timeval)
	this->_timeData.onSetsocket5OutTimes(timeval);


	readTimeMs = iniFile.GetIntValue("socketouttimeMs","time",1);
	MS2TIMEVAL(readTimeMs,timeval)
	this->_timeData.onSetsocketoutTimes(timeval);


	readTimeMs = iniFile.GetIntValue("icmpalivetimeSec","time",5);
	this->_timeData.onSetuiIcmpActiveTimes(readTimeMs);

	readTimeMs = iniFile.GetIntValue("linktimeSec","time",10*60*60);
	this->_timeData.onSetuiLinkTimeouts(readTimeMs);
	return true;
}
bool	CConfig::onParaseSocket5Data(const char *szFilePath)
{
	if (!szFilePath)
	{
		DEBUGINFO("conf file param is null ");
		return true;
	}
	if (access(szFilePath,F_OK) != 0)
	{
		DEBUGINFO("szFilePath can't find in disk,plz check ");
		return true;
	}
	CIniFile iniFile;
	CSTD_STR szLocalIp;
	if (!iniFile.Load(szFilePath))
	{
		DEBUGINFO("open conf failed,plz check");
		return true;
	}

	this->_socket5Data.onSetszNodeIp(iniFile.GetStrValue("serverip","node","101.37.168.240"));
	this->_socket5Data.onSetszNodePort(iniFile.GetStrValue("serverort","node","8769"));

	return true;
}
bool	CConfig::onParaseServerData(const char *szFilePath)
{
	if (!szFilePath)
	{
		DEBUGINFO("conf file param is null ");
		return true;
	}
	if (access(szFilePath,F_OK) != 0)
	{
		DEBUGINFO("szFilePath can't find in disk,plz check ");
		return true;
	}
	CIniFile iniFile;
	CSTD_STR szLocalIp;
	if (!iniFile.Load(szFilePath))
	{
		DEBUGINFO("open conf failed,plz check");
		return true;
	}

	this->_serverData.onSetszLocalPort(iniFile.GetStrValue("internalport","server","10001"));
	this->_serverData.onSetszVpcAddr(iniFile.GetStrValue("vpcaddr","server",""));
	this->_serverData.onSetuiMaxClientCount(iniFile.GetIntValue("maxconn","server",8192));
	this->_serverData.onSetuiNoticePort(iniFile.GetIntValue("noticeport","server",3081));
	this->_serverData.onSetuiMaxRecvBuffSize(iniFile.GetIntValue("maxbuffsize","server",4096));
	this->_serverData.onSetbStd(iniFile.GetIntValue("showstd","server",0));
	this->_serverData.onSetbChkWhite(iniFile.GetIntValue("checkwite","server",0));
	gCCommon->onLoadLocalIp(this->_serverData._localips);

	return true;
}

bool	CConfig::onHelper()
{
	char szLog[1024]={0};
	sprintf(szLog,"/*****************************/\r\n"
			"%s...auth by \r\n"
			"ddvpn help\r\n"
			"-f[F] conf file path\r\n"
			"-demon\r\n"
			"/*****************************/\r\n",VERSION);
	printf("%s",szLog);
	return true;
}
