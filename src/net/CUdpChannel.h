#ifndef SRC_NET_CUDPCHANNEL_H
#define SRC_NET_CUDPCHANNEL_H

#include "CInclude.h"

class CTcpConnect;
class CEpoller;

class CUdpChannel
{
public:
	typedef enum {
		STATE_CLOSED = 0,
		STATE_INITIALIZE = 1,
		STATE_HALFOPENED = 2,
		STATE_FULLOPENED = 3,
	}ESTATE;

	typedef enum {
		DIRECT_SRC = 0,
		DIRECT_DST = 1,
		DURECT_ERR = 2,
	}EDIRECT;

public:
	CUdpChannel(uint32_t id, CEpoller* pReactor, CTcpConnect* pIn, CTcpConnect* pOut);
	~CUdpChannel();
	bool onInit();
	bool onClose();
	bool onIsClosed() { return _eState == STATE_CLOSED; }
 	bool onNotifySrcConnCloseChannel();
 	bool onNotifyDstConnCloseChannel();
 	SOCKADDR_IN onGetLocalAddr() { return _csSocket._localSockaddr;}
 	uint32_t onGetId() { return _uiId; }
	bool onForward();

private:
	bool readFrom();
	bool sendTo(const SOCKADDR_IN& sock);
	bool forward();
	bool echo();
	EDIRECT parseAuthData();

public:
	ESTATE		_eState;
	bool 		_bSrcOpened;
	bool 		_bDstOpened;

	uint32_t	_uiId;
	uint32_t	_uiReadBuffSize;
	uint32_t	_uiSendBuffSize;
	uint32_t	_uiReadSize;
	char*		_szReadBuff;
	char*		_szSendBuff;

	CSocketData _csSocket;
	SOCKADDR_IN	_srcSockAddr;
	SOCKADDR_IN	_dstSockAddr;
	CSTD_STR	_szSrcAddr;
	CSTD_STR	_szDstAddr;

	CEpoller*	 _pReactor;
	CTcpConnect* _pSrcConn;
	CTcpConnect* _pDstConn;
};

#endif
