#ifndef SHARED_JSON_BINARY_H
#define SHARED_JSON_BINARY_H

#include "boost/json/value_from.hpp"
#include "shared/meta.h"
#include "shared/serialization/json/pretty_print.hpp"

#include <boost/json.hpp>
#include <boost/pfr/core.hpp>
#include <boost/pfr/core_name.hpp>
#include <sstream>

template <IsClass T>
void tag_invoke(const boost::json::value_from_tag &,
                boost::json::value &v,
                const T            &data)
{
  auto      &dest  = v.emplace_object();
  const auto names = boost::pfr::names_as_array<T>();
  boost::pfr::for_each_field(//
      data,
      [&names, &dest](const auto &field, std::size_t i)
      {
        dest[names[i]] = boost::json::value_from(field);
      });
}

template <IsClass T>
std::string serializeAsJson(const T &data)
{
  /* return boost::json::serialize(boost::json::value_from(data)); */
  std::ostringstream ss;
  const auto        &jsonValue = boost::json::value_from(data);
  pretty_print(ss, jsonValue);
  return ss.str();
}

#endif
