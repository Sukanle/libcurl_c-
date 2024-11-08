#pragma once
#ifndef HELP_CPLUSPLUS11_HPP
#define HELP_CPLUSPLUS11_HPP 1
#warning                                                                                           \
    "This header file is used to fill in the missing parts of C++11 for subsequent versions of functionality"
#include <utility>

namespace std {
template <typename T, typename U = T>
__attribute__((warn_unused_result)) T exchange(T& obj, U&& new_value) noexcept(
    is_nothrow_move_constructible<T>::value && is_nothrow_move_assignable<T>::value)
{
    T old_value = std::move(obj);
    obj = std::forward<U>(new_value);
    return old_value;
}
} // namespace std
#endif
