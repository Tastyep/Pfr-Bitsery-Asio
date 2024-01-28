#ifndef SHARED_SERIALIZATION_BINARY_H
#define SHARED_SERIALIZATION_BINARY_H

#include "shared/meta.h"

#include <boost/pfr/core.hpp>
#include <boost/pfr/core_name.hpp>
#include <type_traits>
/* #include <iostream> */
#include <bitsery/traits/core/traits.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>

template <typename S>
void serialize(S &s, IsClass auto &data)
{
  boost::pfr::for_each_field(//
      data,
      Overloaded{
                             // clang-format off
          [&s]<typename T>
            requires std::is_enum_v<T>
          (T & field)
          {
            using UnderlyingType    = std::underlying_type_t<T>;
            constexpr auto typeSize = sizeof(UnderlyingType);
            auto          underlyingValue =
                static_cast<UnderlyingType>(field);
            s.template value<typeSize>(underlyingValue);
            field = static_cast<T>(underlyingValue);
          },
          [&s]<typename T>(T &field)
            requires std::is_arithmetic_v<T>
          {
                             // clang-format on
            /* std::cout << "Serialize Arithmetic: " << field */
            /*           << " size: " << (sizeof field) << std::endl; */
            s.template value<sizeof field>(field);
          },
          [&s](std::string &field)
          {
            /* std::cout << "Serialize Text" << std::endl; */
            s.text1b(field, 100);
          },
          [&s](std::ranges::range auto &field)
          {
            /* std::cout << "Serialize Range" << std::endl; */
            s.container(field, 2048);
          },
          [&s](IsClass auto &field)
          {
            /* std::cout << "Serialize aggregate" << std::endl; */
            serialize(s, field);
          },
      });
}

#endif
