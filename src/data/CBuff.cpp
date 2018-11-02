#include <CBuff.h>
void	CBuff::onRelease()
{
	if(_szLastRecvBuff)
	{
		delete []_szLastRecvBuff;
		_szLastRecvBuff = NULL;
	}
	if(_szReadBuff)
	{
		delete []_szReadBuff;
		_szReadBuff = NULL;
	}
	if(_szRecvBuff)
	{
		delete []_szRecvBuff;
		_szRecvBuff = NULL;
	}
	if(_szWriteBuff)
	{
		delete []_szWriteBuff;
		_szWriteBuff = NULL;
	}
}
bool	CBuff::onInit(const int &nMaxLen)
{
	onRelease();
	_nLastRecvBuffLength = 0;
	_nReadLength = 0;
	_nRecvBuffLength = 0;
	_nWriteLength = 0;
	_nCurrentReadMove = 0;
	if(nMaxLen == 0)
	{
		_nMaxBuffLen = CACHEMAXLEN;
	}
	else
	{
		_nMaxBuffLen = nMaxLen;
	}
	_szLastRecvBuff = new char[nMaxLen]();
	if (!_szLastRecvBuff)
		return false;
	_szReadBuff = new char [nMaxLen]();
	if (!_szReadBuff)
		return false;
	_szRecvBuff = new char [nMaxLen]();
	if (!_szRecvBuff)
		return false;
	_szWriteBuff = new char [nMaxLen]();
	if (!_szWriteBuff)
		return false;
	return true;
}
void	CBuff::onReset()
{
	onInit();
}
bool	CBuff::onWriteWithLength(const char *pVoidData,const short &nDataLen)
{
	if(_nWriteLength + nDataLen >= _nMaxBuffLen)
		return false;

	PMEMCPY(_szWriteBuff+_nWriteLength, pVoidData ,nDataLen );
	_nWriteLength += nDataLen;
	return true;
}
bool	CBuff::onWrite(const unsigned char *szData,const unsigned char &nDataLen)
{
	if(!onWrite(nDataLen))
		return false;

	if(_nWriteLength + nDataLen >= _nMaxBuffLen)
		return false;

	PMEMCPY(_szWriteBuff+_nWriteLength, szData ,nDataLen );
	_nWriteLength += nDataLen;
	return true;
}
bool	CBuff::onWrite(const char *pVoidData,const unsigned char &nDataLen)
{
	if(!onWrite(nDataLen))
		return false;

	if(_nWriteLength + nDataLen >= _nMaxBuffLen )
		return false;

	PMEMCPY(_szWriteBuff+_nWriteLength, pVoidData ,nDataLen );
	_nWriteLength += nDataLen;
	return true;
}
bool	CBuff::onWrite(const void *pVoidData,const unsigned short &nDataLen)
{
	if(!onWrite(nDataLen))
		return false;

	if(_nWriteLength + nDataLen >= _nMaxBuffLen )
		return false;

	PMEMCPY(_szWriteBuff+_nWriteLength, pVoidData ,nDataLen );
	_nWriteLength += nDataLen;
	return true;
}
bool	CBuff::onWrite(const void *pVoidData,const int &nDataLen)
{
	if(_nWriteLength + nDataLen >= _nMaxBuffLen )
		return false;

	PMEMCPY(_szWriteBuff+_nWriteLength, pVoidData ,nDataLen );
	_nWriteLength += nDataLen;
	return true;
}
bool	CBuff::onWrite(const uint32_t &nData)
{
	if(_nWriteLength +SIZEOF(nData) >= _nMaxBuffLen )
		return false;

	PMEMCPY(_szWriteBuff+_nWriteLength, &nData ,sizeof(nData) );
	_nWriteLength += sizeof(nData);
	return true;
}
bool	CBuff::onWrite(const unsigned short &nData)
{
	if(_nWriteLength +SIZEOF(nData) >= _nMaxBuffLen )
		return false;

	PMEMCPY(_szWriteBuff+_nWriteLength, &nData ,sizeof(nData) );
	_nWriteLength += sizeof(nData);
	return true;
}
bool CBuff::onWrite(const short &nData)
{
	if(_nWriteLength +SIZEOF(nData) >= _nMaxBuffLen )
		return false;

	PMEMCPY(_szWriteBuff+_nWriteLength, &nData ,sizeof(nData) );
	_nWriteLength += sizeof(nData);
	return true;
}
bool CBuff::onWrite(const char &nData)
{
	if(_nWriteLength +SIZEOF(nData) >= _nMaxBuffLen )
		return false;

	PMEMCPY(_szWriteBuff+_nWriteLength, &nData ,sizeof(nData) );
	_nWriteLength += sizeof(nData);
	return true;
}
bool CBuff::onWrite(const int &nData)
{
	if(_nWriteLength +SIZEOF(nData) >= _nMaxBuffLen )
		return false;

	PMEMCPY(_szWriteBuff+_nWriteLength, &nData ,sizeof(nData) );
	_nWriteLength += sizeof(nData);
	return true;
}

