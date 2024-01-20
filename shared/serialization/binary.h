#ifndef SHARED_SERIALIZATION_BINARY_H
#define SHARED_SERIALIZATION_BINARY_H

#include <boost/pfr/core.hpp>
#include <boost/pfr/core_name.hpp>
#include <concepts>
#include <type_traits>

template <typename... T>
struct Overloaded : public T...
{
  using T::operator()...;
};
template <class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

template <typename T>
concept IsClass = std::is_class_v<T>;

template <typename S>
void serialize(S &s, IsClass auto &data)
{
  boost::pfr::for_each_field(//
      data,
      Overloaded{
          [&s]<typename T>(T &field)
            requires std::is_arithmetic_v<T>
          {
            s.template value<sizeof field>(field);
          },
          [&s](std::string &field)
          {
            s.text1b(field, 100);
          },
          [&s](IsClass auto &field)
          {
            serialize(s, field);
          },
      });
}

#endif
