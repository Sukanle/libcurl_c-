#include "_curl.h"
#include "_curl_multi.h"

namespace web
{
    std::atomic<size_t> curl_multi::_multi_extant={0};
    std::atomic_bool curl_multi::_auto_manage={false};

/*=============================================================*/

    void curl_multi::auto_manage(bool flag) NOEXCEPT {
        _auto_manage=flag;
    }
    void curl_multi::add_multi_extant(unsigned int num) noexcept {
        _multi_extant+=num;
    }
    void curl_multi::sub_multi_extant(unsigned int num) noexcept {
        _multi_extant-=num;
    }
    size_t curl_multi::extant() noexcept {
        return _multi_extant;
    }

/*=============================================================*/

    curl_multi::curl_multi() NOEXCEPT {
        lock.clear();
        if(_auto_manage==true)
            curl_global::global_init();

        FD_ZERO(&_multi_fd[fd_read]);
        FD_ZERO(&_multi_fd[fd_write]);
        FD_ZERO(&_multi_fd[fd_except]);

        _curlm = curl_multi_init();
        if (_curlm == nullptr){
            _error=CURLM_INTERNAL_ERROR;
            _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
            THROW_CURL_ERROR(_error);
#else
            fprintf(stderr, "curl_multi_init failed\n");
#endif
        }
        _multi_extant++;
    }
    curl_multi::curl_multi(const struct timeval &timeval)NOEXCEPT:curl_multi(){
        _timeout=timeval;
    }
    curl_multi::curl_multi(curl_multi&& other) noexcept {
        if(this==&other)
            return;
        _curlm = std::exchange(other._curlm,nullptr);
        _easy_extant = std::exchange(other._easy_extant,0);
        _error_vec = std::exchange(other._error_vec,{});
        _error = std::exchange(other._error,CURLM_OK);
        _is_running = std::exchange(other._is_running,false);
        _multi_extant++;
    }
    curl_multi& curl_multi::operator=(curl_multi&& other) noexcept {
        if(this==&other)
            return *this;
        _curlm = other._curlm;
        other._curlm = nullptr;
        _easy_extant = other._easy_extant;
        other._easy_extant = 0;
        _error = other._error;
        other._error = CURLM_OK;
        _is_running = other._is_running;
        other._is_running = false;
        return *this;
    }
    curl_multi::~curl_multi() {
        if(_curlm==nullptr)
            return;
        if(_easy_extant<=0){
            curl_multi_cleanup(_curlm);
            _multi_extant--;
        }
        if(_auto_manage==true&&_multi_extant==0)
            curl_global::global_cleanup();
    }
    curl_multi::operator bool() const noexcept {
        return _curlm==nullptr;
    }

/*=============================================================*/

