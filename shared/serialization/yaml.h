#ifndef SHARED_YAML_BINARY_H
#define SHARED_YAML_BINARY_H

#include "shared/meta.h"
#include "yaml-cpp/emittermanip.h"

#include <boost/pfr/core.hpp>
#include <boost/pfr/core_name.hpp>
#include <yaml-cpp/yaml.h>

namespace Detail
{

template <IsClass T>
void serializeAsYaml(const T &data, YAML::Emitter &dest);

template <typename T>
void serializeAsYaml(const T &data, YAML::Emitter &dest)
  requires std::is_arithmetic_v<T> || std::same_as<T, std::string>
{
  dest << data;
}

template <std::ranges::range T>
void serializeAsYaml(const T &data, YAML::Emitter &dest)
  requires(!std::same_as<T, std::string>)
{
  dest << YAML::BeginSeq;
        /* std::cout << "Serialize Range" << std::endl; */
  for (const auto &field : data)
  {
    Detail::serializeAsYaml(field, dest);
  }
  dest << YAML::EndSeq;
}

template <IsClass T>
void serializeAsYaml(const T &data, YAML::Emitter &dest)
{
  dest << YAML::BeginMap;
  const auto names = boost::pfr::names_as_array<T>();
  boost::pfr::for_each_field( //
      data,
      [&names, &dest](const auto &field, std::size_t i)
      {
        dest << YAML::Key << std::string{names[i]} << YAML::Value;
        Detail::serializeAsYaml(field, dest);
      });
  dest << YAML::EndMap;
}

} // namespace Detail

template <IsClass T>
std::string serializeAsYaml(const T &data)
{
  std::ostringstream ss;

  YAML::Emitter out;
  out.SetIndent(4);
  /* out.SetMapStyle(YAML::Flow); */

  Detail::serializeAsYaml(data, out);

  ss << std::string_view{out.c_str(), out.size()};
  return ss.str();
}

#endif
