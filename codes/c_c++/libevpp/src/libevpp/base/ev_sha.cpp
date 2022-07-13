/*
  100% free public domain implementation of the SHA-1 algorithm
  by Dominik Reichl <dominik.reichl@t-online.de>
  Web: http://www.dominik-reichl.de/

  ================ Test Vectors ================

  SHA1("abc" in ANSI) =
    A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D

  SHA1("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq" in ANSI) =
    84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1

  SHA1(A million repetitions of "a" in ANSI) =
    34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
*/

/*
 * Copyright 2006 Apple Computer, Inc.  All rights reserved.
 * 
 * iTunes U Sample Code License
 * IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc. ("Apple") 
 * in consideration of your agreement to the following terms, and your use, 
 * installation, modification or distribution of this Apple software constitutes 
 * acceptance of these terms.  If you do not agree with these terms, please do not use, 
 * install, modify or distribute this Apple software.
 * 
 * In consideration of your agreement to abide by the following terms and subject to
 * these terms, Apple grants you a personal, non-exclusive, non-transferable license, 
 * under Apple's copyrights in this original Apple software (the "Apple Software"): 
 * 
 * (a) to internally use, reproduce, modify and internally distribute the Apple 
 * Software, with or without modifications, in source and binary forms, within your 
 * educational organization or internal campus network for the sole purpose of 
 * integrating Apple's iTunes U software with your internal campus network systems; and 
 * 
 * (b) to redistribute the Apple Software to other universities or educational 
 * organizations, with or without modifications, in source and binary forms, for the 
 * sole purpose of integrating Apple's iTunes U software with their internal campus 
 * network systems; provided that the following conditions are met:
 * 
 * 	-  If you redistribute the Apple Software in its entirety and without 
 *     modifications, you must retain the above copyright notice, this entire license 
 *     and the disclaimer provisions in all such redistributions of the Apple Software.
 * 	-  If you modify and redistribute the Apple Software, you must indicate that you
 *     have made changes to the Apple Software, and you must retain the above
 *     copyright notice, this entire license and the disclaimer provisions in all
 *     such redistributions of the Apple Software and/or derivatives thereof created
 *     by you.
 *     -  Neither the name, trademarks, service marks or logos of Apple may be used to 
 *     endorse or promote products derived from the Apple Software without specific 
 *     prior written permission from Apple.  
 * 
 * Except as expressly stated above, no other rights or licenses, express or implied, 
 * are granted by Apple herein, including but not limited to any patent rights that may
 * be infringed by your derivative works or by other works in which the Apple Software 
 * may be incorporated.  THE APPLE SOFTWARE IS PROVIDED BY APPLE ON AN "AS IS" BASIS.  
 * APPLE MAKES NO WARRANTIES, EXPRESS OR IMPLIED, AND HEREBY DISCLAIMS ALL WARRANTIES, 
 * INCLUDING WITHOUT LIMITATION THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE 
 * OR ITS USE AND OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS OR SYSTEMS.  
 * APPLE IS NOT OBLIGATED TO PROVIDE ANY MAINTENANCE, TECHNICAL OR OTHER SUPPORT FOR 
 * THE APPLE SOFTWARE, OR TO PROVIDE ANY UPDATES TO THE APPLE SOFTWARE.  IN NO EVENT 
 * SHALL APPLE BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION 
 * OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT 
 * (INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN 
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.         
 * 
 * Rev.  120806												
 *
 * This source code file contains a self-contained ANSI C program with no
 * external dependencies except for standard ANSI C libraries. On Mac OS X, it
 * can be compiled and run by executing the following commands in a terminal
 * window:
 *     gcc -o seconds seconds.c
 *     ./seconds
 */

// Compile note added by RKW
//    gcc -o hmac-sha256 hmac-sha256.c
// should work on latter-day gcc installs, but c99 can be made explicit this way:
//    gcc -std=c99 -o hmac-sha256 hmac-sha256.c
#include "base/ev_sha.h"
#include "base/ev_common.h"
#include "base/ev_byte_order.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>

