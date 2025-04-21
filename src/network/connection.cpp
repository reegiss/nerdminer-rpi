/**
 * Project: nerdminer-rpi
 * File: connection.cpp
 * Description: implements the connection to the Bitcoin network
 *
 * Author: Regis Araujo Melo
 * Date: 2025-04-20
 * Version: 0.1.0
 *
 * MIT License
 * © 2025 Regis Araujo Melo
 */

#include <netdb.h>
#include <nerdminer/network/connection.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

namespace nerdminer {

    Connection::Connection(const std::string& host, int port)
    : host_(host), port_(port), socket_fd_(-1) {}
    
    Connection::~Connection() {
        disconnect();
    }
    
    bool Connection::connect() {
        struct addrinfo hints{}, *res;
    
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
    
        int status = getaddrinfo(host_.c_str(), std::to_string(port_).c_str(), &hints, &res);
        if (status != 0) {
            std::cerr << "Failed to resolve host: " << gai_strerror(status) << std::endl;
            return false;
        }
    
        socket_fd_ = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (socket_fd_ == -1) {
            std::cerr << "Failed to create socket\n";
            freeaddrinfo(res);
            return false;
        }
    
        if (::connect(socket_fd_, res->ai_addr, res->ai_addrlen) == -1) {
            std::cerr << "Failed to connect to " << host_ << ":" << port_ << std::endl;
            close(socket_fd_);
            socket_fd_ = -1;
            freeaddrinfo(res);
            return false;
        }
    
        freeaddrinfo(res);
        std::cout << "Connected to " << host_ << ":" << port_ << std::endl;
        return true;
    }
    
    void Connection::disconnect() {
        if (socket_fd_ != -1) {
            close(socket_fd_);
            socket_fd_ = -1;
            std::cout << "Disconnected.\n";
        }
    }
    
    bool Connection::send(const std::string& data) {
        if (socket_fd_ <= 0) {
            std::cerr << "Connection is not open." << std::endl;
            return false;
        }
    
        ssize_t bytes_sent = write(socket_fd_, data.c_str(), data.size());
        if (bytes_sent < 0) {
            std::cerr << "Failed to send data: " << strerror(errno) << std::endl;
            return false;
        }
    
        return true;
    }
    
} 