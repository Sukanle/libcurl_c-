#pragma once
#ifndef MAKEFN_CPP11_H
#define MAKEFN_CPP11_H 1

#include <cstdio>
#include <functional>
#include <memory>


#ifndef CPP_CURL_MACRO_SETTINGS_H
#if defined CURL_ERROR_ENABLE || defined CURL_ERROR_DETAILED
#undef NOEXCEPT
#define NOEXCEPT noexcept(false)
#define CURL_ERROR_ON
#else
#define NOEXCEPT
#endif
#endif

namespace make_fn {
// 返回类型判断模板
template <typename T, bool new_instance> struct return_type {
    using type = typename std::conditional<new_instance, std::unique_ptr<T>, T>::type;
};

// 工厂函数模板返回类型判断
template <typename T, bool new_instance> struct make_fn;

// 特化：new_instance为true，返回一个unique_ptr<T>对象
template <typename T> struct make_fn<T, true> {
    /*
     * @brief: 构造一个T对象，为堆区对象
     * @param: func: 回调函数，用于设置T对象的属性，默认为空
     * @return: 返回一个unique_ptr<T>对象
     * @attention:
     * func必须使用std::function，如果使用auto自动推导，会导致make_fn内部判断对象是否为空时出错
     * */
    template <typename Callback = std::nullptr_t>
    static std::unique_ptr<T> make(Callback func = nullptr) NOEXCEPT
    {
        std::unique_ptr<T> tmp;
        tmp(new T());
        if (func && !std::is_same<Callback, std::nullptr_t>::value) {
            std::function<void(T&)> callback = func;
            callback(*tmp);
        }
        return tmp;
    }
};
// 特化：new_instance为false，返回一个栈上的T对象
template <typename T> struct make_fn<T, false> {
    /*
     * @brief: 构造一个T对象，为栈区对象
     * @param: func: 回调函数，用于设置T对象的属性，默认为空
     * @return: 返回一个T对象
     * @attention:
     * func必须使用std::function，如果使用auto自动推导，会导致make_fn内部判断对象是否为空时出错
     * */
    template <typename Callback = std::nullptr_t> static T make(Callback func = nullptr) NOEXCEPT
    {
        T tmp;
        if (func && !std::is_same<Callback, std::nullptr_t>::value) {
            std::function<void(T&)> callback = func;
            callback(tmp);
        }
        return tmp;
    }
};
} // namespace make_fn

#endif // _MAKEFN_C11_H