namespace evpp {
namespace sha1 {

///////////////////////////////////////////////////////////////////////////
// Define variable types
typedef evpp::uint8  UINT_8;
typedef evpp::uint32 UINT_32;
typedef evpp::int64  INT_64;
typedef evpp::uint64 UINT_64;

///////////////////////////////////////////////////////////////////////////
// Declare SHA-1 workspace
typedef union
{
	UINT_8  c[64];
	UINT_32 l[16];
}SHA1_WORKSPACE_BLOCK;

///////////////////////////////////////////////////////////////////////////
// Declare SHA-1 context
typedef struct 
{
	UINT_32               state_[5];
	UINT_32               count_[2];
	UINT_32               reserved0_[1];   // Memory alignment padding
	UINT_8                buffer_[64];
	UINT_8                digest_[20];
	UINT_32               reserved1_[3];   // Memory alignment padding
	UINT_8                workspace_[64];
	SHA1_WORKSPACE_BLOCK* block_;          // SHA1 pointer to the byte array above
}SHA1_CTX;

#define ROTATE_LEFT(x, n) (          \
    ((x) << (n)) | ((x) >> (32-(n))) \
    )
#define SHABLK(ctx, i)  (                 \
    ctx->block_->l[i & 15] = ROTATE_LEFT( \
          ctx->block_->l[(i + 13) & 15]   \
        ^ ctx->block_->l[(i + 8)  & 15]   \
        ^ ctx->block_->l[(i + 2)  & 15]   \
        ^ ctx->block_->l[i & 15], 1)      \
    )

// SHA-1 rounds
#define S_R0(ctx, v, w, x, y, z, i) {                                                             \
    ctx->block_->l[i] = evpp::ev_byte_order::hton32(ctx->block_->l[i]);                           \
    z                += ((w & (x ^ y)) ^ y) + ctx->block_->l[i] + 0x5A827999 + ROTATE_LEFT(v, 5); \
    w                 = ROTATE_LEFT(w, 30);                                                       \
    }
#define S_R1(ctx, v, w, x, y, z, i) {                                           \
    z += ((w & (x ^ y)) ^ y) + SHABLK(ctx, i) + 0x5A827999 + ROTATE_LEFT(v, 5); \
    w  = ROTATE_LEFT(w, 30);                                                    \
    }
#define S_R2(ctx, v, w, x, y, z, i) {                                   \
    z += (w ^ x ^ y) + SHABLK(ctx, i) + 0x6ED9EBA1 + ROTATE_LEFT(v, 5); \
    w  = ROTATE_LEFT(w, 30);                                            \
    }
#define S_R3(ctx, v, w, x, y, z, i) {                                                 \
    z += (((w | x) & y) | (w & x)) + SHABLK(ctx, i) + 0x8F1BBCDC + ROTATE_LEFT(v, 5); \
    w  = ROTATE_LEFT(w, 30);                                                          \
    }
#define S_R4(ctx, v, w, x, y, z, i) {                                   \
    z += (w ^ x ^ y) + SHABLK(ctx, i) + 0xCA62C1D6 + ROTATE_LEFT(v, 5); \
    w  = ROTATE_LEFT(w, 30);                                            \
    }

static void SHA1Transform(SHA1_CTX* context, UINT_32* state, const UINT_8* buffer);
static void SHA1Init(SHA1_CTX* context);
static void SHA1Update(SHA1_CTX* context, const UINT_8* data, UINT_32 len);
static void SHA1Final(SHA1_CTX* context);

