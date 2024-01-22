#ifndef SHARED_TOML_H
#define SHARED_TOML_H

#include "shared/meta.h"

#include <boost/pfr/core.hpp>
#include <boost/pfr/core_name.hpp>
#include <iostream>
#include <sstream>
#include <toml.hpp>
#include <type_traits>
namespace Detail
{

template <IsClass T>
void serializeAsToml(const T &data, toml::value &dest)
{
  const auto names      = boost::pfr::names_as_array<T>();
  const auto serializer = Overloaded{
      [&dest]<typename F>(const F &field, const std::string &name)
        requires std::is_arithmetic_v<F>
      {
        /* std::cout << "Serialize arithmetic" << std::endl; */
        dest[name] = field;
      },
      [&dest](const std::ranges::range auto &values, const std::string &name)
      {
        /* std::cout << "Serialize Range" << std::endl; */
        toml::value array(toml::array{});
        for (const auto &value : values)
        {
          toml::value subValue;
          serializeAsToml(value, subValue);
          array.push_back(std::move(subValue));
        }
        dest[name] = std::move(array);
      },
      [&dest](const IsClass auto &field, const std::string &name)
      {
        /* std::cout << "Serialize aggregate" << std::endl; */
        toml::value table(toml::table{});
        serializeAsToml(field, table);
        dest[name] = std::move(table);
      },
      [&dest](const std::string &field, const std::string &name)
      {
        /* std::cout << "Serialize string" << std::endl; */
        dest[name] = field;
      },
  };
  const auto nameInjecter =
      [&names, &serializer](const auto &field, std::size_t i)
  {
    /* std::cout << names[i] << std::endl; */
    serializer(field, std::string{names[i]});
  };
  boost::pfr::for_each_field(data, nameInjecter);
}

} // namespace Detail

template <IsClass T>
std::string serializeAsToml(const T &data)
{
  std::ostringstream ss;

  toml::value table;
  Detail::serializeAsToml(data, table);

  ss << table;
  return ss.str();
}

#endif // !SHARED_TOML_H
