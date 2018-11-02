#ifndef SRC_UTILITY_COPENSSL_H_
#define SRC_UTILITY_COPENSSL_H_

#include "CDefine.h"
#include "rand_key.h"

class COpenssl
{
public:
	bool	onEncryption();/*加密*/
	bool	onDecode();/*解密*/
private:
	void 	SimpleXor_Crype(unsigned char *out, unsigned char *in, int len);
	bool	onGetRandKey();
public:
	bool	onEncryption(const char *szBuff,const int &nBuffLen,const unsigned char &keyIndex);/*加密*/
	bool	onDecde(const char *szBuff,const int &nBuffLen,const unsigned char &keyIndex);/*解密*/
public:
	bool  	onDesMd5( const char *data,char *pout);
	bool 	onMd5KeyCreate(char *pout);
public:
	char	*onGetDstBuff(){return this->_dstBuff;}
	int		onGetDstBuffLen(){return this->_dstBuffLen;}
private:
	char	_srcBuff[BUFFMAXLEN];
	int		_srcBuffLen;
	char	_dstBuff[BUFFMAXLEN];
	int		_dstBuffLen;
	unsigned char _encIndex;
	char	_keyBuff[8];
};
#endif /* SRC_UTILITY_COPENSSL_H_ */
