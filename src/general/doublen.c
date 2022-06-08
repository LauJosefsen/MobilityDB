/*****************************************************************************
 *
 * This MobilityDB code is provided under The PostgreSQL License.
 * Copyright (c) 2016-2022, Université libre de Bruxelles and MobilityDB
 * contributors
 *
 * MobilityDB includes portions of PostGIS version 3 source code released
 * under the GNU General Public License (GPLv2 or later).
 * Copyright (c) 2001-2022, PostGIS contributors
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without a written
 * agreement is hereby granted, provided that the above copyright notice and
 * this paragraph and the following two paragraphs appear in all copies.
 *
 * IN NO EVENT SHALL UNIVERSITE LIBRE DE BRUXELLES BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING
 * LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
 * EVEN IF UNIVERSITE LIBRE DE BRUXELLES HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * UNIVERSITE LIBRE DE BRUXELLES SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON
 * AN "AS IS" BASIS, AND UNIVERSITE LIBRE DE BRUXELLES HAS NO OBLIGATIONS TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
 *
 *****************************************************************************/

/**
 * @file doublen.c
 * @brief Internal types used in particular for computing the average and
 * centroid temporal aggregates.
 *
 * The `double2`, `double3`, and `double4` types are base types composed,
 * respectively, of two, three, and four `double` values. The `tdouble2`,
 * `tdouble3`, and `tdouble4` types are the corresponding temporal types.
 * The in/out functions of all these types are stubs since all access should
 * be internal. These types are used for the transition function of the
 * aggregates, where the first components of the `doubleN` values store the
 * sum and the last one stores the count of the values. The final function
 * computes the average from the `doubleN` values.
*/
#include "general/doublen.h"

/* C */
#include <float.h>
/* PostgreSQL */
#if ! MEOS
  #include <libpq/pqformat.h>
#endif
#if POSTGRESQL_VERSION_NUMBER >= 120000
  #include <utils/float.h>
#endif
/* MobilityDB */
#include "general/temporal_util.h"

/*****************************************************************************
 * Functions
 *****************************************************************************/

#if MEOS
/**
 * Create a double2 value from the double values
 */
double2 *
double2_make(double a, double b)
{
  /* Note: zero-fill is done in function double2_set */
  double2 *result = palloc(sizeof(double2));
  double2_set(a, b, result);
  return result;
}

/**
 * @brief Output function for double2 values
 */
char *
double2_out(double2 *d)
{
  char *astr = basetype_output(T_FLOAT8, Float8GetDatum(d->a));
  char *bstr = basetype_output(T_FLOAT8, Float8GetDatum(d->b));
  char *result = palloc(strlen(astr) + strlen(bstr) + 4);
  sprintf(result, "(%s,%s)", astr, bstr);
  pfree(astr);
  pfree(bstr);
  return result;
}
#else
/**
 * @brief Receive function for double2 values
 */
double2 *
double2_recv(StringInfo buf)
{
  double2 *result = palloc(sizeof(double2));
  const char *bytes = pq_getmsgbytes(buf, sizeof(double2));
  memcpy(result, bytes, sizeof(double2));
  return result;
}

/**
 * @brief Send function for double2 values
 */
bytea *
double2_send(double2 *d)
{
  StringInfoData buf;
  pq_begintypsend(&buf);
  pq_sendbytes(&buf, (void *) d, sizeof(double2));
  return (bytea *) pq_endtypsend(&buf);
}
#endif /* MEOS */

/**
 * Set a double2 value from the double values
 */
void
double2_set(double a, double b, double2 *result)
{
  /* Note: zero-fill is required here, just as in heap tuples */
  memset(result, 0, sizeof(double2));
  result->a = a;
  result->b = b;
}

/**
 * Return the addition of the double2 values
 */
double2 *
double2_add(const double2 *d1, const double2 *d2)
{
  double2 *result = palloc0(sizeof(double2));
  result->a = d1->a + d2->a;
  result->b = d1->b + d2->b;
  return result;
}

/**
 * Return true if the double2 values are equal
 */
bool
double2_eq(const double2 *d1, const double2 *d2)
{
  return (d1->a == d2->a && d1->b == d2->b);
}

#if MEOS
/**
 * Return -1, 0, or 1 depending on whether the first double2
 * is less than, equal, or greater than the second one
 */
int
double2_cmp(double2 *d1, double2 *d2)
{
  int cmp = float8_cmp_internal(d1->a, d2->a);
  if (cmp == 0)
    cmp = float8_cmp_internal(d1->b, d2->b);
  return cmp;
}
#endif

/*****************************************************************************
 * Functions
 *****************************************************************************/

#if MEOS
/**
 * Create a double2 value from the double values
 */
double3 *
double3_make(double a, double b, double c)
{
  /* Note: zero-fill is done in function double3_set */
  double3 *result = palloc(sizeof(double3));
  double3_set(a, b, c, result);
  return result;
}

/**
 * @brief Output function for double3 values
 */
