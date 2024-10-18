#pragma once
#ifndef _CURL_ERROR_SETTINGS_H
#define _CURL_ERROR_SETTINGS_H 1

#if defined CURL_ERROR_ENABLE
#define NOEXCEPT noexcept(false)
#define CURL_ERROR_ON
#elif defined CURL_ERROR_DETAILED
#define NOEXCEPT noexcept(false)
#define CURL_ERROR_ON
#else
#define NOEXCEPT noexcept(true)
#endif

#if __cplusplus >= 201703L
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD __attribute__((warn_unused_result))
#endif

#ifdef START_BEFORE_MAIN
#define CONSTRUCTOR __attribute__((constructor))
#else
#define CONSTRUCTOR
#endif

#define CURL_MAX_INPUT_LENGTH 8,388,608 /* 8MB */

#define ANSI_COLOR_RED "\033[31m"
#define ANSI_COLOR_GREEN "\033[32m"
#define ANSI_COLOR_YELLOW "\033[33m"
#define ANSI_COLOR_BLUE "\033[34m"
#define ANSI_COLOR_MAGENTA "\033[35m"
#define ANSI_COLOR_CYAN "\033[36m"
#define ANSI_COLOR_WHITE "\033[37m"
#define ANSI_COLOR_DEFAULT "\033[39m"
#define ANSI_COLOR_RESET "\033[0m"

// #define PROMPT_HEADER(_state,_color) \
//     _color _state ": " __FILE__ ": %lu(" __FUNCTION__ "): " ANSI_COLOR_RESET

#define LOG_PROMPT(message,_state,_color) \
    fprintf(stderr, _color "%s: %s: %lu(%s): " ANSI_COLOR_RESET "%s", \
        _state, __FILE__, (unsigned long)__LINE__, __FUNCTION__, message)

#define ERROR_LOG(_message) LOG_PROMPT(_message,"Error",ANSI_COLOR_RED)
#define WARNING_LOG(_message) LOG_PROMPT(_message,"Warning",ANSI_COLOR_YELLOW)
#define INFO_LOG(_message) LOG_PROMPT(_message,"Info",ANSI_COLOR_CYAN)
#define DEBUG_LOG(_message) LOG_PROMPT(_message,"Debug",ANSI_COLOR_GREEN)
#endif