bool CBuff::onWrite(const unsigned char &ucData )
{
	if(_nWriteLength +SIZEOF(ucData) >= _nMaxBuffLen )
		return false;

	PMEMCPY(_szWriteBuff+_nWriteLength, &ucData ,sizeof(ucData) );
	_nWriteLength += sizeof(ucData);
	return true;
}
bool	CBuff::onWrite(const char *szData,const short &nLength)
{
	if(_nWriteLength +nLength >= _nMaxBuffLen )
		return false;

	onWrite(nLength);
	PMEMCPY(_szWriteBuff+_nWriteLength, szData ,nLength );
	_nWriteLength += nLength;
	return true;
}
bool CBuff::onWrite(const char *szData,const int &nLength)
{
	if(_nWriteLength +nLength >= _nMaxBuffLen )
		return false;

	onWrite(nLength);
	PMEMCPY(_szWriteBuff+_nWriteLength, szData ,nLength );
	_nWriteLength += nLength;
	return true;
}
bool	CBuff::onRead(void *pVoidData,unsigned short &nDataLen)
{
	if (!pVoidData)
		return false;

	if (!onRead(nDataLen))
		return false;

	if (_nCurrentReadMove + nDataLen > _nReadLength)
		return false;

	PMEMCPY(pVoidData,_szReadBuff+_nCurrentReadMove,nDataLen);
	_nCurrentReadMove += nDataLen;
	return true;
}
bool	CBuff::onRead(void *pVoidData,const int &nDataLen)
{
	if (!pVoidData)
		return false;

	if (_nCurrentReadMove + nDataLen > _nReadLength)
		return false;

	PMEMCPY(pVoidData,_szReadBuff+_nCurrentReadMove,nDataLen);
	_nCurrentReadMove += nDataLen;
	return true;
}
bool	CBuff::onRead(unsigned short &uData)
{
	if (_nCurrentReadMove + sizeof(uData) > (size_t)_nReadLength)
		return false;

	PMEMCPY(&uData,_szReadBuff+_nCurrentReadMove,sizeof(uData));
	_nCurrentReadMove += sizeof(uData);
	return true;
}
bool	CBuff::onRead(uint32_t &uData)
{
	if (_nCurrentReadMove + sizeof(uData) > (size_t)_nReadLength)
		return false;

	PMEMCPY(&uData,_szReadBuff+_nCurrentReadMove,sizeof(uData));
	_nCurrentReadMove += sizeof(uData);
	return true;
}
bool	CBuff::onRead(short &nData)
{
	if (_nCurrentReadMove + sizeof(nData) > (size_t)_nReadLength)
		return false;

	PMEMCPY(&nData,_szReadBuff+_nCurrentReadMove,sizeof(nData));
	_nCurrentReadMove += sizeof(nData);
	return true;
}
bool CBuff::onRead( int &nData)
{
	if (_nCurrentReadMove + sizeof(nData) > (size_t)_nReadLength)
		return false;

	PMEMCPY(&nData,_szReadBuff+_nCurrentReadMove,sizeof(nData));
	_nCurrentReadMove += sizeof(nData);
	return true;
}
bool CBuff::onRead( unsigned char &ucData )
{
	if (_nCurrentReadMove + sizeof(ucData) > (size_t)_nReadLength)
		return false;

	PMEMCPY(&ucData,_szReadBuff+_nCurrentReadMove,sizeof(ucData));
	_nCurrentReadMove += sizeof(ucData);
	return true;
}
bool CBuff::onRead( char &ucData )
{
	if (_nCurrentReadMove + sizeof(ucData) > (size_t)_nReadLength)
		return false;

	PMEMCPY(&ucData,_szReadBuff+_nCurrentReadMove,sizeof(ucData));
	_nCurrentReadMove += sizeof(ucData);
	return true;
}
bool	CBuff::onRead(char *szBuff,unsigned char &cLen)
{
	if (!onRead(cLen))
		return false;

	if (_nCurrentReadMove + cLen > _nReadLength)
		return false;

	PMEMCPY(szBuff,_szReadBuff+_nCurrentReadMove,cLen);
	szBuff[cLen] = '\0';
	_nCurrentReadMove += cLen;
	return true;
}
bool CBuff::onRead(char *szBuff,int &nBuffLength)
{
	if (!onRead(nBuffLength))
		return false;

	if (_nCurrentReadMove + nBuffLength > _nReadLength)
		return false;

	if(nBuffLength == 0)
		return true;

	PMEMCPY(szBuff,_szReadBuff+_nCurrentReadMove,nBuffLength);
	_nCurrentReadMove += nBuffLength;
	return true;
}
bool	CBuff::onDelHeadEnd(BuffType opType)
{
	switch(opType)
	{
	case	CWRITE:
		onDelHeadEnd(_szWriteBuff,_nWriteLength);
		break;
	case	CREAD:
		onDelHeadEnd(_szReadBuff,_nReadLength);
		break;
	default:
		return false;
	}
	return true;
}
bool	CBuff::onAddHeadEnd(BuffType opType)
{
	switch(opType)
	{
	case	CWRITE:
		onAddHeadEnd(_szWriteBuff,_nWriteLength);
		break;
	case	CREAD:
		onAddHeadEnd(_szReadBuff,_nReadLength);
		break;
	default:
		return false;
	}
	return true;
}

