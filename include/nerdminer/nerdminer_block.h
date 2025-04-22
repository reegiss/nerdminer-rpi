/**
* Project: nerdminer-rpi
* File: nerdminer_block.h
* Description: header file for the NerdMiner block
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
#include <cstdint>

namespace nerdminer {

    struct BlockHeader {
        uint32_t version;
        std::string prevHash;    // 32 bytes, little-endian
        std::string merkleRoot;  // 32 bytes, little-endian
        uint32_t timestamp;
        uint32_t bits;
        uint32_t nonce;
    };

    std::vector<uint8_t> doubleSHA256(const std::vector<uint8_t>& data);
    std::vector<uint8_t> buildBlockHeader(const BlockHeader& header);
    std::vector<uint8_t> targetFromBits(uint32_t bits);
    bool isHashBelowTarget(const std::vector<uint8_t>& hash, const std::vector<uint8_t>& target);
    std::string bytesToHex(const std::vector<uint8_t>& bytes);
    std::string buildCoinbaseTransaction(const std::string& coinb1, const std::string& extranonce, const std::string& coinb2);
    std::string calculateMerkleRoot(const std::string& coinbaseTransaction, const std::vector<std::string>& merkleBranches);
    std::vector<uint8_t> buildBlockHeader(const BlockHeader& header);

} // namespace nerdminer