#include "shared/data.h"
// clang-format off
#include <cstdint>
// clang-format on

#include <bitsery/adapter/buffer.h>
#include <bitsery/bitsery.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>
#include <boost/asio.hpp>
#include <boost/pfr/core.hpp>
#include <boost/pfr/core_name.hpp>
#include <concepts>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <type_traits>
#include <vector>

using boost::asio::ip::tcp;

using Buffer        = std::vector<std::uint8_t>;
using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
using InputAdapter  = bitsery::InputBufferAdapter<Buffer>;

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
void serialize(S &s, const IsClass auto &data)
{
  /* const auto fieldnames = boost::pfr::names_as_array<data>(); */
  boost::pfr::for_each_field(// 
                             data,
                             Overloaded{
                                 [&s]<typename T>(const T &field) requires std::is_arithmetic_v<T> 
                                 {
                                   s.template value<sizeof field>(field);
                                 },
                                 [&s](const std::string &field)
                                 {
                                   s.text1b(field, field.size());
                                 },
                                 [&s](const IsClass auto &field)
                                 {
                                   serialize(s, field);
                                 },
                             });
}

std::pair<Buffer, std::size_t> serialize(const Data &data)
{
  Buffer     buffer;
  const auto writenSize =
      bitsery::quickSerialization(OutputAdapter{buffer}, data);
  return {std::move(buffer), writenSize};
}

int main(int argc, char *argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
      return 1;
    }

    /* boost::asio::io_context io_context; */
    /**/
    /* tcp::socket   s(io_context); */
    /* tcp::resolver resolver(io_context); */
    /* boost::asio::connect(s, resolver.resolve(argv[1], argv[2])); */

    auto [buffer, size] = serialize(Data{
        .header = {.size = 0},
        .number = 42,
        .str    = "Hello World!",
    });
    std::cout << buffer.size() << " | " << size << std::endl;
    /* for (int i = 0; i < 1'000'000; ++i) */
    /* { */
    /*   boost::asio::write(s, boost::asio::buffer(buffer, size)); */
    /* } */
  }
  catch (std::exception &e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
