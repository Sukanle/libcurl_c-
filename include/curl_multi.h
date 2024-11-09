#pragma once
#ifndef CPP_CURL_MULTI_H
#define CPP_CURL_MULTI_H 1

#include "curl.h"

namespace web {
class curl_msg;
class curl_multi {
private:
    CURLM* _curlm = nullptr;
    bool _is_running = false;
    CURLMsg *_msg = nullptr;

    int _easy_extant = 0;
    std::vector<curl_easy*> _easy_handles;

    CURLMcode _error = CURLM_OK;
    std::vector<CURLMcode> _error_vec;

    int _max_fd = 0;
    std::array<fd_set, 3> _multi_fd = {};
    timeval _timeout = { 0, 0 };

    static std::atomic_bool _auto_manage;
    static std::atomic<size_t> _multi_extant;

    friend class curl_easy;
    friend class curl_global;
    friend class curl_msg;

public:
    mutable std::atomic_flag lock = ATOMIC_FLAG_INIT;

    /*===============================================================================================*/

    /*
     * @brief: 设置是否自动管理全局初始化和清理
     * @param flag: 是否自动管理，默认为true
     * */
    static void auto_manage(bool flag = true) NOEXCEPT;
    /*
     * @brief: 获取自增curl_multi对象数量
     * @param num: 自增数量，默认为1
     * @attention: 仅在启用自动管理时有意义
     * */
    static void add_multi_extant(unsigned int num = 1) noexcept;
    /*
     * @brief: 获取自减curl_multi对象数量
     * @param num: 自减数量，默认为1
     * @attention: 仅在启用自动管理时有意义
     * */
    static void sub_multi_extant(unsigned int num = 1) noexcept;
    /**
     * @brief: 提供设置curl_multi_setopt的选项和值
     * @param option: 选项组，里面包含了选项和值
     * @note: 通过vector<pair<CURMLoption,void*>>来设置,静态函数
     * */
    NODISCARD static CURLMcode setOption(CURLM* multi_handle, const curlm_option& option) NOEXCEPT;

    //@brief: 获取curl_multi现存数量
    NODISCARD static size_t extant() noexcept;

    /*===============================================================================================*/
    curl_multi(const curl_multi&) = delete;
    curl_multi& operator=(const curl_multi&) = delete;

    curl_multi() noexcept;
    explicit curl_multi(const timeval& timeout) noexcept;
    curl_multi(curl_multi&&) noexcept;
    curl_multi& operator=(curl_multi&&) noexcept;
    explicit operator bool() const noexcept;
    explicit operator CURLM*() const noexcept;

    ~curl_multi();

    /*===============================================================================================*/

    //@brief: 立即清理curl_multi对象
    //@warning: 请确保在调用此函数前已经释放了所有的curl_easy对象
    //@note: 建议使用析构函数来自动清理
    void cleanup() { _easy_extant = 0; }
    /**
     * @brief: 提供设置一对curl_multi_setopt的选项和值
     * @param option: 选项
     * @param value: 选项值
     * @note: 可直接通过CURLMoption枚举类型来设置
     * */
    template <typename T> bool setOption(CURLMoption&& option, T&& value) NOEXCEPT
    {
        _error
            = curl_multi_setopt(_curlm, std::forward<CURLMoption>(option), std::forward<T>(value));
        _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
        CURL_ERROR(_error);
#endif
        return _error == CURLM_OK;
    }

    /**
     * @brief: 提供设置多对curl_multi_setopt的选项和值
     * @param option: 选项
     * @param value: 选项值
     * @note: 可直接通过CURLMoption枚举类型来设置,可变参数模板，递归调用
     * */
    template <typename T, typename... Args>
    bool setOption(CURLMoption&& option, T&& value, Args&&... args) NOEXCEPT
    {
        return setOption(std::forward<CURLMoption>(option), std::forward<T>(value))
            && setOption(std::forward<Args>(args)...);
    }

    /**
     * @brief: 提供设置curl_multi_setopt的选项和值
     * @param option: 选项组，里面包含了选项和值
     * @note: 通过vector<pair<CURMLoption,void*>>来设置
     * */
    bool setOption(curlm_option& options) NOEXCEPT;

    /**
     * @brief: 添加一个curl_easy对象到multi_handle
     * @param easy: curl_easy对象
     * */
    bool addHandle(const curl_easy& easy) NOEXCEPT;
    bool addHandle(curl_easy* easy) NOEXCEPT { return addHandle(*easy); }

