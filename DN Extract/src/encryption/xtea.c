/*
 *  An 32-bit implementation of the XTEA algorithm
 *
 *  Copyright (C) 2009  Paul Bakker <polarssl_maintainer at polarssl dot org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

//#include "polarssl/config.h"

#define POLARSSL_XTEA_C
#if defined(POLARSSL_XTEA_C)

#include "xtea.h"

#include <string.h>

/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef GET_ULONG_BE
#define GET_ULONG_BE(n,b,i)                             \
{                                                       \
    (n) = ( (unsigned long) (b)[(i)    ] << 24 )        \
	    | ( (unsigned long) (b)[(i) + 1] << 16 )        \
	    | ( (unsigned long) (b)[(i) + 2] <<  8 )        \
	    | ( (unsigned long) (b)[(i) + 3]       );       \
}
#endif

#ifndef PUT_ULONG_BE
#define PUT_ULONG_BE(n,b,i)                             \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif

/*
 * XTEA key schedule
 */
void xtea_setupx( xtea_context *ctx, unsigned char key[16] )
{
    int i;

    memset(ctx, 0, sizeof(xtea_context));

    for( i = 0; i < 4; i++ )
    {
    	GET_ULONG_BE( ctx->k[i], key, i << 2 );
    }
}

/*
 * XTEA encrypt function
 */
void xtea_crypt_ecb( xtea_context *ctx, int mode, unsigned char input[8],
                     unsigned char output[8])
{
    uint32_t *k, v0, v1, i;

    k = ctx->k;
    
    GET_ULONG_BE( v0, input, 0 );
    GET_ULONG_BE( v1, input, 4 );

    if( mode == XTEA_ENCRYPT )
    {
	    uint32_t sum = 0, delta = 0x9E3779B9;

	    for( i = 0; i < 32; i++ )
	    {
		    v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
		    sum += delta;
		    v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum>>11) & 3]);
	    }
    }
    else /* XTEA_DECRYPT */
    {
	    uint32_t delta = 0x9E3779B9, sum = delta * 32;

	    for( i = 0; i < 32; i++ )
	    {
		    v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum>>11) & 3]);
		    sum -= delta;
		    v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
	    }
    }

    PUT_ULONG_BE( v0, output, 0 );
    PUT_ULONG_BE( v1, output, 4 );
}

#endif
