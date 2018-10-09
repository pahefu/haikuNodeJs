#ifndef LINUX_EPOLL_HPP
#define LINUX_EPOLL_HPP


#include <stdio.h> 
#include <stdarg.h>
#include <syslog.h>
#include <sys/time.h>
#include <sys/select.h>

#include <stdlib.h>

#include "logger.hpp"

/***********
DEFINES
************/

#define FIONCLEX        0x5450
#define FIOCLEX         0x5451
#define FIOASYNC        0x5452

#define EPOLL_CLOEXEC 02000000
#define EPOLL_CTL_ADD 1
#define EPOLL_CTL_DEL 2
#define EPOLL_CTL_MOD 3

enum EPOLL_EVENTS
  {
    EPOLLIN = 0x001,
#define EPOLLIN EPOLLIN
    EPOLLPRI = 0x002,
#define EPOLLPRI EPOLLPRI
    EPOLLOUT = 0x004,
#define EPOLLOUT EPOLLOUT
    EPOLLRDNORM = 0x040,
#define EPOLLRDNORM EPOLLRDNORM
    EPOLLRDBAND = 0x080,
#define EPOLLRDBAND EPOLLRDBAND
    EPOLLWRNORM = 0x100,
#define EPOLLWRNORM EPOLLWRNORM
    EPOLLWRBAND = 0x200,
#define EPOLLWRBAND EPOLLWRBAND
    EPOLLMSG = 0x400,
#define EPOLLMSG EPOLLMSG
    EPOLLERR = 0x008,
#define EPOLLERR EPOLLERR
    EPOLLHUP = 0x010,
#define EPOLLHUP EPOLLHUP
    EPOLLRDHUP = 0x2000,
#define EPOLLRDHUP EPOLLRDHUP
    EPOLLEXCLUSIVE = 1u << 28,
#define EPOLLEXCLUSIVE EPOLLEXCLUSIVE
    EPOLLWAKEUP = 1u << 29,
#define EPOLLWAKEUP EPOLLWAKEUP
    EPOLLONESHOT = 1u << 30,
#define EPOLLONESHOT EPOLLONESHOT
    EPOLLET = 1u << 31
#define EPOLLET EPOLLET
  };

typedef union epoll_data {
    void    *ptr;
    int      fd;
    uint32_t u32;
    uint64_t u64;
} epoll_data_t;

typedef struct epoll_event {
    uint32_t     events;    /* Epoll events */
    epoll_data_t data;      /* User data variable */
} epoll_event;


/******************************

class EpollCreationObject{
public:
	
};

******************************/


// Ultra Quick and dirty hack. DONT DO THIS AT HOME

#define EPM_MAX_EPOLLS 100
#define FD_PER_EFD 20

typedef struct{
	int efd;
	int total_fds;
	int fd[FD_PER_EFD];	
	int active[FD_PER_EFD];	
	int params[FD_PER_EFD];
} EpollEntry;

typedef struct EpollManager{
	int initialized;
	int total_epolls;
	EpollEntry epolls[EPM_MAX_EPOLLS];
	int epoll_params[EPM_MAX_EPOLLS];
}EpollManager;

static EpollManager epm;
static int epm_init = 0;


static void epoll_manager_initialize(){
	int i = 0;
	int j = 0;
	if(epm_init){
		return;
	}
	epm.total_epolls = 0;	
	for(i = 0;i< EPM_MAX_EPOLLS;i++){
		epm.epolls[i].efd = -1;
		epm.epolls[i].total_fds = 0;
		for(j = 0;j<FD_PER_EFD;j++){		
			epm.epolls[i].fd[j] = -1;
			epm.epolls[i].active[j] = 0;
			epm.epolls[i].params[j] = 0;
		}
	}
	epm_init = 1;
}

static EpollEntry* find_epoll_by_id(int epfd){
	EpollEntry* ptr = NULL;
	int i = 0;
	for(i = 0;i<epm.total_epolls;i++){
		if(epm.epolls[i].efd == epfd){
			ptr = &epm.epolls[i];
			break;
		}
	}
	return ptr;
}

static void remove_fd_from_epoll(int epfd, int fd){
	EpollEntry* ptr = find_epoll_by_id(epfd);
	int i = -1;
	if(ptr==NULL){
		Logger("[CRITICAL] Asked to remove a FD(%d) from a non existing epfd(%d) \n", fd, epfd);
		if(1) {
			exit(1);
		}
		return;
	}
	
	for(i = 0;i<ptr->total_fds;i++){
		if(ptr->fd[i] == fd){ // Found the file descriptor
			ptr->active[i] = 0;
			break;
		}
	}

	Logger("[CRITICAL] FD(%d) not found in epfd(%d) fds\n", fd, epfd);

}


static int epoll_create(int size){
	int i = 0;
	epoll_manager_initialize();
	i = epm.total_epolls;
	epm.epolls[i].efd = (int)tmpfile();
	epm.epoll_params[i] = size;

	epm.total_epolls++;

	
	Logger("[WARNING][UNIMPLEMENTED][epoll_create] params=> size: %d\n", size);
	return epm.epolls[i].efd;
}


