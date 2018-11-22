#include "CUdpChannel.h"
#include "CTcpConnect.h"
#include "CEpoller.h"
#include "CSocket.h"

CUdpChannel::CUdpChannel(uint32_t id, CEpoller* pReactor, CTcpConnect* pSrc, CTcpConnect* pDst)
: _eState(STATE_INITIALIZE)
, _bSrcOpened(false)
, _bDstOpened(false)
, _uiId(id)
, _uiReadBuffSize(0)
, _uiSendBuffSize(0)
, _uiReadSize(0)
, _szReadBuff(NULL)
, _szSendBuff(NULL)
, _pReactor(pReactor)
, _pSrcConn(pSrc)
, _pDstConn(pDst)
{
	bzero(&_csSocket, sizeof(CSocketData));

	_uiReadBuffSize = 1500;
	_uiSendBuffSize = 1500;

	_srcSockAddr.sin_addr = pSrc->_csClientCtrl._remoteSockAddr.sin_addr;
	_dstSockAddr.sin_addr = pDst->_csClientCtrl._remoteSockAddr.sin_addr;

	gCCommon->onSockAddr2String(_srcSockAddr,_szSrcAddr);
	gCCommon->onSockAddr2String(_dstSockAddr,_szDstAddr);
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

	DEBUGINFO("[UDP] initialize channel[%u] (%u)[%s --> %u --> %s](%u)."
			, _uiId, _pSrcConn->onGetId(), _szSrcAddr.c_str(), ntohs(_csSocket._localSockaddr.sin_port), _szDstAddr.c_str(), _pDstConn->onGetId());
	return true;
}

bool CUdpChannel::onClose()
{
	if (STATE_CLOSED == _eState)
		return false;

	_eState = STATE_CLOSED;
	if (_pReactor)
	{
		DEBUGINFO("[UDP] channel[%u] remove from reactor.", _uiId);
		int events = EPOLLIN | EPOLLOUT;
		_pReactor->del(_csSocket._nFd, events);
	}

	CSocket::onClose(_csSocket);
	FREE(_szReadBuff);
	FREE(_szSendBuff);

	DEBUGINFO("[UDP] close channel[%u] (%u)[%s --> %u --> %s](%u)."
			, _uiId, _pSrcConn->onGetId(), _szSrcAddr.c_str(), ntohs(_csSocket._localSockaddr.sin_port), _szDstAddr.c_str(), _pDstConn->onGetId());
	_pReactor = NULL;
	_pSrcConn = NULL;
	_pDstConn = NULL;

	return true;
}


bool CUdpChannel::onNotifySrcConnCloseChannel()
{
	_bSrcOpened = false;
	DEBUGINFO("[UDP] channel[%u] notify source client[%u] to close source udp point.", _uiId, _pSrcConn->onGetId());
	return _pSrcConn->onSrcChannelClose(onGetId());
}

bool CUdpChannel::onNotifyDstConnCloseChannel()
{
	_bDstOpened = false;
	DEBUGINFO("[UDP] channel[%u] notify destination client[%u] to close destination udp point.", _uiId, _pDstConn->onGetId());
	return _pDstConn->onDstChannelClose(onGetId());
}


bool CUdpChannel::onForward()
{
	if (!readFrom())
		return false;

	switch (_eState)
	{
		case STATE_FULLOPENED: {
			if (_uiReadSize == 1)
				break;

			return forward();
		}
		break;
		case STATE_HALFOPENED: {
			DEBUGINFO("[UDP] channel[%u] half open cannot forward data.", _uiId);
			return false;
		}
		break;
		case STATE_INITIALIZE: {
			return echo();
		}
		break;
		case STATE_CLOSED: {
			DEBUGINFO("[UDP] channel[%u] closed cannot forward data.", _uiId);
			return false;
		}
		break;
		default: {
			DEBUGINFO("[UDP] channel[%u] state abnormal cannot forward data.", _uiId);
			return false;
		}
		break;
	}
	return true;
}

bool CUdpChannel::forward()
{
	if (CSocket::isSameAddr(_csSocket._remoteSockAddr,_srcSockAddr))
	{
		CSTD_STR szLocal;
		gCCommon->onSockAddr2String(_csSocket._localSockaddr,szLocal);
		DEBUGINFO("[UDP] channel[%u] (%u)[%s --> %u --> %s](%u)"
				, _uiId, _pSrcConn->onGetId(), _szSrcAddr.c_str(), ntohs(_csSocket._localSockaddr.sin_port), _szDstAddr.c_str(), _pDstConn->onGetId());

		uint8_t byte1 = _szReadBuff[0];
		uint8_t byte2 = _szReadBuff[1];
		_szReadBuff[0] = byte1;
		_szReadBuff[1] = byte2;

		return sendTo(_dstSockAddr);
	}
	else if (CSocket::isSameAddr(_csSocket._remoteSockAddr, _dstSockAddr))
	{
		CSTD_STR szLocal;
		gCCommon->onSockAddr2String(_csSocket._localSockaddr,szLocal);

		DEBUGINFO("[UDP] channel[%u] (%u)[%s <-- %u <-- %s](%u) "
				, _uiId, _pSrcConn->onGetId(), _szSrcAddr.c_str(), ntohs(_csSocket._localSockaddr.sin_port), _szDstAddr.c_str(), _pDstConn->onGetId());

		uint8_t byte1 = _szReadBuff[0];
		uint8_t byte2 = _szReadBuff[1];
		_szReadBuff[0] = byte1;
		_szReadBuff[1] = byte2;

		return sendTo(_srcSockAddr);
	}
	else
	{
		CSTD_STR szRemote, szSrc, szDst;
		gCCommon->onSockAddr2String(_csSocket._remoteSockAddr, szRemote);
		DEBUGINFO("[UDP] channel[%u] remote addr invalid: %s NOT IN: [%s, %s]", _uiId, szRemote.c_str(), _szSrcAddr.c_str(), _szDstAddr.c_str());
		return false;
	}
}

