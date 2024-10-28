#pragma once
#ifndef _CURL_HTTP_H
#define _CURL_HTTP_H 1
#include "_curl.h"

namespace web{
    enum http_error
    {
        OK=0,               // 正常
        INIT_ERROR,         // 初始化错误
        URL_ERROR,          // URL错误
        HTTPS_ERROR,        // HTTPS错误
        SSLKEY_ERROR,       // SSLKEY错误
        SSLKEY_PUBLIC_ERROR // SSLKEY_PUBLIC错误
    };
    enum http_method
    {
        GET=0, //默认
        POST,  //POST
        PUT,   //PUT
        DELETE //DELETE
    };
    enum http_key_t
    {
        NO_USE=0, //不使用
        PRIVATE,  //私钥
        PUBLIC    //公钥
    };

    using ssl_verify=std::array<long,2>;
    class curl_easy_http
    {
    private:
        curl_easy _http;
        bool _is_https=false;
        bool _is_set_sslkey=false;
        int _use_key_T=http_key_t::NO_USE;
        int _method=http_method::GET;
        http_error _error=http_error::OK;
        std::string _private_key;
        static std::string _public_key;
    public:
        curl_easy_http()=delete;
        explicit curl_easy_http(const std::string &url,const ssl_verify& opt={0L,0L},bool is_https=false)NOEXCEPT;
        curl_easy_http(const curl_easy_http &other)NOEXCEPT;
        curl_easy_http(curl_easy_http &&other) noexcept;
        curl_easy_http &operator=(const curl_easy_http &other)NOEXCEPT;
        curl_easy_http &operator=(curl_easy_http &&other)noexcept;
        ~curl_easy_http() NOEXCEPT;

        bool setUrl(const std::string &url)NOEXCEPT;
        bool setHttps(bool is_https)NOEXCEPT;
        bool setSllkey(const std::string &key)NOEXCEPT;
        static bool setPubSllkey(const std::string &key)NOEXCEPT;
        bool perform()NOEXCEPT;

        NODISCARD const std::string &getUrl() const NOEXCEPT;
        NODISCARD bool getHttps() const NOEXCEPT;
        NODISCARD bool getSllkey() const NOEXCEPT;
        NODISCARD static const std::string &getPubSllkey() NOEXCEPT;
    };

    const std::map<http_error, std::string> http_error_map = {
        {http_error::OK, "OK"},
        {http_error::INIT_ERROR, "INIT_ERROR"},
        {http_error::URL_ERROR, "URL_ERROR"},
        {http_error::HTTPS_ERROR, "HTTPS_ERROR"},
        {http_error::SSLKEY_ERROR, "SSLKEY_ERROR"},
        {http_error::SSLKEY_PUBLIC_ERROR, "SSLKEY_PUBLIC_ERROR"}
    };

    template<>
    const std::string &getstrerr<http_error>(const http_error&err) noexcept;

} // namespace web
#endif
