#ifndef SHARED_META_H
#define SHARED_META_H

#include <concepts>
#include <ranges>
#include <type_traits>

template <typename... T>
struct Overloaded : public T...
{
  using T::operator()...;
};
template <class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

template <typename T>
concept IsClass = std::is_aggregate_v<T>;

#endif // !SHARED_META_H
