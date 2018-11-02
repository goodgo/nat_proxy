#include "CProtocol.h"
#include "CInclude.h"
#include "CDefine.h"
#include "CLog.h"
#include <endian.h>

bool CLoginReqPackage::onConv(CBuff& cbuff)
{
	char* guid = (char*)MALLOC(64);
	if(!cbuff.onRead(guid, ucGuidLen))
	{
		FREE(guid);
		DEBUGINFO("read guid len error: %u", ucGuidLen);
		return false;
	}
	szGuid = guid;
	FREE(guid);

	if(!cbuff.onRead(uiPrivateAddr))
	{
		DEBUGINFO("read private addr error");
		return false;
	}
	return true;
}

bool CAccelationReqPackage::onConv(CBuff& cbuff)
{
	if(!cbuff.onRead(uiId))
	{
		DEBUGINFO("parse id failed.");
		return false;
	}
	if(!cbuff.onRead(uiDstId))
	{
		DEBUGINFO("parse dest id failed.");
		return false;
	}

	char* gameid = (char*)MALLOC(128);
	if(!gameid)
	{
		return false;
	}
	if(!cbuff.onRead(gameid, ucGameIdLen))
	{
		DEBUGINFO("parse game id failed.");
		FREE(gameid);
		return false;
	}

	szGameId = gameid;
	FREE(gameid);
	return true;
}

bool CGetConsolesReqPackage::onConv(CBuff& cbuff)
{
	if(!cbuff.onRead(uiId))
	{
		return false;
	}
	return true;
}
