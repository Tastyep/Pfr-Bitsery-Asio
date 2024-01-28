#ifndef SHARED_ENUM_HPP
#define SHARED_ENUM_HPP

#include <boost/describe.hpp>
#include <boost/mp11.hpp>
#include <type_traits>

template <class E>
char const *enum_to_string(E e)
  requires std::is_enum_v<E>
{
  char const *name = "(unnamed)";

  boost::mp11::mp_for_each<boost::describe::describe_enumerators<E>>(
      [&](auto D)
      {
        if (e == D.value)
        {
          name = D.name;
        }
      });

  return name;
}

#endif
