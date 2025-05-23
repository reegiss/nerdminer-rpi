/**
 * Project: nerdminer-rpi
 * File: stratum_client.cpp
 * Description: implements the Stratum client
 *
 * Author: Regis Araujo Melo
 * Date: 2025-04-20
 * Version: 0.1.0
 *
 * MIT License
 * © 2025 Regis Araujo Melo
 */

#include <nerdminer/stratum_client.h>
#include <nerdminer/miner_job.h>
#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include <functional>
#include <iomanip>

static std::string toHex(uint32_t value, int width = 8) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(width) << value;
    return ss.str();
}

 namespace nerdminer {

    StratumClient::StratumClient(const std::string& host, uint16_t port,
        const std::string& user, const std::string& password)
        : socket_(ioContext_), 
        host_(host), 
        port_(port),
        user_(user), 
        password_(password), 
        requestId_(0) {}
    
    StratumClient::~StratumClient() {
        if (socket_.is_open()) {
            socket_.close();
        }
    }

    void StratumClient::connect() {
        tcp::resolver resolver(ioContext_);
        auto endpoints = resolver.resolve(host_, std::to_string(port_));
        boost::asio::connect(socket_, endpoints);
    }

    void StratumClient::subscribe() {
        json req = {
            {"id", requestId_++},
            {"method", "mining.subscribe"},
            {"params", json::array()}
        };
        sendRequest(req);
    }

    void StratumClient::authorize() {
        json req = {
            {"id", requestId_++},
            {"method", "mining.authorize"},
            {"params", {user_, password_}}
        };
        sendRequest(req);
    }

    void StratumClient::sendRequest(const json& req) {
        std::string msg = req.dump() + "\n";
        boost::asio::async_write(socket_, boost::asio::buffer(msg),
            [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
                handleWrite(ec, bytes_transferred);
            });
    }

    void StratumClient::listen() {
        doRead();
        ioContext_.run();
    }

    void StratumClient::doRead() {
        boost::asio::async_read_until(socket_, buffer_, '\n',
            [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
                handleRead(ec, bytes_transferred);
            });
    }

    void StratumClient::handleRead(const boost::system::error_code& ec, std::size_t bytes_transferred) {
        (void)bytes_transferred;  // evitar warning de parâmetro não usado
        if (!ec) {
            std::istream is(&buffer_);
            std::string line;
            std::getline(is, line);
            json resp = json::parse(line);

            if (resp.contains("method")) {
                onNotification(resp);
            } else if (resp.contains("result")) {
                onResponse(resp);
            } else {
                std::cerr << "Unexpected response: " << resp.dump() << std::endl;
            }

            doRead();
        } else {
            std::cerr << "Read error: " << ec.message() << std::endl;
        }
    }

    void StratumClient::handleWrite(const boost::system::error_code& ec, std::size_t) {
        if (ec) {
            std::cerr << "Write error: " << ec.message() << std::endl;
        }
    }

    void StratumClient::submitShare(const nerdminer::MiningJob& job, uint32_t nonce) {
        json req = {
            {"id", requestId_++},
            {"method", "mining.submit"},
            {"params", {
                user_,                    // worker_name
                job.jobId,                 // job_id
                job.extraNonce,            // extranonce2
                job.nTime,                 // ntime (timestamp já string em hexadecimal)
                toHex(nonce)            // nonce convertido para string hexadecimal
            }}
        };

        std::cout << "Submitting share: Job ID: " << job.jobId << ", Nonce: " << nonce << std::endl;
    
        sendRequest(req);
    }

    void nerdminer::StratumClient::handleSubmitResponse(const json& response) {
        if (response.contains("result") && response["result"].is_boolean()) {
            if (response["result"].get<bool>()) {
                std::cout << "[*] Share accepted!" << std::endl;
            } else {
                std::cout << "[!] Share rejected!" << std::endl;
            }
        } else if (response.contains("error") && !response["error"].is_null()) {
            std::cout << "[!] Share rejected with error: " << response["error"].dump() << std::endl;
        } else {
            std::cout << "[?] Unknown response to share submission." << std::endl;
        }
    }

 }