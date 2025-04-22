/**
 * Project: nerdminer-rpi
 * File: miner_session.cpp
 * Description: implementation of the miner session
 *
 * Author: Regis Araujo Melo
 * Date: 2025-04-21
 * Version: 0.1.0
 *
 * MIT License
 * © 2025 Regis Araujo Melo
 */

#include "nerdminer/miner_session.h"
#include "nerdminer/miner_job.h"
#include <nerdminer/nerdminer_block.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <openssl/sha.h>

namespace nerdminer {

MinerSession::MinerSession(const std::string& host, uint16_t port, const std::string& user, const std::string& password)
    : client_(host, port, user, password), miningActive(false) {
    numThreads_ = std::thread::hardware_concurrency();
    if (numThreads_ == 0) {
        numThreads_ = 1; // fallback, se falhar
    }
    std::cout << "Detected " << numThreads_ << " CPU cores. Starting " << numThreads_ << " mining threads." << std::endl;

    client_.onResponse = [](const nerdminer::json& resp) {
        std::cout << "Response: " << resp.dump() << std::endl;
    };

    client_.onNotification = [this](const nerdminer::json& note) {
        handleNotification(note);
    };
}

void MinerSession::start() {
    std::cout << "Connecting to pool server...\n";
    client_.connect();
    client_.subscribe();
    client_.authorize();
    client_.listen();
    stopMiningThreads();
    startMiningThreads();
}

void MinerSession::handleNotification(const nerdminer::json& note) {
    // std::cout << "Received notification: " << note.dump() << std::endl;
    if (note.contains("method")) {
        const std::string method = note["method"].get<std::string>();
        if (method == "mining.notify") {
            MiningJob newJob = MiningJob::fromNotification(note);
            if (newJob.valid) {
                std::lock_guard<std::mutex> lock(currentJobMutex_);
                currentJob_ = newJob;
                std::cout << "Current job: " << currentJob_.jobId << std::endl;
            } else {
                std::cerr << "Received invalid mining job." << std::endl;
            }
        } else {
            std::cout << "Ignored notification: " << method << std::endl;
        }
    }
}

void MinerSession::startMiningThreads() {
    miningActive = true;
    std::cout << "Starting mining threads..." << std::endl;
    for (int i = 0; i < numThreads_; ++i) {
        miners_.emplace_back(&MinerSession::miningLoop, this, i);
    }
}

void MinerSession::stopMiningThreads() {
    miningActive = false;
    std::cout << "Stopping mining threads..." << std::endl;

    for (auto& miner : miners_) {
        if (miner.joinable()) {
            miner.join();
        }
    }

    miners_.clear();
}

void MinerSession::miningLoop(int threadId) {
    std::cout << "[Thread " << threadId << "] Starting mining loop..." << std::endl;
    while (miningActive) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simula o trabalho de mineração

        std::lock_guard<std::mutex> lock(currentJobMutex_);
        if (currentJob_.valid) {
            // Simula a mineração
            std::cout << "[Thread " << threadId << "] Mining job: " << currentJob_.jobId << std::endl;

            // Aqui você pode adicionar a lógica de mineração real
            // Por exemplo, calcular o hash e verificar se é válido
            // ...

            // Simula o envio de uma solução
            client_.submitShare(currentJob_, 0); // Envia um nonce fictício
        }
    }
}

} // namespace nerdminer
 