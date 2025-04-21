/**
 * Project: nerdminer-rpi
 * File: stratum_client.h
 * Description: header file for the Stratum client
 *
 * Author: Regis Araujo Melo
 * Date: 2025-04-20
 * Version: 0.1.0
 *
 * MIT License
 * © 2025 Regis Araujo Melo
 */

 #pragma once

 #include "network/connection.h"

 namespace nerdminer {

class StratumClient {
    public:
        explicit StratumClient(Connection& connection);

        bool subscribe();

    private:
        Connection& conn_;
};

} // namespace nerdminer