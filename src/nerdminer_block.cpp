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

#include <nerdminer/nerdminer_block.h>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>


namespace nerdminer {

    std::string buildCoinbaseTransaction(const std::string& coinb1, const std::string& extranonce, const std::string& coinb2) {
        return coinb1 + extranonce + coinb2;
    }

    std::string calculateMerkleRoot(const std::string& coinbaseTransaction, const std::vector<std::string>& merkleBranches) {
        auto doubleSHA256 = [](const std::string& hex) {
            std::vector<uint8_t> bytes;
            for (size_t i = 0; i < hex.length(); i += 2) {
                bytes.push_back(std::stoi(hex.substr(i, 2), nullptr, 16));
            }
            uint8_t hash1[SHA256_DIGEST_LENGTH];
            SHA256(bytes.data(), bytes.size(), hash1);
            uint8_t hash2[SHA256_DIGEST_LENGTH];
            SHA256(hash1, SHA256_DIGEST_LENGTH, hash2);
            std::ostringstream oss;
            for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
                oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash2[i];
            }
            return oss.str();
        };
    
        std::string merkle = doubleSHA256(coinbaseTransaction);
        for (const auto& branch : merkleBranches) {
            merkle = doubleSHA256(merkle + branch);
        }
        return merkle;
    }

    std::vector<uint8_t> buildBlockHeader(const BlockHeader& header) {
        auto hexToBytes = [](const std::string& hex) {
            std::vector<uint8_t> bytes;
            for (size_t i = 0; i < hex.length(); i += 2) {
                bytes.push_back(std::stoi(hex.substr(i, 2), nullptr, 16));
            }
            return bytes;
        };
    
        std::vector<uint8_t> block;
    
        // Adiciona campos do header
        for (int i = 0; i < 4; ++i) block.push_back((header.version >> (i * 8)) & 0xFF);
    
        auto prevHashBytes = hexToBytes(header.prevHash);
        block.insert(block.end(), prevHashBytes.begin(), prevHashBytes.end());
    
        auto merkleRootBytes = hexToBytes(header.merkleRoot);
        block.insert(block.end(), merkleRootBytes.begin(), merkleRootBytes.end());
    
        for (int i = 0; i < 4; ++i) block.push_back((header.timestamp >> (i * 8)) & 0xFF);
        for (int i = 0; i < 4; ++i) block.push_back((header.bits >> (i * 8)) & 0xFF);
        for (int i = 0; i < 4; ++i) block.push_back((header.nonce >> (i * 8)) & 0xFF);
    
        return block;
    }

}