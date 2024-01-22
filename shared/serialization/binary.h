#ifndef SHARED_SERIALIZATION_BINARY_H
#define SHARED_SERIALIZATION_BINARY_H

#include "shared/meta.h"

#include <boost/pfr/core.hpp>
#include <boost/pfr/core_name.hpp>
/* #include <iostream> */

template <typename S>
void serialize(S &s, IsClass auto &data)
{
  boost::pfr::for_each_field(//
      data,
      Overloaded{
          [&s]<typename T>(T &field)
            requires std::is_arithmetic_v<T>
          {
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
