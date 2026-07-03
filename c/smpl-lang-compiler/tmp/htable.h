/**Copyright John Schember <john@nachtimwald.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */
#ifndef _HTABLE_H_
#define _HTABLE_H_

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

struct htable;
typedef struct htable htable_t;

struct htable_enum;
typedef struct htable_enum htable_enum_t;

typedef unsigned int
(*htable_hash) (const void *in,
								unsigned int seed);

typedef void *
(*htable_kcopy) (void *in);

typedef bool
(*htable_keq) (const void *a,
							 const void *b);
typedef void
(*htable_kfree) (void *in);

typedef void *
(*htable_vcopy) (void *in);

typedef void
(*htable_vfree) (void *in);

typedef struct {
	htable_kcopy key_copy;
	htable_kfree key_free;
	htable_vcopy val_copy;
	htable_vfree val_free;
} htable_cbs;

htable_t *
htable_create (htable_hash hfunc,
							 htable_keq keq,
							 htable_cbs *cbs);

void
htable_destroy (htable_t *ht);

void
htable_insert (htable_t *ht,
							 void *key,
							 void *val);

void
htable_remove (htable_t *ht,
							 void *key);

bool
htable_get (htable_t *ht,
						void *key,
						void **val);

void *
htable_get_direct (htable_t *ht,
									 void *key);

htable_enum_t *
htable_enum_create (htable_t *ht);

bool
htable_enum_next (htable_enum_t *he,
									void **key,
									void **val);

void
htable_enum_destroy (htable_enum_t *he);

#endif//_HTABLE_H_
