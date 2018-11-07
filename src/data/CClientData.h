#ifndef SRC_DATA_CLIENT_DATA_H
#define SRC_DATA_CLIENT_DATA_H

#include "CInclude.h"

typedef struct sockaddr_in SOCKADDR_IN;


class CFuncCode {
public:
	enum {
		FUNC_HEARTBEAT = 0x00,
		FUNC_LOGIN = 0x01,
		FUNC_ACCELERATE = 0x02,
		FUNC_CONSOLES = 0x03,
		FUNC_ACCESS_ACCELERATE = 0x04,
		FUNC_ACCELERATE_STOPPED = 0x05,
	};
};

class CErrorCode {
public:
	enum{
		ERROR_SOCKET5_=0x00,
		ERROR_SOCKET5_SUCCESS=0X00,
		ERROR_SOCKET5_INVALID_VER,
		ERROR_SOCKET5_DATA_ERROR,
		ERROR_SOCKET5_AUTH_ERROR,
		ERROR_SOCKET5_MAXCHAIN_ERROR,
		ERROR_SOCKET5_COONNECT_NEXE_ERROR,
		ERROR_SOCKET5_NEXT_RESPONSE_ERROR,
		ERROR_SOCKET5_OUTIP_ERROR,
		ERROR_SOCKET5_INIP_ERROR,
		ERROR_SOCKET5_IN_BLACK=0x16,
		ERROR_SOCKET5_OTHER_ERROR=0xFF,
		/****************************************/
		ERROR_SYSTEM_=0x10,
		ERROR_SYSTEM_NET,
		ERROR_SYSTEM_MEMORY,
		ERROR_SYSTEM_TOO_OPEN,
		ERROR_SYSTEM_BIND_ERROR,
		ERROR_SYSTEM_SOCKETBUFF_TOOSHORT,
		ERROR_SYSTEM_SETOPT_FAILED,
		/****************************************/
		ERROR_WEB_
	};
};

struct CSocketData
{
	uint32_t	_nProtol;
	uint32_t	_nFd;
	uint32_t	_nOptions;

	SOCKADDR_IN	_localSockaddr;
	SOCKADDR_IN	_remoteSockAddr;
};

class CSocket5Data
{
public:
	uint8_t 	_cVersion1;
	uint8_t 	_cVersion2;
	uint8_t 	_cMethod;
	uint8_t 	_cSecond;
	uint8_t 	_cCommand;
	uint8_t 	_cReserved;
	uint8_t 	_cAtype;
	uint32_t 	_nTagIp;
	uint16_t 	_usTagPort;
	uint8_t		_szAddr[6];
	uint8_t 	_cRandIndex;
	uint8_t 	_cErrorCode;
	uint16_t 	_nProtol;
	uint16_t 	_nDDauthPort;
	SOCKADDR_IN _srcAddr;
	SOCKADDR_IN _tagAddr;
	SOCKADDR_IN _nodeAddr;
	SOCKADDR_IN _nodeResAddr;
	CSocketData _udpSock;
	CSocketData _tcpSock;
};



class CUserData
{
public:
	uint8_t* _pszUserName;
	uint8_t* _pszUserPwd;
	uint8_t* _pszUserId;
	uint8_t* _pszGameId;
	uint8_t* _pszAddrList;
	uint8_t* _pszBandLimit;
	uint8_t* _pszGuid;
	uint8_t* _pszDstGuid;
	uint8_t* _pszAuthToken;
	uint8_t _cUserNameLength;
	uint8_t _cUserPwdLength;
	uint8_t _cUserIdLength;
	uint8_t _cGameIdLength;
	uint8_t _cAddrLength;
	uint8_t _cBandLimitLength;
	uint8_t _cGuidLength;
	uint8_t _cDstGuidLength;
	uint8_t _cAuthTokenLength;
	uint8_t _cNatType;
	uint8_t _cVersion;
	uint8_t _cMaxChain;
	uint8_t _cCurrentChain;
	uint32_t _uiMtu;
	uint32_t _uiIdentification;
	uint8_t _ucConnectAddr[4];
	uint8_t* _szVirtualAddr;
	uint32_t _uiVirtualAddrLength;

public:
	bool	onPack(uint8_t *szData,uint32_t &nDataLen,const uint32_t nszDataMaxLen);
};

class CClientDataEx
{
public:
	uint8_t*	_szDesData;
	uint8_t		_cDesLength;
	uint8_t*	_szRredirectData;
	uint32_t	_uiRedirectLength;
	uint8_t*	_szAuthCodeData;
	uint32_t	_uiAuthCodeDataLength;
};

class CClientData
{
public:
	uint8_t			_cComm;
	uint8_t			_cRes;
	uint32_t		_uiClientAddr;
	CClientDataEx	_csParamex;
	CUserData		_csUserData;
	CSocket5Data	_csClientSocket5;
};

class CBufInfo
{
public:
	CBufInfo(): _szCliBuf(NULL), _szCliTcpBuf(NULL), _szCliValidBuf(NULL)
				, _uiCliBufMaxSize(0), _uiCliBufSize(0), _uiCliOffset(0), _uiCliValidBufSize(0)
	{}
	~CBufInfo()
	{
		FREE(_szCliBuf);
		FREE(_szCliTcpBuf);
		FREE(_szCliValidBuf);

		_uiCliBufMaxSize = _uiCliBufSize = _uiCliOffset = _uiCliValidBufSize = 0;
	}

public:
	uint8_t*		_szCliBuf;
	uint8_t*		_szCliTcpBuf;
	uint8_t*		_szCliValidBuf;
	uint32_t		_uiCliBufMaxSize;
	uint32_t		_uiCliBufSize;
	uint32_t		_uiCliOffset;
	uint32_t		_uiCliValidBufSize;
};



#endif
