#ifndef _CURL_ERROR_H
#define _CURL_ERROR_H 1
#include <curl/curl.h>
#include <exception>

#define CURL_ERROR(_error) \
        throw idc::web::curl_error(_error);

namespace idc{
    namespace web{
        class curl_error:public std::exception
        {
        private:
            const char *_error;
        public:
            curl_error(CURLcode errorcode):_error(curl_easy_strerror(errorcode)){}
            curl_error(const curl_error &obj):_error(obj._error){}
            curl_error(curl_error &&obj):_error(obj._error){obj._error=nullptr;}
            curl_error& operator=(const curl_error&obj){
                if(this!=&obj){
                    _error=obj._error;
                }
                return *this;
            }
            curl_error& operator=(curl_error &&obj){
                if(this!=&obj){
                    _error=obj._error;
                    obj._error=nullptr;
                }
                return *this;
            }
            ~curl_error() = default;

            const char* what() const noexcept{
                return _error;
            }
        };
    } // namespace web
} // namespace idc
#endif // _CURL_ERROR_H
