// Generator tool to create embedded wordlist data from text files
// Run this once to generate wordlist_data.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#define WORD_LENGTH 5
#define BLOOM_SIZE_BITS (32 * 1024 * 8)
#define BLOOM_SIZE_BYTES (BLOOM_SIZE_BITS / 8)
#define BLOOM_K 7

// Bloom filter bit array
static uint8_t bloom[BLOOM_SIZE_BYTES] = {0};

// Encode word to 25-bit integer
uint32_t encode_word(const char *word) {
    uint32_t result = 0;
    for (int i = 0; i < WORD_LENGTH; i++) {
        char c = tolower(word[i]);
        result = (result << 5) | (c - 'a');
    }
    return result;
}

// FNV-1a hash
static uint32_t fnv1a(const char *word, uint32_t seed) {
    uint32_t hash = 2166136261u ^ seed;
    for (int i = 0; i < WORD_LENGTH; i++) {
        hash ^= (uint8_t)tolower(word[i]);
        hash *= 16777619u;
    }
    return hash;
}

// Set bit in bloom filter
static void bloom_set(const char *word) {
    for (int i = 0; i < BLOOM_K; i++) {
        uint32_t hash = fnv1a(word, i * 0x9E3779B9);
        uint32_t bit = hash % BLOOM_SIZE_BITS;
        bloom[bit / 8] |= (1 << (bit % 8));
    }
}

