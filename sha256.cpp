/** @file
 * @brief Implementation of the SHA256 Algorithm
 */

#include "sha256.h"
#include <string>
#include <string.h>
#include <iomanip>
#include <sstream>

typedef uint32_t uint32;

/**
 * SHA256 class constructor
 */
SHA256::SHA256() {
}

/**
 * SHA256 class destructor
 */
SHA256::~SHA256() {
}

uint32 SHA256::hPrime[8] = {static_cast<unsigned int>(0xc6a09e667u),
                            static_cast<unsigned int>(0xbb67ae85u),
                            static_cast<unsigned int>(0x3c6ef372u),
                            static_cast<unsigned int>(0xa54ff53au),
                            static_cast<unsigned int>(0x510e527fu),
                            static_cast<unsigned int>(0x9b05688cu),
                            static_cast<unsigned int>(0x1f83d9abu),
                            static_cast<unsigned int>(0x5be0cd19u)
};

uint32 SHA256::k[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

/**
 * Returns a message digest using the SHA256 algorithm
 * @param input message string used as an input to the SHA256 algorithm, must be < size_t bits
 */
std::string SHA256::hash(const std::string input) {
    size_t nBuffer; // amount of message blocks
    uint32 **buffer; // message block buffers (each 512-bit = 16 32-bit words)
    uint32 *h = new uint32[HASH_LEN]; // buffer holding the message digest (256-bit = 8 32-bit words)

    buffer = preprocess((unsigned char *) input.c_str(), nBuffer);
    process(buffer, nBuffer, h);

    freeBuffer(buffer, nBuffer);
    return digest(h);
}

/**
 * Preprocessing of the SHA256 algorithm
 * @param input message in byte representation
 * @param nBuffer amount of message blocks
 */
uint32 **SHA256::preprocess(const unsigned char *input, size_t &nBuffer) {
    // Padding: input || 1 || 0*k || l (in 64-bit representation)
    size_t mLen = strlen((const char *) input);
    size_t l = mLen * CHAR_LEN_BITS; // length of input in bits
    size_t k = (448 - 1 - l) % MESSAGE_BLOCK_SIZE; // length of zero bit padding (l + 1 + k = 448 mod 512)
    nBuffer = (l + 1 + k + 64) / MESSAGE_BLOCK_SIZE;

    uint32 **buffer = new uint32 *[nBuffer];

    for (size_t i = 0; i < nBuffer; i++) {
        buffer[i] = new uint32[SEQUENCE_LEN];
    }

    uint32 in;
    size_t index;

    // Either copy existing message, add 1 bit or add 0 bit
    for (size_t i = 0; i < nBuffer; i++) {
        for (size_t j = 0; j < SEQUENCE_LEN; j++) {
            in = static_cast<unsigned int>(0x00u);
            for (size_t k = 0; k < WORD_LEN; k++) {
                index = i * 64 + j * 4 + k;
                if (index < mLen) {
                    in = in << 8 | static_cast<unsigned int>(input[index]);
                } else if (index == mLen) {
                    in = in << 8 | static_cast<unsigned int>(0x80u);
                } else {
                    in = in << 8 | static_cast<unsigned int>(0x00u);
                }
            }
            buffer[i][j] = in;
        }
    }

    // Append the length to the last two 32-bit blocks
    appendLen(l, buffer[nBuffer - 1][SEQUENCE_LEN - 1], buffer[nBuffer - 1][SEQUENCE_LEN - 2]);
    return buffer;
}

/**
 * Processing of the SHA256 algorithm
 * @param buffer array holding the preprocessed
 * @param nBuffer amount of message blocks
 * @param h array of output message digest
 */
void SHA256::process(uint32 **buffer, size_t nBuffer, uint32 *h) {
    uint32 s[WORKING_VAR_LEN];
    uint32 w[MESSAGE_SCHEDULE_LEN];

    memcpy(h, hPrime, WORKING_VAR_LEN * sizeof(uint32));

    for (size_t i = 0; i < nBuffer; i++) {
        // copy over to message schedule
        memcpy(w, buffer[i], SEQUENCE_LEN * sizeof(uint32));

        // Prepare the message schedule
        for (size_t j = 16; j < MESSAGE_SCHEDULE_LEN; j++) {
            w[j] = w[j - 16] + sig0_s(w[j - 15]) + w[j - 7] + sig1_s(w[j - 2]);
        }
        // Initialize the working variables
        memcpy(s, h, WORKING_VAR_LEN * sizeof(uint32));

        // Compression
        for (size_t j = 0; j < MESSAGE_SCHEDULE_LEN; j++) {
            uint32 temp1 = s[7] + Sig1_s(s[4]) + Ch_s(s[4], s[5], s[6]) + k[j] + w[j];
            uint32 temp2 = Sig0_s(s[0]) + Maj_s(s[0], s[1], s[2]);

            s[7] = s[6];
            s[6] = s[5];
            s[5] = s[4];
            s[4] = s[3] + temp1;
            s[3] = s[2];
            s[2] = s[1];
            s[1] = s[0];
            s[0] = temp1 + temp2;
        }

        // Compute the intermediate hash values
        for (size_t j = 0; j < WORKING_VAR_LEN; j++) {
            h[j] += s[j];
        }
    }

}

/**
 * Appends the length of the message in the last two message blocks
 * @param l message size in bits
 * @param lo pointer to second last message block
 * @param hi pointer to last message block
 */
void SHA256::appendLen(size_t l, uint32 &lo, uint32 &hi) {
    lo = l;
    hi = 0x00;
}

/**
 * Outputs the final message digest in hex representation
 * @param h array of output message digest
 */
std::string SHA256::digest(uint32 *h) {
    std::stringstream ss;
    for (size_t i = 0; i < OUTPUT_LEN; i++) {
        ss << std::hex << std::setw(8) << std::setfill('0') << h[i];
    }
    delete[] h;
    return ss.str();
}

/**
 * Free the buffer correctly
 * @param buffer array holding the preprocessed
 * @param nBuffer amount of message blocks
 */
void SHA256::freeBuffer(uint32 **buffer, size_t nBuffer) {
    for (size_t i = 0; i < nBuffer; i++) {
        delete[] buffer[i];
    }

    delete[] buffer;
}