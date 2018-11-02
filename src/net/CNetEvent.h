#ifndef SRC_NET_CNETEVENT_H_
#define SRC_NET_CNETEVENT_H_

#include <sys/epoll.h>
#include "CInclude.h"

class CNetEvent
{
public:
	typedef bool (*CALLBACK)(void* obj, struct epoll_event& ev);
	CNetEvent(CSocketData& sock, void* ptr, CALLBACK callback);
	~CNetEvent();

public:
	CSocketData _csSocket;
	uint32_t 	_events;
	void* 		_ptr;
	CALLBACK 	_callback;
};


#endif /* SRC_NET_CNETEVENT_H_ */
