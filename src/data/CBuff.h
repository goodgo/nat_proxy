#ifndef SRC_LOGIC_CBUFF_H_
#define SRC_LOGIC_CBUFF_H_
#include "CInclude.h"
#define CHKBUFFEND(buff,readlen) 				(!buff || int(readlen) > strlen((char *)buff))
#define PMEMCPY( data1,data2,len ) 			memcpy(data1,data2,len)
#define MAX_PACK	4096
#define ERROR_PACK	-1
class CBuff
{
public:
	enum	BuffType
	{
		CWRITE,
		CREAD
	};
public:
	CBuff():_szRecvBuff(NULL),_szLastRecvBuff(NULL),_szWriteBuff(NULL),_szReadBuff(NULL), _nRecvBuffLength(0)
			, _nLastRecvBuffLength(0), _nMaxBuffLen(0), _nWriteLength(0), _nReadLength(0), _nCurrentReadMove(0)
	{}
	~CBuff(){this->onRelease();}
	bool	onInit(const int &nMaxLen = CACHEMAXLEN);
	void	onReset();
	void	onRelease();
public:
	bool	onWrite(const void *pVoidData,const unsigned short &nDataLen);
	bool	onWrite(const void *pVoidData,const int &nDataLen);
	bool	onWrite(const int &nData);
	bool	onWrite(const char &nData);
	bool	onWrite(const short &nData);
	bool	onWrite(const unsigned short &nData);
	bool	onWrite(const uint32_t &nData);
	bool	onWrite(const unsigned char	&cData);
	bool	onWrite(const char *szData,const int &nLen);
	bool	onWrite(const char *szData,const short &nLen);
	bool	onWrite(const char *szData,const unsigned char &nDataLen);
	bool	onWrite(const unsigned char *szData,const unsigned char &nDataLen);
	bool	onWriteWithLength(const char *szData,const short &nLen);
public:
	bool	onRead(void *pVoidData,const int &nDataLen);
	bool	onRead(void *pVoidData,unsigned short &nDataLen);
	bool	onRead(uint32_t &uData);
	bool	onRead(unsigned short &uData);
	bool	onRead(int &nData);
	bool	onRead(unsigned char	&ucData);
	bool	onRead(char *szData,int &nLen);
	bool	onRead(char *szData,unsigned char &cLen);
	bool 	onRead(char &ucData );
	bool	onRead(short &nData);
public:
	bool	onAddHeadEnd(BuffType opType);
	bool	onDelHeadEnd(BuffType opType);
private:
	bool	onAddHeadEnd(char *szBuff,int &nBuffLen);
	bool	onDelHeadEnd(char *szBuff,int &nBuffLen);
public:
	bool	onAppendRecvBuff(const char *szBuff,const int &nBuffLength);
	bool	onGetValidBuff(char *szBuff,int &nBuffLength,int &nErrno);
	bool	onGetValidBuff(char *szBuff,int &nBuffLength);
public:
	bool	onSetReadBuff(const char *szBuff,const int &nBuffLen);
	bool	onSetWriteBuff(const char *szBuff,const int &nBuffLen);
public:
	char	*_szRecvBuff;
	char	*_szLastRecvBuff;
	char	*_szWriteBuff;
	char	*_szReadBuff;
	int		_nRecvBuffLength;
	int		_nLastRecvBuffLength;
private:
	int		_nMaxBuffLen;
public:
	int		_nWriteLength;
	int		_nReadLength;
	int		_nCurrentReadMove;
};

#endif /* SRC_LOGIC_CBUFF_H_ */
