#pragma once
#ifndef _CURL_MULTI_H
#define _CURL_MULTI_H 1

#include "_curl.h"

namespace web
{
    class curl_multi
    {
    private:
        CURLM* _curlm=nullptr;
        int _easy_extant=0;
        CURLMcode _error=CURLM_OK;
        std::vector<CURLMcode> _error_vec;
        bool _is_running=false;
        std::vector<curl_easy*> _easy_handles;

        static std::atomic_bool _auto_manage;
        static std::atomic<size_t> _multi_extant;

        friend class curl_easy;
        friend class curl_global;

    public:
        mutable std::atomic_flag lock=ATOMIC_FLAG_INIT;

        /*===============================================================================================*/

        /*
         * @brief: 设置是否自动管理全局初始化和清理
         * @param flag: 是否自动管理，默认为true
         * */
        static void auto_manage(bool flag=true) NOEXCEPT;
        /*
         * @brief: 获取自增curl_multi对象数量
         * @param num: 自增数量，默认为1
         * @attention: 仅在启用自动管理时有意义
         * */
        static void add_multi_extant(unsigned int num=1) noexcept;
        /*
         * @brief: 获取自减curl_multi对象数量
         * @param num: 自减数量，默认为1
         * @attention: 仅在启用自动管理时有意义
         * */
        static void sub_multi_extant(unsigned int num=1) noexcept;
        /**
         * @brief: 提供设置curl_multi_setopt的选项和值
         * @param option: 选项组，里面包含了选项和值
         * @note: 通过vector<pair<CURMLoption,void*>>来设置,静态函数
         * */
        NODISCARD static CURLMcode setOption(CURLM* multi_handle,const curlm_option &option) NOEXCEPT;

        //@brief: 获取curl_multi现存数量
        NODISCARD static size_t extant() noexcept;

        /*===============================================================================================*/
        curl_multi(const curl_multi&) = delete;
        curl_multi& operator=(const curl_multi&) = delete;

        curl_multi()NOEXCEPT;
        curl_multi(curl_multi&&) noexcept;
        curl_multi& operator=(curl_multi&&) noexcept;
        explicit operator bool()const noexcept;

        ~curl_multi();

        /*===============================================================================================*/

        /**
         * @brief: 提供设置一对curl_multi_setopt的选项和值
         * @param option: 选项
         * @param value: 选项值
         * @note: 可直接通过CURLMoption枚举类型来设置
         * */
        template<typename T>
        bool setOption(CURLMoption &&option,T &&value) NOEXCEPT {
            _error=curl_multi_setopt(_curlm,
                    std::forward<CURLMoption>(option),
                    std::forward<T>(value));
            _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
            CURL_ERROR(_error);
#endif
            return _error==CURLM_OK;
        }

        /**
         * @brief: 提供设置多对curl_multi_setopt的选项和值
         * @param option: 选项
         * @param value: 选项值
         * @note: 可直接通过CURLMoption枚举类型来设置,可变参数模板，递归调用
         * */
        template<typename T,typename... Args>
        bool setOption(CURLMoption &&option,T &&value,Args&&... args) NOEXCEPT {
            return setOption(std::forward<CURLMoption>(option),
                    std::forward<T>(value))&&
                setOption(std::forward<Args>(args)...);
        }

        /**
         * @brief: 提供设置curl_multi_setopt的选项和值
         * @param option: 选项组，里面包含了选项和值
         * @note: 通过vector<pair<CURMLoption,void*>>来设置
         * */
        bool setOption(curlm_option &options) NOEXCEPT;

        /**
         * @brief: 添加一个curl_easy对象到multi_handle
         * @param easy: curl_easy对象
         * */
        bool addHandle(curl_easy &easy) NOEXCEPT;

        //@brief: 非阻塞执行内部添加的easy句柄
        bool perform() NOEXCEPT;

        bool fdset(fd_set *read_fd_set, fd_set *write_fd_set, fd_set *exc_fd_set, int *max_fd) NOEXCEPT;

        /**
         * @brief: 检测multi_handle中的easy句柄是否全部完成
         * @param msgs_in_queue: 消息队列中的消息数量
         * */
        NODISCARD CURLMsg *info_read(int msgs_in_queue) NOEXCEPT;
        bool socket_action(curl_socket_t s, int ev_bitmask, int *running_handles) NOEXCEPT;

        /*===============================================================================================*/

        //@brief: 获取curl_multi的句柄
        NODISCARD CURL *getHandle() const noexcept;
        //@brief: 获取curl_multi的错误码
        NODISCARD CURLMcode getError() const noexcept;
        //@brief: 获取curl_multi的错误信息
        NODISCARD const char* getErrorText() const noexcept;
        //@brief: 获取curl_multi的easy句柄数量
        NODISCARD int getEasyExtant() const noexcept;
        //@brief: 获取curl_multi是否正在运行
        NODISCARD bool isRunning() const noexcept;
        //@brief: 获取curl_multi的存储的所以easy句柄
        NODISCARD std::vector<curl_easy*> getEasyHandles() const NOEXCEPT;
    };
}//namespace web

#endif // _CURL_MULTI_H
