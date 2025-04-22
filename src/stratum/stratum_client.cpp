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

 namespace nerdminer {

    StratumClient::StratumClient(const std::string& host, uint16_t port,
        const std::string& user, const std::string& password)
        : socket_(ioContext_), host_(host), port_(port),
        user_(user), password_(password), requestId_(0) {}
    
        StratumClient::~StratumClient() {
        if (socket_.is_open()) socket_.close();
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
            std::bind(&StratumClient::handleWrite, this,
                      std::placeholders::_1, std::placeholders::_2));
    }

    void StratumClient::listen() {
        doRead();
        ioContext_.run();
    }

    void StratumClient::doRead() {
        boost::asio::async_read_until(socket_, buffer_, '\n',
            std::bind(&StratumClient::handleRead, this,
                      std::placeholders::_1, std::placeholders::_2));
    }

    void StratumClient::handleRead(const boost::system::error_code& ec, std::size_t bytes_transferred) {
        (void)bytes_transferred;  // evita warning de parâmetro não usado
        if (!ec) {
            std::istream is(&buffer_);
            std::string line;
            std::getline(is, line);
            json resp = json::parse(line);
            if (resp.contains("method")) {
                onNotification(resp);
            } else {
                onResponse(resp);
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
        // Crie o JSON para enviar ao servidor Stratum
        json req = {
            {"id", requestId_++},
            {"method", "mining.submit"},
            {"params", {
                job.coinbase1,               // Aqui você usaria os dados da coinbase1
                job.merkleBranches,          // Merkle branches
                nonce                        // Nonce encontrado pela mineração
            }}
        };
    
        // Envia a requisição para o servidor Stratum
        sendRequest(req);
    }
 }