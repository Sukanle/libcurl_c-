#include "_curl.h"

namespace web{
    curl_easy::curl_easy() noexcept{
#ifdef _WIN32
        curl_global_init(CURL_GLOBAL_WIN32);
#else
        curl_global_init(CURL_GLOBAL_SSL);
#endif
        _curl=curl_easy_init();
    }
    curl_easy::curl_easy(const curl_easy& obj) noexcept{
        if(this!=&obj)
        {
            _curl=curl_easy_duphandle(obj._curl);
        }
    }
    curl_easy::curl_easy(curl_easy &&obj) noexcept{
        if(this!=&obj)
        {
            _curl=std::move(obj._curl);
            obj._curl=nullptr;
        }
    }

    curl_easy::~curl_easy() noexcept {
        curl_easy_cleanup(_curl);
        curl_global_cleanup();
    }

    curl_easy& curl_easy::operator=(const curl_easy&obj) noexcept {
        if(this!=&obj)
        {
            _curl=curl_easy_duphandle(obj._curl);
        }
        return *this;
    }
    curl_easy& curl_easy::operator=(curl_easy&&obj) noexcept {
        if(this!=&obj)
        {
            _curl=std::move(obj.getHandle());
            obj._curl=nullptr;
        }
        return *this;
    }

    bool curl_easy::setOption(curl_option &options) NOEXCEPT {
        if(_curl==nullptr)
            return false;
#ifdef CURL_ERROR_ENABLE
        CURLcode _error;
        for(const auto &opt:options)
        {
            if((_error=curl_easy_setopt(_curl,opt.first,opt.second))==CURLE_OK)
                continue;
            CURL_ERROR(_error);
        }
        return true;
#else
        for(const auto &option:options)
        {
            if(curl_easy_setopt(_curl,option.first,option.second)==CURLE_OK)
                continue;
            return false;
        }
        return true;
#endif
    }

    bool curl_easy::perform() NOEXCEPT {
        if(_curl==nullptr)
            return false;
#ifdef CURL_ERROR_ENABLE
        CURLcode _error=curl_easy_perform(_curl);
        if(_error==CURLE_OK)
            return true;
        CURL_ERROR(_error);
#else
        return curl_easy_perform(_curl)==CURLE_OK;
#endif
    }

    CURL *curl_easy::getHandle() noexcept {
        return _curl;
    }

} // namespace web
