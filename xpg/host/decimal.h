/**********
Copyright 2017 Xilinx, Inc.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
**********/
#ifndef SQLENGINE_DECIMAL_H
#define SQLENGINE_DECIMAL_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include <iostream>
#include <sstream>
#include <string>
using namespace std;

typedef unsigned int uint32;
typedef unsigned short int uint16;
typedef short int int16;
typedef int int32;

#define FLEXIBLE_ARRAY_MEMBER

#define HIGHBIT					(0x80)
#define IS_HIGHBIT_SET(ch)		((unsigned char)(ch) & HIGHBIT)
#define LONG_ALIGN_MASK (sizeof(long) - 1)
#define MEMSET_LOOP_LIMIT 1024

#define NBASE		10000
#define HALF_NBASE	5000


#define DEC_DIGITS	4			/* decimal digits per NBASE digit */
#if DEC_DIGITS == 4
static const int round_powers[4] = {0, 1000, 100, 10};
#endif

#define Assert(p) assert(p)

#define NUMERIC_POS			0x0000
#define NUMERIC_NEG			0x4000
#define NUMERIC_NAN			0xC000
#define NUMERIC_SHORT		0x8000

#define NUMERIC_MAX_PRECISION		1000

#define NUMERIC_DIGITS(num) (NUMERIC_HEADER_IS_SHORT(num) ?		\
			     (num)->choice.n_short.n_data : (num)->choice.n_long.n_data)
#define NUMERIC_NDIGITS(num)						\
  ((VARSIZE(num) - NUMERIC_HEADER_SIZE(num)) / sizeof(NumericDigit))
#define NUMERIC_CAN_BE_SHORT(scale,weight)	\
  ((scale) <= NUMERIC_SHORT_DSCALE_MAX &&	\
   (weight) <= NUMERIC_SHORT_WEIGHT_MAX &&	\
   (weight) >= NUMERIC_SHORT_WEIGHT_MIN)

#define NUMERIC_SHORT_SIGN_MASK			0x2000
#define NUMERIC_SHORT_DSCALE_MASK		0x1F80
#define NUMERIC_SHORT_DSCALE_SHIFT		7
#define NUMERIC_SHORT_DSCALE_MAX				\
  (NUMERIC_SHORT_DSCALE_MASK >> NUMERIC_SHORT_DSCALE_SHIFT)

#define NUMERIC_SHORT_WEIGHT_SIGN_MASK	0x0040
#define NUMERIC_SHORT_WEIGHT_MASK		0x003F
#define NUMERIC_SHORT_WEIGHT_MAX		NUMERIC_SHORT_WEIGHT_MASK
#define NUMERIC_SHORT_WEIGHT_MIN		(-(NUMERIC_SHORT_WEIGHT_MASK+1))

#define VARHDRSZ		((int32) sizeof(int32))
#define NUMERIC_HDRSZ	(VARHDRSZ + sizeof(uint16) + sizeof(int16))
#define NUMERIC_HDRSZ_SHORT (VARHDRSZ + sizeof(uint16))

#define NUMERIC_DSCALE_MASK			0x3FFF
#define NUMERIC_DSCALE(n)	(NUMERIC_HEADER_IS_SHORT((n)) ?		\
				 ((n)->choice.n_short.n_header & NUMERIC_SHORT_DSCALE_MASK) \
				 >> NUMERIC_SHORT_DSCALE_SHIFT		\
				 : ((n)->choice.n_long.n_sign_dscale & NUMERIC_DSCALE_MASK))
#define NUMERIC_HEADER_SIZE(n)				\
  (VARHDRSZ + sizeof(uint16) +				\
   (NUMERIC_HEADER_IS_SHORT(n) ? 0 : sizeof(int16)))
#define NUMERIC_WEIGHT(n)	(NUMERIC_HEADER_IS_SHORT((n)) ?		\
				 (((n)->choice.n_short.n_header & NUMERIC_SHORT_WEIGHT_SIGN_MASK ? \
				   ~NUMERIC_SHORT_WEIGHT_MASK : 0)	\
				  | ((n)->choice.n_short.n_header & NUMERIC_SHORT_WEIGHT_MASK)) \
				 : ((n)->choice.n_long.n_weight))

/*
 * These structs describe the header of a varlena object that may have been
 * TOASTed.  Generally, don't reference these structs directly, but use the
 * macros below.
 *
 * We use separate structs for the aligned and unaligned cases because the
 * compiler might otherwise think it could generate code that assumes
 * alignment while touching fields of a 1-byte-header varlena.
 */
