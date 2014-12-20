#ifdef __ANDROID__
#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Info", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "Warning", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "Error", __VA_ARGS__))
#else

#ifndef MO_LOGGING_H
#define	MO_LOGGING_H

#include <cstdio>
template <typename... Args>
static void LOGI(const char * format, Args... args){
    std::printf(format, args...);
}

template <typename... Args>
static void LOGW(const char * format, Args... args){
    std::printf(format, args...);
}

template <typename... Args>
static void LOGE(const char * format, Args... args){
    std::printf(format, args...);
}
#endif
#endif