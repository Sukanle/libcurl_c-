#include "_curl_multi.h"

namespace web
{
    std::atomic<size_t> curl_multi::_multi_extant=0;
    std::atomic_bool curl_multi::_auto_manage=false;

/*=============================================================*/

    bool curl_multi::global_init() NOEXCEPT {
#ifdef _WIN32
        _global_init=curl_global_init(CURL_GLOBAL_WIN32);
#else
        _global_init=curl_global_init(CURL_GLOBAL_SSL);
#endif
#ifdef CURL_ERROR_ON
        if(_global_init!=CURLE_OK)
            THROW_CURL_ERROR(_global_init);
#else
        if(_global_init!=CURLE_OK)
            return false;
#endif
        return true;
    }
    bool curl_multi::global_cleanup() NOEXCEPT {
        if(curl_easy::_easy_extant!=0||_multi_extant!=0){
#ifdef CURL_ERROR_ON
            throw std::runtime_error(std::string("Error: There are ")+
                    std::to_string(curl_easy::_easy_extant+_multi_extant)+
                    " curl_easy or curl_multi are still extant.");
#else
            fprintf(stderr,"Warning: %zu curl_easy or curl_multi are still extant.\n",_multi_extant.load());
            return false;
#endif
        }
        curl_global_cleanup();
        _global_init=CURLE_FAILED_INIT;
        return true;
    }
    void curl_multi::auto_manage(bool flag) NOEXCEPT {
        _auto_manage=flag;
    }
    void curl_multi::add_multi_extant(unsigned int num) noexcept {
        _multi_extant+=num;
    }
    void curl_multi::sub_multi_extant(unsigned int num) noexcept {
        _multi_extant-=num;
    }

/*=============================================================*/

    curl_multi::curl_multi() NOEXCEPT {
        lock.clear();
        _multi_handle = curl_multi_init();
        if (_multi_handle == nullptr){
            _error=CURLM_INTERNAL_ERROR;
            _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
            THROW_CURL_ERROR(_error);
#else
            fprintf(stderr, "curl_multi_init failed\n");
#endif
        }
    }
    curl_multi::curl_multi(curl_multi&& other) noexcept {
        if(this==&other)
            return;
        _multi_handle = std::exchange(other._multi_handle,nullptr);
        _easy_extant = std::exchange(other._easy_extant,0);
        _error_vec = std::exchange(other._error_vec,{});
        _error = std::exchange(other._error,CURLM_OK);
        _is_running = std::exchange(other._is_running,false);
    }
    curl_multi& curl_multi::operator=(curl_multi&& other) noexcept {
        if(this==&other)
            return *this;
        _multi_handle = other._multi_handle;
        other._multi_handle = nullptr;
        _easy_extant = other._easy_extant;
        other._easy_extant = 0;
        _error = other._error;
        other._error = CURLM_OK;
        _is_running = other._is_running;
        other._is_running = false;
        return *this;
    }
    curl_multi::operator bool() const noexcept {
        return _multi_handle==nullptr;
    }

/*=============================================================*/

    bool curl_multi::setOption(curlm_option &options) NOEXCEPT {
        if(_multi_handle==nullptr)
            return false;
        for(const auto &opt:options){
            if((_error=curl_multi_setopt(_multi_handle,opt.first,opt.second))!=CURLM_OK){
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
    bool curl_multi::addHandle(curl_easy &easy) NOEXCEPT {
        _error=curl_multi_add_handle(_multi_handle,easy._curl);
        if(_error!=CURLM_OK){
            _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
            THROW_CURL_ERROR(_error);
#else
            return false;
#endif
        }
        _easy_extant++;
        return true;
    }
    bool curl_multi::addHandle(CURL *easy) NOEXCEPT {
        _error=curl_multi_add_handle(_multi_handle,easy);
        if(_error!=CURLM_OK){
            _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
            THROW_CURL_ERROR(_error);
#else
            return false;
#endif
        }
        _easy_extant++;
        return true;
    }
    bool curl_multi::perform() NOEXCEPT {
        _error=curl_multi_perform(_multi_handle,&_easy_extant);
        _is_running=true;
        if(_error!=CURLM_OK){
            _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
        THROW_CURL_ERROR(_error);
#else
            return false;
#endif
        }
        return _error==CURLM_OK;
    }
    CURLMsg *curl_multi::info_read(int msgs_in_queue) NOEXCEPT {
        return curl_multi_info_read(_multi_handle,&msgs_in_queue);
    }
    bool curl_multi::socket_action(curl_socket_t s, int ev_bitmask, int *running_handles) NOEXCEPT {
        _error=curl_multi_socket_action(_multi_handle,s,ev_bitmask,running_handles);
        if(_error!=CURLM_OK){
            _error_vec.emplace_back(_error);
        }
    }
/*=============================================================*/

    CURL *curl_multi::getHandle()const noexcept {
        return _multi_handle;
    }
    CURLMcode curl_multi::getError()const noexcept {
        return _error;
    }
    const char *curl_multi::getErrorText()const noexcept {
        return curl_multi_strerror(_error);
    }
    int curl_multi::getEasyExtant()const noexcept {
        return _easy_extant;
    }
    size_t curl_multi::extant() noexcept {
        return _multi_extant;
    }
}//namespace web