static void SHA1Transform(SHA1_CTX* context, UINT_32* state, const UINT_8* buffer)
{
	UINT_32 a = state[0], b = state[1], c = state[2], d = state[3], e = state[4];

	memcpy(context->block_, buffer, 64);

	// 4 rounds of 20 operations each, loop unrolled
	S_R0(context,a,b,c,d,e, 0); S_R0(context,e,a,b,c,d, 1); S_R0(context,d,e,a,b,c, 2); S_R0(context,c,d,e,a,b, 3);
	S_R0(context,b,c,d,e,a, 4); S_R0(context,a,b,c,d,e, 5); S_R0(context,e,a,b,c,d, 6); S_R0(context,d,e,a,b,c, 7);
	S_R0(context,c,d,e,a,b, 8); S_R0(context,b,c,d,e,a, 9); S_R0(context,a,b,c,d,e,10); S_R0(context,e,a,b,c,d,11);
	S_R0(context,d,e,a,b,c,12); S_R0(context,c,d,e,a,b,13); S_R0(context,b,c,d,e,a,14); S_R0(context,a,b,c,d,e,15);
	S_R1(context,e,a,b,c,d,16); S_R1(context,d,e,a,b,c,17); S_R1(context,c,d,e,a,b,18); S_R1(context,b,c,d,e,a,19);
	S_R2(context,a,b,c,d,e,20); S_R2(context,e,a,b,c,d,21); S_R2(context,d,e,a,b,c,22); S_R2(context,c,d,e,a,b,23);
	S_R2(context,b,c,d,e,a,24); S_R2(context,a,b,c,d,e,25); S_R2(context,e,a,b,c,d,26); S_R2(context,d,e,a,b,c,27);
	S_R2(context,c,d,e,a,b,28); S_R2(context,b,c,d,e,a,29); S_R2(context,a,b,c,d,e,30); S_R2(context,e,a,b,c,d,31);
	S_R2(context,d,e,a,b,c,32); S_R2(context,c,d,e,a,b,33); S_R2(context,b,c,d,e,a,34); S_R2(context,a,b,c,d,e,35);
	S_R2(context,e,a,b,c,d,36); S_R2(context,d,e,a,b,c,37); S_R2(context,c,d,e,a,b,38); S_R2(context,b,c,d,e,a,39);
	S_R3(context,a,b,c,d,e,40); S_R3(context,e,a,b,c,d,41); S_R3(context,d,e,a,b,c,42); S_R3(context,c,d,e,a,b,43);
	S_R3(context,b,c,d,e,a,44); S_R3(context,a,b,c,d,e,45); S_R3(context,e,a,b,c,d,46); S_R3(context,d,e,a,b,c,47);
	S_R3(context,c,d,e,a,b,48); S_R3(context,b,c,d,e,a,49); S_R3(context,a,b,c,d,e,50); S_R3(context,e,a,b,c,d,51);
	S_R3(context,d,e,a,b,c,52); S_R3(context,c,d,e,a,b,53); S_R3(context,b,c,d,e,a,54); S_R3(context,a,b,c,d,e,55);
	S_R3(context,e,a,b,c,d,56); S_R3(context,d,e,a,b,c,57); S_R3(context,c,d,e,a,b,58); S_R3(context,b,c,d,e,a,59);
	S_R4(context,a,b,c,d,e,60); S_R4(context,e,a,b,c,d,61); S_R4(context,d,e,a,b,c,62); S_R4(context,c,d,e,a,b,63);
	S_R4(context,b,c,d,e,a,64); S_R4(context,a,b,c,d,e,65); S_R4(context,e,a,b,c,d,66); S_R4(context,d,e,a,b,c,67);
	S_R4(context,c,d,e,a,b,68); S_R4(context,b,c,d,e,a,69); S_R4(context,a,b,c,d,e,70); S_R4(context,e,a,b,c,d,71);
	S_R4(context,d,e,a,b,c,72); S_R4(context,c,d,e,a,b,73); S_R4(context,b,c,d,e,a,74); S_R4(context,a,b,c,d,e,75);
	S_R4(context,e,a,b,c,d,76); S_R4(context,d,e,a,b,c,77); S_R4(context,c,d,e,a,b,78); S_R4(context,b,c,d,e,a,79);

	// Add the working vars back into state
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;

	// Wipe variables
	a = b = c = d = e = 0;
}

static void SHA1Init(SHA1_CTX* context)
{
	// SHA1 initialization
	context->state_[0] = 0x67452301;
	context->state_[1] = 0xEFCDAB89;
	context->state_[2] = 0x98BADCFE;
	context->state_[3] = 0x10325476;
	context->state_[4] = 0xC3D2E1F0;

	context->count_[0] = 0;
	context->count_[1] = 0;

	context->block_ = (SHA1_WORKSPACE_BLOCK*)context->workspace_;
}

static void SHA1Update(SHA1_CTX* context, const UINT_8* data, UINT_32 len)
{
	UINT_32 j = ((context->count_[0] >> 3) & 0x3F);

	if((context->count_[0] += (len << 3)) < (len << 3))
		++context->count_[1]; // Overflow

	context->count_[1] += (len >> 29);

	UINT_32 i;
	if((j + len) > 63)
	{
		i = 64 - j;
		memcpy(&context->buffer_[j], data, i);
		SHA1Transform(context, context->state_, context->buffer_);

		for( ; (i + 63) < len; i += 64)
			SHA1Transform(context, context->state_, &data[i]);

		j = 0;
	}
	else i = 0;

	if((len - i) != 0)
		memcpy(&context->buffer_[j], &data[i], len - i);
}