bool	CBuff::onAddHeadEnd(char *szBuff,int &nBuffLen)
{
	if (!szBuff)
		return false;

	if (nBuffLen > _nMaxBuffLen -4)
		return false;

	char szTmpBuff[TOINT(_nMaxBuffLen)];
	int	nTmpLength = 0;
	bzero(szTmpBuff,sizeof(szTmpBuff));
	//szTmpBuff[nTmpLength++]=BUFFHEADER;
	//szTmpBuff[nTmpLength++]=BUFFHEADER2;
	memcpy(szTmpBuff+nTmpLength,szBuff,nBuffLen);
	nTmpLength += nBuffLen;
	//szTmpBuff[nTmpLength++]=BUFFEND;
	//szTmpBuff[nTmpLength++]=BUFFEND2;
	nBuffLen = nTmpLength;
	bzero(szBuff,nBuffLen);
	memcpy(szBuff,szTmpBuff,nBuffLen);
	return true;
}

bool	CBuff::onDelHeadEnd(char *szBuff,int &nBuffLen)
{
	if (!szBuff)
		return false;

	char szTmpBuff[TOINT(_nMaxBuffLen)];
	bzero(szTmpBuff,sizeof(szTmpBuff));
	memcpy(szTmpBuff,szBuff+2,nBuffLen-4);
	bzero(szBuff,nBuffLen);
	nBuffLen = nBuffLen-4;
	memcpy(szBuff,szTmpBuff,nBuffLen);
	return true;
}

