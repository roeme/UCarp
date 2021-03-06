#include <config.h>
#include <sys/time.h>
#include "log.h"
#include "ucarp.h"

#ifdef WITH_DMALLOC
# include <dmalloc.h>
#endif

void logfile(const int crit, const char *format, ...)
{
    const char *urgency;    
    va_list va;
    char line[MAX_SYSLOG_LINE];

    if (crit == LOG_DEBUG && ! debug)
        return;
    
    va_start(va, format);
    vsnprintf(line, sizeof line, format, va);
    switch (crit) {
    case LOG_INFO:
        urgency = "[INFO] ";
        break;
    case LOG_WARNING:
        urgency = "[WARNING] ";
        break;
    case LOG_ERR:
        urgency = "[ERROR] ";
        break;
    case LOG_NOTICE:
        urgency = "[NOTICE] ";
        break;
    case LOG_DEBUG:
        urgency = "[DEBUG] ";
        break;
    default:
        urgency = "";
    }
    if (no_syslog == 0) {
#ifdef SAVE_DESCRIPTORS
        openlog("ucarp", LOG_PID, syslog_facility);
#endif
        syslog(crit, "%s%s", urgency, line);
#ifdef SAVE_DESCRIPTORS
        closelog();
#endif
    }    
    if (daemonize == 0) {
        char timestr[200];
        struct timeval tv;
        struct tm *tmp;

        if (gettimeofday(&tv, NULL)) {
            perror("gettimeofday");
            return;
        }
        if (!(tmp = localtime(&tv.tv_sec))) {
            perror("localtime");
            return;
        }

        if (strftime(timestr, sizeof(timestr), "%Y-%m-%dT%H:%M:%S", tmp) == 0) {
            fprintf(stderr, "strftime returned 0");
            return;
        }

        switch (crit) {
        case LOG_WARNING:
        case LOG_ERR:
            fprintf(stderr, "%s.%06ld: %s%s\n", timestr, tv.tv_usec, urgency, line);
            break;
        default:
            printf("%s.%06ld: %s%s\n", timestr, tv.tv_usec, urgency, line);
        }
    }    
    va_end(va);
}
