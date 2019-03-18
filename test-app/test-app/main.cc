/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include <algorithm>
#include <climits>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

template <typename T> T rotl (T v, unsigned int b)
{
        static_assert (std::is_integral<T>::value, "rotate of non-integral type");
        static_assert (!std::is_signed<T>::value, "rotate of signed type");
        constexpr unsigned int num_bits{ std::numeric_limits<T>::digits };
        static_assert (0 == (num_bits & (num_bits - 1)), "rotate value bit length not power of two");
        constexpr unsigned int count_mask{ num_bits - 1 };
        const unsigned int mb{ b & count_mask };
        using promoted_type = typename std::common_type<int, T>::type;
        using unsigned_promoted_type = typename std::make_unsigned<promoted_type>::type;
        return ((unsigned_promoted_type{ v } << mb) | (unsigned_promoted_type{ v } >> (-mb & count_mask)));
}

static inline uint32_t rotl32 (uint32_t n, unsigned int c)
{
        const unsigned int mask = (CHAR_BIT * sizeof (n) - 1); // assumes width is a power of 2.

        // assert ( (c<=mask) &&"rotate by type width or more");
        c &= mask;
        return (n << c) | (n >> ((-c) & mask));
}

static inline uint32_t rotr32 (uint32_t n, unsigned int c)
{
        const unsigned int mask = (CHAR_BIT * sizeof (n) - 1);

        // assert ( (c<=mask) &&"rotate by type width or more");
        c &= mask;
        return (n >> c) | (n << ((-c) & mask));
}

/*
 * i, j positions of bit sequences to swap
 * b bits to swap reside in b
 * r bit-swapped result goes here
 */
uint8_t swapBits (uint8_t b, uint8_t i, uint8_t j)
{
        if (i > 7) {
                i = 0;
        }
        if (j > 7) {
                j = 0;
        }

        unsigned int n = 1;
        unsigned int x = ((b >> i) ^ (b >> j)) & ((1U << n) - 1); // XOR temporary
        return b ^ ((x << i) | (x << j));
}

uint8_t swapConsecutiveBits (uint8_t b, uint8_t i) { return swapBits (b, i, i + 1); }

uint8_t rearange (uint8_t c, uint8_t num)
{
        // 7 6 5 4 3 2 1 0   00000000

        // 6 7 5 4 3 2 1 0   10000000
        // 6 5 7 4 3 2 1 0   01
        // 6 5 4 7 3 2 1 0   001
        // 6 5 4 3 7 2 1 0   0001
        // 6 5 4 3 2 7 1 0   00001
        // 6 5 4 3 2 1 7 0   000001
        // 6 5 4 3 2 1 0 7   00000010
        // 7 6 5 4 3 2 1 0   000000001

        if (num & 0b00000001) {
        }

        0b00000001;
}

unsigned char reverse (unsigned char b)
{
        b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
        b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
        b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
        return b;
}

int main ()
{

        std::ifstream file ("/home/iwasz/workspace/camera-h7/build-camera-h7-stm32h743xx-Debug/data.dat", std::ios::binary | std::ios::ate);
        std::streamsize size = file.tellg ();
        file.seekg (0, std::ios::beg);

        std::vector<char> buffer (size);

        if (!file.read (buffer.data (), size)) {
                throw std::exception ();
        }

        int i = 0;
        for (char c : buffer) {
                //                std::cout << rotl<uint8_t> (reverse (c), 7);

                uint8_t b; // 5 6 5

                if (i % 2 == 0) {
                        b = 0b00000111;
                }
                else {
                        b = 0b11100000;
                }

                std::cout << b;
                ++i;
        }

        return 0;
}
