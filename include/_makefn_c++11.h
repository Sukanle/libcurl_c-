#ifndef _MAKEFN_C11_H
#define _MAKEFN_C11_H 1

#ifdef CURL_ERROR_ENABLE
#define NOEXCEPT noexcept(false)
#else
#define NOEXCEPT noexcept(true)
#endif

#include <memory>
#include <functional>
namespace make_fn{
    // 返回类型判断模板
    template<bool new_instance,typename T>
        struct return_type{
            using type=typename std::conditional<new_instance,std::unique_ptr<T>,T>::type;
    };

    // 工厂函数模板返回类型判断
    template<bool new_instance,typename T>
    struct make_fn;

    // 特化：new_instance为true，返回一个unique_ptr<T>对象
    template<typename T>
    struct make_fn<true,T>{
        /*
         * @brief: 构造一个T对象，为堆区对象
         * @param: func: 回调函数，用于设置T对象的属性，默认为空
         * @return: 返回一个unique_ptr<T>对象
         * @attention: func必须使用std::function，如果使用auto自动推导，会导致make_fn内部判断对象是否为空时出错
         * */
        template<typename Callback = std::nullptr_t>
        static std::unique_ptr<T> make(Callback func=nullptr) NOEXCEPT {
            std::unique_ptr<T> tmp;
            try{
                tmp(new T());
                if(func && !std::is_same<Callback,std::nullptr_t>::value){
                    std::function<void(T&)> callback = func;
                callback(*tmp);
                }
            }
            catch(const std::bad_alloc &e){
                return make_fn<false,T>::make(func);
            }
            return tmp;
        }
    };
    // 特化：new_instance为false，返回一个栈上的T对象
    template<typename T>
    struct make_fn<false,T>{
        /*
         * @brief: 构造一个T对象，为栈区对象
         * @param: func: 回调函数，用于设置T对象的属性，默认为空
         * @return: 返回一个T对象
         * @attention: func必须使用std::function，如果使用auto自动推导，会导致make_fn内部判断对象是否为空时出错
         * */
        template<typename Callback = std::nullptr_t>
        static T make(Callback func=nullptr) NOEXCEPT {
            T tmp;
            if(func && !std::is_same<Callback,std::nullptr_t>::value){
                std::function<void(T&)> callback = func;
                callback(tmp);
            }
            return tmp;
        }
    };
}// namespace make_fn

#endif // _MAKEFN_C11_H
