#include "CUdpChannel.h"
#include "CTcpConnect.h"
#include "CSocket.h"

CUdpChannel::CUdpChannel(uint32_t id, SOCKADDR_IN& localaddr, SOCKADDR_IN& in, SOCKADDR_IN& out)
: _eState(STATE_INITIALIZE)
, _bOpenedIn(false)
, _bOpenedOut(false)
, _uiId(id)
, _uiReadBuffSize(0)
, _uiSendBuffSize(0)
, _uiReadSize(0)
, _szReadBuff(NULL)
, _szSendBuff(NULL)
{
	bzero(&_csSocket, sizeof(CSocketData));
	_csSocket._localSockaddr.sin_addr = localaddr.sin_addr;

	_uiReadBuffSize = 1500;
	_uiSendBuffSize = 1500;

	_inSockAddr.sin_addr = in.sin_addr;
	_outSockAddr.sin_addr = out.sin_addr;

	gCCommon->onSockAddr2String(_inSockAddr,_szInAddr);
	gCCommon->onSockAddr2String(_outSockAddr,_szOutAddr);
}

CUdpChannel::~CUdpChannel()
{
	onClose();
}

bool CUdpChannel::onInit()
{
	_szReadBuff = (char*)MALLOC(_uiReadBuffSize);
	if (NULL == _szReadBuff)
		return false;

	_szSendBuff = (char*)MALLOC(_uiSendBuffSize);
	if (NULL == _szSendBuff)
		return false;

	if (!CSocket::onCreateLocalUdp(_csSocket))
	{
		DEBUGINFO("[UDP] create in udp failed.");
		return false;
	}

	DEBUGINFO("[UDP] open channel success: [%04u][%s --> %s]", _uiId, _szInAddr.c_str(), _szOutAddr.c_str());
	return true;
}

bool CUdpChannel::onClose()
{
	if (STATE_CLOSED != _eState)
		return false;

	_eState = STATE_CLOSED;
	CSocket::onClose(_csSocket);
	FREE(_szReadBuff);
	FREE(_szSendBuff);

	DEBUGINFO("[UDP] close channel: [%04u][%s ===> %s]", _uiId, _szInAddr.c_str(), _szOutAddr.c_str());
	return true;
}


bool CUdpChannel::onShutdonw(uint8_t dire)
{
	switch (dire)
	{
	case DIRECT_IN: {
		_bOpenedIn = false;
		_eState = _bOpenedOut ? STATE_HALFOPENED : STATE_CLOSED;
		DEBUGINFO("[UDP] channel%04u shutdown in end.", _uiId);
		onClose();
	}
	break;
	case DIRECT_OUT: {
		_bOpenedOut = false;
		_eState = _bOpenedIn ? STATE_HALFOPENED : STATE_CLOSED;
		DEBUGINFO("[UDP] channel%04u shutdown out end.", _uiId);
		onClose();
	}
	break;
	default:
		DEBUGINFO("[UDP] channel%04u shutdown failed.", _uiId);
		return false;
	}
	return true;
}

bool CUdpChannel::onForward()
{
	if (!readFrom())
		return false;

	switch (_eState)
	{
		case STATE_FULLOPENED: {
			DEBUGINFO("[UDP] channel[%04u] forwrad data.]", _uiId);
			return forward();
		}
		break;
		case STATE_HALFOPENED: {
			DEBUGINFO("[UDP] channel[%04u] half open cannot forward data.", _uiId);
			return false;
		}
		break;
		case STATE_INITIALIZE: {
			DEBUGINFO("[UDP] channel[%04u] echo data.]", _uiId);
			return echo();
		}
		break;
		case STATE_CLOSED: {
			DEBUGINFO("[UDP] channel[%04u] closed cannot forward data.", _uiId);
			return false;
		}
		break;
		default: {
			DEBUGINFO("[UDP] channel[%04u] state abnormal cannot forward data.", _uiId);
			return false;
		}
		break;
	}
	return true;
}

bool CUdpChannel::forward()
{
	if (CSocket::isSameAddr(_csSocket._remoteSockAddr,_inSockAddr))
	{
		CSTD_STR szLocal;
		gCCommon->onSockAddr2String(_csSocket._localSockaddr,szLocal);
		DEBUGINFO("[UDP] [%04u][%s --> %s --> %s]", _uiId, _szInAddr.c_str(), szLocal.c_str(), _szOutAddr.c_str());

		_szReadBuff[0] = 0x45;
		_szReadBuff[1] = 0;
		return sendTo(_outSockAddr);
	}
	else if (CSocket::isSameAddr(_csSocket._remoteSockAddr, _outSockAddr))
	{
		CSTD_STR szLocal;
		gCCommon->onSockAddr2String(_csSocket._localSockaddr,szLocal);
		DEBUGINFO("[UDP] [%04u][%s <-- %s <-- %s]", _uiId, _szInAddr.c_str(), szLocal.c_str(), _szOutAddr.c_str());
		_szReadBuff[0] = 0x45;
		_szReadBuff[1] = 0;
		return sendTo(_inSockAddr);
	}
	else
	{
		CSTD_STR szRemote, szIn, szOut;
		gCCommon->onSockAddr2String(_csSocket._remoteSockAddr, szRemote);
		gCCommon->onSockAddr2String(_inSockAddr, szIn);
		gCCommon->onSockAddr2String(_outSockAddr, szOut);
		DEBUGINFO("[UDP] channel %04u remote addr invalid: %s NOT IN: [%s, %s]", szRemote.c_str(), szIn.c_str(), szOut.c_str());
		return false;
	}
}

