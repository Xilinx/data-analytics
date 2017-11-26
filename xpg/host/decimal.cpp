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
#include "postgres.h"
#include "decimal.h"
#endif

NumericVar const_nan =
  {0, 0, NUMERIC_NAN, 0, NULL, NULL};

/*
 * alloc_var() -
 *
 *	Allocate a digit buffer of ndigits digits (plus a spare digit for rounding)
 */
void
alloc_var(NumericVar *var, int ndigits)
{
  free(var->buf);
  var->buf = (NumericDigit *)malloc((ndigits + 1) * sizeof(NumericDigit));
  var->buf[0] = 0;			/* spare digit for rounding */
  var->digits = var->buf + 1;
  var->ndigits = ndigits;
}

/*
 * Case-independent comparison of two not-necessarily-null-terminated strings.
 * At most n bytes will be examined from each string.
 */
int
pg_strncasecmp(const char *s1, const char *s2, size_t n)
{
  while (n-- > 0)
    {
      unsigned char ch1 = (unsigned char) *s1++;
      unsigned char ch2 = (unsigned char) *s2++;

      if (ch1 != ch2)
	{
	  if (ch1 >= 'A' && ch1 <= 'Z')
	    ch1 += 'a' - 'A';
	  else if (IS_HIGHBIT_SET(ch1) && isupper(ch1))
	    ch1 = tolower(ch1);

	  if (ch2 >= 'A' && ch2 <= 'Z')
	    ch2 += 'a' - 'A';
	  else if (IS_HIGHBIT_SET(ch2) && isupper(ch2))
	    ch2 = tolower(ch2);

	  if (ch1 != ch2)
	    return (int) ch1 - (int) ch2;
	}
      if (ch1 == 0)
	break;
    }
  return 0;
}

/*
 * free_var() -
 *
 *	Return the digit buffer of a variable to the free pool
 */
void
free_var(NumericVar *var)
{
  free(var->buf);
  var->buf = NULL;
  var->digits = NULL;
  var->sign = NUMERIC_NAN;
}

/*
 * round_var
 *
 * Round the value of a variable to no more than rscale decimal digits
 * after the decimal point.  NOTE: we allow rscale < 0 here, implying
 * rounding before the decimal point.
 */
void
round_var(NumericVar *var, int rscale)
{
  NumericDigit *digits = var->digits;
  int			di;
  int			ndigits;
  int			carry;

  var->dscale = rscale;

  /* decimal digits wanted */
  di = (var->weight + 1) * DEC_DIGITS + rscale;

  /*
   * If di = 0, the value loses all digits, but could round up to 1 if its
   * first extra digit is >= 5.  If di < 0 the result must be 0.
   */
  if (di < 0)
    {
      var->ndigits = 0;
      var->weight = 0;
      var->sign = NUMERIC_POS;
    }
  else
    {
      /* NBASE digits wanted */
      ndigits = (di + DEC_DIGITS - 1) / DEC_DIGITS;

      /* 0, or number of decimal digits to keep in last NBASE digit */
      di %= DEC_DIGITS;

      if (ndigits < var->ndigits ||
	  (ndigits == var->ndigits && di > 0))
	{
	  var->ndigits = ndigits;

#if DEC_DIGITS == 1
	  /* di must be zero */
	  carry = (digits[ndigits] >= HALF_NBASE) ? 1 : 0;
#else
	  if (di == 0)
	    carry = (digits[ndigits] >= HALF_NBASE) ? 1 : 0;
	  else
	    {
	      /* Must round within last NBASE digit */
	      int			extra,
		pow10;

#if DEC_DIGITS == 4
	      pow10 = round_powers[di];
#elif DEC_DIGITS == 2
	      pow10 = 10;
#else
#error unsupported NBASE
#endif
	      extra = digits[--ndigits] % pow10;
	      digits[ndigits] -= extra;
	      carry = 0;
	      if (extra >= pow10 / 2)
		{
		  pow10 += digits[ndigits];
		  if (pow10 >= NBASE)
		    {
		      pow10 -= NBASE;
		      carry = 1;
		    }
		  digits[ndigits] = pow10;
		}
	    }
#endif

	  /* Propagate carry if needed */
	  while (carry)
	    {
	      carry += digits[--ndigits];
	      if (carry >= NBASE)
		{
		  digits[ndigits] = carry - NBASE;
		  carry = 1;
		}
	      else
		{
		  digits[ndigits] = carry;
		  carry = 0;
		}
	    }

	  if (ndigits < 0)
	    {
	      Assert(ndigits == -1);	/* better not have added > 1 digit */
	      Assert(var->digits > var->buf);
	      var->digits--;
	      var->ndigits++;
	      var->weight++;
	    }
	}
    }
}

