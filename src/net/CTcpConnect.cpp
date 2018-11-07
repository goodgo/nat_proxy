#include "CTcpConnect.h"
#include "CEpoller.h"
#include "CClientData.h"
#include "COpenssl.h"
#include "CSocket.h"
#include "CAccepter.h"

CTcpConnect::CTcpConnect(CAccepter* pAccepter, CEpoller* pReactor, CSocketData& sock)
: _bConnect(true)
, _cRes(0)
, _uiId(0)
, _uiPrivaAddr(0)
, _eState(STATE_NOLOGIN)
, _pNet(NULL)
, _pReactor(pReactor)
, _pAccepter(pAccepter)
{
	bzero(&_csClientCtrl, sizeof(CSocketData));
	_csClientCtrl = sock;
	_csReadBuf._uiCliBufMaxSize = 4096;
}

CTcpConnect::~CTcpConnect()
{
	DEBUGINFO("client [%04u] delete.", _uiId);
	onDisconnectd();
}

bool CTcpConnect::onInit(CNetEvent* pNet)
{
	_csReadBuf._szCliBuf = (uint8_t*)MALLOC(_csReadBuf._uiCliBufMaxSize);
	if (!_csReadBuf._szCliBuf)
		return false;
	_pNet = pNet;

	gCCommon->onSockAddr2String(_csClientCtrl._remoteSockAddr, _szAddr);
	return true;
}

bool CTcpConnect::onDisconnectd()
{
	DEBUGINFO("client[%04u] disconnected.", _uiId);

	if (!_bConnect)
		return false;

	_bConnect = false;
	if (NULL != _pReactor)
	{
		int events = EPOLLOUT | EPOLLIN;
		_pReactor->del(_csClientCtrl._nFd, events);
	}

	CSocket::onClose(_csClientCtrl);
	std::map<uint32_t, CUdpChannel*>::iterator it = _mapSrcChannels.begin();
	for (; it != _mapSrcChannels.end();)
	{
		if (it->second)
		{
			it->second->onNotifyDstConnCloseChannel();
			delete it->second;
			_mapSrcChannels.erase(it++);
		}
		else
			++it;
	}

	for (it = _mapDstChannels.begin(); it != _mapDstChannels.end();)
	{
		if (it->second)
		{
			it->second->onNotifySrcConnCloseChannel();
			_mapDstChannels.erase(it++);
		}
		else
			++it;
	}

	_pNet = NULL;
	_pReactor = NULL;
	_pAccepter = NULL;
	return true;
}


bool CTcpConnect::onCreateUdpChannel(uint32_t id, CTcpConnect* pDstConn, SOCKADDR_IN& udp_addr)
{
	CUdpChannel* pChannel = new CUdpChannel(id, _pReactor, this, pDstConn);

	if (pChannel->onInit())
	{
		udp_addr = pChannel->onGetLocalAddr();
		if(pDstConn->onResponseAccess(udp_addr, onGetPrivateAddr()))
		{
			CNetEvent* pNewNet = new CNetEvent(pChannel->_csSocket, pChannel, CAccepter::onUdpEventCallback);
			_pReactor->add(pChannel->_csSocket._nFd, pNewNet);

			_mapSrcChannels.insert(std::make_pair(pChannel->onGetId(), pChannel));
			pDstConn->onAddDstUdpChannel(pChannel);

			onChangeState(CTcpConnect::STATE_ACCELERATING);
			onSetErr(CErrorCode::ERROR_SOCKET5_SUCCESS);
			return true;
		}

		onSetErr(CErrorCode::ERROR_SOCKET5_COONNECT_NEXE_ERROR);
		DEBUGINFO("client[%04u] response dst client [%4u] access acceleration failed.", _uiId, pDstConn->onGetId());
	}

	if (CErrorCode::ERROR_SOCKET5_SUCCESS != onGetErr())
	{
		delete pChannel;
		return false;
	}

	return true;
}

bool CTcpConnect::onAddDstUdpChannel(CUdpChannel* pChannel)
{
	std::map<uint32_t, CUdpChannel*>::iterator it = _mapDstChannels.find(pChannel->onGetId());
	if (it != _mapDstChannels.end())
	{
		DEBUGINFO("client[%04u] repeate to add dst udp channel [%04u].", _uiId, pChannel->onGetId());
		return false;
	}
	_mapDstChannels.insert(std::make_pair(pChannel->onGetId(), pChannel));
	DEBUGINFO("client[%04u] add dst udp channel [%04u] ok.", _uiId, pChannel->onGetId());
	return true;
}

