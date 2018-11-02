#ifndef SRC_DATA_CPROTOCOL_H_
#define SRC_DATA_CPROTOCOL_H_

#include "CInclude.h"
#include "CBuff.h"

class CLoginReqPackage
{
public:
	bool onConv(CBuff& cbuff);

public:
	uint8_t ucGuidLen;
	CSTD_STR szGuid;
	uint32_t uiPrivateAddr;
};

class CAccelationReqPackage
{
public:
	bool onConv(CBuff& cbuff);

public:
	uint32_t uiId;
	uint32_t uiDstId;
	uint8_t ucGameIdLen;
	CSTD_STR szGameId;
};

class CGetConsolesReqPackage
{
public:
	bool onConv(CBuff& cbuff);

public:
	uint32_t uiId;
};

#endif
