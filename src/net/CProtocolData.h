#ifndef SRC_PROTOL_CPROTOCOLDATA_H_
#define SRC_PROTOL_CPROTOCOLDATA_H_

typedef struct _offset
{
	uint8_t		_ucFlag;
	uint8_t		_ucOffset;
}SIPHeaderOffset;

typedef struct _header{
	uint8_t		_cVersion;
	uint8_t		_cTypeService;
	uint16_t	_usDataLength;
	uint16_t	_usIdentification;
	uint16_t	_usFlagOffset;
	uint8_t		_cTtl;
	uint8_t		_cProtocol;
	uint16_t	_usChkSum;
	uint32_t	_uiLocalAddr;
	uint32_t	_uiRemoteAddr;
}SIPHeaderData;

typedef struct _udp{
	uint16_t	_usLocalPort;
	uint16_t	_usRemotePort;
	uint16_t	_usLength;
	uint16_t	_usChkSum;
}SUdpHeaderData;

typedef struct _tcp{
	uint16_t	_usLocalPort;
	uint16_t	_usRemotePort;
	uint32_t	_uiSequenceNum;
	uint32_t	_uiAcknowledgeNum;
	uint8_t		_cLength;
	uint8_t		_cFlag;
	uint16_t	_usWindowSize;
	uint16_t	_usChkSum;
	uint16_t	_usUrg;
}STcpHeaderData;

typedef struct _tcp_last{
	uint8_t		_ucFlag;
	uint32_t	_uiSequenceNum;
	uint32_t	_uiAcknowledgeNum;
	uint32_t	_uiAppLength;
}STcpLastPackage;

typedef struct _imcp{
	uint8_t 	_cType;//消息类型
	uint8_t	    _cCode;//代码
	uint16_t	_usChecksum;//校验和
	uint16_t	_usPid;//用来唯一标识此请求的id号，通常设置为进程id
	uint16_t	_usSequence;//序列号
	uint64_t	_ultimestamp;//时间戳
}SIcmpHeaderData;

typedef struct _tcp_option{
	uint8_t		_cKind;
	uint8_t		_cLength;
	uint8_t		_szValue[32];
}STcpOptionData;

typedef struct _tcp_send_recv{
	uint8_t	   *_pszReadTunBuff;
	uint32_t	_usReadTunBuffLength;
	uint32_t	_usReadTunBuffMaxLength;

	uint8_t	   *_pszReadSockBuff;
	uint32_t	_usReadSockBuffLength;
	uint32_t	_usReadSockBuffMaxLength;

	uint16_t	_usCurrentWindowsize;
	uint16_t	_usPeerCurrentWindowsize;

	pthread_mutex_t  _mutex_sock;
	pthread_mutex_t  _mutex_tun;
	pthread_mutex_t  _mutex_snd_tun;

}STcpNetData;

typedef struct{
	char	_flag;
	short	_offset;
}SIpOffset;


/////////////////////////////////////////////////

class SERVER_VERSION
{
public:
	enum{
		NO_ENCRYPTION = 0x02,
		ENCRYPTION = 0x04,
	};
};

typedef struct
{
	uint8_t ucHead1;
	uint8_t ucHead2;
	uint8_t ucProtoVersion;
	uint8_t ucSerVersion;
	uint8_t ucFunc;
	uint8_t ucKeyIndex;
	uint16_t usBodyLen;
}SPackageHeader;

typedef struct
{
	uint8_t cErrno;
	uint32_t uiId;
}SLoginRespPackage;

typedef struct
{
	uint32_t uiId;
}SGetConsolesRespPackage;

#endif /* SRC_PROTOL_CPROTOCOLDATA_H_ */
