#include "_curl_http.h"

namespace web{
    curl_easy_http::curl_easy_http(const std::string &url,const ssl_verify&opt,bool is_https)NOEXCEPT
        :_is_https(is_https){
        _http.setOption(CURLOPT_URL,url.c_str());
        _http.setOption(CURLOPT_SSL_VERIFYPEER,opt[0]);
        _http.setOption(CURLOPT_SSL_VERIFYHOST,opt[1]);
    }
    curl_easy_http::curl_easy_http(const curl_easy_http &other) NOEXCEPT{
        if(this==&other)
            return;
        _http=other._http;
        _is_https=other._is_https;
        _is_set_sslkey=other._is_set_sslkey;
        _use_key_T=other._use_key_T;
        _method=other._method;
        _error=other._error;
        _private_key=other._private_key;
    }
    curl_easy_http::curl_easy_http(curl_easy_http &&other) noexcept{
        if(this==&other)
            return;
        _http=std::move(other._http);
        _is_https=std::exchange(other._is_https,false);
        _is_set_sslkey=std::exchange(other._is_set_sslkey,false);
        _use_key_T=std::exchange(other._use_key_T,http_key_t::NO_USE);
        _method=std::exchange(other._method,http_method::GET);
        _error=std::exchange(other._error,http_error::OK);
        _private_key=std::move(_private_key);
    }
    curl_easy_http& curl_easy_http::operator=(const curl_easy_http &other)NOEXCEPT{
        
    }
    template<>
    const std::string &getstrerr<http_error>(const http_error&err) noexcept{
        static const std::string empty("");
        auto iter = http_error_map.find(err);
        if (iter != http_error_map.end())
            return iter->second;
        return empty;
    }
} //namespace web
