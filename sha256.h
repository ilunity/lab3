/** @file
 * @brief Header file describing the function signatures and the constants of the SHA256 Algorithm
 */

#ifndef H_SHA256
#define H_SHA256

#include <string>
#include <stdint.h>

class SHA256{
private:
    typedef uint32_t uint32;

    static uint32 hPrime[];

    static uint32 k[];

    uint32** preprocess(const unsigned char* input, size_t& nBuffer);
    void appendLen(size_t l, uint32& lo, uint32& hi);
    void process(uint32** buffer, size_t nBuffer, uint32* h);
    std::string digest(uint32* h);
    void freeBuffer(uint32** buffer, size_t nBuffer);

    // Operations
#define Ch_s(x,y,z) ((x&y)^(~x&z))
#define Maj_s(x,y,z) ((x&y)^(x&z)^(y&z))
#define RotR_s(x, n) ((x>>n)|(x<<((sizeof(x)<<3)-n)))
#define Sig0_s(x) ((RotR_s(x, 2))^(RotR_s(x,13))^(RotR_s(x, 22)))
#define Sig1_s(x) ((RotR_s(x, 6))^(RotR_s(x,11))^(RotR_s(x, 25)))
#define sig0_s(x) (RotR_s(x, 7)^RotR_s(x,18)^(x>>3))
#define sig1_s(x) (RotR_s(x, 17)^RotR_s(x,19)^(x>>10))

    // Constants
    unsigned int const SEQUENCE_LEN = (512/32);
    size_t const HASH_LEN = 8;
    size_t const WORKING_VAR_LEN = 8;
    size_t const MESSAGE_SCHEDULE_LEN = 64;
    size_t const MESSAGE_BLOCK_SIZE = 512;
    size_t const CHAR_LEN_BITS = 8;
    size_t const OUTPUT_LEN = 8;
    size_t const WORD_LEN = 4;

public:
    std::string hash(const std::string input);

    SHA256();
    ~SHA256();
};



#endif