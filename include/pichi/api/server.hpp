#ifndef PICHI_API_SERVER_HPP
#define PICHI_API_SERVER_HPP

#include <array>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <pichi/api/egress_manager.hpp>
#include <pichi/api/ingress_manager.hpp>
#include <pichi/api/rest.hpp>
#include <pichi/api/router.hpp>
#include <pichi/buffer.hpp>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>

namespace pichi::api {

class Server {
private:
  using Acceptor = boost::asio::basic_socket_acceptor<boost::asio::ip::tcp>;
  using ResolveResult = boost::asio::ip::tcp::resolver::results_type;

  template <typename Yield> void listen(Acceptor&, std::string_view, IngressVO const&, Yield);
  template <typename ExceptionPtr> void removeIngress(ExceptionPtr, std::string_view);
  EgressVO const& route(net::Endpoint const&, std::string_view ingress, AdapterType,
                        ResolveResult const&);
  template <typename Yield> bool isDuplicated(ConstBuffer<uint8_t>, Yield);

public:
  Server(Server const&) = delete;
  Server(Server&&) = delete;
  Server& operator=(Server const&) = delete;
  Server& operator=(Server&&) = delete;

  Server(boost::asio::io_context&, char const*);
  ~Server() = default;

  void listen(std::string_view, uint16_t);
  void startIngress(Acceptor&, std::string_view, IngressVO const&);

private:
  boost::asio::io_context::strand strand_;
  std::unordered_set<std::string> ivs_;
  Router router_;
  EgressManager egresses_;
  IngressManager ingresses_;
  Rest rest_;
};

} // namespace pichi::api

#endif // PICHI_API_SERVER_HPP
