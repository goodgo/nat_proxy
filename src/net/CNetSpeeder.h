/*
 * CNetSpeeder.h
 *
 *  Created on: Nov 6, 2018
 *      Author: root
 */

#ifndef SRC_NET_CNETSPEEDER_H_
#define SRC_NET_CNETSPEEDER_H_

#include "CInclude.h"
#include "CDefine.h"

class CNetSpeeder
{
public:
	CNetSpeeder(uint32_t channelId, );
	void onCounter(uint32_t bytes);
	void onReset();

public:
	uint64_t	_bytes;
	TIMEVAL		_tvStart;
	TIMEVAL		_tvEnd;
};


#endif /* SRC_NET_CNETSPEEDER_H_ */
