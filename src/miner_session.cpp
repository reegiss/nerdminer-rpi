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
    stopMiningThreads();
    startMiningThreads();
    client_.listen();
}

void MinerSession::handleNotification(const nerdminer::json& note) {
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
    miners_.clear();
    threadHashCounts_.resize(numThreads_, 0);
    lastHashrateTime_ = std::chrono::steady_clock::now();
    for (int i = 0; i < numThreads_; ++i) {
        std::cout << "Starting thread " << i << std::endl;
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
    std::cout << "Thread " << threadId << " started mining loop." << std::endl;
    while (miningActive) {
        if (!currentJob_.valid) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        nerdminer::BlockHeader header;
        header.version = std::stoul(currentJob_.version, nullptr, 16);
        header.prevHash = currentJob_.prevHash;
        header.merkleRoot = nerdminer::calculateMerkleRoot(currentJob_.coinbase1 + extranonce1_ + currentJob_.coinbase2, currentJob_.merkleBranches);
        header.timestamp = std::stoul(currentJob_.nTime, nullptr, 16);
        header.bits = std::stoul(currentJob_.nBits, nullptr, 16);
        header.nonce = 0;

        auto target = nerdminer::targetFromBits(header.bits);

        for (uint32_t nonce = 0; nonce < 0xFFFFFFFF; ++nonce) {
            header.nonce = nonce;
            auto headerBytes = nerdminer::buildBlockHeader(header);
            auto hash = nerdminer::doubleSHA256(headerBytes);

            threadHashCounts_[threadId]++;

            if (nerdminer::isHashBelowTarget(hash, target)) {
                {
                    std::lock_guard<std::mutex> lock(outputMutex_);
                    std::cout << "\033[1;34mThread " << threadId << " found valid nonce: " << nonce << "\n"
                            << "Hash: " << nerdminer::bytesToHex(hash) << "\033[0m" << std::endl;
                }
                client_.submitShare(currentJob_, nonce);
                break;
            }

            if (!miningActive) {
                break;
            }

            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastHashrateTime_).count();
            if (elapsed >= 5) {
                uint64_t totalHashes = 0;
                for (const auto& count : threadHashCounts_) {
                    totalHashes += count;
                }
                double hashrate = static_cast<double>(totalHashes) / elapsed;
                
                {
                    std::lock_guard<std::mutex> lock(outputMutex_);
                    std::cout << "\033[1;32mHashrate: " << hashrate << " H/s\033[0m" << std::endl;
                }

                for (auto& count : threadHashCounts_) {
                    count = 0;
                }
                lastHashrateTime_ = now;
            }
        }
    }
}

} // namespace nerdminer
