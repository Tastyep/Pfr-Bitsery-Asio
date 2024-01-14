#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>
#include <chrono>
#include <cstdio>
#include <iostream>

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

/* awaitable<std::size_t> echo_once(tcp::socket &socket) */
/* { */
/*   char data[4096]; */
/*   co_await async_write(socket, boost::asio::buffer(data, n), use_awaitable);
 */
/**/
/*   co_return n; */
/* } */

awaitable<void> echo(tcp::socket socket)
{
  std::printf("New client connected");

  std::vector<uint8_t> data(4096);
  std::size_t          receivedBytes = 0;

  auto       start         = std::chrono::high_resolution_clock::now();
  const auto speedInterval = std::chrono::milliseconds{100};
  const auto intervalsPerSecond =
      std::chrono::milliseconds{1000}.count() / speedInterval.count();
  try
  {
    for (;;)
    {
      // The asynchronous operations to echo a single chunk of data have been
      // refactored into a separate function. When this function is called, the
      // operations are still performed in the context of the current
      // coroutine, and the behaviour is functionally equivalent.
      std::size_t readSize = co_await socket.async_read_some(
          boost::asio::buffer(data), use_awaitable);
      /* auto packageSize     = co_await echo_once(socket); */
      receivedBytes       += readSize;
      const auto now       = std::chrono::high_resolution_clock::now();
      const auto interval  = now - start;
      if (interval > speedInterval)
      {
        const auto transferSpeed = receivedBytes * intervalsPerSecond;
        std::cout << "Speed: " << HumanReadable{transferSpeed} << " / second"
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
