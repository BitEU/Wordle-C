// Embedded wordlist data using Bloom filter for validation
// and direct array for solutions
// Inspired by Unix spell's memory-efficient techniques

#ifndef WORDLIST_H
#define WORDLIST_H

#include <stdint.h>

// Bloom filter size in bits (must be power of 2)
// 32KB gives very low false positive rate for ~3000 words
#define BLOOM_SIZE_BITS (32 * 1024 * 8)
#define BLOOM_SIZE_BYTES (BLOOM_SIZE_BITS / 8)

// Number of hash functions for Bloom filter
#define BLOOM_K 7

// Word encoding: 5 letters -> 25 bits (5 bits per letter, a=0..z=25)
// This allows efficient storage and comparison

// Bloom filter bit array (BSS - zero-initialized)
extern uint8_t bloom_filter[BLOOM_SIZE_BYTES];

// Solution words stored as packed 32-bit integers (25 bits used)
extern const uint32_t solutions[];
extern const int solution_count;

// Initialize bloom filter with all valid words
void bloom_init(void);

// Check if word exists in bloom filter (may have false positives)
int bloom_check(const char *word);

// Get solution word at index
void get_solution(int index, char *out);

// Encode 5-letter word to 25-bit integer
uint32_t encode_word(const char *word);

// Decode 25-bit integer to 5-letter word
void decode_word(uint32_t encoded, char *out);

#endif
