/**
 * Project: nerdminer-rpi
 * File: miner_session.h
 * Description: header file for the miner session
 *
 * Author: Regis Araujo Melo
 * Date: 2025-04-21
 * Version: 0.1.0
 *
 * MIT License
 * © 2025 Regis Araujo Melo
 */

#pragma once

#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>
#include "nerdminer/stratum_client.h"
#include "nerdminer/miner_job.h"

namespace nerdminer {

class MinerSession {
public:
    MinerSession(const std::string& host, uint16_t port, const std::string& user, const std::string& password);
    void handleNotification(const nerdminer::json& note);
    void handleResponse(const json& response);
    void handleSubmitResponse(const json& response);
    void start();
private:
    bool running_ = true;
    std::string extranonce1_;
    void miningLoop(int threadId);
    void startMiningThreads();
    void stopMiningThreads();

private:
    StratumClient client_;
    MiningJob currentJob_;
    std::vector<std::thread> miners_;
    std::atomic<bool> miningActive;
    int numThreads_;
    std::mutex currentJobMutex_;
    std::vector<uint64_t> threadHashCounts_;
    std::chrono::time_point<std::chrono::steady_clock> lastHashrateTime_;
    mutable std::mutex outputMutex_;
    std::unordered_map<int, std::chrono::steady_clock::time_point> pendingSubmits_;
};

} // namespace nerdminer