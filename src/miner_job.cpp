/**
* Project: nerdminer-rpi
* File: miner_job.cpp
* Description: implementation of the miner job
*
* Author: Regis Araujo Melo
* Date: 2025-04-21
* Version: 0.1.0
*
* MIT License
* © 2025 Regis Araujo Melo
*/

/**
* Project: nerdminer-rpi
* File: miner_job.cpp
* Description: implementation of the miner job
*
* Author: Regis Araujo Melo
* Date: 2025-04-21
* Version: 0.1.0
*
* MIT License
* © 2025 Regis Araujo Melo
*/

#include "nerdminer/miner_job.h"
#include <iostream>
#include <nlohmann/json.hpp> // Certifique-se de incluir o cabeçalho correto

namespace nerdminer {

MiningJob MiningJob::fromNotification(const json& note) {
    MiningJob job;

    // Verifica se o campo 'method' existe e é 'mining.notify'
    if (note.contains("method") && note["method"] == "mining.notify") {
        auto params = note["params"];

        // Verifica se 'params' é um array e tem o número esperado de elementos
        if (params.is_array() && params.size() >= 9) {
            try {
                job.jobId = params[0].get<std::string>();
                job.prevHash = params[1].get<std::string>();
                job.coinbase1 = params[2].get<std::string>();
                job.coinbase2 = params[3].get<std::string>();
                job.merkleBranches.clear();
                for (const auto& branch : params[4]) {
                    job.merkleBranches.push_back(branch.get<std::string>());
                }
                job.version = params[5].get<std::string>();
                job.versionInt = std::stoul(job.version, nullptr, 16);
                job.nBits = params[6].get<std::string>();
                job.bits = std::stoul(job.nBits, nullptr, 16);
                job.nTime = params[7].get<std::string>();
                job.ntime = std::stoul(job.nTime, nullptr, 16);

                // Verifica se o último campo é um booleano
                if (params[8].is_boolean()) {
                    job.cleanJobs = params[8].get<bool>();
                } else {
                    std::cerr << "Error: 'cleanJobs' field is not a boolean." << std::endl;
                    job.valid = false;
                    return job;
                }

                job.valid = true; // Se tudo deu certo, o trabalho é válido
                std::cout << "New mining job received: " << job.jobId << "\n";
            } catch (const json::type_error& e) {
                std::cerr << "Failed to parse mining.notify: Type error - " << e.what() << std::endl;
                job.valid = false;
            } catch (const std::exception& e) {
                std::cerr << "Failed to parse mining.notify: General error - " << e.what() << std::endl;
                job.valid = false;
            }
        } else {
            std::cerr << "Error: 'params' field is not a valid array or has insufficient elements." << std::endl;
            job.valid = false;
        }
    } else {
        std::cerr << "Error: Invalid method in notification. Expected 'mining.notify'." << std::endl;
        job.valid = false;
    }

    return job;
}

} // namespace nerdminer