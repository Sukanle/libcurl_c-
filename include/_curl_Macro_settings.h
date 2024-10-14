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

#ifdef _WIN32
#define CURL_GLOBAL CURL_GLOBAL_WIN32
#else
#define CURL_GLOBAL CURL_GLOBAL_SSL
#endif

#endif
