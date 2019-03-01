/*
 * Fowler/Noll/Vo (FNV)
 *
 * Implementation of FNV-1a (32bits) hash algorithm
 * Based on the version 5.0 (updated 2012 May 20)
 * 
 * SOURCE: http://www.isthe.com/chongo/tech/comp/fnv/#FNV-reference-source
 */

#include <stdio.h>
#include "hash.h"

/*
 * print_hash - print an FNV hash
 *
 * given:
 *  hash    the hash value to print
 *  mask    lower bit mask
 *  verbose 1 => print args with hash
 *  args    string or filename args
 */
void print_hash(Hash hash, Hash mask, int verbose, char* args) {

    if (verbose) {
        printf("0x%08lx %s\n", hash & mask, args);
    } else {
        printf("0x%08lx\n", hash & mask);
    }
}

/*
 * get_hash - perform a 32 bit Fowler/Noll/Vo (FNV-1a) hash on a string
 *
 * input:
 *  data    string to hash
 *  hash    previous hash value or 0 if first call
 *
 * returns:
 *  32 bit hash as a static hash type
 *
 * NOTE: To use the recommended 32 bit FNV-1a hash, use FNV1_32A_INIT as the
 *  	 hash arg on the first call to get_hash().
 */
Hash get_hash(char* data, Hash hash) {
    /* unsigned string */
    unsigned char* str = (unsigned char*) data;

    /*
     * FNV-1a hash each octet in the buffer
     */
    while (*str) {
        /* xor the bottom with the current octet */
        hash ^= (Hash) *str++;

        /* multiply by the 32 bit FNV magic prime mod 2^32 */
        #if defined(NO_FNV_GCC_OPTIMIZATION)
            hash *= FNV_32_PRIME;
        #else
            hash += (hash<<1) + (hash<<4) + (hash<<7) + (hash<<8) + (hash<<24);
        #endif
    }

    /* return the new hash value */
    return hash;
}