bool CTcpConnect::onSrcChannelClose(uint32_t id)
{
	std::map<uint32_t, CUdpChannel*>::iterator it = _mapSrcChannels.find(id);
	if (it == _mapSrcChannels.end())
	{
		DEBUGINFO("client[%04u] remove src udp channel[%04u] faild.", _uiId, id);
		return false;
	}
	SOCKADDR_IN addr = it->second->onGetLocalAddr();
	onResponseCloseUdp(addr);
	// delete
	delete it->second;
	_mapSrcChannels.erase(it);

	DEBUGINFO("client[%04u] remove src udp channel[%04u] ok.", _uiId, id);
	return true;
}
bool CTcpConnect::onDstChannelClose(uint32_t id)
{
	std::map<uint32_t, CUdpChannel*>::iterator it = _mapDstChannels.find(id);
	if (it == _mapDstChannels.end())
	{
		DEBUGINFO("client[%04u] remove dst udp channel[%04u] faild.", _uiId, id);
		return false;
	}
	SOCKADDR_IN addr = it->second->onGetLocalAddr();
	onResponseCloseUdp(addr);
	_mapDstChannels.erase(it);

	DEBUGINFO("client[%04u] remove dst udp channel[%04u] ok.", _uiId, id);
	return true;
}

bool CTcpConnect::onReadByte(uint8_t& c)
{
	if(_csReadBuf._uiCliBufSize && _csReadBuf._uiCliOffset < _csReadBuf._uiCliBufSize){
		c = (int)_csReadBuf._szCliBuf[_csReadBuf._uiCliOffset++];
	}
	else
	{
		_csReadBuf._uiCliOffset = _csReadBuf._uiCliBufSize = 0;
		if (!onRead())
		{
			return false;
		}
		_csReadBuf._uiCliOffset = 1;
		c = *_csReadBuf._szCliBuf;
	}
	return true;
}

bool CTcpConnect::onReadLine(uint8_t *szBuff,const uint32_t &uiBuffSize)
{
	if (!szBuff || uiBuffSize <= 0)
	{
		DEBUGINFO("param is null");
		return false;
	}
	uint8_t c;
	uint32_t i=0;
	if(uiBuffSize<2)
		return 0;

	if(!onReadByte(c))
	{
		return false;
	}
	if (c == EOF)
	{
		return false;
	}
	do
	{
		szBuff[i++] = c;
		if(c == EOF )
		{
			return false;
		}
	}while(i < uiBuffSize && (onReadByte(c)));
	return true;
}

bool CTcpConnect::onRead()
{
	int nRead = recv(_csClientCtrl._nFd, _csReadBuf._szCliBuf + _csReadBuf._uiCliBufSize, 8192, 0);
	DEBUGINFO("client[%04u] socket[%d] read %d bypes, buffer remain %d bypes.", _uiId, _csClientCtrl._nFd, nRead, _csReadBuf._uiCliBufSize);

	if(nRead <= 0)
	{
		DEBUGINFO("client[%04u] recv from socket[%d] failed errno %d: %m", _uiId, _csClientCtrl._nFd, errno);
		if (nRead < 0 && (EAGAIN == errno || EINTR == errno || EWOULDBLOCK == errno))
		{
			DEBUGINFO("client[%04u] read again.", _uiId);
		}
		else if (nRead == 0)
		{
			DEBUGINFO("client[%04u] recv faield close socket. err: %m", _uiId);
			onDisconnectd();
		}
		return false;
	}

	_csReadBuf._uiCliBufSize += nRead;
	return true;
}