/*
 * apply_typmod() -
 *
 *	Do bounds checking and rounding according to the attributes
 *	typmod field.
 */
void
apply_typmod(NumericVar *var, int32 typmod)
{
  int			precision;
  int			scale;
  int			maxdigits;
  int			ddigits;
  int			i;

  /* Do nothing if we have a default typmod (-1) */
  if (typmod < (int32) (VARHDRSZ))
    return;

  typmod -= VARHDRSZ;
  precision = (typmod >> 16) & 0xffff;
  scale = typmod & 0xffff;
  maxdigits = precision - scale;

  /* Round to target scale (and set var->dscale) */
  round_var(var, scale);

  /*
   * Check for overflow - note we can't do this before rounding, because
   * rounding could raise the weight.  Also note that the var's weight could
   * be inflated by leading zeroes, which will be stripped before storage
   * but perhaps might not have been yet. In any case, we must recognize a
   * true zero, whose weight doesn't mean anything.
   */
  ddigits = (var->weight + 1) * DEC_DIGITS;
  if (ddigits > maxdigits)
    {
      /* Determine true weight; and check for all-zero result */
      for (i = 0; i < var->ndigits; i++)
	{
	  NumericDigit dig = var->digits[i];

	  if (dig)
	    {
	      /* Adjust for any high-order decimal zero digits */
#if DEC_DIGITS == 4
	      if (dig < 10)
		ddigits -= 3;
	      else if (dig < 100)
		ddigits -= 2;
	      else if (dig < 1000)
		ddigits -= 1;
#elif DEC_DIGITS == 2
	      if (dig < 10)
		ddigits -= 1;
#elif DEC_DIGITS == 1
	      /* no adjustment */
#else
#error unsupported NBASE
#endif
	      break;
	    }
	  ddigits -= DEC_DIGITS;
	}
    }
}

/*
 * strip_var
 *
 * Strip any leading and trailing zeroes from a numeric variable
 */
