/**
* Project: nerdminer-rpi
* File: nerdminer_block.cpp
* Description: implementation of the NerdMiner block
*
* Author: Regis Araujo Melo
* Date: 2025-04-21
* Version: 0.1.0
*
* MIT License
* © 2025 Regis Araujo Melo
*/

#include "nerdminer/nerdminer_block.h"
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <atomic>
#include <thread>
#include <iostream>

namespace nerdminer {

// Funções auxiliares internas
namespace {
    std::vector<uint8_t> hexStringToBytes(const std::string& hex) {
        std::vector<uint8_t> bytes;
        for (size_t i = 0; i < hex.length(); i += 2) {
            uint8_t byte = (uint8_t) strtol(hex.substr(i, 2).c_str(), nullptr, 16);
            bytes.push_back(byte);
        }
        return bytes;
    }

    std::string bytesToHex(const std::vector<uint8_t>& bytes) {
        std::ostringstream oss;
        for (auto b : bytes) {
            oss << std::hex << std::setw(2) << std::setfill('0') << (int)b;
        }
        return oss.str();
    }

    void appendInt32(std::vector<uint8_t>& buffer, uint32_t value) {
        buffer.push_back(value & 0xFF);
        buffer.push_back((value >> 8) & 0xFF);
        buffer.push_back((value >> 16) & 0xFF);
        buffer.push_back((value >> 24) & 0xFF);
    }

    std::vector<uint8_t> doubleSHA256(const std::vector<uint8_t>& data) {
        uint8_t hash1[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;

        SHA256_Init(&sha256);
        SHA256_Update(&sha256, data.data(), data.size());
        SHA256_Final(hash1, &sha256);

        uint8_t hash2[SHA256_DIGEST_LENGTH];
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, hash1, SHA256_DIGEST_LENGTH);
        SHA256_Final(hash2, &sha256);

        return std::vector<uint8_t>(hash2, hash2 + SHA256_DIGEST_LENGTH);
    }
}

// Implementações públicas

std::string buildCoinbaseTransaction(const std::string& coinb1, const std::string& extranonce, const std::string& coinb2) {
    return coinb1 + extranonce + coinb2;
}

std::string calculateMerkleRoot(const std::string& coinbaseTransaction, const std::vector<std::string>& merkleBranches) {
    std::vector<uint8_t> merkle = doubleSHA256(hexStringToBytes(coinbaseTransaction));

    for (const auto& branch : merkleBranches) {
        std::vector<uint8_t> branchBytes = hexStringToBytes(branch);
        merkle.insert(merkle.end(), branchBytes.begin(), branchBytes.end());
        merkle = doubleSHA256(merkle);
    }

    return bytesToHex(merkle);
}

std::vector<uint8_t> buildBlockHeader(const BlockHeader& header) {
    std::vector<uint8_t> result;

    appendInt32(result, header.version);
    result.insert(result.end(), hexStringToBytes(header.prevHash).begin(), hexStringToBytes(header.prevHash).end());
    result.insert(result.end(), hexStringToBytes(header.merkleRoot).begin(), hexStringToBytes(header.merkleRoot).end());
    appendInt32(result, header.timestamp);
    appendInt32(result, header.bits);
    appendInt32(result, header.nonce);

    return result;
}

} // namespace nerdminer