typedef union
{
  struct						/* Normal varlena (4-byte length) */
  {
    uint32		va_header;
    char		va_data[FLEXIBLE_ARRAY_MEMBER];
  }			va_4byte;
  struct						/* Compressed-in-line format */
  {
    uint32		va_header;
    uint32		va_rawsize; /* Original data size (excludes header) */
    char		va_data[FLEXIBLE_ARRAY_MEMBER];		/* Compressed data */
  }			va_compressed;
} varattrib_4b_x;


typedef int16 NumericDigit;

/* ----------
 * NumericVar is the format we use for arithmetic.  The digit-array part
 * is the same as the NumericData storage format, but the header is more
 * complex.
 *
 * The value represented by a NumericVar is determined by the sign, weight,
 * ndigits, and digits[] array.
 *
 * Note: the first digit of a NumericVar's value is assumed to be multiplied
 * by NBASE ** weight.  Another way to say it is that there are weight+1
 * digits before the decimal point.  It is possible to have weight < 0.
 *
 * buf points at the physical start of the malloc'd digit buffer for the
 * NumericVar.  digits points at the first digit in actual use (the one
 * with the specified weight).  We normally leave an unused digit or two
 * (preset to zeroes) between buf and digits, so that there is room to store
 * a carry out of the top digit without reallocating space.  We just need to
 * decrement digits (and increment weight) to make room for the carry digit.
 * (There is no such extra space in a numeric value stored in the database,
 * only in a NumericVar in memory.)
 *
 * If buf is NULL then the digit buffer isn't actually malloc'd and should
 * not be freed --- see the constants below for an example.
 *
 * dscale, or display scale, is the nominal precision expressed as number
 * of digits after the decimal point (it must always be >= 0 at present).
 * dscale may be more than the number of physically stored fractional digits,
 * implying that we have suppressed storage of significant trailing zeroes.
 * It should never be less than the number of stored digits, since that would
 * imply hiding digits that are present.  NOTE that dscale is always expressed
 * in *decimal* digits, and so it may correspond to a fractional number of
 * base-NBASE digits --- divide by DEC_DIGITS to convert to NBASE digits.
 *
 * rscale, or result scale, is the target precision for a computation.
 * Like dscale it is expressed as number of *decimal* digits after the decimal
 * point, and is always >= 0 at present.
 * Note that rscale is not stored in variables --- it's figured on-the-fly
 * from the dscales of the inputs.
 *
 * While we consistently use "weight" to refer to the base-NBASE weight of
 * a numeric value, it is convenient in some scale-related calculations to
 * make use of the base-10 weight (ie, the approximate log10 of the value).
 * To avoid confusion, such a decimal-units weight is called a "dweight".
 *
 * NB: All the variable-level functions are written in a style that makes it
 * possible to give one and the same variable as argument and destination.
 * This is feasible because the digit buffer is separate from the variable.
 * ----------
 */
typedef struct NumericVar
{
  int			ndigits;		/* # of digits in digits[] - can be 0! */
  int			weight;			/* weight of first digit */
  int			sign;			/* NUMERIC_POS, NUMERIC_NEG, or NUMERIC_NAN */
  int			dscale;			/* display scale */
  NumericDigit *buf;			/* start of malloc'd space for digits[] */
  NumericDigit *digits;		/* base-NBASE digits */
} NumericVar;

/*
 * The Numeric type as stored on disk.
 *
 * If the high bits of the first word of a NumericChoice (n_header, or
 * n_short.n_header, or n_long.n_sign_dscale) are NUMERIC_SHORT, then the
 * numeric follows the NumericShort format; if they are NUMERIC_POS or
 * NUMERIC_NEG, it follows the NumericLong format.  If they are NUMERIC_NAN,
 * it is a NaN.  We currently always store a NaN using just two bytes (i.e.
 * only n_header), but previous releases used only the NumericLong format,
 * so we might find 4-byte NaNs on disk if a database has been migrated using
 * pg_upgrade.  In either case, when the high bits indicate a NaN, the
 * remaining bits are never examined.  Currently, we always initialize these
 * to zero, but it might be possible to use them for some other purpose in
 * the future.
 *
 * In the NumericShort format, the remaining 14 bits of the header word
 * (n_short.n_header) are allocated as follows: 1 for sign (positive or
 * negative), 6 for dynamic scale, and 7 for weight.  In practice, most
 * commonly-encountered values can be represented this way.
 *
 * In the NumericLong format, the remaining 14 bits of the header word
 * (n_long.n_sign_dscale) represent the display scale; and the weight is
 * stored separately in n_weight.
 *
 * NOTE: by convention, values in the packed form have been stripped of
 * all leading and trailing zero digits (where a "digit" is of base NBASE).
 * In particular, if the value is zero, there will be no digits at all!
 * The weight is arbitrary in that case, but we normally set it to zero.
 */

