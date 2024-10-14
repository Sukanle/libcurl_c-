#pragma once
#ifndef __CURL_H
#define __CURL_H 1

#if __cplusplus < 201103L
#error "C++11 or later version is required"
#endif

#include <vector>
#include <utility>
#include <atomic>
#include <string>

#include <curl/curl.h>

#include "_curl_Macro_settings.h"
#include "_curlOptVal.h"

#if defined CURL_ERROR_ENABLE
#include "_curl_error.h"
#elif defined CURL_ERROR_DETAILED
#include "_curl_error.h"
#endif

namespace web {

    using curl_option=std::vector<std::pair<CURLoption,OptionValue>>;
    using curlm_option=std::vector<std::pair<CURLMoption,OptionValue>>;

    extern CURLcode _global_init;

    class lock_guard
    {
    private:
        std::atomic_flag& _lock;
    public:
        explicit lock_guard(std::atomic_flag& lock)noexcept:_lock(lock) {
            while(_lock.test_and_set(std::memory_order_acquire));
        }
        ~lock_guard()noexcept {
            _lock.clear(std::memory_order_release);
        }
    };

}// namespace web
#include "_curl_easy.h"
// 工厂函数头文件，支持C++11及以上版本
#include "_curl_makefn.h"

#include "_curl_multi.h"

#endif // __CURL_H
