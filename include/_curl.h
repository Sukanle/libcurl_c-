#ifndef __CURL_H
#define __CURL_H 1

#include <memory>
#include <string>
#include <vector>
#include <utility>
#include <functional>

#include <curl/curl.h>

#include "_curlOptVal.h"
#ifdef CURL_ERROR_ENABLE
#include "_curl_error.h"
#define NOEXCEPT noexcept(false)
#else
#define NOEXCEPT noexcept(true)
#endif

namespace web {

        using curl_option=std::vector<std::pair<CURLoption,OptionValue>>;

        // curl_error类，用于处理、记录curl错误
        class curl_error;
        // curl_easy类,提供简单的curl_easy接口
        class curl_easy
        {
        private:
            CURL *_curl=nullptr;

        public:
            curl_easy()noexcept;
            curl_easy(const curl_easy& obj)noexcept;
            curl_easy(curl_easy &&obj)noexcept;

            ~curl_easy()noexcept;

            curl_easy& operator=(const curl_easy&obj)noexcept;
            curl_easy& operator=(curl_easy&&obj)noexcept;

            /*
             * @brief: 提供设置一对curl_easy_setopt的选项和值
             * @param: option: 选项
             * @param: value: 选项值
             * @note: 可直接通过CURLoption枚举类型来设置
             * */
            template<typename T>
            bool setOption(CURLoption &&option,T &&value) NOEXCEPT {
#ifdef CURL_ERROR_ENABLE
                CURLcode _error=curl_easy_setopt(_curl,
                        std::forward<CURLoption>(option),
                        std::forward<T>(value));
                if(_error!=CURLE_OK)
                    CURL_ERROR(_error);
                return true;
#else
                return curl_easy_setopt(_curl,
                        std::forward<CURLoption>(option),
                        std::forward<T>(value))==CURLE_OK;
#endif
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
            static bool setOption(CURL *curl,const curl_option &option) noexcept {
                CURLcode text;
                for(const auto &i:option)
                    if(curl_easy_setopt(curl,i.first,i.second)!=CURLE_OK)
                        return false;
                return true;
            }

            /*
             * @brief: 执行curl_easy_perform
             * @note: 执行成功返回true，否则返回false
             * */
            bool perform() NOEXCEPT;

            /*
             * @brief: 获取curl_easy的句柄
             * */
            CURL *getHandle() noexcept;
        };

}// namespace web

// 工厂函数头文件，支持C++11及以上版本
#include "_curl_makefn.h"

#endif // __CURL_H
