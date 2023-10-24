#pragma once

#include <type_traits>
#include <utility>

namespace detail {

template <typename T>
struct IsComplete
{
private:
    template <typename U, std::size_t = sizeof(U)>
    static std::true_type impl(U*);
    static std::false_type impl(...);
    using bool_constant = decltype(impl(std::declval<T*>()));

public:
    static constexpr bool value = bool_constant::value;
};

} // namespace detail
