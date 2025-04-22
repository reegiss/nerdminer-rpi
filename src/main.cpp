/**
 * Project: nerdminer-rpi
 * File: main.cpp
 * Description: Main file of project,
 *
 * Author: Regis Araujo Melo
 * Date: YYYY-MM-DD
 * Version: 0.1.0
 *
 * MIT License
 * © 2025 Regis Araujo Melo
 */

#include <iostream>
#include <string>
#include "nerdminer/version.h"
#include "nerdminer/stratum_client.h"
#include "nerdminer/miner_session.h"

class NerdMinerApp {
public:
    bool run(int argc, char** argv) {
        if (argc > 1) {
            std::string arg = argv[1];
            if (arg == "--help" || arg == "-h") {
                printHelp();
                return true;
            } else {
                std::cerr << "Error: unknown argument '" << arg << "'\n\n";
                printHelp();
                return false;
            }
        }

        printBanner();
        startSession();
        
        return true;
    }

private:
    const std::string host = "public-pool.io";
    const uint16_t port = 21496;
    const std::string user = "bc1qcdlauj9j9jnxcdlxqkrrus40p7cp9ph6ermkfz.raspberrypi";
    const std::string password = "x";

    void printBanner() const {
        std::cout << "====================================\n";
        std::cout << "      " << nerdminer::PROJECT_NAME << " - v" << nerdminer::PROJECT_VERSION << "\n";
        std::cout << "      " << nerdminer::PROJECT_PLATFORM << "\n";
        std::cout << "====================================\n\n";
    }

    void printHelp() const {
        std::cout << "Usage: " << nerdminer::PROJECT_NAME << " [options]\n\n"
                    << "Options:\n"
                    << "  -h, --help        Show this help message and exit\n"
                    << "\n";
    }

    void startSession() {
        std::cout << "Starting miner session...\n";
        nerdminer::MinerSession session(host, port, user, password);
        session.start();
    }
};

int main(int argc, char* argv[]) {
    NerdMinerApp app;
    if (!app.run(argc, argv)) {
        return 1;
    }
    return 0;
}