#include "CAccepter.h"
#include "CLog.h"
#include "CCommon.h"
#include "CSocket.h"
#include "CUdpChannel.h"
#include "CClientData.h"
#include "COpenssl.h"
#include "CProtocol.h"

CAccepter::CAccepter(int id)
: _bRunning(false)
, _uiId(id)
, _usPort(0)
, _uiClientId(0)
, _uiChannelId(0)
, _uiConnNum(0)
, _pReactor(NULL)
{
	_usPort = atoi(gCConfig->onGetserverData().onGetszLocalPort().c_str());
	bzero(&_csSockServer, sizeof(CSocketData));
}

CAccepter::~CAccepter()
{
	onStop();
}

bool CAccepter::onInit()
{
	_pReactor = new CEpoller(false);
	if(!_pReactor)
	{
		USERINFO("epool or server not malloc plz");
		return false;
	}

	if(!_pReactor->create(2048))
	{
		USERINFO("create epoller failed");
		return false;
	}

	if(!initServer())
	{
		USERINFO("init server faile");
		return false;
	}

	if (!CSocket::onIsValid(_csSockServer))
	{
		USERINFO("invalid socket!");
		return false;
	}

	CNetEvent* pNet = new CNetEvent(_csSockServer, this, CAccepter::onAcceptCallback);
	if(!_pReactor->add(_csSockServer._nFd, pNet))
	{
		USERINFO("epoller add sockfd faile");
		return false;
	}

	return true;
}

bool CAccepter::onStop()
{
	if (!_bRunning)
		return false;

	_bRunning = false;
	if (_pReactor)
	{
		delete _pReactor;
		_pReactor = NULL;
	}

	std::map<uint32_t, CTcpConnect*>::iterator conn_it = _mapClients.begin();
	for (; conn_it != _mapClients.end(); )
	{
		_mapClients.erase(conn_it++);
	}

	DEBUGINFO("accepter %u stop.", _uiId);
	return true;
}

bool CAccepter::onRun()
{
	DEBUGINFO("................%s start success..........",VERSION);
	DEBUGINFO("Accepter[%02u] listening port: %u...", _uiId, _usPort);
	_bRunning = true;

	StartInnerThread(this);
	return true;
}

bool CAccepter::initServer()
{
	if (!CSocket::onCreate(_csSockServer, IPPROTO_TCP))
	{
		USERCRITIAL("create server socket failed: %m");
		return false;
	}

	if (!CSocket::onReuseaddr(_csSockServer))
	{
		USERCRITIAL("set reuse addr failed: %m");
		return false;
	}

	if (!CSocket::onReuseport(_csSockServer))
	{
		USERCRITIAL("set reuse port failed: %m");
		return false;
	}

	if (!CSocket::onSetNonBlocking(_csSockServer, true))
	{
		USERCRITIAL("set socket non blocking failed: %m");
		return false;
	}

	if (!CSocket::onBind(_csSockServer, _usPort))
	{
		USERCRITIAL("bind port[%d] failed: %m", _usPort);
		return false;
	}

	if (!CSocket::onListen(_csSockServer, 100))
	{
		USERCRITIAL("bind port[%d] failed: %m", _usPort);
		return false;
	}
	return true;
}

bool CAccepter::delTcpNetEvent(CNetEvent* pNet)
{
	if (!pNet)
		return false;

	CTcpConnect* pConn = (CTcpConnect*)pNet->_ptr;

	std::set<CSTD_STR>::iterator setit = _setGuids.find(pConn->onGetGuid());
	if (setit != _setGuids.end())
	{
		DEBUGINFO("erase guid: %s", setit->c_str());
		_setGuids.erase(setit);
	}

	std::map<uint32_t, CTcpConnect*>::iterator mapit = _mapClients.find(pConn->onGetId());
	if (mapit != _mapClients.end())
	{
		DEBUGINFO("erase cient: %u", mapit->first);
		_mapClients.erase(mapit);
	}

	DEBUGINFO("client disconnect. ID: %u | GUID: %s | IP: %s | CN: %u | AN: %u"
			, pConn->onGetId(), pConn->onGetGuid().c_str(), pConn->onGetAddr().c_str(), --_uiConnNum, onGetActiveNum());

	delete pConn;
	pNet->_ptr = NULL;
	delete pNet; pNet = NULL;

	return true;
}

CTcpConnect* CAccepter::onGetClient(uint32_t clientid)
{
	std::map<uint32_t, CTcpConnect*>::iterator it = _mapClients.find(clientid);
	if (it != _mapClients.end())
	{
		return it->second;
	}
	return NULL;
}

