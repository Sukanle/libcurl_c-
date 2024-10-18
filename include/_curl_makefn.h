#pragma once
#ifndef _CURL_MAKEFN_H
#define _CURL_MAKEFN_H 1
#include <memory>
#include <functional>

#include "_curl_Macro_settings.h"

#include "_curl.h"

#ifdef _CURL_ERROR_ON
#include "_curl_error.h"
#endif

#if __cplusplus < 201703L
#include "_makefn_c++11.h"
#endif

namespace web{
#if __cplusplus < 201703L
    /*
     * @brief: 构造一个curl对象，支持easy,share,multi
     * @param: new_instance: 是否创建一个新的curl对象，默认为false
     * @attention: 如果new_instance为true，会创建一个新的curl对象，否则返回一个栈上的对象
     * */
    template <bool new_instance=false,typename curl_t>
    NODISCARD auto make_curl() ->
    typename make_fn::return_type<new_instance,curl_t>::type{
        return make_fn::make_fn<new_instance,curl_t>::make(nullptr);
    }

    /*
     * @brief: 构造一个curl_easy对象，并设置选项，支持easy,share,multi
     * @param: option: 选项组，里面包含了选项和值
     * @param: new_instance: 是否创建一个新的curl_easy对象,默认为false
     * @attention: 如果new_instance为true，会创建一个新的curl_easy对象，否则返回一个栈上的对象
     * */
    template<bool new_instance,typename curl_t,typename curl_opt_t>
    NODISCARD auto make_curl(curl_opt_t &options) NOEXCEPT ->
    typename make_fn::return_type<new_instance,curl_t>::type{
        //@attention: 必须使用std::function，如果使用auto自动推导，会导致make_fn内部判断对象是否为空时出错
        std::function<void(curl_t&)> func = [&options](curl_t& obj) {
            obj.setOption(options);
        };
        return make_fn::make_fn<new_instance,curl_t>::make(func);
    }

    /*
     * @brief: 构造一个curl_easy对象，并设置选项（可变参数模板）
     * @param: option: 选项
     * @param: value: 选项值
     * @attention: 如果new_instance为true，会创建一个新的curl_easy对象，否则返回一个栈上的对象
     * */
    template<bool new_instance=false,
        typename curl_t,typename CURLopt_t,
        typename val_t,typename ...Args>
    NODISCARD auto make_curl(CURLopt_t &&option,val_t &&value,Args&& ...args) NOEXCEPT ->
    typename make_fn::return_type<new_instance,curl_t>::type{
        //@attention: 必须使用std::function，如果使用auto自动推导，会导致make_fn内部判断对象是否为空时出错
        std::function<void(curl_t&)> func =[&option,&value,&args...](curl_t& obj){
            obj.setOption(std::forward<CURLopt_t>(option),
                    std::forward<val_t>(value),
                    std::forward<Args>(args)...);
        };
        return make_fn::make_fn<new_instance,curl_t>::make(func);
    }
#define RETURN_T(new_instance,curl_t) typename make_fn::return_type<new_instance,curl_t>::type
#else
    // curl的工厂函数
    /*
     * @brief: 构造一个curl对象，支持easy,share,multi
     * @param: new_instance: 是否创建一个新的curl对象，默认为false
     * @attention: 如果new_instance为true，会创建一个新的curl对象，否则返回一个栈上的对象
     * */
    template<bool new_instance=false,typename curl_t>
    [[nodiscard]] auto make_curl() ->
    std::conditional_t<new_instance,std::unique_ptr<curl_t>,curl_t> {
        if constexpr(new_instance)
            return std::make_unique<curl_t>();
        else
            return curl_t();
    }
    /*
     * @brief: 构造一个curl对象，并设置选项，支持easy,share,multi
     * @param: option: 选项组，里面包含了选项和值
     * @param: new_instance: 是否创建一个新的curl对象，默认为false
     * @attention: 如果new_instance为true，会创建一个新的curl对象，否则返回一个栈上的对象
     * */
    template<bool new_instance=false,typename curl_t,typename curl_opt_t>
    [[nodiscard]] auto make_curl(curl_opt_t &options) ->
    std::conditional_t<new_instance,std::unique_ptr<curl_t>,curl_t>{
        if constexpr(new_instance){
                auto tmp = std::make_unique<curl_t>();
                tmp->setOption(options);
                return tmp;
        }
        else{
            curl_easy tmp;
            tmp.setOption(options);
            return tmp;
        }
    }