bool CTcpConnect::onReadHeader()
{
	bzero(&_stHeader, sizeof(SPackageHeader));
	if(!onReadByte(_stHeader.ucHead1))
	{
		_cRes = CErrorCode::ERROR_SYSTEM_NET;
		return false;
	}
	if (_stHeader.ucHead1 != 0xDD)
	{
		DEBUGINFO("header1 error: 0x%X", _stHeader.ucHead1);
		_cRes = CErrorCode::ERROR_SOCKET5_INVALID_VER;
		return false;
	}

	if(!onReadByte(_stHeader.ucHead2))
	{
		_cRes = CErrorCode::ERROR_SYSTEM_NET;
		return false;
	}
	if (_stHeader.ucHead2 != 0x05)
	{
		DEBUGINFO("header2 error: 0x%X", _stHeader.ucHead1);
		_cRes = CErrorCode::ERROR_SOCKET5_INVALID_VER;
		return false;
	}

	if(!onReadByte(_stHeader.ucProtoVersion))
	{
		_cRes = CErrorCode::ERROR_SYSTEM_NET;
		return false;
	}
	if(!onReadByte(_stHeader.ucSerVersion))
	{
		_cRes = CErrorCode::ERROR_SYSTEM_NET;
		return false;
	}
	if(!onReadByte(_stHeader.ucFunc))
	{
		_cRes = CErrorCode::ERROR_SYSTEM_NET;
		return false;
	}

	if(!onReadByte(_stHeader.ucKeyIndex))
	{
		_cRes = CErrorCode::ERROR_SYSTEM_NET;
		return false;
	}

	uint8_t szDesLength[2];
	if(!onReadByte(szDesLength[0]))
	{
		_cRes = CErrorCode::ERROR_SYSTEM_NET;
		return false;
	}
	if(!onReadByte(szDesLength[1]))
	{
		_cRes = CErrorCode::ERROR_SYSTEM_NET;
		return false;
	}

	memcpy(&_stHeader.usBodyLen, szDesLength,2);
//	if(_stHeader.usBodyLen <= 0 || _stHeader.usBodyLen >= 0xffff)
//	{
//		_cRes = CErrorCode::ERROR_SOCKET5_DATA_ERROR;
//		return false;
//	}

	return true;
}


bool CTcpConnect::onReadBody(CBuff& cbuff)
{
	if (_stHeader.usBodyLen == 0)
		return true;

	char* pBuff = NULL;
	if(!(pBuff = (char*)MALLOC(_stHeader.usBodyLen + 1)))
	{
		_cRes = CErrorCode::ERROR_SYSTEM_NET;
		return false;
	}
	bzero(pBuff, _stHeader.usBodyLen);
	if(!onReadLine((uint8_t*)pBuff, _stHeader.usBodyLen))
	{
		_cRes = CErrorCode::ERROR_SYSTEM_NET;
		FREE(pBuff);
		return false;
	}

	DISPLAY_BUFF_VALUE(pBuff, _stHeader.usBodyLen);

	if (SERVER_VERSION::ENCRYPTION == _stHeader.ucSerVersion)
	{
		COpenssl ssl;
		if(!ssl.onDecde((char*)pBuff, _stHeader.usBodyLen, (int)_stHeader.ucKeyIndex))
		{
			DEBUGINFO("decde failed");
			FREE(pBuff);
			return false;
		}

		if(!cbuff.onInit(ssl.onGetDstBuffLen()+1))
		{
			DEBUGINFO("cbuff failed");
			FREE(pBuff);
			return false;
		}
		if(!cbuff.onSetReadBuff(ssl.onGetDstBuff(),ssl.onGetDstBuffLen()))
		{
			DEBUGINFO("set read buff failed");
			FREE(pBuff);
			return false;
		}
	}
	else
	{
		if(!cbuff.onInit(_stHeader.usBodyLen+1))
		{
			DEBUGINFO("cbuff failed");
			FREE(pBuff);
			return false;
		}
		if(!cbuff.onSetReadBuff((char*)pBuff, _stHeader.usBodyLen))
		{
			DEBUGINFO("set read buff failed");
			FREE(pBuff);
			return false;
		}
	}
	FREE(pBuff);
	return true;
}

bool CTcpConnect::onSend(const char* buf, size_t len)
{
	int nSend = send(_csClientCtrl._nFd, buf, len, 0);
	if (nSend < (int)len)
	{
		_pNet->_events |= EPOLLOUT;
		_pReactor->mod(_csClientCtrl._nFd, _pNet->_events, _pNet);

		DEBUGINFO("client[%04u] send %d bytes failed: %m", _uiId, nSend);
		return false;
	}
	return true;
}

void CTcpConnect::onDisplayHead()
{
	DEBUGINFO("client[%04u] [HEAD] 0x%02X%02X | P:%u | V:%u | F:%u | K:%u | L:%u", _uiId,
			_stHeader.ucHead1, _stHeader.ucHead2, _stHeader.ucProtoVersion, _stHeader.ucSerVersion,
			_stHeader.ucFunc, _stHeader.ucKeyIndex, _stHeader.usBodyLen);
}

