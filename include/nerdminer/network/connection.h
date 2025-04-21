/**
 * Project: nerdminer-rpi
 * File: connection.h
 * Description: manages the connection to the Bitcoin network
 *
 * Author: Regis Araujo Melo
 * Date: 2025-04-20
 * Version: 0.1.0
 *
 * MIT License
 * © 2025 Regis Araujo Melo
 */

 #pragma once

 #include <string>
 
 namespace nerdminer {
 
 class Connection {
 public:
     Connection(const std::string& host, int port);
     ~Connection();
 
     bool connect();
     void disconnect();
     bool send(const std::string& data);
 
 private:
    std::string host_;
    int port_;
    int socket_fd_;
    struct addrinfo* server_info_;
 };
 
 } // namespace nerdminer