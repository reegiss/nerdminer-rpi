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
#include <openssl/evp.h>

namespace nerdminer {

    std::vector<uint8_t> hexStringToBytes(const std::string& hex) {
        std::vector<uint8_t> bytes;
        for (size_t i = 0; i < hex.length(); i += 2) {
            std::string byteString = hex.substr(i, 2);
            uint8_t byte = (uint8_t)strtol(byteString.c_str(), nullptr, 16);
            bytes.push_back(byte);
        }
        return bytes;
    }

    void appendInt32(std::vector<uint8_t>& result, uint32_t value) {
        result.push_back((value >> 24) & 0xFF);
        result.push_back((value >> 16) & 0xFF);
        result.push_back((value >> 8) & 0xFF);
        result.push_back(value & 0xFF);
    }

    std::string bytesToHex(const std::vector<uint8_t>& bytes) {
        std::stringstream ss;
        for (size_t i = 0; i < bytes.size(); ++i) {
            ss << std::setw(2) << std::setfill('0') << std::hex << (int)bytes[i];
        }
        return ss.str();
    }

/**
 * Realiza o duplo SHA256 em um vetor de dados.
 * @param data Dados de entrada para o cálculo do hash.
 * @return O resultado do duplo SHA256.
 */
std::vector<uint8_t> doubleSHA256(const std::vector<uint8_t>& data) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
    EVP_DigestUpdate(ctx, data.data(), data.size());
    unsigned char hash1[EVP_MAX_MD_SIZE];
    unsigned int len;
    EVP_DigestFinal_ex(ctx, hash1, &len);
    EVP_MD_CTX_free(ctx);

    // Fazendo o segundo SHA256
    std::vector<uint8_t> hash2(EVP_MAX_MD_SIZE);
    ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
    EVP_DigestUpdate(ctx, hash1, len);
    EVP_DigestFinal_ex(ctx, hash2.data(), &len);
    EVP_MD_CTX_free(ctx);

    // Redimensiona a resposta ao tamanho correto (32 bytes)
    hash2.resize(32);
    return hash2;
}

/**
 * Constrói a transação Coinbase.
 * @param coinb1 Parte 1 da transação.
 * @param extranonce O extranonce.
 * @param coinb2 Parte 2 da transação.
 * @return A transação Coinbase construída.
 */
std::string buildCoinbaseTransaction(const std::string& coinb1, const std::string& extranonce, const std::string& coinb2) {
    return coinb1 + extranonce + coinb2;
}

/**
 * Calcula a raiz Merkle de uma transação Coinbase e seus ramos Merkle.
 * @param coinbaseTransaction A transação Coinbase.
 * @param merkleBranches Os ramos Merkle.
 * @return A raiz Merkle calculada.
 */
std::string calculateMerkleRoot(const std::string& coinbaseTransaction, const std::vector<std::string>& merkleBranches) {
    std::vector<uint8_t> merkle = doubleSHA256(hexStringToBytes(coinbaseTransaction));

    for (const auto& branch : merkleBranches) {
        std::vector<uint8_t> branchBytes = hexStringToBytes(branch);
        merkle.insert(merkle.end(), branchBytes.begin(), branchBytes.end());
        merkle = doubleSHA256(merkle);
    }

    return bytesToHex(merkle);
}

/**
 * Constrói o cabeçalho do bloco a partir dos dados fornecidos.
 * @param header Cabeçalho do bloco.
 * @return O cabeçalho do bloco como um vetor de bytes.
 */
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

/**
 * Expande o alvo de mineração a partir dos bits fornecidos.
 * @param bits Os bits que representam o alvo.
 * @return O alvo expandido como um vetor de 32 bytes.
 */
std::vector<uint8_t> expandTarget(uint32_t bits) {
    std::vector<uint8_t> target(32, 0);

    uint32_t exponent = bits >> 24;
    uint32_t mantissa = bits & 0xFFFFFF;

    if (exponent <= 3) {
        mantissa >>= 8 * (3 - exponent);
        target[31] = mantissa & 0xFF;
        target[30] = (mantissa >> 8) & 0xFF;
        target[29] = (mantissa >> 16) & 0xFF;
    } else {
        int offset = 32 - exponent;
        target[offset] = (mantissa >> 16) & 0xFF;
        target[offset + 1] = (mantissa >> 8) & 0xFF;
        target[offset + 2] = mantissa & 0xFF;
    }

    return target;
}

/**
 * Verifica se o hash fornecido é menor que o alvo.
 * @param hash O hash gerado.
 * @param target O alvo a ser comparado.
 * @return True se o hash for menor que o alvo, caso contrário, false.
 */
bool checkHashMeetsTarget(const std::vector<uint8_t>& hash, const std::vector<uint8_t>& target) {
    for (size_t i = 0; i < 32; ++i) {
        if (hash[i] < target[i]) {
            return true;
        } else if (hash[i] > target[i]) {
            return false;
        }
    }
    return true; // Hash é igual ao alvo
}

/**
 * Converte os bits para o formato de alvo.
 * @param bits Os bits que representam o alvo.
 * @return O alvo como um vetor de 32 bytes.
 */
std::vector<uint8_t> targetFromBits(uint32_t bits) {
    uint32_t exponent = bits >> 24;
    uint32_t mantissa = bits & 0xFFFFFF;

    std::vector<uint8_t> target(32, 0);

    int shift = exponent - 3;

    if (shift >= 0 && shift <= 29) {
        target[shift] = (mantissa >> 16) & 0xFF;
        target[shift + 1] = (mantissa >> 8) & 0xFF;
        target[shift + 2] = mantissa & 0xFF;
    } else {
        // Expoente inválido, devolve alvo impossível (nunca será atingido)
        std::fill(target.begin(), target.end(), 0xFF);
    }

    return target;
}

/**
 * Verifica se o hash gerado é menor ou igual ao alvo.
 * @param hash O hash gerado.
 * @param target O alvo a ser comparado.
 * @return True se o hash for menor ou igual ao alvo, caso contrário, false.
 */
bool isHashBelowTarget(const std::vector<uint8_t>& hash, const std::vector<uint8_t>& target) {
    for (size_t i = 0; i < 32; ++i) {
        if (hash[i] < target[i]) {
            return true;
        } else if (hash[i] > target[i]) {
            return false;
        }
    }
    return true; // hash == target
}

} // namespace nerdminer
