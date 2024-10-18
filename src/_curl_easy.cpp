#include "_curl.h"
#include "_curl_easy.h"

namespace web{
    std::atomic<size_t> curl_easy::_easy_extant={0};
    std::atomic_bool curl_easy::_auto_manage={false}; // 默认不自动管理全局初始化和清理

/*=============================================================*/

    void curl_easy::auto_manage(bool flag) noexcept {
        _auto_manage=flag;
    }

    void curl_easy::add_easy_extant(unsigned int num) noexcept {
        _easy_extant+=num;
    }
    void curl_easy::sub_easy_extant(unsigned int num) noexcept {
        _easy_extant-=num;
    }

    size_t curl_easy::getExtant() noexcept {
        return _easy_extant;
    }

    CURLcode curl_easy::setOption(CURL *curl,const curl_option &option) NOEXCEPT {
        CURLcode _error=CURLE_OK;
        for(const auto &i:option)
            if((_error=curl_easy_setopt(curl,i.first,i.second))!=CURLE_OK)
#ifdef CURL_ERROR_ON
                THROW_CURL_ERROR(_error);
#else
                break;
#endif
        return _error;
    }
    CURLcode curl_easy::getinfo(CURL *curl,const curl_info_opt &info) NOEXCEPT {
        CURLcode _error=CURLE_OK;
        for(const auto &i:info)
            if((_error=curl_easy_getinfo(curl,i.first,i.second))!=CURLE_OK)
#ifdef CURL_ERROR_ON
                THROW_CURL_ERROR(_error);
#else
                break;
#endif

        return _error;
    }

/*=============================================================*/

    curl_easy::curl_easy() NOEXCEPT{
        lock.clear();
        {
            lock_guard guard(lock);
            if(curl_global::_global!=CURLE_OK&&_auto_manage==true)
                curl_global::global_init();
            else if(curl_global::_global!=CURLE_OK&&_auto_manage==false)
#ifdef CURL_ERROR_ON
                throw std::logic_error("Error: global is not inited.");
#else
            fprintf(stderr,"Warning: global is not inited.\n");
#endif
        }
        _curl=curl_easy_init();
        if(_curl==nullptr){
            _error=CURLE_FAILED_INIT;
            _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
            THROW_CURL_ERROR(_error);
#else
            fprintf(stderr,"curl_easy_init() failed:%s\n",curl_easy_strerror(_error));
            return;
#endif
        }
            ++_easy_extant;
    }
    curl_easy::curl_easy(curl_easy& obj) noexcept{
        if(this==&obj)
            return;
            _curl=curl_easy_duphandle(obj._curl);
            ++_easy_extant;
    }
    curl_easy::curl_easy(curl_easy &&obj) noexcept{
        if(this==&obj)
            return;
        _curl=std::exchange(obj._curl,nullptr);
        _error=std::exchange(obj._error,CURLE_OK);
        _error_vec=std::exchange(obj._error_vec,{});
        ++_easy_extant;
    }

    curl_easy::~curl_easy() noexcept {
        if(_curl==nullptr)
            return;
        curl_easy_cleanup(_curl);
        --_easy_extant;
        if(_auto_manage==true&&_easy_extant==0)
            curl_global::global_cleanup();
    }

    curl_easy& curl_easy::operator=(const curl_easy&obj) noexcept {
        if(this==&obj)
            return *this;
        _curl=curl_easy_duphandle(obj._curl);
        _error=obj._error;
        _error_vec=obj._error_vec;
        ++_easy_extant;
        return *this;
    }
    curl_easy& curl_easy::operator=(curl_easy&&obj) noexcept {
        if(this==&obj)
            return *this;
        _curl=std::exchange(obj._curl,nullptr);
        _error=std::exchange(obj._error,CURLE_OK);
        _error_vec=std::exchange(obj._error_vec,{});
        return *this;
    }

    curl_easy::operator bool() const noexcept {
        return _curl==nullptr;
    }

/*=============================================================*/

    bool curl_easy::setOption(curl_option &options) NOEXCEPT {
        if(_curl==nullptr)
            return false;
        for(const auto &opt:options){
            if((_error=curl_easy_setopt(_curl,opt.first,opt.second))!=CURLE_OK)
            {
                _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
                THROW_CURL_ERROR(_error);
#else
                return false;
#endif
            }
        }
        return true;
    }
    bool curl_easy::getinfo(curl_info_opt &info) NOEXCEPT {
        if(_curl==nullptr)
            return false;
        for(const auto &i:info)
            if((_error=curl_easy_getinfo(_curl,i.first,i.second))!=CURLE_OK)
            {
                _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
                THROW_CURL_ERROR(_error);
#else
                return false;
#endif
            }
        return true;
    }

    bool curl_easy::perform() NOEXCEPT {
        if(_curl==nullptr)
            return false;
        _error=curl_easy_perform(_curl);
        if(_error!=CURLE_OK){
            _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
            THROW_CURL_ERROR(_error);
#endif
        }
        return _error==CURLE_OK;
    }

    bool curl_easy::restart() noexcept {
        if(_curl==nullptr)
            return false;
        curl_easy_reset(_curl);
        return true;
    }
    bool curl_easy::pause(int bitmask) NOEXCEPT {
        if(_curl==nullptr)
            return false;
        _error=curl_easy_pause(_curl,bitmask);
        if(_error!=CURLE_OK){
            _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
            THROW_CURL_ERROR(_error);
#endif
        }
        return _error==CURLE_OK;
    }

    bool curl_easy::upkeep() NOEXCEPT {
        if(_curl==nullptr)
            return false;
        _error=curl_easy_upkeep(_curl);
        if(_error!=CURLE_OK){
            _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
            THROW_CURL_ERROR(_error);
#endif
        }
        return _error==CURLE_OK;
    }

    char *curl_easy::escape(const char *string, int length) NOEXCEPT {
        if(_curl==nullptr)
            return nullptr;
        if(length>CURL_MAX_INPUT_LENGTH)
#ifdef CURL_ERROR_ON
            throw std::invalid_argument("Error: The length of string is too long.");
#else
            return nullptr;
#endif
        return curl_easy_escape(_curl,string,length);
    }

    char *curl_easy::unescape(const char *string, int inlength, int *outlength) NOEXCEPT {
        if(_curl==nullptr)
            return nullptr;
        if(*outlength>INT_MAX)
#ifdef CURL_ERROR_ON
            throw std::invalid_argument("Error: The length of string is too long.");
#else
        return nullptr;
#endif
        return curl_easy_unescape(_curl,string,inlength,outlength);
    }

    void curl_easy::reset() noexcept {
        if(_curl==nullptr)
            return;
        curl_easy_reset(_curl);
    }
/*=============================================================*/

    CURL *curl_easy::getHandle() const noexcept {
        return _curl;
    }
    CURLcode curl_easy::getError() const noexcept {
        return _error;
    }
    const char *curl_easy::getErrorText() const noexcept {
        return curl_easy_strerror(_error);
    }

}
