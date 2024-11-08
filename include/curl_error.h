#pragma once
#ifndef CPP_CURL_ERROR_H
#define CPP_CURL_ERROR_H 1

#include <exception>

#if defined CURL_ERROR_ENABLE || defined CURL_ERROR_DETAILED

#if __cplusplus < 201703L
#define NODISCARD __attribute__((warn_unused_result))
#else
#define NODISCARD [[nodiscard]]
#endif


#ifdef CURL_ERROR_DETAILED
#define THROW_CURL_ERROR(_error) \
    throw web::curl_error(_error, __FILE__, __LINE__, __FUNCTION__);
#else
#define THROW_CURL_ERROR(_error) \
        throw web::curl_error(_error)
#endif

#define CURL_ERROR(_error) \
    if(_error!=CURLM_OK) \
        THROW_CURL_ERROR(_error)

#define ERR_FILE_LINE_FN(_file,_line,_func) \
    ANSI_COLOR_RED "Error: "+_file+":"+std::to_string(_line)+"("+_func+"): " ANSI_COLOR_RESET


namespace web{
    class curl_error:public std::exception
    {
    private:
        std::string _error;

    public:
        curl_error()=delete;
#ifdef CURL_ERROR_DETAILED
        curl_error(CURLcode &errorcode, const std::string &file, int line, const std::string &func)noexcept :
            _error(ERR_FILE_LINE_FN(file,line,func)+curl_easy_strerror(errorcode)){}
        curl_error(CURLMcode &errorcode, const std::string &file, int line, const std::string &func)noexcept :
            _error(ERR_FILE_LINE_FN(file,line,func)+curl_multi_strerror(errorcode)){}
#else
        curl_error(CURLcode &errorcode)noexcept :_error(curl_easy_strerror(errorcode)){}
        curl_error(CURLMcode &errorcode)noexcept :_error(curl_multi_strerror(errorcode)){}
#endif
        curl_error(const curl_error&)=delete;
        curl_error(curl_error&& obj)noexcept : _error(std::move(obj._error)){}
        curl_error& operator=(const curl_error&)=delete;
        curl_error& operator=(curl_error&& obj)noexcept {
            if(this!=&obj){
                _error=obj._error;
            }
            return *this;
        }

        ~curl_error() noexcept override = default;

        NODISCARD const char* what() const noexcept override{
            return _error.c_str();
        }
    };
} // namespace web
#endif
namespace web{
    template<typename ErrorCode>
    const std::string &getstrerr(const ErrorCode &errorcode) noexcept;
}// namespace web
#endif // _CURL_ERROR_H
