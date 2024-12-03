#include <android/log.h>

#ifndef __LOGGER_HPP_
#define __LOGGER_HPP_

#define __LOGTAG "xyz.anon.arcsider"

#define LOGA(TYPE, MSG, ...) __android_log_print(TYPE, __LOGTAG, MSG, ...)
#define LOG(TYPE, MSG) __android_log_print(TYPE, __LOGTAG, MSG)

#define LOGIA(MSG, ...) LOGA(ANDROID_LOG_INFO, MSG, ...)
#define LOGI(MSG) LOG(ANDROID_LOG_INFO, MSG)

#define LOGWA(MSG, ...) LOGA(ANDROID_LOG_WARN, MSG, ...)
#define LOGW(MSG) LOG(ANDROID_LOG_WARN, MSG)

#define LOGEA(MSG, ...) LOGA(ANDROID_LOG_ERROR, MSG, ...)
#define LOGE(MSG) LOG(ANDROID_LOG_ERROR, MSG)

#define LOGFATALA(MSG, ...) LOGA(ANDROID_LOG_FATAL, MSG, ...)
#define LOGFATAL(MSG) LOG(ANDROID_LOG_FATAL, MSG)

#define LOGDA(MSG, ...) LOGA(ANDROID_LOG_DEBUG, MSG, ...)

//#define LOGD(MSG) LOG(ANDROID_LOG_DEBUG, MSG)

#define LOGD(MSG)

/*namespace Logger
{
    void Log(int type, const char *fmt, ...)
    {
        va_list l;
        va_start(l, fmt);
        __android_log_print(type, __LOGTAG, fmt, l);
        va_end(l);
    }
    void D(const char *fmt, ...)
    {
        va_list l;
        va_start(l, fmt);
        Log(ANDROID_LOG_DEBUG, fmt, l);
        va_end(l);
    }
    void I(const char *fmt, ...)
    {
        va_list l;
        va_start(l, fmt);
        Log(ANDROID_LOG_INFO, fmt, l);
        va_end(l);
    }
    void E(const char *fmt, ...)
    {
        va_list l;
        va_start(l, fmt);
        Log(ANDROID_LOG_ERROR, fmt, l);
        va_end(l);
    }
    void W(const char *fmt, ...)
    {
        va_list l;
        va_start(l, fmt);
        Log(ANDROID_LOG_WARN, fmt, l);
        va_end(l);
    }
    void F(const char *fmt, ...)
    {
        va_list l;
        va_start(l, fmt);
        Log(ANDROID_LOG_FATAL, fmt, l);
        va_end(l);
    }
}*/

#endif