#include "shared/data.h"
// clang-format off
#include <cstdint>
// clang-format on
#include "shared/serialization/binary.h"

#include <bitsery/adapter/buffer.h>
#include <bitsery/bitsery.h>
#include <bitsery/traits/core/traits.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>
#include <boost/asio.hpp>
#include <boost/pfr/io.hpp>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <format>
#include <iostream>
#include <thread>
#include <utility>
#include <vector>

using boost::asio::ip::tcp;

using Buffer        = std::vector<uint8_t>;
using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
using InputAdapter  = bitsery::InputBufferAdapter<Buffer>;

std::pair<Buffer, std::size_t> serializeToBin(const auto &data)
{
  Buffer buffer;
  auto   writtenSize = bitsery::quickSerialization(OutputAdapter{buffer}, data);
  return {buffer, writtenSize};
}

void benchmarkBitsery()
{
  Buffer     buffer;
  const auto data = Data{
      .number    = 42,
      .firstStr  = "Hello",
      .secondStr = "World!",
  };
  Data        output{};
  std::size_t totalSize{0};

  const auto beg = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 1'000'000; ++i)
  {
    const auto writtenSize =
        bitsery::quickSerialization(OutputAdapter{buffer}, data);
    bitsery::quickDeserialization(InputAdapter{buffer.begin(), writtenSize},
                                  output);
    totalSize += writtenSize;
  }

  const auto end = std::chrono::high_resolution_clock::now();
  const auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - beg);
  std::cout << std::format("Ser / Deser of  {} bytes took {} ms",
                           totalSize,
                           duration.count())
            << std::endl;
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

    /* benchmarkBitsery(); */

    boost::asio::io_context io_context;

    tcp::socket   s(io_context);
    tcp::resolver resolver(io_context);
    boost::asio::connect(s, resolver.resolve(argv[1], argv[2]));

    const auto data = Data{
        .number    = 42,
        .firstStr  = "Hello",
        .secondStr = "World!",
    };
    const auto largeData = LargeData{
        .large    = true,
        .children = std::vector(2, data),
    };

    auto [body, bodySize] = serializeToBin(largeData);
    auto [header, headerSize] =
        serializeToBin(Header{.size = static_cast<int>(bodySize)});

    Buffer package(headerSize + bodySize);
    std::copy(
        header.begin(), std::next(header.begin(), headerSize), package.begin());
    std::copy(body.begin(),
              std::next(body.begin(), bodySize),
              std::next(package.begin(), headerSize));
    const auto packageSize = headerSize + bodySize;

    std::cout << "package size: " << packageSize << std::endl;
    for (int i = 0; i < 100'000; ++i)
    {
      boost::asio::write(s, boost::asio::buffer(package, packageSize));
    }
  }
  catch (std::exception &e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