    /*
     * @brief: 构造一个curl对象，并设置选项「可变参数模板」,支持easy,share,multi
     * @param: option: 选项
     * @param: value: 选项值
     * @attention: 如果new_instance为true，会创建一个新的curl对象，否则返回一个栈上的对象
     * */
    template<bool new_instance=false,
        typename curl_t,typename CURLopt_t,
        typename val_t,typename ...Args>
    [[nodiscard]] auto make_curl(
            CURLopt_t &&option,val_t &&value,Args&& ...args) ->
    std::conditional_t<new_instance,std::unique_ptr<curl_t>,curl_t>{
        if constexpr(new_instance){
            auto tmp = std::make_unique<curl_t>();
            tmp->setOption(std::forward<CURLopt_t>(option),
                    std::forward<val_t>(value),
                    std::forward<Args>(args)...);
            return tmp;
        }
        else{
            curl_t tmp;
            tmp.setOption(std::forward<CURLopt_t>(option),
                    std::forward<val_t>(value),
                    std::forward<Args>(args)...);
            return tmp;
        }
        }
#define RETURN_T(new_instance,curl_t) std::conditional_t<new_instance,std::unique_ptr<curl_t>,curl_t>
#endif
    /*======================================curl_easy======================================*/
    /*
     * @brief: 构造一个curl_easy对象
     * @param: new_instance: 是否创建一个新的curl_easy对象，默认为false
     * @attention: 如果new_instance为true，会创建一个新的curl_easy对象，否则返回一个栈上的对象
     * */
    template<bool new_instance=false>
    NODISCARD auto make_curl_easy() ->
    RETURN_T(new_instance,curl_easy){
        return make_curl<new_instance,curl_easy>();
    }

    /*
     * @brief: 构造一个curl_easy对象，并设置选项
     * @param: option: 选项组，里面包含了选项和值
     * @param: new_instance: 是否创建一个新的curl_easy对象，默认为false
     * @attention: 如果new_instance为true，会创建一个新的curl_easy对象，否则返回一个栈上的对象
     * */
    template<bool new_instance=false>
    NODISCARD auto make_curl_easy(curl_option &options) ->
    RETURN_T(new_instance,curl_easy){
        return make_curl<new_instance,curl_easy>(options);
    }

    /*
     * @brief: 构造一个curl_easy对象，并设置选项（可变参数模板）
     * @param: option: 选项
     * @param: value: 选项值
     * @attention: 如果new_instance为true，会创建一个新的curl_easy对象，否则返回一个栈上的对象
     * */
    template <bool new_instance=false,typename val_t,typename ...Args>
    NODISCARD auto make_curl_easy(CURLoption &&option,val_t &&value,Args&& ...args) ->
    RETURN_T(new_instance,curl_easy){
        return make_curl<new_instance,curl_easy>(std::forward<CURLoption>(option),
                std::forward<val_t>(value),
                std::forward<Args>(args)...);
    }

    /*======================================curl_multi======================================*/
    /*
     * @brief: 构造一个curl_multi对象
     * @param: new_instance: 是否创建一个新的curl_multi对象，默认为false
     * @attention: 如果new_instance为true，会创建一个新的curl_multi对象，否则返回一个栈上的对象
     * */
    template<bool new_instance=false>
    NODISCARD auto make_curl_multi() ->
    RETURN_T(new_instance,curl_multi){
        return make_curl<new_instance,curl_multi>();
    }

    /*
     * @brief: 构造一个curl_multi对象，并设置选项
     * @param: option: 选项组，里面包含了选项和值
     * @param: new_instance: 是否创建一个新的curl_multi对象，默认为false
     * @attention: 如果new_instance为true，会创建一个新的curl_multi对象，否则返回一个栈上的对象
     * */
    template<bool new_instance=false>
    NODISCARD auto make_curl_multi(curl_option &options) ->
    RETURN_T(new_instance,curl_multi){
        return make_curl<new_instance,curl_multi>(options);
    }

    /*
     * @brief: 构造一个curl_multi对象，并设置选项（可变参数模板）
     * @param: option: 选项
     * @param: value: 选项值
     * @attention: 如果new_instance为true，会创建一个新的curl_multi对象，否则返回一个栈上的对象
     * */
    template <bool new_instance=false,typename val_t,typename ...Args>
    NODISCARD auto make_curl_multi(CURLoption &&option,val_t &&value,Args&& ...args) ->
    RETURN_T(new_instance,curl_multi){
        return make_curl<new_instance,curl_multi>(std::forward<CURLoption>(option),
                std::forward<val_t>(value),
                std::forward<Args>(args)...);
    }
}//namespace web
#endif // _CURL_MAKEFN_H
