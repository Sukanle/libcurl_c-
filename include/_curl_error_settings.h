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

#endif