bool CTcpConnect::onPackSock5Respon(CBuff &bodybuf)
{
	CBuff pkgbuf;
	if(!pkgbuf.onInit(128))
		return false;

	if(!pkgbuf.onWrite((uint8_t)0xDD))
		return false;

	if(!pkgbuf.onWrite((uint8_t)0x05))
		return false;

	if(!pkgbuf.onWrite(_stHeader.ucProtoVersion))
		return false;

	if(!pkgbuf.onWrite(_stHeader.ucSerVersion))
		return false;

	if(!pkgbuf.onWrite(_stHeader.ucFunc))
		return false;

	if(!pkgbuf.onWrite(_stHeader.ucKeyIndex))
		return false;

	if (SERVER_VERSION::ENCRYPTION == _stHeader.ucSerVersion)
	{
		COpenssl ssl;
		if(!ssl.onEncryption(bodybuf._szWriteBuff,bodybuf._nWriteLength, 168))
		{
			DEBUGINFO("encry failed");
			return false;
		}
		if(!pkgbuf.onWrite(ssl.onGetDstBuff(), ssl.onGetDstBuffLen()))
			return false;
	}
	else
	{
		uint16_t len = bodybuf._nWriteLength;
		if(!pkgbuf.onWrite((void*)bodybuf._szWriteBuff, len))
			return false;
	}

	if (!onSend(pkgbuf._szWriteBuff, (uint32_t)pkgbuf._nWriteLength))
		return false;

	DISPLAY_BUFF_VALUE(pkgbuf._szWriteBuff, (uint32_t)pkgbuf._nWriteLength);
	return true;
}

bool CTcpConnect::onResponseLogin()
{
	CBuff cbuff;
	if (!cbuff.onInit(128))
		return false;

	if(!cbuff.onWrite(onGetErr()))
		return false;

	if(!cbuff.onWrite(onGetId()))
		return false;

	onPackSock5Respon( cbuff);
	return true;
}

bool CTcpConnect::onResponseConsoles(std::vector<SGetConsolesRespPackage>& consoles)
{
	CBuff cbuff;
	const int len = 128;
	if (!cbuff.onInit(len))
		return false;

	uint8_t num = (uint8_t)consoles.size();
	if(!cbuff.onWrite(num))
		return false;

	std::vector<SGetConsolesRespPackage>::iterator it = consoles.begin();
	for (; it != consoles.end(); ++it)
		if(!cbuff.onWrite(it->uiId))
			continue;

	onPackSock5Respon(cbuff);
	return true;
}

bool CTcpConnect::onResponseAccelate(SOCKADDR_IN& localaddr)
{
	CBuff cbuff;
	if(!cbuff.onInit(128))
		return false;

	if(!cbuff.onWrite(onGetErr()))
		return false;

	if(!cbuff.onWrite(localaddr.sin_addr.s_addr))
		return false;

	if(!cbuff.onWriteWithLength((char*)&localaddr.sin_port,2))
		return false;

	onPackSock5Respon(cbuff);
	return true;
}

bool CTcpConnect::onResponseAccess(SOCKADDR_IN& localaddr, uint32_t private_addr)
{
	CBuff cbuff;
	if(!cbuff.onInit(16))
		return false;

	if(!cbuff.onWrite(localaddr.sin_addr.s_addr))
		return false;

	if(!cbuff.onWriteWithLength((char*)&localaddr.sin_port,2))
		return false;

	if(!cbuff.onWrite(private_addr))
		return false;

	_stHeader.ucFunc = CFuncCode::FUNC_ACCESS_ACCELERATE;
	onPackSock5Respon(cbuff);

	return true;
}

bool CTcpConnect::onResponseCloseUdp(SOCKADDR_IN& addr)
{
	CBuff cbuff;
	if(!cbuff.onInit(16))
		return false;

	if(!cbuff.onWrite(addr.sin_addr.s_addr))
		return false;

	if(!cbuff.onWriteWithLength((char*)&addr.sin_port,2))
		return false;

	_stHeader.ucFunc = CFuncCode::FUNC_ACCELERATE_STOPPED;
	onPackSock5Respon(cbuff);
}
