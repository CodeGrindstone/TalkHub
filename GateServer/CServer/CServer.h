#pragma once

#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "../AsioIOServicePool/AsioIOServicePool.h"
namespace  beast = boost::beast;
namespace http = boost::beast::http;
namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;

class CServer : public std::enable_shared_from_this<CServer>
{
public:
    CServer(asio::io_context& ioc, unsigned short port);
    void Start();

private:
    asio::io_context& m_ioc;
    tcp::acceptor m_acceptor;
    tcp::socket m_socket;
};