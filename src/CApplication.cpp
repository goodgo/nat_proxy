#include "CApplication.h"
#include "CLog.h"
#include "CConfig.h"

bool CApplication::g_staic_run = false;

CApplication::CApplication()
: _uiAccepterNum(1)
, _pAccepters(std::vector<CAccepter*>())
{

}
CApplication::~CApplication()
{
	onStop();
}

bool CApplication::onInit(int argc,char *argv[])
{
	if(!gLogModule->onInit("",""))
	{
		printf("init log failed\r\n");
		return false;
	}

	if(argc <= 2 )
	{
		gCConfig->onHelper();
		return false;
	}
	if(!gCConfig->onInit(argc,argv))
	{
		DEBUGINFO("init config failed.");
		return false;
	}

	CApplication::g_staic_run = true;
	onInstallationPauseSign();

	if (gCConfig->onGetserverData().onGetbDemon())
	{
		if (!gCCommon->onDaemon())
		{
			DEBUGINFO("init daemon failed");
			return false;
		}
		DEBUGINFO("[server using daemon mode.]");
	}

	for (int i = 0; i < _uiAccepterNum; i++)
	{
		CAccepter* pAccepter = new CAccepter(1);
		if (!pAccepter->onInit())
		{
			DEBUGINFO("init accepter %d failed.", pAccepter->onGetId());
			return false;
		}
		_pAccepters.push_back(pAccepter);
	}


	return true;
}

bool CApplication::onRun()
{
	for (int i = 0; i < _uiAccepterNum; i++)
	{
		_pAccepters[i]->onRun();
	}
	return true;
}

bool CApplication::onStop()
{
	if (CApplication::g_staic_run)
	{
		CApplication::g_staic_run = false;
		for (int i = 0; i < _uiAccepterNum; i++)
		{
			_pAccepters[i]->onStop();
			delete _pAccepters[i];
		}
	}
	return true;
}

void CApplication::onHandleSign(int nSign)
{
	DEBUGINFO("received signal %d. server will stop!", nSign);
	//g_staic_run = false;
	gApplication->onStop();
}

bool CApplication::onInstallationPauseSign()
{
	signal(SIGTERM, onHandleSign);
	return true;
}