    bool rmHandle(const curl_easy& easy) NOEXCEPT;
    bool rmHandle(curl_easy* easy) NOEXCEPT { return rmHandle(*easy); }

    int add_easy_extant(int num=1) noexcept;
    int sub_easy_extant(int num=1) noexcept;

    //@brief: 设置超时时间
    void setTimeval(const struct timeval& tv) noexcept { _timeout = tv; }

    //@brief: 设置超时时间
    void setTimeval(const long& sec, const long& usec) noexcept
    {
        _timeout.tv_sec = sec;
        _timeout.tv_usec = usec;
    }

    void setMaxFd(int max_fd) noexcept { _max_fd = max_fd; }
    //@brief: 非阻塞执行内部添加的easy句柄
    bool perform() NOEXCEPT;

    //@brief: 在给定操作的情况下读取/写入可用数据
    bool socket_action(curl_socket_t skt, int ev_bitmask) NOEXCEPT;

    bool socket_all() NOEXCEPT;

    //@brief: 获取文件描述符集合，用于I/O多路复用(select/poll/epoll)
    bool fdset() NOEXCEPT;

    //@brief: 用于I/O多路复用(select)
    bool select() NOEXCEPT;

    //@brief: 用于I/O多路复用(poll)
    bool poll(struct curl_waitfd extra_fds[], unsigned int extra_nfds, int timeout_ms,
        int& numfds) NOEXCEPT;

    //@brief: 设置超时时间
    bool timeout(long timeout) NOEXCEPT;

    bool wait(struct curl_waitfd extra_fds[], unsigned int extra_nfds, int timeout_ms,
        int &numfds) NOEXCEPT;

    bool assign(curl_socket_t sockfd, void* sockp) NOEXCEPT;

    #if LIBCURL_VERSION_NUM >= 0x080800
        bool waitfds(struct curl_waitfd extra_fds[], uint size, uint& fd_count) NOEXCEPT;
    #endif
    bool wakeup() NOEXCEPT;
    /**
     * @brief: 检测multi_handle中的easy句柄是否全部完成
     * @param msgs_in_queue: 消息队列中的消息数量
     * */
    NODISCARD CURLMsg* info_read(int *msgs_in_queue) noexcept;

    /*===============================================================================================*/

    //@brief: 获取curl_multi的句柄
    NODISCARD CURLM* getHandle() const noexcept;
    //@brief: 获取curl_multi的错误码
    NODISCARD CURLMcode getError() const noexcept;
    //@brief: 获取curl_multi的错误信息
    NODISCARD const char* getErrorText() const noexcept;
    //@brief: 获取curl_multi的错误码集合
    NODISCARD const std::vector<CURLMcode>& getErrorVec() const NOEXCEPT;
    //@brief: 获取curl_multi的easy句柄数量
    NODISCARD int getEasyExtant() const noexcept;
    //@brief: 获取curl_multi是否正在运行
    NODISCARD bool isRunning() const noexcept;
    //@brief: 获取curl_multi的存储的所以easy句柄
    NODISCARD std::vector<curl_easy*> getEasyHandles() const NOEXCEPT;
    //@brief: 获取curl_multi的最大文件描述符
    NODISCARD int getMaxFd() const noexcept;
    //@brief: 获取curl_multi的文件描述符集合
    NODISCARD std::array<fd_set, 3> getMultiFd() const noexcept;
    //@brief: 获取curl_multi的超时时间
    NODISCARD struct timeval getTimeout() const noexcept;
};

class curl_msg {
private:
    CURLMsg* _msg = nullptr;
    curl_easy* _easy = nullptr;

    friend class curl_multi;

#define getResult() getMsg()->data.result
#define getWhatever() getMsg()->data.whatever

public:
    curl_msg() noexcept = default;
    curl_msg(CURLMsg* msg,curl_multi &multi) noexcept;
    curl_msg(const curl_msg&) = default;
    curl_msg& operator=(const curl_msg&) = default;
    curl_msg(curl_msg&&) noexcept = default;
    curl_msg& operator=(curl_msg&&) noexcept = default;
    explicit operator bool() const noexcept;
    ~curl_msg() = default;

    bool init(CURLMsg* msg,curl_multi &multi) noexcept;

    NODISCARD CURLMsg* getMsg() const noexcept;
    NODISCARD curl_easy* getEasy() const noexcept;
};
} // namespace web

#endif // _CURL_MULTI_H
