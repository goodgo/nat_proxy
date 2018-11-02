#ifndef SRC_THREAD_CTHREADBASE_H_
#define SRC_THREAD_CTHREADBASE_H_
#include "../util/CDefine.h"
#include "../util/CInclude.h"
class CThreadBase {
public:
	static
	bool	onStartThread(void *param,void *(*pRun)(void *));
};

#endif /* SRC_THREAD_CTHREADBASE_H_ */
