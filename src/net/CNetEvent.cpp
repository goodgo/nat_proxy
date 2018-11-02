/*
 * CNetEvent.cpp
 *
 *  Created on: Oct 29, 2018
 *      Author: root
 */

#include "CNetEvent.h"

CNetEvent::CNetEvent(CSocketData& sock, void* ptr, CALLBACK callback)
: _events(0)
, _ptr(ptr)
, _callback(callback)
{
	_csSocket = sock;
}

CNetEvent::~CNetEvent()
{

}
