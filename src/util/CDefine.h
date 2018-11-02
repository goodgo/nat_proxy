#ifndef SRC_UTILITY_CDEFINE_H_
#define SRC_UTILITY_CDEFINE_H_
#define	VERSION		"nat_proxy@root.copyright.2018.10.30"

#define _DEBUG
#define MALLOC(size) malloc(size)
#define FREE(data) if((data)){free((data));(data)=NULL;}
#define DELETE(data) if((data)){delete((data));(data)=NULL;}

#define SIZEOF(data) (int)sizeof((data))
#define BUFFMAXLEN	 8192
#define CACHEMAXLEN	8192
#define TOINT(data)	(data)
#ifndef TOSTRIG
#define TOSTRING (a) (#a)
#endif

#ifndef TMPVALUEMAXLEN
#define TMPVALUEMAXLEN	256
#endif

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE -1
#endif

#ifndef IPHEADERLENGTH
#define IPHEADERLENGTH		20
#endif

#ifndef UDPHEADERLENGTH
#define UDPHEADERLENGTH		8
#endif

#ifndef TCPHEADERLENGTH
#define TCPHEADERLENGTH		20
#endif

#ifndef USERDATALENGTH
#define USERDATALENGTH		(255 +1 )
#endif

#ifndef TUNREADMAXLEN
#define TUNREADMAXLEN	1500
#endif

typedef int	SOCKET;
typedef int TUNFD;

#ifndef  __CLASS_ADD_MEMBER__
#define  __CLASS_ADD_MEMBER__( privilege,type, var)	\
privilege:	\
	type	_##var;	\
public:		\
	type	on##Get##var() const {return	_##var;}	\
	void	on##Set##var(const type &var){_##var = var;}
#endif

#ifndef  __CLASS_ADD_MEMBER_ARRARY__
#define  __CLASS_ADD_MEMBER_ARRARY__(privilege,type, var,len)	\
privilege:	\
	type	_##var[len];	\
	int		_n##var##Len;		\
public:		\
	const	type*	on##Get##var()const {return	_##var;}	\
	void	on##Set##var(const type *data,int datalen){memcpy(_##var,data,datalen); _n##var##Len = datalen; }	\
	int		on##Get##var##Len() {return _n##var##Len;}
#endif

#ifndef  __CLASS_ADD_MEMBER_POINT__
#define  __CLASS_ADD_MEMBER_POINT__(privilege,type, var)	\
privilege:	\
	type  *_p##var;	\