static void SHA1Final(SHA1_CTX* context)
{
	UINT_32 i;

	UINT_8 pbFinalCount[8];
	for(i = 0; i < 8; ++i)
		pbFinalCount[i] = static_cast<UINT_8>((context->count_[((i >= 4) ? 0 : 1)] >>
			((3 - (i & 3)) * 8) ) & 0xFF); // Endian independent

	SHA1Update(context, (UINT_8*)"\200", 1);

	while((context->count_[0] & 504) != 448)
		SHA1Update(context, (UINT_8*)"\0", 1);

	SHA1Update(context, pbFinalCount, 8); // Cause a Transform()

	for(i = 0; i < 20; ++i)
		context->digest_[i] = static_cast<UINT_8>((context->state_[i >> 2] >> ((3 - (i & 3)) * 8)) & 0xFF);

	// Wipe variables for security reasons
	memset(context->buffer_, 0, 64);
	memset(context->state_, 0, 20);
	memset(context->count_, 0, 8);
	memset(pbFinalCount, 0, 8);

	SHA1Transform(context, context->state_, context->buffer_);
}

} // namespace sha1

namespace sha256 {

/******************************************************************************
 * SHA-256.
 */	
typedef struct {
	uint8_t  hash[32];    // Changed by RKW, unsigned char becomes uint8_t
	uint32_t buffer[16];  // Changed by RKW, unsigned long becomes uint32_t
	uint32_t state[8];    // Changed by RKW, unsinged long becomes uint32_t
	uint8_t  length[8];   // Changed by RKW, unsigned char becomes uint8_t
} sha256_block;

static void sha256_initialize(sha256_block* sha)
{
	int i;
	for (i = 0; i < 16; ++i) sha->buffer[i] = 0;
	sha->state[0] = 0x6a09e667;
	sha->state[1] = 0xbb67ae85;
	sha->state[2] = 0x3c6ef372;
	sha->state[3] = 0xa54ff53a;
	sha->state[4] = 0x510e527f;
	sha->state[5] = 0x9b05688c;
	sha->state[6] = 0x1f83d9ab;
	sha->state[7] = 0x5be0cd19;
	for (i = 0; i < 8; ++i) sha->length[i] = 0;
}

//  Changed by RKW, formal args are now const uint8_t, uint_32
//    from const unsigned char, unsigned long respectively
static void sha256_update(sha256_block *sha, const uint8_t *message, uint32_t length)
{
	int i, j;
	/* Add the length of the received message, counted in
	 * bytes, to the total length of the messages hashed to
	 * date, counted in bits and stored in 8 separate bytes. */
	for (i = 7; i >= 0; --i) {
		int bits;
		if (i == 7)
			bits = length << 3;
		else if (i == 0 || i == 1 || i == 2)
			bits = 0;
		else
			bits = length >> (53 - 8 * i);
		bits &= 0xff;
		if (sha->length[i] + bits > 0xff) {
			for (j = i - 1; j >= 0 && sha->length[j]++ == 0xff; --j);
		}
		sha->length[i] += bits;
	}
	/* Add the received message to the SHA buffer, updating the
	 * hash at each block (each time the buffer is filled). */
	while (length > 0) {
		/* Find the index in the SHA buffer at which to
		 * append what's left of the received message. */
		int index = sha->length[6] % 2 * 32 + sha->length[7] / 8;
		index = (index + 64 - length % 64) % 64;
		/* Append the received message bytes to the SHA buffer until
		 * we run out of message bytes or until the buffer is filled. */
		for (;length > 0 && index < 64; ++message, ++index, --length) {
			sha->buffer[index / 4] |= *message << (24 - index % 4 * 8);
		}
		/* Update the hash with the buffer contents if the buffer is full. */
		if (index == 64) {
			/* Update the hash with a block of message content. See FIPS 180-2
			 * (<csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf>)
			 * for a description of and details on the algorithm used here. */
			// Changed by RKW, const unsigned long becomes const uint32_t
			const uint32_t k[64] = {
				0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
				0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
				0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
				0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
				0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
				0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
				0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
				0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
				0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
				0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
				0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
				0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
				0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
				0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
				0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
				0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
			};
			// Changed by RKW, unsigned long becomes uint32_t
			uint32_t w[64], a, b, c, d, e, f, g, h;
			int t;
			for (t = 0; t < 16; ++t) {
				w[t] = sha->buffer[t];
				sha->buffer[t] = 0;
			}
			for (t = 16; t < 64; ++t) {
				// Changed by RKW, unsigned long becomes uint32_t
				uint32_t s0, s1;
				s0 = (w[t - 15] >> 7 | w[t - 15] << 25);
				s0 ^= (w[t - 15] >> 18 | w[t - 15] << 14);
				s0 ^= (w[t - 15] >> 3);
				s1 = (w[t - 2] >> 17 | w[t - 2] << 15);
				s1 ^= (w[t - 2] >> 19 | w[t - 2] << 13);
				s1 ^= (w[t - 2] >> 10);
				w[t] = (s1 + w[t - 7] + s0 + w[t - 16]) & 0xffffffffU;
			}
			a = sha->state[0];
			b = sha->state[1];
			c = sha->state[2];
			d = sha->state[3];
			e = sha->state[4];
			f = sha->state[5];
			g = sha->state[6];
			h = sha->state[7];
			for (t = 0; t < 64; ++t) {
				// Changed by RKW, unsigned long becomes uint32_t
				uint32_t e0, e1, t1, t2;
				e0 = (a >> 2 | a << 30);
				e0 ^= (a >> 13 | a << 19);
				e0 ^= (a >> 22 | a << 10);
				e1 = (e >> 6 | e << 26);
				e1 ^= (e >> 11 | e << 21);
				e1 ^= (e >> 25 | e << 7);
				t1 = h + e1 + ((e & f) ^ (~e & g)) + k[t] + w[t];
				t2 = e0 + ((a & b) ^ (a & c) ^ (b & c));
				h = g;
				g = f;
				f = e;
				e = d + t1;
				d = c;
				c = b;
				b = a;
				a = t1 + t2;
			}
			sha->state[0] = (sha->state[0] + a) & 0xffffffffU;
			sha->state[1] = (sha->state[1] + b) & 0xffffffffU;
			sha->state[2] = (sha->state[2] + c) & 0xffffffffU;
			sha->state[3] = (sha->state[3] + d) & 0xffffffffU;
			sha->state[4] = (sha->state[4] + e) & 0xffffffffU;
			sha->state[5] = (sha->state[5] + f) & 0xffffffffU;
			sha->state[6] = (sha->state[6] + g) & 0xffffffffU;
			sha->state[7] = (sha->state[7] + h) & 0xffffffffU;
		}
	}
}

//  Changed by RKW, formal args are now const uint8_t, uint_32
//    from const unsigned char, unsigned long respectively
static void sha256_finalize(sha256_block *sha, const uint8_t *message, uint32_t length)
{
	int i;
	// Changed by RKW, unsigned char becomes uint8_t
	uint8_t terminator[64 + 8] = { 0x80 };
	/* Hash the final message bytes if necessary. */
	if (length > 0) sha256_update(sha, message, length);
	/* Create a terminator that includes a stop bit, padding, and
	 * the the total message length. See FIPS 180-2 for details. */
	length = 64 - sha->length[6] % 2 * 32 - sha->length[7] / 8;
	if (length < 9) length += 64;
	for (i = 0; i < 8; ++i) terminator[length - 8 + i] = sha->length[i];
	/* Hash the terminator to finalize the message digest. */
	sha256_update(sha, terminator, length);
	/* Extract the message digest. */
	for (i = 0; i < 32; ++i) {
		sha->hash[i] = (sha->state[i / 4] >> (24 - 8 * (i % 4))) & 0xff;
	}
}

/******************************************************************************
 * HMAC-SHA256.
 */
typedef struct {
    uint8_t      digest[32];  // Changed by RKW, unsigned char becomes uint_8
    uint8_t      key[64];     // Changed by RKW, unsigned char becomes uint_8
    sha256_block sha;
} hmac_sha256_block;

//  Changed by RKW, formal arg is now const uint8_t
//    from const unsigned char
static void hmac_sha256_initialize(hmac_sha256_block *hmac, const uint8_t *key, uint32_t length)
{
	uint32_t i;
	/* Prepare the inner hash key block, hashing the key if it's too long. */
	if (length <= 64) {
		for (i = 0; i < length; ++i) hmac->key[i] = key[i] ^ 0x36;
		for (; i < 64; ++i) hmac->key[i] = 0x36;
	} else {
		sha256_initialize(&(hmac->sha));
		sha256_finalize(&(hmac->sha), key, length);
		for (i = 0; i < 32; ++i) hmac->key[i] = hmac->sha.hash[i] ^ 0x36;
		for (; i < 64; ++i) hmac->key[i] = 0x36;
	}
	/* Initialize the inner hash with the key block. */
	sha256_initialize(&(hmac->sha));
	sha256_update(&(hmac->sha), hmac->key, 64);
}

//  Changed by RKW, formal arg is now const uint8_t
//    from const unsigned char
//static void hmac_sha256_update(hmac_sha256_block *hmac, const uint8_t *message, uint32_t length)
//{
//	/* Update the inner hash. */
//	sha256_update(&(hmac->sha), message, length);
//}

//  Changed by RKW, formal arg is now const uint8_t
//    from const unsigned char
static void hmac_sha256_finalize(hmac_sha256_block *hmac, const uint8_t *message, uint32_t length)
{
	uint32_t i;
	/* Finalize the inner hash and store its value in the digest array. */
	sha256_finalize(&(hmac->sha), message, length);
	for (i = 0; i < 32; ++i) hmac->digest[i] = hmac->sha.hash[i];
	/* Convert the inner hash key block to the outer hash key block. */
	for (i = 0; i < 64; ++i) hmac->key[i] ^= (0x36 ^ 0x5c);
	/* Calculate the outer hash. */
	sha256_initialize(&(hmac->sha));
	sha256_update(&(hmac->sha), hmac->key, 64);
	sha256_finalize(&(hmac->sha), hmac->digest, 32);
	/* Use the outer hash value as the HMAC digest. */
	for (i = 0; i < 32; ++i) hmac->digest[i] = hmac->sha.hash[i];
}

} // namespace sha256

