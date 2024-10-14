#pragma once
#ifndef _CURL_EASY_H
#define _CURL_EASY_H 1

#include "_curl.h"

namespace web{
    // curl_easy类,提供简单的curl_easy接口
    class curl_easy
    {
    private:
        CURL *_curl=nullptr;
        CURLcode _error=CURLE_OK;
        std::vector<CURLcode> _error_vec;

        static bool _auto_manage;
        static std::atomic<size_t>_easy_extant;

        friend class curl_multi;

    public:
        mutable std::atomic_flag lock=ATOMIC_FLAG_INIT;

        /*
         * @brief: 全局初始化curl
         * @return: 初始化成功返回true，否则返回false
         * @attention: 请确保在使用curl_easy之前调用
         * */
        static bool global_init() NOEXCEPT;
        /*
         * @brief: 全局清理curl
         * @attention: 请确保所有curl相关对象已经销毁
         * */
        static bool global_cleanup() NOEXCEPT;
        /*
         * @brief: 设置是否自动管理全局初始化和清理
         * @param: flag: false表示不自动管理，true表示自动管理
         * */
        static void auto_manage(bool flag) noexcept;
        /*
         * @brief: 增加现存数量
         * @param: num: 增加的数量
         * @attention: 仅在启用自动管理时有意义
         * */
        static void add_easy_extant(unsigned int num) noexcept;
        /*
         * @brief: 减少现存数量
         * @param: num: 减少的数量
         * @attention: 仅在启用自动管理时有意义
         * */
        static void sub_easy_extant(unsigned int num) noexcept;
        // @brief: 获取现存数量
        static size_t getExtant() noexcept;

        curl_easy()NOEXCEPT;
        curl_easy(curl_easy &obj)noexcept;
        curl_easy(curl_easy &&obj)noexcept;

        ~curl_easy()noexcept;

        curl_easy& operator=(const curl_easy&obj)noexcept;
        curl_easy& operator=(curl_easy&&obj)noexcept;


        // 重载bool类型转换,判断是否初始化成功
        explicit operator bool()const noexcept;
        /*
         * @brief: 提供设置一对curl_easy_setopt的选项和值
         * @param: option: 选项
         * @param: value: 选项值
         * @note: 可直接通过CURLoption枚举类型来设置
         * */
        template<typename T>
        bool setOption(CURLoption &&option,T &&value) NOEXCEPT {
            _error=curl_easy_setopt(_curl,
                    std::forward<CURLoption>(option),
                    std::forward<T>(value));
            if(_error!=CURLE_OK){
                _error_vec.emplace_back(_error);
#ifdef CURL_ERROR_ON
                THROW_CURL_ERROR(_error);
#endif
            }
            return _error==CURLE_OK;
        }
        /*
         * @brief: 提供设置一组curl_easy_setopt的选项和值
         * @param: option: 选项
         * @param: value: 选项值
         * @note: 可直接通过CURLoption枚举类型来设置,可变参数模板，递归调用
         * */
        template<typename T,typename ...Args>
        bool setOption(CURLoption &&option,T &&value,Args&& ...args) NOEXCEPT {
            return setOption(std::forward<CURLoption>(option),
                    std::forward<T>(value)
                    ) && setOption(std::forward<Args>(args)...);
        }

        /*
         * @brief: 提供设置curl_easy_setopt的选项和值
         * @param: option: 选项组，里面包含了选项和值
         * @note: 通过vector<pair<CURLoption,void*>>来设置
         * */
        bool setOption(curl_option &option) NOEXCEPT;

        /*
         * @brief: 提供设置curl_easy_setopt的选项和值
         * @param: option: 选项组，里面包含了选项和值
         * @note: 通过vector<pair<CURLoption,void*>>来设置,静态函数
         * */
        NODISCARD static CURLcode setOption(CURL *curl,const curl_option &option) NOEXCEPT;

        // @brief: 执行curl_easy_perform
        bool perform() NOEXCEPT;

        // @brief: 获取curl_easy的句柄
        NODISCARD CURL *getHandle() const noexcept;
        // @brief: 获取curl_easy的错误码
        NODISCARD CURLcode getError() const noexcept;
        // @brief: 获取curl_easy的错误信息
        NODISCARD const char *getErrorText() const noexcept;
    };
}// namespace web

#endif
