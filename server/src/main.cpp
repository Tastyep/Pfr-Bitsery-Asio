#include "shared/data.h"
#include "shared/serialization/binary.h"
// clang-format off
#include <cstdint>
// clang-format on

#include "bitsery/details/adapter_common.h"

#include <bitsery/adapter/buffer.h>
#include <bitsery/bitsery.h>
#include <bitsery/traits/core/traits.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>
#include <boost/pfr/io.hpp>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <utility>

using Buffer        = std::vector<uint8_t>;
using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
using InputAdapter  = bitsery::InputBufferAdapter<Buffer>;

using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;
namespace this_coro = boost::asio::this_coro;

struct HumanReadable
{
  std::uintmax_t size{};

  template <typename Os>
  friend Os &operator<<(Os &os, HumanReadable hr)
  {
    int    i{};
    double mantissa = hr.size;
    for (; mantissa >= 1024.0; mantissa /= 1024.0, ++i)
    {}
    os << std::ceil(mantissa * 10.0) / 10.0 << i["BKMGTPE"];
    return i ? os << "B (" << hr.size << ')' : os;
  }
};

std::pair<std::vector<Data>, std::size_t> parsePackets(const Buffer &buffer,
                                                       std::size_t   bufferSize)
{
  std::size_t parsedSize{0};

  std::vector<Data> packets;
  Header            header{};

  while (parsedSize < bufferSize)
  {
    if (auto [code, _] = bitsery::quickDeserialization(
            InputAdapter{std::next(buffer.begin(), parsedSize),
                         bufferSize - parsedSize},
            header);
        code != bitsery::ReaderError::NoError)
    {
      break;
    }
    parsedSize += sizeof(Header);

    Data data{};
    if (auto [code, _] = bitsery::quickDeserialization(
            InputAdapter{std::next(buffer.begin(), parsedSize),
                         bufferSize - parsedSize},
            data);
        code != bitsery::ReaderError::NoError)
    {
      // We want to parse the header next time
      parsedSize -= sizeof(Header);
      break;
    }

    parsedSize += header.size;
    /* std::cout << boost::pfr::io(header) << " | " << boost::pfr::io(data) */
    /*           << std::endl; */
    packets.push_back(std::move(data));
  }

  return {packets, parsedSize};
}

awaitable<void> echo(tcp::socket socket)
{
  std::printf("New client connected");

  Buffer            data(4096);
  std::size_t       receivedBytes{0};
  std::size_t       remainingBytes{0};
  std::vector<Data> receivedPackets;

  auto       start         = std::chrono::high_resolution_clock::now();
  const auto speedInterval = std::chrono::milliseconds{100};
  const auto intervalsPerSecond =
      std::chrono::milliseconds{1000}.count() / speedInterval.count();
  try
  {
    for (;;)
    {
      auto        buffer = boost::asio::buffer(data) + remainingBytes;
      std::size_t readSize =
          co_await socket.async_read_some(buffer, use_awaitable);
      const auto bufferSize            = remainingBytes + readSize;
      auto [parsedPackets, parsedSize] = parsePackets(data, bufferSize);
      std::shift_left(data.begin(), data.end(), parsedSize);
      remainingBytes = bufferSize - parsedSize;

      std::ranges::move(parsedPackets, std::back_inserter(receivedPackets));

      receivedBytes       += readSize;
      const auto now       = std::chrono::high_resolution_clock::now();
      const auto interval  = now - start;
      if (interval > speedInterval)
      {
        const auto transferSpeed = receivedBytes * intervalsPerSecond;
        std::cout << "Speed: " << HumanReadable{transferSpeed} << " / second"
                  << std::endl;
        std::cout << "Received: " << receivedPackets.size() << " packets."
                  << std::endl;
        start         = now;
        receivedBytes = 0;
      }
    }
  }
  catch (std::exception &e)
  {
    std::printf("echo Exception: %s\n", e.what());
  }
}

awaitable<void> listener()
{
  auto          executor = co_await this_coro::executor;
  tcp::acceptor acceptor(executor, {tcp::v4(), 55555});
  for (;;)
  {
    tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
    co_spawn(executor, echo(std::move(socket)), detached);
  }
}

int main()
{
  try
  {
    boost::asio::io_context io_context(1);

    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
    signals.async_wait(
        [&](auto, auto)
        {
          io_context.stop();
        });

    co_spawn(io_context, listener(), detached);

    io_context.run();
  }
  catch (std::exception &e)
  {
    std::printf("Exception: %s\n", e.what());
  }
}
