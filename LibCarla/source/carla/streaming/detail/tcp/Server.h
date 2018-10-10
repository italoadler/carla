// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/NonCopyable.h"
#include "carla/Time.h"
#include "carla/streaming/detail/tcp/ServerSession.h"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <atomic>

namespace carla {
namespace streaming {
namespace detail {
namespace tcp {

  /// @warning This server cannot be destructed before its @a io_service is
  /// stopped.
  class Server : private NonCopyable {
  public:

    using endpoint = boost::asio::ip::tcp::endpoint;
    using protocol_type = endpoint::protocol_type;

    explicit Server(boost::asio::io_service &io_service, endpoint ep);

    /// Set session time-out. Applies only to newly created sessions. By default
    /// the time-out is set to 10 seconds.
    void SetTimeout(time_duration timeout) {
      _timeout = timeout;
    }

    /// Start listening for connections. On each new connection, @a
    /// on_session_opened is called, and @a on_session_closed when the session
    /// is closed.
    template <typename FunctorT1, typename FunctorT2>
    void Listen(FunctorT1 on_session_opened, FunctorT2 on_session_closed) {
      _acceptor.get_io_service().post([=]() {
        OpenSession(
            _timeout,
            std::move(on_session_opened),
            std::move(on_session_closed));
      });
    }

  private:

    void OpenSession(
        time_duration timeout,
        ServerSession::callback_function_type on_session_opened,
        ServerSession::callback_function_type on_session_closed);

    boost::asio::ip::tcp::acceptor _acceptor;

    std::atomic<time_duration> _timeout;
  };

} // namespace tcp
} // namespace detail
} // namespace streaming
} // namespace carla