std::string ev_sha1(const void* data, uint32 datalen, bool uppercase)
{
	sha1::SHA1_CTX context;
	char           outbuf[sizeof(context.digest_) * 2 + 1];
	if (NULL == data || 0 == datalen)
	{
		return std::string();
	}
	
	sha1::SHA1Init(&context);
	sha1::SHA1Update(&context, reinterpret_cast<const sha1::UINT_8*>(data), datalen);
	sha1::SHA1Final(&context);

	if (!ev_memory_to_hex_string(context.digest_, sizeof(context.digest_), outbuf, sizeof(outbuf), uppercase))
	{
		return std::string();
	}

	memset(&context, 0, sizeof(context));

	return std::string(outbuf, sizeof(outbuf) - 1);
}

std::string ev_sha1_file(const char* filename, bool uppercase)
{
	sha1::SHA1_CTX context;
	sha1::UINT_32  len;
	sha1::UINT_8   buffer[8192];
	FILE*          file;
	char           outbuf[sizeof(context.digest_) * 2 + 1];
	if (NULL == filename)
	{
		return std::string();
	}

	if (NULL == (file = fopen(filename, "rb")))
		return std::string();
	else {
		sha1::SHA1Init(&context);
		while ((len = (sha1::UINT_32)fread(buffer, 1, 8192, file)))
			sha1::SHA1Update(&context, buffer, len);
		sha1::SHA1Final(&context);
		fclose(file);
	}

	if (!ev_memory_to_hex_string(context.digest_, sizeof(context.digest_), outbuf, sizeof(outbuf), uppercase))
	{
		return std::string();
	}

	memset(&context, 0, sizeof(context));

	return std::string(outbuf, sizeof(outbuf) - 1);
}

