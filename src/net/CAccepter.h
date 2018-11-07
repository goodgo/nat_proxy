#ifndef SRC_ACCEPTER_H
#define SRC_ACCEPTER_H

#include "ObjInnerThread.h"
#include "CEpoller.h"
#include "CClientData.h"
#include "CTcpConnect.h"
#include "CUdpChannel.h"
#include "CProtocolData.h"
#include "CProtocol.h"
#include "CNetEvent.h"
#include <map>
#include <set>

class CAccepter: public ObjInnerThread<CAccepter>
{
public:
	CAccepter(int id);
	~CAccepter();
	bool onInit();
	bool onStop();

	bool onRun();
	void InnerThreadProc();

	uint32_t onGetId() { return _uiId; }
	CSocketData onGetSock() { return _csSockServer; }
	CEpoller* onGetReactor() { return _pReactor; }

	uint32_t onGetConnNum() {return _uiConnNum;}
	void onIncrConnNum() { _uiConnNum++; }
	uint32_t onGetActiveNum() { return _mapClients.size(); }

	bool onShutdownChannel(uint32_t id, uint8_t direct);

	static bool onAcceptCallback(void* obj, struct epoll_event& ev);
	static bool onTcpEventCallback(void* obj, struct epoll_event& ev);
	static bool onUdpEventCallback(void* obj, struct epoll_event& ev);

private:
	bool initServer();
	bool delTcpNetEvent(CNetEvent* pConn);

	uint32_t getSelfId() { return _uiId; }
	uint32_t getClientId() { return ++_uiClientId; }
	uint32_t getChannelId() { return ++_uiChannelId;}

	bool onDataOut(struct epoll_event& pev);
	bool onGetAllClientInfo(uint32_t id, std::vector<SGetConsolesRespPackage>& vecConsoles);
	CTcpConnect* onGetClient(uint32_t clientid);

	bool onLogin(CNetEvent* pNet, CLoginReqPackage& pkg);
	bool onGetConsoles(CNetEvent* pNet, CGetConsolesReqPackage& pkg);
	bool onAccelerate(CNetEvent* pNet, CAccelationReqPackage& pkg);

private:
	bool _bRunning;
	const uint32_t _uiId;
	uint16_t _usPort;
	uint32_t _uiClientId;
	uint32_t _uiChannelId;
	uint32_t _uiConnNum;

	CSocketData _csSockServer;
	CEpoller *_pReactor;

	std::set<CSTD_STR> _setGuids;
	std::map<uint32_t, CTcpConnect*> _mapClients;
};

#endif