void
strip_var(NumericVar *var)
{
  NumericDigit *digits = var->digits;
  int			ndigits = var->ndigits;

  /* Strip leading zeroes */
  while (ndigits > 0 && *digits == 0)
    {
      digits++;
      var->weight--;
      ndigits--;
    }

  /* Strip trailing zeroes */
  while (ndigits > 0 && digits[ndigits - 1] == 0)
    ndigits--;

  /* If it's zero, normalize the sign and weight */
  if (ndigits == 0)
    {
      var->sign = NUMERIC_POS;
      var->weight = 0;
    }

  var->digits = digits;
  var->ndigits = ndigits;
}

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
set_var_from_str(const char *str, const char *cp, NumericVar *dest)
{
  bool		have_dp = false;
  int			i;
  unsigned char *decdigits;
  int			sign = NUMERIC_POS;
  int			dweight = -1;
  int			ddigits;
  int			dscale = 0;
  int			weight;
  int			ndigits;
  int			offset;
  NumericDigit *digits;

  /*
   * We first parse the string to extract decimal digits and determine the
   * correct decimal weight.  Then convert to NBASE representation.
   */
  switch (*cp)
    {
    case '+':
      sign = NUMERIC_POS;
      cp++;
      break;

    case '-':
      sign = NUMERIC_NEG;
      cp++;
      break;
    }

  if (*cp == '.')
    {
      have_dp = true;
      cp++;
    }

  if (!isdigit((unsigned char) *cp))
    printf("%s", "invalid input syntax for type numeric\n");

  decdigits = (unsigned char *) malloc(strlen(cp) + DEC_DIGITS * 2);

  /* leading padding for digit alignment later */
  memset(decdigits, 0, DEC_DIGITS);
  i = DEC_DIGITS;

  while (*cp)
    {
      if (isdigit((unsigned char) *cp))
	{
	  decdigits[i++] = *cp++ - '0';
	  if (!have_dp)
	    dweight++;
	  else
	    dscale++;
	}
      else if (*cp == '.')
	{
	  if (have_dp)
	    printf("%s", "invalid input syntax for type numeric\n");
	  have_dp = true;
	  cp++;
	}
      else
	break;
    }

  ddigits = i - DEC_DIGITS;
  /* trailing padding for digit alignment later */
  memset(decdigits + i, 0, DEC_DIGITS - 1);

  /* Handle exponent, if any */
  if (*cp == 'e' || *cp == 'E')
    {
      long		exponent;
      char	   *endptr;

      cp++;
      exponent = strtol(cp, &endptr, 10);
      if (endptr == cp)
	printf("%s", "invalid input syntax for type numeric\n");
      cp = endptr;
      if (exponent > NUMERIC_MAX_PRECISION || exponent < -NUMERIC_MAX_PRECISION)
	printf("%s", "invalid input syntax for type numeric\n");
      dweight += (int) exponent;
      dscale -= (int) exponent;
      if (dscale < 0)
	dscale = 0;
    }

  /*
   * Okay, convert pure-decimal representation to base NBASE.  First we need
   * to determine the converted weight and ndigits.  offset is the number of
   * decimal zeroes to insert before the first given digit to have a
   * correctly aligned first NBASE digit.
   */
  if (dweight >= 0)
    weight = (dweight + 1 + DEC_DIGITS - 1) / DEC_DIGITS - 1;
  else
    weight = -((-dweight - 1) / DEC_DIGITS + 1);
  offset = (weight + 1) * DEC_DIGITS - (dweight + 1);
  ndigits = (ddigits + offset + DEC_DIGITS - 1) / DEC_DIGITS;

  alloc_var(dest, ndigits);
  dest->sign = sign;
  dest->weight = weight;
  dest->dscale = dscale;

  i = DEC_DIGITS - offset;
  digits = dest->digits;

  while (ndigits-- > 0)
    {
#if DEC_DIGITS == 4
      *digits++ = ((decdigits[i] * 10 + decdigits[i + 1]) * 10 +
		   decdigits[i + 2]) * 10 + decdigits[i + 3];
#elif DEC_DIGITS == 2
      *digits++ = decdigits[i] * 10 + decdigits[i + 1];
#elif DEC_DIGITS == 1
      *digits++ = decdigits[i];
#else
#error unsupported NBASE
#endif
      i += DEC_DIGITS;
    }

  free(decdigits);

  /* Strip any leading/trailing zeroes, and normalize weight if zero */
  strip_var(dest);

  /* Return end+1 position for caller */
  return cp;
}

/*
 * make_result() -
 *
 *	Create the packed db numeric format in malloc()'d memory from
 *	a variable.
 */