std::string ev_sha256(const void* data, uint32 datalen, bool uppercase)
{
	sha256::sha256_block context;
	char                 outbuf[sizeof(context.hash) * 2 + 1];
	if (NULL == data || 0 == datalen)
	{
		return std::string();
	}

	sha256::sha256_initialize(&context);
	sha256::sha256_finalize(&context, reinterpret_cast<const uint8*>(data), datalen);

	if (!ev_memory_to_hex_string(context.hash, sizeof(context.hash), outbuf, sizeof(outbuf), uppercase))
	{
		return std::string();
	}

	memset(&context, 0, sizeof(context));

	return std::string(outbuf, sizeof(outbuf) - 1);
}

std::string ev_hmacsha256(const void* data, uint32 datalen, const void* key, uint32 keylen, bool uppercase)
{
	sha256::hmac_sha256_block context;
	char                      outbuf[sizeof(context.digest) * 2 + 1];
	if (NULL == data || 0 == datalen || NULL == key || 0 == keylen)
	{
		return std::string();
	}

	sha256::hmac_sha256_initialize(&context, reinterpret_cast<const uint8*>(key), keylen);
	sha256::hmac_sha256_finalize(&context, reinterpret_cast<const uint8*>(data), datalen);

	if (!ev_memory_to_hex_string(context.digest, sizeof(context.digest), outbuf, sizeof(outbuf), uppercase))
	{
		return std::string();
	}

	memset(&context, 0, sizeof(context));

	return std::string(outbuf, sizeof(outbuf) - 1);
}

} // namespace evpp