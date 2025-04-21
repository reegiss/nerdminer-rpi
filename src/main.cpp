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
 #include <nerdminer/version.h>
 #include <nerdminer/network/connection.h>

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
        printStartConnection();
        
        nerdminer::Connection connection("public-pool.io", 21496);
        if (!connection.connect()) {
            std::cerr << "Failed to connect to mining pool." << std::endl;
            return false;
        }
        std::cout << "Successfully connected to mining pool." << std::endl;

        connection.disconnect();
        std::cout << "Disconnected." << std::endl;
        return true;
    }

private:
    void printBanner() const {
        std::cout << "====================================\n";
        std::cout << "      " << nerdminer::PROJECT_NAME << " - v" << nerdminer::PROJECT_VERSION << "\n";
        std::cout << "      Raspberry Pi 4 Bitcoin Miner\n";
        std::cout << "====================================\n\n";
    }

    void printStartConnection() const {
        std::cout << "Connecting to mining pool...\n";
    }

    void printHelp() const {
        std::cout << "Usage: " << nerdminer::PROJECT_NAME << " [options]\n\n"
                    << "Options:\n"
                    << "  -h, --help        Show this help message and exit\n"
                    << "\n";
    }
};

int main(int argc, char* argv[]) {
    NerdMinerApp app;
    if (!app.run(argc, argv)) {
        return 1;
    }
    return 0;
}