    bool curl_multi::setOption(curlm_option &options) NOEXCEPT {
        if(_curlm==nullptr)
            return false;
        for(const auto &opt:options){
            if((_error=curl_multi_setopt(_curlm,opt.first,opt.second))!=CURLM_OK){
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
        if(_curlm==nullptr)
            return false;
        _easy_handles.emplace_back(&easy);
        _error=curl_multi_add_handle(_curlm,easy._curl);
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
    bool curl_multi::rmHandle(curl_easy &easy) NOEXCEPT {
        if(_curlm==nullptr)
            return false;
        _error=curl_multi_remove_handle(_curlm,easy._curl);
        if(_error!=CURLM_OK){
            _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
            THROW_CURL_ERROR(_error);
#else
            return false;
#endif
        }
        _easy_extant--;
        return true;
    }
    bool curl_multi::perform() NOEXCEPT {
        _error=curl_multi_perform(_curlm,&_easy_extant);
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
        return curl_multi_info_read(_curlm,&msgs_in_queue);
    }
    bool curl_multi::socket_action(curl_socket_t skt, int ev_bitmask) NOEXCEPT {
        _error=curl_multi_socket_action(_curlm,skt,ev_bitmask,&_easy_extant);
        if(_error!=CURLM_OK){
            _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
        THROW_CURL_ERROR(_error);
#endif
        }
        return _error==CURLM_OK;
    }
    bool curl_multi::fdset() NOEXCEPT {
        FD_ZERO(&_multi_fd[fd_read]);
        FD_ZERO(&_multi_fd[fd_write]);
        FD_ZERO(&_multi_fd[fd_except]);
        _error=curl_multi_fdset(_curlm,&_multi_fd[fd_read],&_multi_fd[fd_write],&_multi_fd[fd_except],&_max_fd);
        if(_error!=CURLM_OK){
            _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
        THROW_CURL_ERROR(_error);
#endif
        }
        return _error==CURLM_OK;
    }
    bool curl_multi::select() NOEXCEPT {
        int _select=::select(_max_fd+1,&_multi_fd[fd_read],&_multi_fd[fd_write],&_multi_fd[fd_except],&_timeout);
#ifdef CURL_ERROR_ON
        if(_select==-1)
            throw std::runtime_error("select failed");
#endif
        return _select>0;
    }
    bool curl_multi::poll(struct curl_waitfd extra_fds[],uint extra_nfds,int timeout_ms,int &numfds) NOEXCEPT{
        _error=curl_multi_poll(_curlm,extra_fds,extra_nfds,timeout_ms,&numfds);
        if(_error!=CURLM_OK){
            _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
            THROW_CURL_ERROR(_error);
#endif
        }
        return _error==CURLM_OK;
    }
    bool curl_multi::timeout(long timeout) NOEXCEPT {
        _error=curl_multi_timeout(_curlm,&timeout);
        if(_error!=CURLM_OK){
            _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
        THROW_CURL_ERROR(_error);
#endif
        }
        return _error==CURLM_OK;
    }
    bool curl_multi::wait(struct curl_waitfd extra_fds[],uint extra_nfds,int timeout_ms,int &numfds) NOEXCEPT{
        _error=curl_multi_wait(_curlm,extra_fds,extra_nfds,timeout_ms,&numfds);
        if(_error!=CURLM_OK){
            _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
        THROW_CURL_ERROR(_error);
#endif
        }
        return _error==CURLM_OK;
    }
#if LIBCURL_VERSION_NUM >= 0x080800
    bool curl_multi::waitfds(struct curl_waitfd extra_fds[],uint extra_nfds,uint &numfds) NOEXCEPT{
        _error=curl_multi_waitfds(_curlm,extra_fds,extra_nfds,&numfds);
        if(_error!=CURLM_OK){
            _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
        THROW_CURL_ERROR(_error);
#endif
        }
        return _error==CURLM_OK;
    }
#endif
    bool curl_multi::wakeup() NOEXCEPT {
        _error=curl_multi_wakeup(_curlm);
        if(_error!=CURLM_OK){
            _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
        THROW_CURL_ERROR(_error);
#endif
        }
        return _error==CURLM_OK;
    }
    bool curl_multi::assign(curl_socket_t sockfd, void *sockp) NOEXCEPT {
        _error=curl_multi_assign(_curlm,sockfd,sockp);
        if(_error!=CURLM_OK){
            _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
        THROW_CURL_ERROR(_error);
#endif
        }
        return _error==CURLM_OK;
    }

/*=============================================================*/

    CURL *curl_multi::getHandle()const noexcept {
        return _curlm;
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
    const std::vector<CURLMcode> &curl_multi::getErrorVec()const NOEXCEPT {
        return _error_vec;
    }
    bool curl_multi::isRunning()const noexcept {
        return _is_running;
    }
    std::vector<curl_easy*> curl_multi::getEasyHandles()const NOEXCEPT {
        return _easy_handles;
    }
    int curl_multi::getMaxFd()const noexcept {
        return _max_fd;
    }
    std::array<fd_set,3> curl_multi::getMultiFd()const noexcept {
        return _multi_fd;
    }
    struct timeval curl_multi::getTimeout()const noexcept {
        return _timeout;
    }
}//namespace web
