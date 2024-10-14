#include "_curl.h"
#include "_curl_easy.h"

namespace web{
    std::atomic<size_t> curl_easy::_easy_extant=0;
    bool curl_easy::_auto_manage=false; // 默认不自动管理全局初始化和清理

/*=============================================================*/

    bool curl_easy::global_init() NOEXCEPT {
        _global_init=curl_global_init(CURL_GLOBAL);
#ifdef CURL_ERROR_ON
        if(_global_init!=CURLE_OK)
            THROW_CURL_ERROR(_global_init);
#else
        if(_global_init!=CURLE_OK)
            return false;
#endif
        return true;
        }
    bool curl_easy::global_cleanup() NOEXCEPT {
        if(_easy_extant!=0||curl_multi::_multi_extant!=0){
#ifdef CURL_ERROR_ON
            throw std::runtime_error(std::string("Error: There are ")+
                    std::to_string(_easy_extant+curl_multi::_multi_extant)+
                    " curl_easy or curl_multi are still extant.");
#else
            fprintf(stderr,"Error: There are %zu curl_easy or curl_multi are still extant.\n",_easy_extant+curl_multi::_multi_extant);
#endif
            return false;
        }
        curl_global_cleanup();
        _global_init=CURLE_FAILED_INIT;
        return true;
    }
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

/*=============================================================*/

    curl_easy::curl_easy() NOEXCEPT{
        lock.clear();
        {
            lock_guard guard(lock);
            if(_global_init!=CURLE_OK&&_auto_manage==true)
                global_init();
            else if(_global_init!=CURLE_OK&&_auto_manage==false)
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
        curl_easy_cleanup(_curl);
        --_easy_extant;
        if(_auto_manage==true)
            global_cleanup();
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
    CURLcode curl_easy::setOption(CURL *curl,const curl_option &option) NOEXCEPT {
        CURLcode _error=CURLE_OK;
        for(const auto &i:option)
            if((_error=curl_easy_setopt(curl,i.first,i.second))!=CURLE_OK)
#ifdef CURL_ERROR_ON
                THROW_CURL_ERROR(_error)
#else
                break;
#endif
        return _error;
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
