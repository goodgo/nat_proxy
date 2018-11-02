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
	DEBUGINFO("Delete CTcpConnect. ID: %u", onGetId());
	onDisconnectd();
	_pNet = NULL;
	_pReactor = NULL;
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
	if (!_bConnect)
		return false;

	_bConnect = false;
	if (NULL != _pReactor)
	{
		int events = EPOLLOUT | EPOLLIN;
		_pReactor->del(_csClientCtrl._nFd, events);
	}

	CSocket::onClose(_csClientCtrl);
	for (size_t i = 0; i < _vInChannels.size(); i++)
	{
		_pAccepter->onShutdownChannel(_vInChannels[i], 0);
	}

	for (size_t i = 0; i < _vOutChannels.size(); i++)
	{
		_pAccepter->onShutdownChannel(_vOutChannels[i], 1);
	}
	return true;
}

bool CTcpConnect::onChangeState(ESTATE state)
{
	_eState = state;
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
	DEBUGINFO("client[%u] socket[%d] read %d bypes, buffer remain %d bypes.", _uiId, _csClientCtrl._nFd, nRead, _csReadBuf._uiCliBufSize);

	if(nRead <= 0)
	{
		DEBUGINFO("recv from socket[%d] failed errno %d: %m", _csClientCtrl._nFd, errno);
		if (EAGAIN == errno || EINTR == errno)
		{
			DEBUGINFO("client%u read again.", _uiId);
		}
		else
		{
			DEBUGINFO("client%u recv faield %m", _uiId);
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

		DEBUGINFO("client%u send %d bytes failed: %m", _csClientCtrl._nFd, nSend);
		return false;
	}
	return true;
}

void CTcpConnect::onDisplayHead()
{
	DEBUGINFO("[HEAD] 0x%X 0x%X | P:%u | V:%u | F:%u | K:%u | L:%u",
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
