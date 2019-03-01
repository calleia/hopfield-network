/*
 * Fowler/Noll/Vo (FNV)
 *
 * Implementation of FNV-1a (32bits) hash algorithm
 * Based on the version 5.0 (updated 2012 May 20)
 * 
 * SOURCE: http://www.isthe.com/chongo/tech/comp/fnv/#FNV-reference-source
 */

#ifndef HASH_H_
#define HASH_H_

/*
 * 32 bit FNV-0 hash type
 */
typedef u_int32_t Fnv32_t;
typedef Fnv32_t Hash;

/*
 * 32 bit FNV-1 and FNV-1a non-zero initial basis
 *
 * The FNV-1 initial basis is the FNV-0 hash of the following 32 octets:
 *
 *              chongo <Landon Curt Noll> /\../\
 *
 * NOTE: The \'s above are not back-slashing escape characters.
 * They are literal ASCII backslash 0x5c characters.
 *
 * NOTE: The FNV-1a initial basis is the same value as FNV-1 by definition.
 */
#define FNV1_32_INIT ((Fnv32_t)0x811c9dc5)
#define FNV1_32A_INIT FNV1_32_INIT

/*
 * 32 bit magic FNV-1a prime
 */
#define FNV_32_PRIME ((Fnv32_t)0x01000193)

/*
 * print_hash - print an FNV hash
 *
 * given:
 *  hash    the hash value to print
 *  mask    lower bit mask
 *  verbose 1 => print args with hash
 *  args    string or filename args
 */
void print_hash(Hash hash, Hash mask, int verbose, char* args);

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
Hash get_hash(char* data, Hash hash);

#endif