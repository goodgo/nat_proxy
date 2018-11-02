#include "CEpoller.h"


CEpoller::CEpoller(bool bEt)
{
	_iEpollfd   = -1;
	_pevs       = NULL;
	_et         = bEt;
	_max_connections = 1024;
}

CEpoller::~CEpoller()
{
	if(_pevs != NULL)
	{
		delete[] _pevs;
		_pevs = NULL;
	}
	if(_iEpollfd > 0)
	{
		close(_iEpollfd);
	}
}


void CEpoller::ctrl(int fd, uint32_t events, int op, void *ptr)
{
	struct epoll_event ev;
	ev.events = events;
	if(_et)
		ev.events |= EPOLLET ;

	if (NULL != ptr)
		ev.data.ptr = ptr;
	else
		ev.data.fd = fd;

	epoll_ctl(_iEpollfd , op, fd, &ev);
}


bool CEpoller::create(int max_connections)
{
	_max_connections = max_connections;

	_iEpollfd = epoll_create1(EPOLL_CLOEXEC);

	if(_pevs != NULL)
	{
		delete[] _pevs;
	}
	_pevs = new epoll_event[_max_connections + 1];
	return true;
}


bool CEpoller::add(int fd, void* event_data)
{
	 ctrl(fd,  EPOLLIN | EPOLLERR | EPOLLHUP, EPOLL_CTL_ADD, event_data);
	 return true;
}


void CEpoller::mod(int fd, uint32_t event, void* event_data)
{
     ctrl(fd,  event, EPOLL_CTL_MOD, event_data);
}


void CEpoller::del(int fd, uint32_t event)
{
     ctrl(fd, event, EPOLL_CTL_DEL);
}


int CEpoller::wait(int millsecond)
{
     return epoll_wait(_iEpollfd , _pevs , _max_connections + 1, millsecond);
}


