#ifndef _CURL_MAKEFN_H
#define _CURL_MAKEFN_H 1
#include "_curl_error.h"
#include "_curl.h"

namespace web{
#if __cplusplus < 201703L
#include "_makefn_c++11.h"
    /*
     * @brief: 构造一个curl_easy对象
     * @param: new_instance: 是否创建一个新的curl_easy对象,默认为false
     * @attention: 如果new_instance为true,会创建一个新的curl_easy对象，否则返回一个栈上的对象
     * @return: 返回一个curl_easy对象,或者一个unique_ptr<curl_easy>对象
     * */
    template <bool new_instance=false>
    auto make_curl_easy() ->
    typename make_fn::return_type<new_instance,curl_easy>::type{
        return make_fn::make_fn<new_instance,curl_easy>::make(nullptr);
    }

    /*
     * @brief: 构造一个curl_easy对象,并设置选项
     * @param: option: 选项组，里面包含了选项和值
     * @param: new_instance: 是否创建一个新的curl_easy对象,默认为false
     * @attention: 如果new_instance为true,会创建一个新的curl_easy对象，否则返回一个栈上的对象
     * @return: 返回一个curl_easy对象,或者一个unique_ptr<curl_easy>对象
     * */
    template<bool new_instance>
    auto make_curl_easy(curl_option &options) NOEXCEPT ->
    typename make_fn::return_type<new_instance,curl_easy>::type{
        //@attention: 必须使用std::function，如果使用auto自动推导，会导致make_fn内部判断对象是否为空时出错
        std::function<void(curl_easy&)> func = [&options](curl_easy& obj) {
            obj.setOption(options);
        };
        return make_fn::make_fn<new_instance,curl_easy>::make(func);
    }

    /*
     * @brief: 构造一个curl_easy对象,并设置选项（可变参数模板）
     * @param: option: 选项
     * @param: value: 选项值
     * @attention: 如果new_instance为true,会创建一个新的curl_easy对象，否则返回一个栈上的对象
     * @return: 返回一个curl_easy对象,或者一个unique_ptr<curl_easy>对象
     * */
    template<bool new_instance=false,typename T,typename ...Args>
    auto make_curl_easy(CURLoption &&option,T &&value,Args&& ...args) NOEXCEPT ->
    typename make_fn::return_type<new_instance,curl_easy>::type{
        //@attention: 必须使用std::function，如果使用auto自动推导，会导致make_fn内部判断对象是否为空时出错
        std::function<void(curl_easy&)> func =[&option,&value,&args...](curl_easy& obj){
            obj.setOption(std::forward<CURLoption>(option),
                    std::forward<T>(value),
                    std::forward<Args>(args)...);
        };
        return make_fn::make_fn<new_instance,curl_easy>::make(func);
    }

#else
    // curl_easy的工厂函数
    /*
     * @brief: 构造一个curl_easy对象
     * @param: new_instance: 是否创建一个新的curl_easy对象,默认为false
     * @attention: 如果new_instance为true,会创建一个新的curl_easy对象，否则返回一个栈上的对象
     * @return: 返回一个curl_easy对象,或者一个unique_ptr<curl_easy>对象
     * */
    template<bool new_instance=false>
    [[nodiscard]] auto make_curl_easy() NOEXCEPT ->
    std::conditional_t<new_instance,std::unique_ptr<curl_easy>,curl_easy> {
        if constexpr(new_instance)
            try{
            return std::make_unique<curl_easy>();
            }
            catch(std::bad_alloc &e){
                return curl_easy();
            }
        else
            return curl_easy();
    }
    /*
     * @brief: 构造一个curl_easy对象,并设置选项
     * @param: option: 选项组，里面包含了选项和值
     * @param: new_instance: 是否创建一个新的curl_easy对象,默认为false
     * @attention: 如果new_instance为true,会创建一个新的curl_easy对象，否则返回一个栈上的对象
     * @return: 返回一个curl_easy对象,或者一个unique_ptr<curl_easy>对象
     * */
    template<bool new_instance=false>
    [[nodiscard]] auto make_curl_easy(curl_option &options) NOEXCEPT ->
    std::conditional_t<new_instance,std::unique_ptr<curl_easy>,curl_easy>{
        if constexpr(new_instance){
            try{
                auto tmp = std::make_unique<curl_easy>();
                tmp->setOption(options);
                return tmp;
            }
            catch(std::bad_alloc &e){
                curl_easy tmp;
                tmp.setOption(options);
                return tmp;
            }
        }
        else{
            curl_easy tmp;
            tmp.setOption(options);
            return tmp;
        }
    }

    /*
     * @brief: 构造一个curl_easy对象,并设置选项（可变参数模板）
     * @param: option: 选项
     * @param: value: 选项值
     * @attention: 如果new_instance为true,会创建一个新的curl_easy对象，否则返回一个栈上的对象
     * @return: 返回一个curl_easy对象,或者一个unique_ptr<curl_easy>对象
     * */
    template<bool new_instance=false,typename T,typename ...Args>
    [[nodiscard]] auto make_curl_easy(
            CURLoption &&option,T &&value,Args&& ...args) NOEXCEPT ->
    std::conditional_t<new_instance,std::unique_ptr<curl_easy>,curl_easy>{
        if constexpr(new_instance){
            try{
                auto tmp = std::make_unique<curl_easy>();
                tmp->setOption(std::forward<CURLoption>(option),
                        std::forward<T>(value),
                        std::forward<Args>(args)...);
                return tmp;
            }
            catch(std::bad_alloc &e){
                curl_easy tmp;
                tmp.setOption(std::forward<CURLoption>(option),
                        std::forward<T>(value),
                        std::forward<Args>(args)...);
                return tmp;
            }
        }
        else{
            curl_easy tmp;
            tmp.setOption(std::forward<CURLoption>(option),
                    std::forward<T>(value),
                    std::forward<Args>(args)...);
            return tmp;
        }
    }
#endif
}//namespace web
#endif // _CURL_MAKEFN_H
