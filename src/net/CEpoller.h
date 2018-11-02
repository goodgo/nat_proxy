#ifndef SRC_EPOLLER_H_
#define SRC_EPOLLER_H_
#include <assert.h>
#include <iostream>
#include <sys/epoll.h>
#include "CClientData.h"


class CEpoller {
public:
	/**
	      * @brief 构造函数.
	      *
	     * @param bEt 默认是ET模式，当状态发生变化的时候才获得通知
	      */
	     CEpoller(bool bEt = true);

	     /**
	     * @brief 析够函数.
	      */
	     ~CEpoller();
	     /**
	      * @brief 生成 epoll句柄.
	      *
	     * @param max_connections epoll服务需要支持的最大连接数
	      */
	     bool create(int max_connections);
	     /**
	      * @brief 添加监听句柄.
	      *
	     * @param fd    句柄
	     * @param data  辅助的数据, 可以后续在epoll_event中获取到
	     * @param event 需要监听的事件EPOLLIN|EPOLLOUT
	     *
	      */
	     bool add(int fd, void* event_data = NULL);
	     /**
	      * @brief 修改句柄事件.
	      *
	     * @param fd    句柄
	     * @param data  辅助的数据, 可以后续在epoll_event中获取到
	     * @param event 需要监听的事件EPOLLIN|EPOLLOUT
	      */
	     void mod(int fd , uint32_t event, void* event_data);
	     /**
	      * @brief 删除句柄事件.
	      *
	     * @param fd    句柄
	     * @param data  辅助的数据, 可以后续在epoll_event中获取到
	     * @param event 需要监听的事件EPOLLIN|EPOLLOUT
	      */
	     void del(int fd, uint32_t event);
	     /**
	      * @brief 等待时间.
	      *
	      * @param millsecond 毫秒
	     * @return int       有事件触发的句柄数
	      */
	     int wait(int millsecond);
	     /**
	     * @brief 获取被触发的事件.
	      *
	      * @return struct epoll_event&被触发的事件
	      */
	     struct epoll_event& get(int i) { assert( _pevs != 0); return _pevs [i]; }


	protected:


	     /**
	     * @brief 控制 epoll，将EPOLL设为边缘触发EPOLLET模式
	     * @param fd    句柄，在create函数时被赋值
	     * @param data  辅助的数据, 可以后续在epoll_event中获取到
	     * @param event 需要监听的事件
	     * @param op    EPOLL_CTL_ADD： 注册新的 fd到epfd 中；
	     *              EPOLL_CTL_MOD：修改已经注册的 fd的监听事件；
	     *              EPOLL_CTL_DEL：从 epfd中删除一个fd；
	       *
	      */
	     void ctrl(int fd, uint32_t events, int op, void *ptr = NULL);

	public:


	    /**
	     *      epoll
	     */
	    int _iEpollfd;
	     /**
	     * 最大连接数
	      */
	     int   _max_connections;
	     /**
	     * 事件集
	      */
	     struct epoll_event *_pevs;
	    /**
	     * 是否是ET模式
	     */
	    bool _et;
};

#endif /* LINUXNET_CEpoller_H_ */