int main() {
    FILE *fall, *fsol, *out;
    char word[WORD_LENGTH + 4];
    uint32_t *solutions = NULL;
    int sol_count = 0;
    int sol_capacity = 4096;
    int all_count = 0;

    // Allocate solution array
    solutions = malloc(sol_capacity * sizeof(uint32_t));
    if (!solutions) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // Read ALL.TXT and build bloom filter
    fall = fopen("LISTS\\ALL.TXT", "r");
    if (!fall) {
        fall = fopen("lists/ALL.TXT", "r");
    }
    if (!fall) {
        fprintf(stderr, "Cannot open ALL.TXT\n");
        return 1;
    }

    while (fgets(word, sizeof(word), fall)) {
        // Strip newline and check length
        int len = strlen(word);
        while (len > 0 && (word[len-1] == '\n' || word[len-1] == '\r')) {
            word[--len] = '\0';
        }
        if (len == WORD_LENGTH) {
            bloom_set(word);
            all_count++;
        }
    }
    fclose(fall);
    printf("Processed %d words for bloom filter\n", all_count);

    // Read SOLUTION.TXT and store encoded words
    fsol = fopen("LISTS\\SOLUTION.TXT", "r");
    if (!fsol) {
        fsol = fopen("lists/SOLUTION.TXT", "r");
    }
    if (!fsol) {
        fprintf(stderr, "Cannot open SOLUTION.TXT\n");
        return 1;
    }

    while (fgets(word, sizeof(word), fsol)) {
        int len = strlen(word);
        while (len > 0 && (word[len-1] == '\n' || word[len-1] == '\r')) {
            word[--len] = '\0';
        }
        if (len == WORD_LENGTH) {
            if (sol_count >= sol_capacity) {
                sol_capacity *= 2;
                solutions = realloc(solutions, sol_capacity * sizeof(uint32_t));
            }
            solutions[sol_count++] = encode_word(word);
        }
    }
    fclose(fsol);
    printf("Processed %d solution words\n", sol_count);

    // Generate output file
    out = fopen("wordlist_data.c", "w");
    if (!out) {
        fprintf(stderr, "Cannot create wordlist_data.c\n");
        return 1;
    }

    fprintf(out, "// Auto-generated wordlist data - do not edit\n");
    fprintf(out, "// Generated from ALL.TXT (%d words) and SOLUTION.TXT (%d words)\n\n", all_count, sol_count);
    fprintf(out, "#include \"wordlist.h\"\n");
    fprintf(out, "#include <string.h>\n");
    fprintf(out, "#include <ctype.h>\n\n");

    // Output bloom filter data
    fprintf(out, "// Bloom filter data (%d bytes)\n", BLOOM_SIZE_BYTES);
    fprintf(out, "static const uint8_t bloom_data[%d] = {\n    ", BLOOM_SIZE_BYTES);
    int bits_set = 0;
    for (int i = 0; i < BLOOM_SIZE_BYTES; i++) {
        fprintf(out, "0x%02X", bloom[i]);
        if (i < BLOOM_SIZE_BYTES - 1) {
            fprintf(out, ",");
            if ((i + 1) % 16 == 0) {
                fprintf(out, "\n    ");
            }
        }
        for (int b = 0; b < 8; b++) {
            if (bloom[i] & (1 << b)) bits_set++;
        }
    }
    fprintf(out, "\n};\n\n");
    printf("Bloom filter: %d/%d bits set (%.1f%%)\n", bits_set, BLOOM_SIZE_BITS, 100.0 * bits_set / BLOOM_SIZE_BITS);

    // Output BSS bloom filter array
    fprintf(out, "// BSS bloom filter (copied from data on init)\n");
    fprintf(out, "uint8_t bloom_filter[BLOOM_SIZE_BYTES];\n\n");

    // Output solution words
    fprintf(out, "// Solution words encoded as 25-bit integers\n");
    fprintf(out, "const int solution_count = %d;\n", sol_count);
    fprintf(out, "const uint32_t solutions[%d] = {\n    ", sol_count);
    for (int i = 0; i < sol_count; i++) {
        fprintf(out, "0x%07X", solutions[i]);
        if (i < sol_count - 1) {
            fprintf(out, ",");
            if ((i + 1) % 8 == 0) {
                fprintf(out, "\n    ");
            }
        }
    }
    fprintf(out, "\n};\n\n");

    // Output helper functions
    fprintf(out, "// FNV-1a hash function\n");
    fprintf(out, "static uint32_t fnv1a(const char *word, uint32_t seed) {\n");
    fprintf(out, "    uint32_t hash = 2166136261u ^ seed;\n");
    fprintf(out, "    for (int i = 0; i < 5; i++) {\n");
    fprintf(out, "        hash ^= (uint8_t)tolower(word[i]);\n");
    fprintf(out, "        hash *= 16777619u;\n");
    fprintf(out, "    }\n");
    fprintf(out, "    return hash;\n");
    fprintf(out, "}\n\n");

    fprintf(out, "void bloom_init(void) {\n");
    fprintf(out, "    memcpy(bloom_filter, bloom_data, BLOOM_SIZE_BYTES);\n");
    fprintf(out, "}\n\n");

    fprintf(out, "int bloom_check(const char *word) {\n");
    fprintf(out, "    for (int i = 0; i < %d; i++) {\n", BLOOM_K);
    fprintf(out, "        uint32_t hash = fnv1a(word, i * 0x9E3779B9);\n");
    fprintf(out, "        uint32_t bit = hash %% %d;\n", BLOOM_SIZE_BITS);
    fprintf(out, "        if (!(bloom_filter[bit / 8] & (1 << (bit %% 8)))) {\n");
    fprintf(out, "            return 0;\n");
    fprintf(out, "        }\n");
    fprintf(out, "    }\n");
    fprintf(out, "    return 1;\n");
    fprintf(out, "}\n\n");

    fprintf(out, "uint32_t encode_word(const char *word) {\n");
    fprintf(out, "    uint32_t result = 0;\n");
    fprintf(out, "    for (int i = 0; i < 5; i++) {\n");
    fprintf(out, "        result = (result << 5) | (tolower(word[i]) - 'a');\n");
    fprintf(out, "    }\n");
    fprintf(out, "    return result;\n");
    fprintf(out, "}\n\n");

    fprintf(out, "void decode_word(uint32_t encoded, char *out) {\n");
    fprintf(out, "    for (int i = 4; i >= 0; i--) {\n");
    fprintf(out, "        out[i] = 'a' + (encoded & 0x1F);\n");
    fprintf(out, "        encoded >>= 5;\n");
    fprintf(out, "    }\n");
    fprintf(out, "    out[5] = '\\0';\n");
    fprintf(out, "}\n\n");

    fprintf(out, "void get_solution(int index, char *out) {\n");
    fprintf(out, "    if (index >= 0 && index < solution_count) {\n");
    fprintf(out, "        decode_word(solutions[index], out);\n");
    fprintf(out, "    }\n");
    fprintf(out, "}\n");

    fclose(out);
    free(solutions);

    printf("Generated wordlist_data.c\n");
    printf("Memory usage:\n");
    printf("  Bloom filter: %d bytes\n", BLOOM_SIZE_BYTES);
    printf("  Solutions: %d bytes\n", sol_count * 4);
    printf("  Total: %d bytes\n", BLOOM_SIZE_BYTES + sol_count * 4);

    return 0;
}
