#pragma once
#ifndef _CURL_MULTI_H
#define _CURL_MULTI_H 1

#include "_curl.h"

namespace web
{
    class curl_multi
    {
    private:
        CURLM* _multi_handle=nullptr;
        int _easy_extant=0;
        CURLMcode _error=CURLM_OK;
        std::vector<CURLMcode> _error_vec;
        bool _is_running=false;

        static bool _auto_manage;
        static size_t _multi_extant;

        friend class curl_easy;

    public:
        mutable std::atomic_flag lock=ATOMIC_FLAG_INIT;

        /*
         * @brief: 全局初始化curl
         * @return: 初始化成功返回true，否则返回false
         * @attention: 请确保在使用curl_multi之前调用
         * */
        static bool global_init() NOEXCEPT;
        /*
         * @brief: 全局清理curl
         * @attention: 请确保所有curl相关对象已经销毁
         * */
        static bool global_cleanup() NOEXCEPT;
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

        curl_multi(const curl_multi&) = delete;
        curl_multi& operator=(const curl_multi&) = delete;

        curl_multi()NOEXCEPT;
        curl_multi(curl_multi&&) noexcept;
        curl_multi& operator=(curl_multi&&) noexcept;
        explicit operator bool()const noexcept;

        ~curl_multi();

        /**
         * @brief: 提供设置一对curl_multi_setopt的选项和值
         * @param option: 选项
         * @param value: 选项值
         * @note: 可直接通过CURLMoption枚举类型来设置
         * */
        template<typename T>
        bool setOption(CURLMoption &&option,T &&value) NOEXCEPT {
            _error=curl_multi_setopt(_multi_handle,
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
         * @brief: 提供设置curl_multi_setopt的选项和值
         * @param option: 选项组，里面包含了选项和值
         * @note: 通过vector<pair<CURMLoption,void*>>来设置,静态函数
         * */
        NODISCARD static CURLMcode setOption(CURLM* multi_handle,const curlm_option &option) NOEXCEPT;

        /**
         * @brief: 添加一个curl_easy对象到multi_handle
         * @param easy: curl_easy对象
         * */
        bool addHandle(curl_easy &easy) NOEXCEPT;
        /**
         * @brief: 添加一个CURL*对象到multi_handle
         * @param easy: CURL*对象
         * */
        bool addHandle(CURL *easy_handle) NOEXCEPT;
        //@brief: 执行curl_multi_perform
        bool perform() NOEXCEPT;

        static bool addExtant() noexcept;

        //@brief: 获取curl_multi的句柄
        NODISCARD CURL *getHandle() const noexcept;
        //@brief: 获取curl_multi的错误码
        NODISCARD CURLMcode getError() const noexcept;
        //@brief: 获取curl_multi的错误信息
        NODISCARD const char* getErrorText() const noexcept;
        //@brief: 获取curl_multi的easy句柄数量
        NODISCARD int getEasyExtant() const noexcept;
        //@brief: 获取curl_multi现存数量
        NODISCARD static size_t extant() noexcept;
    };
}//namespace web

#endif // _CURL_MULTI_H