Numeric
make_result(NumericVar *var)
{
  Numeric		result;
  NumericDigit *digits = var->digits;
  int			weight = var->weight;
  int			sign = var->sign;
  int			n;
  size_t		len;

  if (sign == NUMERIC_NAN)
    {
      result = (Numeric) malloc(NUMERIC_HDRSZ_SHORT);

      SET_VARSIZE(result, NUMERIC_HDRSZ_SHORT);
      result->choice.n_header = NUMERIC_NAN;
      /* the header word is all we need */

      //dump_numeric("make_result()", result);
      return result;
    }

  n = var->ndigits;

  /* truncate leading zeroes */
  while (n > 0 && *digits == 0)
    {
      digits++;
      weight--;
      n--;
    }
  /* truncate trailing zeroes */
  while (n > 0 && digits[n - 1] == 0)
    n--;

  /* If zero result, force to weight=0 and positive sign */
  if (n == 0)
    {
      weight = 0;
      sign = NUMERIC_POS;
    }

  /* Build the result */
  if (NUMERIC_CAN_BE_SHORT(var->dscale, weight))
    {
      len = NUMERIC_HDRSZ_SHORT + n * sizeof(NumericDigit);
      result = (Numeric) malloc(len);
      SET_VARSIZE(result, len);
      result->choice.n_short.n_header =
	(sign == NUMERIC_NEG ? (NUMERIC_SHORT | NUMERIC_SHORT_SIGN_MASK)
	 : NUMERIC_SHORT)
	| (var->dscale << NUMERIC_SHORT_DSCALE_SHIFT)
	| (weight < 0 ? NUMERIC_SHORT_WEIGHT_SIGN_MASK : 0)
	| (weight & NUMERIC_SHORT_WEIGHT_MASK);
    }
  else
    {
      len = NUMERIC_HDRSZ + n * sizeof(NumericDigit);
      result = (Numeric) malloc(len);
      SET_VARSIZE(result, len);
      result->choice.n_long.n_sign_dscale =
	sign | (var->dscale & NUMERIC_DSCALE_MASK);
      result->choice.n_long.n_weight = weight;
    }

  Assert(NUMERIC_NDIGITS(result) == n);
  if (n > 0)
    memcpy(NUMERIC_DIGITS(result), digits, n * sizeof(NumericDigit));

  /* Check for overflow of int16 fields */
  if (NUMERIC_WEIGHT(result) != weight || NUMERIC_DSCALE(result) != var->dscale)
    printf("%s", "value overflow numeric format\n");
  //dump_numeric("make_result()", result);
  return result;
}

Numeric
decimal_to_numeric(char *str)
{
  int32		typmod = DatumGetInt32(Int32GetDatum(-1)); 
  Numeric		res;
  const char *cp;

  /* Skip leading spaces */
  cp = str;
  while (*cp)
    {
      if (!isspace((unsigned char) *cp))
	break;
      cp++;
    }

  /*
   * Check for NaN
   */
  if (pg_strncasecmp(cp, "NaN", 3) == 0)
    {
      res = make_result(&const_nan);

      /* Should be nothing left but spaces */
      cp += 3;
      while (*cp)
	{
	  if (!isspace((unsigned char) *cp))
	    printf("%s", "invalid input syntax for type numeric\n");
	  cp++;
	}
    }
  else
    {
      /*
       * Use set_var_from_str() to parse a normal numeric value
       */
      NumericVar	value;

      init_var(&value);

      cp = set_var_from_str(str, cp, &value);

      /*
       * We duplicate a few lines of code here because we would like to
       * throw any trailing-junk syntax error before any semantic error
       * resulting from apply_typmod.  We can't easily fold the two cases
       * together because we mustn't apply apply_typmod to a NaN.
       */
      while (*cp)
	{
	  if (!isspace((unsigned char) *cp))
	    printf("%s", "invalid input syntax for type numeric\n");
	  cp++;
	}

      apply_typmod(&value, typmod);

      res = make_result(&value);
      free_var(&value);
    }
  return res;
}

extern string numeric_to_string(Numeric num)
{
  uint16 n_header = num->choice.n_short.n_header;
  NumericDigit *digit = num->choice.n_short.n_data;
  int ndigit = NUMERIC_NDIGITS(num);
  string stnumeric = tostring(n_header);
  int i = 0;
  for (; i < ndigit; i++)
    {
      stnumeric.append("|");
      stnumeric.append(tostring(digit[i]));
    }
  return stnumeric;
}
