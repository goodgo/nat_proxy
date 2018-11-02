#include "COpenssl.h"
#include "CConfig.h"
#include "CInclude.h"

static const char enkey[] = "dxxikwoxnixbccvamiwopoikdnwdvnwezdelfgac";
bool	COpenssl::onEncryption()
{
	EVP_CIPHER_CTX ctx;
	int ret = 0;
	int outl = 0;
	int padl = 0;
	EVP_CIPHER_CTX_init(&ctx);

	unsigned char* outBuf = (unsigned char *)MALLOC(this->_srcBuffLen+EVP_MAX_KEY_LENGTH); //缓冲区
	memset(outBuf,0,this->_srcBuffLen+EVP_MAX_KEY_LENGTH);

	ret = EVP_EncryptInit_ex(&ctx, EVP_des_ecb(), NULL, (unsigned char*)this->_keyBuff, NULL);
	if (0 == ret){
		EVP_CIPHER_CTX_cleanup(&ctx);
		FREE(outBuf);
		return false;
	}

	ret = EVP_EncryptUpdate(&ctx, outBuf, &outl, (unsigned char*)this->_srcBuff, this->_srcBuffLen);
	if (0 == ret){
		EVP_CIPHER_CTX_cleanup(&ctx);
		FREE(outBuf);
		return false;
	}

	ret = EVP_EncryptFinal_ex(&ctx, outBuf+outl, &padl);
	if (0 == ret){
		EVP_CIPHER_CTX_cleanup(&ctx);
		FREE(outBuf);
		return false;
	}

	EVP_CIPHER_CTX_cleanup(&ctx);
	this->_dstBuffLen = outl + padl;
	memcpy(this->_dstBuff,outBuf,outl + padl);
	FREE(outBuf);
	return true;
}
bool	COpenssl::onDecode()
{
	EVP_CIPHER_CTX ctx;
	int res = 0;
	int len1 = 0;
	int len2 = 0;

	EVP_CIPHER_CTX_init(&ctx);
	unsigned char keybuff[8];
	memcpy(keybuff,_keyBuff,8);

	unsigned char srcbuff[1024]={0};
	memcpy(srcbuff,_srcBuff,_srcBuffLen);

	unsigned char dstBuff[1024]={0};

	res = EVP_DecryptInit_ex(&ctx, EVP_des_ecb(), NULL, keybuff, NULL);
	if (0 == res){
		EVP_CIPHER_CTX_cleanup(&ctx);
		return false;
	}

	res = EVP_DecryptUpdate(&ctx, dstBuff, &len1, srcbuff, _srcBuffLen);
	if (0 == res){
		EVP_CIPHER_CTX_cleanup(&ctx);
		return false;
	}

	res = EVP_DecryptFinal_ex(&ctx, dstBuff +len1, &len2);
	if (0 == res){
		EVP_CIPHER_CTX_cleanup(&ctx);
		return false;
	}

	EVP_CIPHER_CTX_cleanup(&ctx);
	_dstBuffLen = len1 + len2;
	memcpy(_dstBuff,dstBuff,_dstBuffLen);
	return true;
}
bool COpenssl::onMd5KeyCreate(char *szPout)
{
	if (!szPout)
	{
		DEBUGINFO("param is error");
		return false;
	}

	char key[32]={0};
	char tmpKey[16][32];
	char tagKey[32]={0};
	int  ikey = 0 , lukey2 = 0 , j = 0 , i = 0 ,imem = 0, count = 0;
	bzero(key,sizeof(key));
	bzero(tagKey,sizeof(tagKey));
	for(i = 0 ; i < 16; ++i){
		bzero(tmpKey[i],sizeof(tmpKey[i]));
	}
	ikey = atoi(gCConfig->onGetopensslData().onGetszMd5Key1().c_str());
	lukey2 = atoi(gCConfig->onGetopensslData().onGetszMd5Key1().c_str());
	ikey = ikey>>3;
	ikey = ikey & lukey2;
	sprintf(key,"%s%d",gCConfig->onGetopensslData().onGetszMd5Key2().c_str(),ikey);
	key[strlen(key)]=NULL;
	char *p = key;
	while(p && *p != '\0')
	{
		bzero(tmpKey[j],sizeof(tmpKey[j]));
		count = strlen(p);
		if(count >= 5)
			count = 5;
		strncpy(tmpKey[j],p,count);
		p = p + count;
		j++;
	}
	for( i = j ; i >= 0 ; --i )
	{
		count = strlen(tmpKey[i]);
		memcpy(tagKey+imem,tmpKey[i],count);
		imem += count;
	}
	this->onDesMd5(tagKey,(char*)szPout);
	return true;
}
bool  COpenssl::onDesMd5( const char *data,char *pout)
{
	MD5_CTX ctx;
	char tmp[3]={0};
	unsigned char buf[1024]={0};
	MD5_Init(&ctx);
	bzero(buf,sizeof(buf));
	MD5_Update(&ctx,data,strlen(data));
	MD5_Final(buf,&ctx);
	int i;
	for( i = 0; i < 16; ++i )
	{
		bzero(tmp,sizeof(tmp));
		sprintf(tmp,"%02X",buf[i]);
		strcat(pout,tmp);
	}
	return true;
}
bool COpenssl::onGetRandKey()
{
	bzero(this->_keyBuff,sizeof(this->_keyBuff));
	unsigned char keybuff[8];
	if (!_encIndex >= 0 && _encIndex < RAND_LENGTH)
	{
		DEBUGINFO("keyIndex error");
		return false;
	}

	bzero(keybuff,sizeof(keybuff));
	memcpy(keybuff,rand_key[_encIndex],8*sizeof(unsigned char));
	SimpleXor_Crype(keybuff, keybuff, 8);
	memcpy(_keyBuff,keybuff,8*sizeof(unsigned char));
	return true;
}
void COpenssl::SimpleXor_Crype(unsigned char *out, unsigned char *in, int len)
{
	if ((out == NULL )||(in == NULL)||(len <= 0))
	{
		return;
	}
	int iKey = 0;
	int i = 0;
	for ( i = 0; i != len; i++)
	{
		out[i] = (in[i] ^ (enkey[iKey] + len));
		if (iKey == sizeof(enkey) - 1)
		{
			iKey = 0;
		}
		else
		{
			iKey++;
		}
	}
}
bool	COpenssl::onEncryption(const char *szBuff,const int &nBuffLen,const unsigned char &keyIndex)/*加密*/
{
	if (!szBuff || nBuffLen <= 0)
	{
		DEBUGINFO("param is null");
		return false;
	}

	if (keyIndex < 0 || keyIndex >= RAND_LENGTH)
	{
		DEBUGINFO("keyIndex error");
		return false;
	}

	bzero(_srcBuff,sizeof(_srcBuff));
	_srcBuffLen = 0;
	bzero(_dstBuff,sizeof(_dstBuff));
	_dstBuffLen = 0;
	memcpy(_srcBuff,szBuff,nBuffLen);
	_srcBuffLen = nBuffLen;
	this->_encIndex = keyIndex;
	if (!this->onGetRandKey())
	{
		DEBUGINFO("get rand key error");
		return false;
	}
	//gCCommon->onDisplay(this->_keyBuff,8);
	if (!this->onEncryption())
	{
		DEBUGINFO("encryption faile");
		return false;
	}
	return true;
}
bool	COpenssl::onDecde(const char *szBuff,const int &nBuffLen,const unsigned char &keyIndex)
{
	if (!szBuff || nBuffLen <= 0)
	{
		DEBUGINFO("param is null");
		return false;
	}
	if (keyIndex < 0 || keyIndex >= RAND_LENGTH)
	{
		DEBUGINFO("keyIndex error");
		return false;
	}

	bzero(_srcBuff,sizeof(_srcBuff));
	_srcBuffLen = 0;
	bzero(_dstBuff,sizeof(_dstBuff));
	_dstBuffLen = 0;

	memcpy(_srcBuff,szBuff,nBuffLen);
	_srcBuffLen = nBuffLen;
	this->_encIndex = keyIndex;

	if (!this->onGetRandKey())
	{
		DEBUGINFO("get rand key error");
		return false;
	}
	//gCCommon->onDisplay(this->_keyBuff,8);

	if (!this->onDecode())
	{
		DEBUGINFO("onDecde faile");
		return false;
	}
	return true;
}
