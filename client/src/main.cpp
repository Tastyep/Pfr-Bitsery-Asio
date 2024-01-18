#include "shared/data.h"
// clang-format off
#include <chrono>
#include <cstdint>
// clang-format on

#include <bitsery/adapter/buffer.h>
#include <bitsery/bitsery.h>
#include <bitsery/traits/core/traits.h>
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

using Buffer        = std::vector<uint8_t>;
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
void serialize(S &s, IsClass auto &data)
{
  /* const auto fieldnames = boost::pfr::names_as_array<data>(); */
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

std::pair<Buffer, std::size_t> serialize(const auto &data)
{
  Buffer     buffer;
  const auto writtenSize =
      bitsery::quickSerialization(OutputAdapter{buffer}, data);
  return {std::move(buffer), writtenSize};
}

void benchmarkBitsery()
{
  Buffer     buffer;
  const auto data = Data{
      .number = 42,
      .str    = "Hello World!",
  };
  Data output{};

  const auto beg = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 1'000'000; ++i)
  {
    const auto writtenSize =
        bitsery::quickSerialization(OutputAdapter{buffer}, data);
    bitsery::quickDeserialization(InputAdapter{buffer.begin(), writtenSize},
                                  output);
  }
  const auto end = std::chrono::high_resolution_clock::now();
  const auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - beg);
  std::cout << "Ser / Deser time: " << duration.count() << " ms" << std::endl;
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

    benchmarkBitsery();

    /* boost::asio::io_context io_context; */
    /**/
    /* tcp::socket   s(io_context); */
    /* tcp::resolver resolver(io_context); */
    /* boost::asio::connect(s, resolver.resolve(argv[1], argv[2])); */
    /**/
    /* auto [body, bodySize] = serialize(Data{ */
    /*     .number = 42, */
            /* .str    = "Hello World!", */
    /* }); */
    /* auto [header, headerSize] = */
    /*     serialize(Header{.size = static_cast<int>(bodySize)}); */
    /* Buffer package = std::move(header); */
    /* std::move(body.begin(), body.end(), std::back_inserter(package)); */
    /* const auto packageSize = headerSize + bodySize; */
    /* for (int i = 0; i < 1'000'000; ++i) */
    /* { */
    /*   boost::asio::write(s, boost::asio::buffer(package, packageSize)); */
    /* } */
  }
  catch (std::exception &e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