char *
double3_out(double3 *d)
{
  char *astr = basetype_output(T_FLOAT8, Float8GetDatum(d->a));
  char *bstr = basetype_output(T_FLOAT8, Float8GetDatum(d->b));
  char *cstr = basetype_output(T_FLOAT8, Float8GetDatum(d->c));
  char *result = palloc(strlen(astr) + strlen(bstr) + strlen(cstr) + 5);
  sprintf(result, "(%s,%s,%s)", astr, bstr, cstr);
  pfree(astr);
  pfree(bstr);
  pfree(cstr);
  return result;
}
#else
/**
 * @brief Receive function for double3 values
 */
double3 *
double3_recv(StringInfo buf)
{
  double3 *result = palloc(sizeof(double3));
  const char *bytes = pq_getmsgbytes(buf, sizeof(double3));
  memcpy(result, bytes, sizeof(double3));
  return result;
}

/**
 * @brief Send function for double3 values
 */
bytea *
double3_send(double3 *d)
{
  StringInfoData buf;
  pq_begintypsend(&buf);
  pq_sendbytes(&buf, (void *) d, sizeof(double3));
  return (bytea *) pq_endtypsend(&buf);
}
#endif /* MEOS */

/**
 * Set a double3 value from the double values
 */
void
double3_set(double a, double b, double c, double3 *result)
{
  /* Note: zero-fill is required here, just as in heap tuples */
  memset(result, 0, sizeof(double3));
  result->a = a;
  result->b = b;
  result->c = c;
}

/**
 * Return the addition of the double3 values
 */
double3 *
double3_add(const double3 *d1, const double3 *d2)
{
  double3 *result = palloc0(sizeof(double3));
  result->a = d1->a + d2->a;
  result->b = d1->b + d2->b;
  result->c = d1->c + d2->c;
  return result;
}

/**
 * Return true if the double3 values are equal
 */
bool
double3_eq(const double3 *d1, const double3 *d2)
{
  return (d1->a == d2->a && d1->b == d2->b && d1->c == d2->c);
}

#if MEOS
/**
 * Return -1, 0, or 1 depending on whether the first double2
 * is less than, equal, or greater than the second one
 */
int
double3_cmp(double3 *d1, double3 *d2)
{
  int cmp = float8_cmp_internal(d1->a, d2->a);
  if (cmp == 0)
  {
    cmp = float8_cmp_internal(d1->b, d2->b);
    if (cmp == 0)
      cmp = float8_cmp_internal(d1->c, d2->c);
  }
  return cmp;
}
#endif

/*****************************************************************************
 * Functions
 *****************************************************************************/

#if MEOS
/**
 * @brief Create a double2 value from the double values
 */
double4 *
double4_make(double a, double b, double c, double d)
{
  /* Note: zero-fill is done in function double4_set */
  double4 *result = palloc(sizeof(double4));
  double4_set(a, b, c, d, result);
  return result;
}

/**
 * @brief Output function for double4 values
 */
char *
double4_out(double4 *d)
{
  char *astr = basetype_output(T_FLOAT8, Float8GetDatum(d->a));
  char *bstr = basetype_output(T_FLOAT8, Float8GetDatum(d->b));
  char *cstr = basetype_output(T_FLOAT8, Float8GetDatum(d->c));
  char *dstr = basetype_output(T_FLOAT8, Float8GetDatum(d->d));
  char *result = palloc(strlen(astr) + strlen(bstr) + strlen(cstr) +
    strlen(dstr) + 6);
  sprintf(result, "(%s,%s,%s,%s)", astr, bstr, cstr, dstr);
  pfree(astr);
  pfree(bstr);
  pfree(cstr);
  pfree(dstr);
  return result;
}
#else
/**
 * @brief Receive function for double4 values
 */
double4 *
double4_recv(StringInfo buf)
{
  double4 *result = palloc(sizeof(double4));
  const char *bytes = pq_getmsgbytes(buf, sizeof(double4));
  memcpy(result, bytes, sizeof(double4));
  return result;
}

/**
 * @brief Send function for double3 values
 */
bytea *
double4_send(double4 *d)
{
  StringInfoData buf;
  pq_begintypsend(&buf);
  pq_sendbytes(&buf, (void *) d, sizeof(double4));
  return (bytea *) pq_endtypsend(&buf);
}
#endif /* MEOS */

/**
 * Set a double4 value from the double values
 */
void
double4_set(double a, double b, double c, double d, double4 *result)
{
  /* Note: zero-fill is required here, just as in heap tuples */
  memset(result, 0, sizeof(double4));
  result->a = a;
  result->b = b;
  result->c = c;
  result->d = d;
}

/**
 * Return the addition of the double4 values
 */
double4 *
double4_add(const double4 *d1, const double4 *d2)
{
  double4 *result = palloc0(sizeof(double4));
  result->a = d1->a + d2->a;
  result->b = d1->b + d2->b;
  result->c = d1->c + d2->c;
  result->d = d1->d + d2->d;
  return result;
}

/**
 * Return true if the double4 values are equal
 */
bool
double4_eq(const double4 *d1, const double4 *d2)
{
  return (d1->a == d2->a && d1->b == d2->b && d1->c == d2->c &&
    d1->d == d2->d);
}

/*****************************************************************************/