struct NumericShort
{
  uint16		n_header;		/* Sign + display scale + weight */
  NumericDigit n_data[FLEXIBLE_ARRAY_MEMBER]; /* Digits */
};

struct NumericLong
{
  uint16		n_sign_dscale;	/* Sign + display scale */
  int16		n_weight;		/* Weight of 1st digit	*/
  NumericDigit n_data[FLEXIBLE_ARRAY_MEMBER]; /* Digits */
};

union NumericChoice
{
  uint16		n_header;		/* Header word */
  struct NumericLong n_long;	/* Long form (4-byte header) */
  struct NumericShort n_short;	/* Short form (2-byte header) */
};

struct NumericData
{
  int32		vl_len_;		/* varlena header (do not touch directly!) */
  union NumericChoice choice; /* choice of format */
};

struct NumericData;
typedef struct NumericData *Numeric;


typedef uintptr_t Datum;

#define SET_4_BYTES(value)	(((Datum) (value)) & 0xffffffff)
#define GET_4_BYTES(datum)	(((Datum) (datum)) & 0xffffffff)

/*
 * DatumGetInt32
 *		Returns 32-bit integer value of a datum.
 */

#define DatumGetInt32(X) ((int32) GET_4_BYTES(X))

/*
 * Int32GetDatum
 *		Returns datum representation for a 32-bit integer.
 */

#define Int32GetDatum(X) ((Datum) SET_4_BYTES(X))

#define NUMERIC_HEADER_IS_SHORT(n)	(((n)->choice.n_header & 0x8000) != 0)

/*
 * alloc_var() -
 *
 *	Allocate a digit buffer of ndigits digits (plus a spare digit for rounding)
 */
void alloc_var(NumericVar *var, int ndigits);

/*
 * Case-independent comparison of two not-necessarily-null-terminated strings.
 * At most n bytes will be examined from each string.
 */
int pg_strncasecmp(const char *s1, const char *s2, size_t n);

/*
 * MemSetAligned is the same as MemSet except it omits the test to see if
 * "start" is word-aligned.  This is okay to use if the caller knows a-priori
 * that the pointer is suitably aligned (typically, because he just got it
 * from malloc(), which always delivers a max-aligned pointer).
 */
#define MemSetAligned(start, val, len)				\
  do								\
    {								\
      long   *_start = (long *) (start);			\
      int		_val = (val);				\
      size_t	_len = (len);					\
								\
      if ((_len & LONG_ALIGN_MASK) == 0 &&			\
	  _val == 0 &&						\
	  _len <= MEMSET_LOOP_LIMIT &&				\
	  MEMSET_LOOP_LIMIT != 0)				\
	{							\
	  long *_stop = (long *) ((char *) _start + _len);	\
	  while (_start < _stop)				\
	    *_start++ = 0;					\
	}							\
      else							\
	memset(_start, _val, _len);				\
    } while (0)

#define init_var(v)		MemSetAligned(v, 0, sizeof(NumericVar))
/*
 * free_var() -
 *
 *	Return the digit buffer of a variable to the free pool
 */
void free_var(NumericVar *var);

/*
 * round_var
 *
 * Round the value of a variable to no more than rscale decimal digits
 * after the decimal point.  NOTE: we allow rscale < 0 here, implying
 * rounding before the decimal point.
 */
void round_var(NumericVar *var, int rscale);

/*
 * apply_typmod() -
 *
 *	Do bounds checking and rounding according to the attributes
 *	typmod field.
 */
void apply_typmod(NumericVar *var, int32 typmod);

/*
 * strip_var
 *
 * Strip any leading and trailing zeroes from a numeric variable
 */
void strip_var(NumericVar *var);

/*
 * set_var_from_str()
 *
 *	Parse a string and put the number into a variable
 *
 * This function does not handle leading or trailing spaces, and it doesn't
 * accept "NaN" either.  It returns the end+1 position so that caller can
 * check for trailing spaces/garbage if deemed necessary.
 *
 * cp is the place to actually start parsing; str is what to use in error
 * reports.  (Typically cp would be the same except advanced over spaces.)
 */
const char *
set_var_from_str(const char *str, const char *cp, NumericVar *dest);

/*
 * make_result() -
 *
 *	Create the packed db numeric format in malloc()'d memory from
 *	a variable.
 */
Numeric
make_result(NumericVar *var);

Numeric
decimal_to_numeric(char *str);

template<typename T>
string tostring(T var)
{
  stringstream ss;
  ss << var;
  return ss.str();
}

template <class T> static void myCpp(T a)
{
  std::cout << ">>================ myCpp(" << a << ")" << std::endl;
}

extern string numeric_to_string(Numeric num);
#endif
