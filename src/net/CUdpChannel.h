#ifndef SRC_NET_CUDPCHANNEL_H
#define SRC_NET_CUDPCHANNEL_H

#include "CInclude.h"

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
		DIRECT_IN = 0,
		DIRECT_OUT = 1,
		DURECT_ERR = 2,
	}EDIRECT;

public:
	CUdpChannel(uint32_t id, SOCKADDR_IN& localaddr, SOCKADDR_IN& in, SOCKADDR_IN& out);
	~CUdpChannel();
	bool onInit();
	bool onClose();
	bool onShutdonw(uint8_t dire);

	bool onForward();
	uint32_t onGetId() { return _uiId; }

private:
	bool readFrom();
	bool sendTo(const SOCKADDR_IN& sock);
	bool forward();
	bool echo();
	EDIRECT parseAuthData();

public:
	ESTATE		_eState;
	bool 		_bOpenedIn;
	bool 		_bOpenedOut;

	uint32_t	_uiId;
	uint32_t	_uiReadBuffSize;
	uint32_t	_uiSendBuffSize;
	uint32_t	_uiReadSize;
	char*		_szReadBuff;
	char*		_szSendBuff;

	CSocketData _csSocket;
	SOCKADDR_IN	_inSockAddr;
	SOCKADDR_IN	_outSockAddr;
	CSTD_STR	_szInAddr;
	CSTD_STR	_szOutAddr;
};

#endif
