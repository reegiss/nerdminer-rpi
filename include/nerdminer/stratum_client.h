/**
 * Project: nerdminer-rpi
 * File: stratum_client.h
 * Description: header file for the Stratum client
 *
 * Author: Regis Araujo Melo
 * Date: 2025-04-20
 * Version: 0.1.0
 *
 * MIT License
 * © 2025 Regis Araujo Melo
 */

 #pragma once

 #include <boost/asio.hpp>
 #include <nlohmann/json.hpp>

namespace nerdminer {

using tcp = boost::asio::ip::tcp;
using json = nlohmann::json;

    class StratumClient {
    public:
        StratumClient(const std::string& host, uint16_t port,
            const std::string& user, const std::string& password);
        ~StratumClient();
        void connect();
        void subscribe();
        void authorize();
        void sendRequest(const json& req);
        void listen();
        std::function<void(const json&)> onNotification;
        std::function<void(const json&)> onResponse;
    private:
        void doRead();
        void handleRead(const boost::system::error_code& ec, std::size_t bytes_transferred);
        void handleWrite(const boost::system::error_code& ec, std::size_t bytes_transferred);
        boost::asio::io_context ioContext_;
        tcp::socket socket_;
        std::string host_;
        uint16_t port_;
        std::string user_;
        std::string password_;
        boost::asio::streambuf buffer_;
        int requestId_;
    };

} // namespace nerdminer