public:		\
	const   type*	on##Get##var()const {return	_p##var;}	\
	void	on##Set##var(type *data,int datalen)\
		{\
			if(!_p##var){\
				_p##var = new type();\
			}\
			memcpy(_p##var,data,datalen);\
		}
#endif

#ifndef  __CLASS_ADD_MEMBER_NOSET__
#define  __CLASS_ADD_MEMBER_NOSET__(privilege,type, var)	\
privilege:	\
	type  _##var;
#endif

#ifndef	ERROR_STR
#define ERROR_STR(data) gCCommon->errstr(data)
#endif
#ifndef DISPLAY_BUFF_VALUE
#define DISPLAY_BUFF_VALUE(data,len) gCCommon->onDisplay(data,len,__FILE__,__FUNCTION__,__LINE__);
#endif

#ifndef MS2TIMEVAL
#define MS2TIMEVAL(readTimeMs,timeval) \
	{	\
		timeval.tv_sec = readTimeMs / 1000;\
		timeval.tv_usec =  (readTimeMs % 1000)*1000;\
	}
#endif
#ifndef VARIBALE
#define	VARIBALE(data) data
#endif

#ifndef EVENT_PROTOCOL
#define EVENT_PROTOCOL(event) (event).onGetHeaderData()._cProtocol
#endif

#ifndef EVENT_LOCALADDR
#define EVENT_LOCALADDR(event,localaddr)\
		{	\
			switch(EVENT_PROTOCOL(event))\
			{	\
				case IPPROTO_UDP:\
				{\
					CProtocolUdp *p = (CProtocolUdp*)((event)._pCProtocol);\
					if(p)\
					{\
						UINT addr = p->_iPHeader._data._uiLocalAddr;\
						gCCommon->onSockAddr2String((const char*)&addr,(const char*)&p->_udpHeaderData._usLocalPort,localaddr);\
					}\
				}\
				break;\
				case IPPROTO_TCP:\
				{\
					CProtocolTcp *p = (CProtocolTcp*)((event)._pCProtocol);\
					if(p)\
					{\
						UINT addr = p->_iPHeader._data._uiLocalAddr;\
						gCCommon->onSockAddr2String((const char*)&addr,(const char*)&p->_tcpHeaderData._usLocalPort,localaddr);\
					}\
				}\
				break;\
				case IPPROTO_ICMP:\
				{\
					CProtocolIcmp *p = (CProtocolIcmp*)((event)._pCProtocol);\
					if(p)\
					{\
						UINT addr = p->_iPHeader._data._uiLocalAddr;\
						UINT port = 0;\
						gCCommon->onSockAddr2String((const char*)&addr,(const char*)&port,localaddr);\
					}\
				}\
				break;\
			}\
		}
#endif



#ifndef EVENT_REMOTEADDR
#define EVENT_REMOTEADDR(event,localaddr)\
		{	\
			switch(EVENT_PROTOCOL(event))\
			{	\
				case IPPROTO_UDP:\
				{\
					CProtocolUdp *p = (CProtocolUdp*)((event)._pCProtocol);\
					if(p)\
					{\
						UINT addr = p->_iPHeader._data._uiRemoteAddr;\
						gCCommon->onSockAddr2String((const char*)&addr,(const char*)&p->_udpHeaderData._usRemotePort,localaddr);\
					}\
				}\
				break;\
				case IPPROTO_TCP:\
				{\
					CProtocolTcp *p = (CProtocolTcp*)((event)._pCProtocol);\
					if(p)\
					{\
						UINT addr = p->_iPHeader._data._uiRemoteAddr;\
						gCCommon->onSockAddr2String((const char*)&addr,(const char*)&p->_tcpHeaderData._usRemotePort,localaddr);\
					}\
				}\
				break;\
				case IPPROTO_ICMP:\
				{\
					CProtocolIcmp *p = (CProtocolIcmp*)((event)._pCProtocol);\
					if(p)\
					{\
						UINT addr = p->_iPHeader._data._uiRemoteAddr;\
						UINT port = 0;\
						gCCommon->onSockAddr2String((const char*)&addr,(const char*)&port,localaddr);\
					}\
				}\
				break;\
			}\
		}
#endif


#ifndef EVENT_PROTOCOL_UDP
#define EVENT_PROTOCOL_UDP(event) (CProtocolUdp*)(&(event))->_pCProtocol
#endif

#define IPV4	4
#define IPV6	6

#define DDVPN_TCPH_LENGTH(optionlen) (((20+(optionlen))/4)<<4)
#define DDVPN_TCPH_OFFSET(clength) ((clength)>>4)*4

#define TCP_WINDOW_SIZE		14600
#define MAX_OPTIONS_COUNT	16

#define SOCKVALID(socket) socket > 2

#define USERWARN util::Logger<util::LogModule::USR_WARN>(__FILE__, __FUNCTION__, __LINE__, GETPID)
#define DEBUGINFO util::Logger<util::LogModule::DBG_INF>(__FILE__, __FUNCTION__, __LINE__, GETPID)
#define USERINFO util::Logger<util::LogModule::USR_INF>(__FILE__, __FUNCTION__, __LINE__, GETPID)
#define USERIMPORTENT util::Logger<util::LogModule::USR_IMP>(__FILE__, __FUNCTION__, __LINE__, GETPID)
#define USERCRITIAL util::Logger<util::LogModule::USR_CRI>(__FILE__, __FUNCTION__, __LINE__, GETPID)
#define SYSTEMINFO util::Logger<util::LogModule::SYS_INF>(__FILE__, __FUNCTION__, __LINE__, GETPID)

#endif /* SRC_UTILITY_CDEFINE_H_ */
