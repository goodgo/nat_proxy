#ifndef SRC_UTILITY_CINCLUDE_H_
#define SRC_UTILITY_CINCLUDE_H_
#include <stdio.h>
#include <string.h>
#include <linux/if_tun.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <net/route.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/select.h>
#include <stdarg.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <openssl/md5.h>
#include <openssl/des.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/md5.h>
#include <openssl/rand.h>
#include <bits/uio.h>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <stdlib.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/param.h>
#include <sys/syscall.h>
#include <execinfo.h>

#define GETPID syscall(__NR_gettid)

typedef std::string CSTD_STR;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct timeval	   TIMEVAL;
typedef unsigned short 	   USHORT;

#include "CSingleton.h"
#include "CLog.h"
#include "CCommon.h"
#include "CConfig.h"
#include "CClientData.h"
#endif /* SRC_UTILITY_CINCLUDE_H_ */
