#pragma once
#ifndef CPP_CURL_H
#define CPP_CURL_H 1

#if __cplusplus < 201103L
#error "C++11 or later version is required"
#elif __cplusplus >= 201103L && __cplusplus < 201703L
#include "help_c++11.hpp"
#endif

// C++ ISO (C++11)
#include <array>
#include <atomic>
#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>

// Linux
#include <sys/select.h>

// libcurl
#include <curl/curl.h>

#include "curlOptVal.h"
#include "curl_macro_settings.h"
#include "curl_error.h"

namespace web {

using curl_option = std::vector<std::pair<CURLoption, OptionValue>>;
using curlm_option = std::vector<std::pair<CURLMoption, OptionValue>>;
using curlsh_option = std::vector<std::pair<CURLSHoption, OptionValue>>;
using curl_info_opt = std::vector<std::pair<CURLINFO, OptionValue>>;

NODISCARD auto getEasyOption(CURLoption id) noexcept -> const struct curl_easyoption*;
NODISCARD auto getEasyOption(const char* name) noexcept -> const struct curl_easyoption*;

using ::curl_free;

extern std::function<const struct curl_easyoption*(const struct curl_easyoption*)> getEasyOptNext;
CONSTRUCTOR void init_easyOptNext();

template <typename curl_t>
void curl_forced_cleanup(curl_t& curl)
{
    curl.cleanup();
    curl.~curl_t();
}

class auto_flag {
private:
    std::atomic_flag& _flag;
public:
    auto_flag(std::atomic_flag& flag) noexcept : _flag(flag)
    {
        while (_flag.test_and_set(std::memory_order_acquire))
            ;
    }
    ~auto_flag() noexcept
    {
        _flag.clear(std::memory_order_release);
    }
};
}// namespace web

#include "curl_easy.h"
#include "curl_multi.h"
#include "curl_share.h"
// 工厂函数头文件，支持C++11及以上版本
#include "curl_makefn.h"

namespace web {

class curl_global {
private:
    static CURLcode _global;
    static std::atomic_flag _lock;

    friend class curl_easy;
    friend class curl_multi;

public:
    curl_global() noexcept = default;
    ~curl_global() noexcept = default;
    curl_global(const curl_global&) = delete;
    curl_global& operator=(const curl_global&) = delete;
    curl_global(curl_global&&) = delete;
    curl_global& operator=(curl_global&&) = delete;
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

    NODISCARD static CURLcode error() noexcept;
};
}; // namespace web

#include "curl_http.h"
#endif // __CURL_H