bool CUdpChannel::echo()
{
	switch(parseAuthData())
	{
		case DIRECT_SRC: {
			if (_bSrcOpened)
			{
				DEBUGINFO("[UDP] channel[%u] have received source point auth data.", _uiId);
				return false;
			}

			_srcSockAddr = _csSocket._remoteSockAddr;
			gCCommon->onSockAddr2String(_srcSockAddr, _szSrcAddr);

			if (_bDstOpened)
			{
				_bSrcOpened = true;
				_eState = STATE_FULLOPENED;
				DEBUGINFO("[UDP] established channel[%u] (%u)[%s => %u => %s](%u)."
						, _uiId, _pSrcConn->onGetId(), _szSrcAddr.c_str(), ntohs(_csSocket._localSockaddr.sin_port), _szDstAddr.c_str(), _pDstConn->onGetId());
			}
			else
			{
				DEBUGINFO("[UDP] channel[%u] receive source point auth data but no receive destination point data.", _uiId);
				bzero(_szReadBuff, _uiReadBuffSize);
			}

			DEBUGINFO("[UDP] channel[%u] echo source point: %s.", _uiId, _szSrcAddr.c_str());
			if (!sendTo(_csSocket._remoteSockAddr))
				return false;
		}
		break;
		case DIRECT_DST: {
			if (_bDstOpened)
			{
				DEBUGINFO("[UDP] channel[%u] repeat to receive destination point auth data.", _uiId, _szDstAddr.c_str());
				return false;
			}

			_bDstOpened = true;
			_dstSockAddr = _csSocket._remoteSockAddr;
			gCCommon->onSockAddr2String(_dstSockAddr,_szDstAddr);

			DEBUGINFO("[UDP] channel[%u] echo destination point: %s.", _uiId, _szDstAddr.c_str());
			if (!sendTo(_csSocket._remoteSockAddr))
				return false;
		}
		break;
		case DURECT_ERR: {
			CSTD_STR szRemote;
			gCCommon->onSockAddr2String(_csSocket._remoteSockAddr, szRemote);
			DEBUGINFO("[UDP] channel[%u] parse auth data failed, get remote udp addr invalid: %s NOT IN: [%s, %s]"
					,_uiId, szRemote.c_str(), _szSrcAddr.c_str(), _szDstAddr.c_str());
			return false;
		}
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
		DEBUGINFO("[UDP] channel[%u] recv from sockfd %d failed: %m", _uiId,_csSocket._nFd);
		return false;
	}
	_uiReadSize = nRead;
#ifdef _DEBUG
	CSTD_STR szRemote,szLocal;
	gCCommon->onSockAddr2String(_csSocket._remoteSockAddr,szRemote);
	gCCommon->onSockAddr2String(_csSocket._localSockaddr,szLocal);
	DEBUGINFO("[UDP] channel[%u] [%s <-- %s] size %d: ", _uiId, szLocal.c_str(), szRemote.c_str(), nRead);
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
		DEBUGINFO("[UDP] channel[%u] sendto %d failed: %m", _uiId, _csSocket._nFd);
		return false;
	}
#ifdef _DEBUG
	CSTD_STR szRemote,szLocal;
	gCCommon->onSockAddr2String(sock,szRemote);
	gCCommon->onSockAddr2String(_csSocket._localSockaddr,szLocal);
	DEBUGINFO("[UDP] channel[%u] [%s --> %s] size %d", _uiId, szLocal.c_str(), szRemote.c_str(), nSend);
	DISPLAY_BUFF_VALUE(_szReadBuff, (uint32_t)nSend);
#endif

	return true;
}

CUdpChannel::EDIRECT CUdpChannel::parseAuthData()
{
	if (_szReadBuff[0] == 0x01)
		return DIRECT_SRC;
	else if (_szReadBuff[0] == 0x02)
		return DIRECT_DST;
	else
		return DURECT_ERR;

//	if (CSocket::isSameIp(_csSocket._remoteSockAddr, _srcSockAddr))
//		return DIRECT_SRC;
//	else if (CSocket::isSameIp(_csSocket._remoteSockAddr, _dstSockAddr))
//		return DIRECT_DST;
//	else
//		return DURECT_ERR;
}