bool	CBuff::onAppendRecvBuff(const char *szBuff,const int &nBuffLength)
{

	if (!szBuff)
		return false;

	if (nBuffLength <= 0 || nBuffLength > _nMaxBuffLen)
	{
		DEBUGINFO("length too max or min");
		return false;
	}

	if (_nRecvBuffLength+nBuffLength > _nMaxBuffLen)
	{
		DEBUGINFO("append faile ,CBuff too full");
		return false;
	}

	memmove(_szRecvBuff+_nRecvBuffLength,szBuff,nBuffLength);
	_nRecvBuffLength += nBuffLength;
	return true;
}
bool	CBuff::onGetValidBuff(char *szBuff,int &nBuffLength)
{
	if (!szBuff)
		return false;
	return true;
}
bool	CBuff::onGetValidBuff(char *szBuff,int &nBuffLength,int &nErrno)
{/*
	bool	bFind = false;

	if (!szBuff);
	char	szTmp[CACHEMAXLEN];
	bzero(szTmp,sizeof(szTmp));
	int	nMove = 0 ,nBegin = -1 , nEnd = -1;
	nErrno = 0;
	while(nMove < _nRecvBuffLength-1)
	{
		if(_szRecvBuff[nMove] == (char)BUFFHEADER &&
			_szRecvBuff[nMove+1] == (char)BUFFHEADER2 &&
			nBegin == -1)
		{
			nBegin = nMove;
		}
		if(_szRecvBuff[nMove] == (char)BUFFEND &&
			_szRecvBuff[nMove+1] == (char)BUFFEND2 &&
			nBegin != -1)
		{
			nEnd = nMove;
		}
		if(nBegin != -1 && nEnd != -1)
		{
			memcpy(szBuff,_szRecvBuff+nBegin,nEnd-nBegin+2);
			nBuffLength = nEnd-nBegin+2;
			if(_nRecvBuffLength-nBuffLength > 0)
			{
				memcpy(szTmp,_szRecvBuff+nEnd+1,_nRecvBuffLength-nBuffLength);
				bzero(_szRecvBuff,sizeof(_szRecvBuff));
				memcpy(_szRecvBuff,szTmp,_nRecvBuffLength-nBuffLength);
				_nRecvBuffLength-=nBuffLength;
			}
			else
			{
				bzero(_szRecvBuff,sizeof(_szRecvBuff));
				_nRecvBuffLength = 0;
			}
			bFind = true;
			break;
		}
		nMove++;
	}
	if(_nRecvBuffLength >= MAX_PACK && nBegin == -1)
	{
		_nRecvBuffLength = 0;
		bzero(_szRecvBuff,sizeof(_szRecvBuff));
		nErrno = ERROR_PACK;
	}

	return bFind;*/
	return true;
}
bool	CBuff::onSetReadBuff(const char *szBuff,const int &nBuffLen)
{
	if (!szBuff)
		return false;

	if (nBuffLen <= 0 || nBuffLen >= _nMaxBuffLen)
		return false;

	_nReadLength = nBuffLen;
	bzero(_szReadBuff,sizeof(_szReadBuff));
	memcpy(_szReadBuff,szBuff,nBuffLen);
	return true;
}
bool	CBuff::onSetWriteBuff(const char *szBuff,const int &nBuffLen)
{
	if (!szBuff)
		return false;

	if (nBuffLen <= 0 || nBuffLen >= _nMaxBuffLen)
		return false;

	bzero(_szWriteBuff,sizeof(_szWriteBuff));
	_nWriteLength = nBuffLen;
	memcpy(_szWriteBuff,szBuff,nBuffLen);
	return true;
}
