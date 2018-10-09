#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <stdio.h> 
#include <stdarg.h>
#include <syslog.h>

//#define USE_STDOUT 1
//#define DISABLE_LOG 1

static void Logger(const char* format, ...){
#ifndef DISABLE_LOG
        #ifdef USE_STDOUT
                va_list argptr; va_start(argptr, format);
                vprintf(format, argptr);
                va_end(argptr);
        #else
                va_list argptr; va_start(argptr, format);
                vsyslog(LOG_ERR, format, argptr);
                va_end(argptr);
        #endif
#endif
}

#endif
