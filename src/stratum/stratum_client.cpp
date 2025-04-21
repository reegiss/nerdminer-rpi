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

 #include "nerdminer/stratum_client.h"
 #include <string>
 #include <iostream>

 namespace nerdminer {

    StratumClient::StratumClient(Connection& connection)
    : conn_(connection) {}

    bool StratumClient::subscribe() {
        std::string subscribe_message = R"({"id": 1, "method": "mining.subscribe", "params": []})";
    
        std::cout << "Sending mining.subscribe request..." << std::endl;
    
        if (!conn_.send(subscribe_message)) {
            std::cerr << "Failed to send subscribe message." << std::endl;
            return false;
        }
    
        std::cout << "Subscribe request sent successfully." << std::endl;
        return true;
    }

 }