#ifndef SRC_NET_CTCPCONNECT_H
#define SRC_NET_CTCPCONNECT_H

#include "CInclude.h"
#include "CClientData.h"
#include "CBuff.h"
#include "CProtocolData.h"
#include "CNetEvent.h"

class CAccepter;
class CEpoller;

class CTcpConnect
{
public:
	typedef enum
	{
		STATE_NOLOGIN = 0,
		STATE_LOGINED = 1,
		STATE_ACCELERATING = 2,
	}ESTATE;

public:
	CTcpConnect(CAccepter* pAccepter, CEpoller* reactor, CSocketData& sock);
	~CTcpConnect();
	bool onInit(CNetEvent* pNet);
	bool onIsConnected() { return _bConnect; }
	bool onDisconnectd();
	uint32_t getChannelId() { return ++_uiChannelId;}

	bool onChangeState(ESTATE state);
	bool onIsLogined() { return STATE_LOGINED == _eState;}

	void onSetErr(uint8_t err) { _cRes = err; }
	uint8_t onGetErr() { return _cRes; }

	void onSetId(uint32_t id) { _uiId = id;}
	uint32_t onGetId() { return _uiId; }

	void onSetGuid(CSTD_STR& guid) { _szGuid = guid;}
	CSTD_STR onGetGuid() { return _szGuid; }

	void onSetAddr(CSTD_STR&  addr) { _szAddr = addr; }
	CSTD_STR onGetAddr() { return _szAddr; }

	void onSetPrivateAddr(uint32_t addr) { _uiPrivaAddr = addr; }
	uint32_t onGetPrivateAddr() { return _uiPrivaAddr; }

	///////////////////////////////////////////////////////////
	bool onRead();
	bool onSend(const char* buf, size_t len);

	bool onReadHeader();
	bool onReadBody(CBuff& cbuff);
	bool onReadByte(uint8_t& c);
	bool onReadLine(uint8_t *szBuff,const uint32_t &uiBuffSize);

	bool onAddUdpChannel();
	void onDisplayHead();

	bool onPackSock5Respon(CBuff &bodybuf);
	bool onResponseLogin();
	bool onResponseConsoles(std::vector<SGetConsolesRespPackage>& consoles);
	bool onResponseAccelate(SOCKADDR_IN& localaddr);
	bool onResponseAccess(SOCKADDR_IN& localaddr, uint32_t private_addr);

	//////////////////////////////////////////////////////////////
	bool onCreateUdpChannel(CTcpConnect* pOutConn);
	bool onAddOutUdpChannel(CUdpChannel* pChannel);
//	std::vector<uint32_t> onGetInChannels() { return _vInChannels; }
//	std::vector<uint32_t> onGetOutChannels() { return _vOutChannels; }
//	void onAddInChannel(uint32_t id) { _vInChannels.push_back(id);}
//	void onAddOutChannel(uint32_t id) { _vOutChannels.push_back(id);}

public:
	SPackageHeader 	_stHeader;
	CSocketData		_csClientCtrl;

private:
	bool		_bConnect;
	uint8_t		_cRes;
	uint32_t	_uiId;
	uint32_t	_uiChannelId;

	uint32_t	_uiPrivaAddr;
	CSTD_STR	_szGuid;
	CSTD_STR	_szAddr;

	ESTATE 		_eState;
	CBufInfo	_csReadBuf;
	CBufInfo	_csSendBuf;

	CNetEvent* 	_pNet;
	CEpoller*   _pReactor;
	CAccepter*	_pAccepter;

	//std::vector<uint32_t> _vInChannels;
	//std::vector<uint32_t> _vOutChannels;

	std::map<uint32_t, CUdpChannel*> _mapInChannels;
	std::map<uint32_t, CUdpChannel*> _mapOutChannels;
};

#endif
