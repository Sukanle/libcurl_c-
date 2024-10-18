#include "_curl.h"

namespace web{
    CURLcode curl_global::_global=CURLE_FAILED_INIT;

    std::function<const struct curl_easyoption*(const struct curl_easyoption*)> easyOptNext;

    auto getoption(CURLoption id) noexcept
        -> const struct curl_easyoption * {
        return curl_easy_option_by_id(id);
    }

    auto getoption(const char *name) noexcept
        -> const struct curl_easyoption * {
        return curl_easy_option_by_name(name);
    }
    void init_easyOptNext() {
        easyOptNext=[](const struct curl_easyoption *opt) noexcept ->
            const struct curl_easyoption*{
            return curl_easy_option_next(opt);
        };
    }

    bool curl_global::global_init() NOEXCEPT {
        if(_global==CURLE_OK)
            return true;
        _global=curl_global_init(CURL_GLOBAL_DEFAULT);
#ifdef CURL_ERROR_ON
        if(_global!=CURLE_OK)
            THROW_CURL_ERROR(_global);
#else
        if(_global!=CURLE_OK)
            return false;
#endif
        return true;
        }
    bool curl_global::global_cleanup() NOEXCEPT {
        if(_global!=CURLE_OK)
            return true;
        if(curl_easy::_easy_extant!=0||curl_multi::_multi_extant!=0){
#ifdef CURL_ERROR_ON
            throw std::runtime_error(std::string("Error: There are ")+
                    std::to_string(curl_easy::_easy_extant+curl_multi::_multi_extant)+
                    " curl_easy or curl_multi are still extant.");
#else
        fprintf(stderr,"Error: There are %zu curl_easy or curl_multi are still extant.\n",curl_easy::_easy_extant+curl_multi::_multi_extant);
#endif
            return false;
        }
        curl_global_cleanup();
        _global=CURLE_FAILED_INIT;
        return true;
    }
}// namespace web
