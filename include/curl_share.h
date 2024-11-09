#pragma once
#ifndef CPP_CURL_SHARE_H
#define CPP_CURL_SHARE_H

#include "curl.h"

namespace web {
class curl_share {
private:
    CURLSH* _share = nullptr;
    CURLSHcode _error = CURLSHE_OK;
    std::vector<CURLSHcode> _error_vec;

public:
    curl_share(const curl_share&) = delete;
    curl_share& operator=(const curl_share&) = delete;

    curl_share() noexcept;
    curl_share(curl_share&&) noexcept;
    curl_share& operator=(curl_share&&) noexcept;
    explicit operator bool() const noexcept;
    explicit operator CURLSH*() const noexcept;

    ~curl_share() noexcept;

    /****************************************************************************************/

    template <typename T> bool setOption(CURLSHoption&& option, T&& value) NOEXCEPT
    {
        _error = curl_share_setopt(_share, option, std::forward<T>(value));
        _error_vec.push_back(_error);
#ifdef CURL_ERROR_ON
        CURL_ERROR(_error);
#endif
        return _error == CURLSHE_OK;
    }

    template <typename T, typename... Args>
    bool setOption(CURLSHoption&& option, T&& value, Args&&... args) NOEXCEPT
    {
        return setOption(std::forward<CURLSHoption>(option), std::forward<T>(value))
            && setOption(std::forward<Args>(args)...);
    }
    bool setOption(curlsh_option& options) NOEXCEPT;

    /****************************************************************************************/

    // @brief: 获取curl_share的句柄
    NODISCARD CURL* getHandle() const noexcept;
    // @brief: 获取curl_share的错误码
    NODISCARD CURLSHcode getError() const noexcept;
    // @brief: 获取curl_share的错误信息
    NODISCARD const char* getErrorText() const noexcept;
    // @brief: 获取curl_share的错误码集合
    NODISCARD const std::vector<CURLSHcode>& getErrorVec() const noexcept;
};
};// namespace web
#endif
