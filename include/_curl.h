#pragma once
#ifndef __CURL_H
#define __CURL_H 1

#if __cplusplus < 201103L
#error "C++11 or later version is required"
#elif __cplusplus >= 201103L && __cplusplus < 201703L
#include "help_c++11.hpp"
#endif

#include <array>
#include <atomic>
#include <utility>
#include <vector>
#include <string>

#include <curl/curl.h>

#include "_curl_Macro_settings.h"
#include "_curlOptVal.h"
#include <functional>
#include <sys/select.h>

#if defined CURL_ERROR_ENABLE || defined CURL_ERROR_DETAILED
#include "_curl_error.h"
#endif

namespace web {

    using curl_option=std::vector<std::pair<CURLoption,OptionValue>>;
    using curlm_option=std::vector<std::pair<CURLMoption,OptionValue>>;
    using curl_info_opt=std::vector<std::pair<CURLINFO,OptionValue>>;

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

    NODISCARD auto getEasyOption(CURLoption id) noexcept
        -> const struct curl_easyoption *;
    NODISCARD auto getEasyOption(const char *name) noexcept
        -> const struct curl_easyoption *;

    using ::curl_free;
    extern std::function<const struct curl_easyoption*
        (const struct curl_easyoption*)> easyOptNext;
    CONSTRUCTOR void init_easyOptNext();
}// namespace web

#include "_curl_easy.h"
#include "_curl_multi.h"
// 工厂函数头文件，支持C++11及以上版本
#include "_curl_makefn.h"

namespace web{

    class curl_global
    {
    private:
        static CURLcode _global;
        friend class curl_easy;
        friend class curl_multi;
    public:
        curl_global()noexcept=default;
        ~curl_global()noexcept=default;
        curl_global(const curl_global&)=delete;
        curl_global& operator=(const curl_global&)=delete;
        curl_global(curl_global&&)=delete;
        curl_global& operator=(curl_global&&)=delete;
        /*
         * @brief: 全局初始化curl
         * @return: 初始化成功返回true，否则返回false
         * @attention: 请确保在使用curl_easy之前调用
         * */
        static bool global_init() NOEXCEPT;
        /*
         * @brief: 全局清理curl
         * @attention: 请确保所有curl相关对象已经销毁
         * */
        static bool global_cleanup() NOEXCEPT;

        NODISCARD static CURLcode error()noexcept;
    };
};// namespace web
#endif // __CURL_H
