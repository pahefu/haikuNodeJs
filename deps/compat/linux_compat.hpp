#ifndef LINUX_COMPAT_HPP
#define LINUX_COMPAT_HPP

#include <stdio.h> 
#include <stdarg.h>
#include <syslog.h>

/* Include these dummy code files too */

#include "linux_ifaddr.hpp"
#include "linux_epoll.hpp"

/* basic code below  */

static int getpriority(int which, id_t who){
	Logger("[WARNING][UNIMPLEMENTED][get_priority] params=> which: %d, it_t: %d\n", which, who);
	return 0;
}

static int setpriority(int which, id_t who, int priority){
	Logger("[WARNING][UNIMPLEMENTED][set_priority] params=> which: %d, it_t: %d, priority: %d\n", which, who, priority);
	return 0;
}

#endif