void CAccepter::InnerThreadProc()
{
	int nready = _pReactor->wait(-1);
	if (nready > 0)
	{
		//DEBUGINFO("epoll wait fd num: %d", nready);
		for (int i = 0; i < nready; i++)
		{
			struct epoll_event& ev = _pReactor->get(i);
			CNetEvent* pNet = (CNetEvent*)ev.data.ptr;
			pNet->_callback(this, ev);
		}
	}
}


bool CAccepter::onAcceptCallback(void* obj, struct epoll_event& ev)
{
	if (!obj)
	{
		DEBUGINFO("obj point is null.");
		return false;
	}
	CAccepter* pThis = (CAccepter*)obj;
	CSocketData client;
	client._nProtol = IPPROTO_TCP;
	client._localSockaddr = pThis->_csSockServer._localSockaddr;
	if (!CSocket::onAccept(pThis->_csSockServer, client))
	{
		return false;
	}

	if(pThis->onGetConnNum() + 1 > gCConfig->onGetserverData().onGetuiMaxClientCount())
	{
		CSocket::onClose(client);
		DEBUGINFO("accetpt failed,too many clients connect,current %d",gCConfig->onGetserverData().onGetuiMaxClientCount());
		return false;
	}

	if(!CSocket::onSetNonBlocking(client,true))
	{
		CSocket::onClose(client);
		return false;
	}

	CTcpConnect* pConn = new CTcpConnect(pThis, pThis->_pReactor, client);
	CNetEvent* pNet = new CNetEvent(client, pConn, CAccepter::onTcpEventCallback);
	pConn->onInit(pNet);

	if (!pThis->_pReactor->add(client._nFd, pNet))
	{
		DEBUGINFO("epoll add event failed: FD: %d, IP: %s.", client._nFd, pConn->onGetAddr().c_str());

		delete pConn;
		delete pNet;
		return false;
	}

	pThis->onIncrConnNum();
	DEBUGINFO("client connected. FD: %d | IP: %s | CN: %u | AN: %u"
			, client._nFd, pConn->onGetAddr().c_str(), pThis->onGetConnNum(), pThis->onGetActiveNum());
	return true;
}

bool CAccepter::onTcpEventCallback(void* obj, struct epoll_event& ev)
{
	if (!obj)
	{
		DEBUGINFO("obj point is null.");
		return false;
	}

	CAccepter* pThis = (CAccepter*)obj;
	CNetEvent* pNet = (CNetEvent*)ev.data.ptr;
	CTcpConnect* pConn = (CTcpConnect*)pNet->_ptr;

	if (!pConn->onReadHeader())
	{
		if (!pConn->onIsConnected())
		{
			pThis->delTcpNetEvent(pNet);
			return false;
		}
	}

	pConn->onDisplayHead();
	if (CFuncCode::FUNC_HEARTBEAT == pConn->_stHeader.ucFunc)
		return true;

	CBuff cbuff;
	if (!pConn->onReadBody(cbuff))
	{
		if (!pConn->onIsConnected())
		{
			pThis->delTcpNetEvent(pNet);
			return false;
		}
	}

	switch (pConn->_stHeader.ucFunc)
	{
	case CFuncCode::FUNC_LOGIN: {
		CLoginReqPackage pkg;
		if (!pkg.onConv(cbuff))
		{
			DEBUGINFO("parse login package error.");
			return false;
		}
		pThis->onLogin(pNet, pkg);
	}
	break;
	case CFuncCode::FUNC_ACCELERATE:{
		CAccelationReqPackage pkg;
		if (!pkg.onConv(cbuff))
		{
			DEBUGINFO("parse accelerate package error.");
			return false;
		}
		pThis->onAccelerate(pNet, pkg);
	}
	break;
	case CFuncCode::FUNC_CONSOLES:{
		CGetConsolesReqPackage pkg;
		if (!pkg.onConv(cbuff))
		{
			DEBUGINFO("parse Consoles package error.");
			return false;
		}
		pThis->onGetConsoles(pNet, pkg);
	}
	break;
	default:
		DEBUGINFO("request function error: %d", pConn->_stHeader.ucFunc);
		return false;
	}

	return true;
}

bool CAccepter::onUdpEventCallback(void* obj, struct epoll_event& ev)
{
	if (!obj)
	{
		DEBUGINFO("obj point is null.");
		return false;
	}

	if (NULL == ev.data.ptr)
	{
		DEBUGINFO("epoll_event data point is null.");
		return false;
	}

	CNetEvent* pNet = (CNetEvent*)ev.data.ptr;
	CUdpChannel* pChannel = (CUdpChannel*)pNet->_ptr;
	if (!pChannel)
	{
		DEBUGINFO("udp channel point is null.");
		return false;
	}
	pChannel->onForward();
	return true;
}