static int epoll_create1(int flags){
	int i = 0;
	epoll_manager_initialize();
	i = epm.total_epolls;
	epm.epolls[i].efd = (int)tmpfile();
	epm.epoll_params[i] = flags;
	epm.total_epolls++;


	Logger("[epoll_create1] Created epoll object: %d. Total EFDS: %d\n", epm.epolls[i].efd, epm.total_epolls);
//	Logger("[WARNING][UNIMPLEMENTED][epoll_create1] params=> flags: %d\n", flags);

	return epm.epolls[i].efd;
}


static int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event){
	char buffer[4];
	int written = 0;

	int i = 0;
	int reusing = 0;

	EpollEntry* ptr = find_epoll_by_id(epfd);
	if(ptr==NULL){
		Logger("[CRITICAL][MALFUNCTION] Failed to find an epfd with id: %d\n", epfd);
		exit(1);
	}

	Logger("[epoll_ctl] epfd: %d, op: %d,_fd %d ", epfd, op, fd);
	//Logger("[epoll_ctl] params=> epfd: %d, op %d, fd %d, event %d\n", epfd, op, fd, event);
	switch(op){
		case EPOLL_CTL_ADD: 
			Logger("[ADD] fd %d\n", fd);

			for(i = 0;i<ptr->total_fds;i++){  // Filter for existing inactive FDS
				if(ptr->active[i]==0){ 
					reusing = 1;
					break;
				}
			}

			i = (reusing) ? i : (ptr->total_fds);
			ptr->active[i] = 1;
			ptr->fd[i] = fd;
			ptr->params[i] = event->events;

			if(!reusing){
				ptr->total_fds++;
			}

		break;

		case EPOLL_CTL_DEL: 
			Logger("[DEL] fd %d\n", fd);
			remove_fd_from_epoll(epfd, fd);
		break;

		case EPOLL_CTL_MOD: 
			Logger("[MOD] fd %d\n", fd);
			
			for(i = 0;i<ptr->total_fds;i++){
				if(ptr->fd[i]==fd){
					ptr->params[i]=event->events;
				}
			}
						
		break;
		default:
			Logger("[CRITICAL][NOTIMPLEMENTED] Unknown operation for ctl\n");
			exit(1);
		break;
	}
	
	return 0;
}


static int epoll_wait(int epfd, struct epoll_event *events,           int maxevents, int timeout){
	Logger("[CRITICAL][UNIMPLEMENTED][epoll_wait] params=> epfd %d, events, %d maxevents %d, timeout %d\n", epfd, events, maxevents,timeout);

	if(1){
		exit(1);
	}

	return 0;
}

static int epoll_pwait(int epfd, struct epoll_event *events,    int maxevents, int timeout,         const sigset_t *sigmask){

	Logger("[epoll_pwait] EPFD(%d), Timeout: %d", epfd,timeout);
	int timeoutStep = 0.5*100;
	int timeoutCurrent = 0;
	int timeoutMax = timeout;
	
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = timeoutStep;

	fd_set readset;
        fd_set writeset;


	EpollEntry* ptr = find_epoll_by_id(epfd);
        if(ptr==NULL){
                Logger("[CRITICAL][MALFUNCTION] Failed to find an epfd with id: %d\n", epfd);
                exit(1);
        }

	int infiniteLoop = 1;
	int i = 0;
	int totalItems = 0;

	while(infiniteLoop){

		totalItems = 0;

		if(timeout!=-1){ 
//			Logger(">>>>>>>>> FINISHED because timeout is not -1\n");
			if(timeoutCurrent>=timeoutMax){
				infiniteLoop = 0;
				break;
			}
		}

		for(i = 0;i<ptr->total_fds;i++){

			int fd_to_query = ptr->fd[i];

			if(ptr->active[i]==0){ // It is a non active one
				continue;
			}

//			Logger("Querying select for FD: %d\n", fd_to_query);
			FD_ZERO(&readset);
			FD_ZERO(&writeset);
			FD_SET(fd_to_query, &readset);
			FD_SET(fd_to_query, &writeset);

			void* READ_PTR = (ptr->params[i] & EPOLLIN) ? (&readset) : NULL;
			void* WRITE_PTR = (ptr->params[i] & EPOLLOUT) ? (&write) : NULL;

			int result = select(fd_to_query + 1, READ_PTR, WRITE_PTR, NULL, &tv);
			if(result>0){
				if(FD_ISSET(fd_to_query, &readset)){
//					Logger(">>> Data IN available on %d file descriptor\n", fd_to_query);
					events[totalItems].events = EPOLLIN;
					events[totalItems].data.fd = fd_to_query;
					totalItems++;
				}
				if(FD_ISSET(fd_to_query, &writeset)){
//					Logger(">>> Data OUT available on %d file descriptor\n", fd_to_query);
					events[totalItems].events = EPOLLOUT;
					events[totalItems].data.fd = fd_to_query;
					totalItems++;
				}
			}
			if(result<0){
//				Logger(">>>>>>>>> FINISHED because error on select\n");
				infiniteLoop = 0;
			}
		}
		if(totalItems>0){
//			Logger(">>>>>>>>> FINISHED because some item has data\n");
			infiniteLoop = 0;
		}
		timeoutCurrent+=timeoutStep;
	}
//	Logger(">>>>>>>>>>> FINISHED ONE LOOP\n");
	return totalItems;
}


static long syscall(long number, ...){
	Logger("[WARNING][UNIMPLEMENTED][syscall] params=> number: %d\n", number);
	if(1){
		exit(1);
	}
        return 0;
}


#endif
