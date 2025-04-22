/**
* Project: nerdminer-rpi
* File: miner_job.h
* Description: header file for the miner job
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
#include <nlohmann/json.hpp>

namespace nerdminer {

using json = nlohmann::json; // define um alias para nlohmann::json

struct MiningJob {
    std::string jobId;
    std::string prevHash;
    std::string coinbase1;
    std::string coinbase2;
    std::vector<std::string> merkleBranches;
    std::string version;
    // uint32_t version;
    std::string nBits;
    std::string nTime;
    bool cleanJobs;
    bool valid = false;

    static MiningJob fromNotification(const json& note);
};

} // namespace nerdminer