bool CUdpChannel::echo()
{
	switch(parseAuthData())
	{
		case DIRECT_IN:{
			if (!_bOpenedIn && _bOpenedOut)
			{
				DEBUGINFO("[UDP] channel %04u is established.", _uiId);

				_inSockAddr = _csSocket._remoteSockAddr;
				_bOpenedIn = true;

				gCCommon->onSockAddr2String(_inSockAddr,_szInAddr);
				DEBUGINFO("[UDP] channel %04u apply end addr: %s", _uiId, _szInAddr.c_str());

				_eState = STATE_FULLOPENED;

				if (!sendTo(_csSocket._remoteSockAddr))
					return false;
			}
			else
				DEBUGINFO("[UDP] channel %04u receive in end auth data but no receive out end auth data.", _uiId);
		}
		break;
		case DIRECT_OUT: {
			if (!_bOpenedOut)
			{
				_outSockAddr = _csSocket._remoteSockAddr;
				gCCommon->onSockAddr2String(_outSockAddr,_szOutAddr);
				DEBUGINFO("[UDP] out addr: %s", _szOutAddr.c_str());
				_bOpenedOut = true;

				if (!sendTo(_csSocket._remoteSockAddr))
					return false;
			}
			else
				DEBUGINFO("[UDP] channel %04u repeat to receive out end auth data.", _szOutAddr.c_str());
		}
		break;
		case DURECT_ERR: {
			DEBUGINFO("[UDP] channel %04u parse auth data failed.", _uiId);
			CSTD_STR szRemote;
			gCCommon->onSockAddr2String(_csSocket._remoteSockAddr, szRemote);
			DEBUGINFO("[UDP] get remote udp addr invalid: %s NOT IN: [%s, %s]"
					, szRemote.c_str(), _szInAddr.c_str(), _szOutAddr.c_str());
			return false;
		}
		break;
	}
	return true;
}

bool CUdpChannel::readFrom()
{
	uint32_t sockaddr_len = sizeof(SOCKADDR_IN);
	bzero(_szReadBuff, _uiReadBuffSize);
	int nRead = recvfrom(_csSocket._nFd, _szReadBuff, _uiReadBuffSize, 0, (struct sockaddr *)&_csSocket._remoteSockAddr, &sockaddr_len);
	if(nRead <= 0)
	{
		_uiReadSize = 0;
		DEBUGINFO("[UDP] recv from sockfd %d failed: %m",_csSocket._nFd);
		return false;
	}
	_uiReadSize = nRead;
#ifdef _DEBUG
	CSTD_STR szRemote,szLocal;
	gCCommon->onSockAddr2String(_csSocket._remoteSockAddr,szRemote);
	gCCommon->onSockAddr2String(_csSocket._localSockaddr,szLocal);
	DEBUGINFO("[UDP] %s <-- %s size %d: ", szLocal.c_str(), szRemote.c_str(), nRead);
	DISPLAY_BUFF_VALUE(_szReadBuff, (uint32_t)nRead);
#endif
	return true;
}

bool CUdpChannel::sendTo(const SOCKADDR_IN& sock)
{
	uint32_t sockaddr_len = sizeof(SOCKADDR_IN);
	int nSend = sendto(_csSocket._nFd,_szReadBuff, _uiReadSize, 0, (struct sockaddr *)&sock,sockaddr_len);
	if(nSend <= 0)
	{
		DEBUGINFO("[UDP] sendto %d failed: %m",_csSocket._nFd);
		return false;
	}
#ifdef _DEBUG
	CSTD_STR szRemote,szLocal;
	gCCommon->onSockAddr2String(sock,szRemote);
	gCCommon->onSockAddr2String(_csSocket._localSockaddr,szLocal);
	DEBUGINFO("[UDP] %s --> %s size %d", szLocal.c_str(), szRemote.c_str(), nSend);
	DISPLAY_BUFF_VALUE(_szReadBuff, (uint32_t)nSend);
#endif

	return true;
}

CUdpChannel::EDIRECT CUdpChannel::parseAuthData()
{
	if (_szReadBuff[0] == 1)
		return DIRECT_IN;
	else if (_szReadBuff[0] == 2)
		return DIRECT_OUT;
	else
		return DURECT_ERR;

//	if (CSocket::isSameIp(_csSocket._remoteSockAddr, _inSockAddr))
//		return DIRECT_IN;
//	else if (CSocket::isSameIp(_csSocket._remoteSockAddr, _outSockAddr))
//		return DIRECT_IN;
//	else
//		return DURECT_ERR;
}