bool CAccepter::onLogin(CNetEvent* pNet, CLoginReqPackage& pkg)
{
	CTcpConnect* pConn = (CTcpConnect*)pNet->_ptr;
	if (_setGuids.find(pkg.szGuid) != _setGuids.end())
	{
		DEBUGINFO("[LOGIN] guid [%s] has logined.", pkg.szGuid.c_str());
		pConn->onSetErr(CErrorCode::ERROR_SOCKET5_AUTH_ERROR);
	}
	else
	{
		pConn->onSetId(getClientId());
		pConn->onSetGuid(pkg.szGuid);
		pConn->onChangeState(CTcpConnect::STATE_LOGINED);
		pConn->onSetPrivateAddr(pkg.uiPrivateAddr);

		_setGuids.insert(pkg.szGuid);
		_mapClients.insert(std::make_pair(pConn->onGetId(), pConn));

		CSTD_STR client_addr;
		gCCommon->onSockAddr2String(pNet->_csSocket._remoteSockAddr, client_addr);

		CSTD_STR priva_addr;
		struct in_addr private_inaddr;
		private_inaddr.s_addr = pkg.uiPrivateAddr;

		gCCommon->onSockAddr2String(private_inaddr, priva_addr);
		DEBUGINFO(" ID: %u | IP: %s | VIP: %s | GUID: %32s | CN: %u | AN: %u"
				, pConn->onGetId(), client_addr.c_str(), priva_addr.c_str(), pConn->onGetGuid().c_str(), onGetConnNum(), onGetActiveNum());
	}

	if(!pConn->onResponseLogin())
	{
		DEBUGINFO("response failed");
		return false;
	}
	return true;
}

bool CAccepter::onGetAllClientInfo(uint32_t id, std::vector<SGetConsolesRespPackage>& vecConsoles)
{
	std::map<uint32_t, CTcpConnect*>::iterator it = _mapClients.begin();
	for (;it != _mapClients.end();++it)
	{
		if (it->first == id)
			continue;

		SGetConsolesRespPackage info;
		info.uiId = it->first;
		vecConsoles.push_back(info);
	}
	return true;
}

bool CAccepter::onGetConsoles(CNetEvent* pNet, CGetConsolesReqPackage& pkg)
{
	CTcpConnect* pConn = (CTcpConnect*)pNet->_ptr;

	std::vector<SGetConsolesRespPackage> vecConsoles;
	if (pConn->onIsLogined())
	{
		onGetAllClientInfo(pConn->onGetId(), vecConsoles);
	}
	else
	{
		DEBUGINFO("client[%04u] no login cannot get consoles.", pConn->onGetId());
		pConn->onSetErr(CErrorCode::ERROR_SOCKET5_AUTH_ERROR);
	}

	if(!pConn->onResponseConsoles(vecConsoles))
	{
		DEBUGINFO("response failed");
		return false;
	}
	return true;
}

bool CAccepter::onAccelerate(CNetEvent* pNet, CAccelationReqPackage& pkg)
{
	CTcpConnect* pInConn = (CTcpConnect*)pNet->_ptr;
	CTcpConnect* pOutConn = onGetClient(pkg.uiDstId);

	SOCKADDR_IN udp_addr;
	bzero(&udp_addr, sizeof(SOCKADDR_IN));
	if (NULL == pOutConn || !pInConn->onIsLogined() || pInConn == pOutConn)
	{
		pInConn->onSetErr(CErrorCode::ERROR_SYSTEM_BIND_ERROR);
		DEBUGINFO("get src client [%04u] %s, dst client [%04u] %s."
				, pkg.uiDstId, pOutConn ? "ok": "failed", pInConn->onGetId(), pInConn->onIsLogined() ? "login": "no login");
	}
	else
	{
		pInConn->onCreateUdpChannel(getChannelId(), pOutConn, udp_addr);
	}

	if(!pInConn->onResponseAccelate(udp_addr))
	{
		DEBUGINFO("response in client [%04u] failed.", pInConn->onGetId());
		return false;
	}

	return true;
}

bool CAccepter::onDataOut(struct epoll_event& ev)
{
	if (NULL == ev.data.ptr)
	{
		DEBUGINFO("event data prt invalid param!");
		return false;
	}

	//CNetEvent* event = (CNetEvent*)ev.data.ptr;

	return true;
}
