#ifndef SHARED_SERIALIZATION_JSON_PRETTY_PRINT_H
#define SHARED_SERIALIZATION_JSON_PRETTY_PRINT_H

#include <boost/json.hpp>
#include <iostream>

void pretty_print(std::ostream             &os,
                  boost::json::value const &jv,
                  std::string              *indent = nullptr);

#endif // !SHARED_PRETTY_PRINT_HPP
