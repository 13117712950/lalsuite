/*-----------------------------------------------------------------------
 *
 * File Name: ComplexFFT.h
 *
 * Author: Creighton, J. D. E.
 *
 * Revision: $Id$
 *
 *-----------------------------------------------------------------------
 *
 * NAME
 * ComplexFFT.h
 *
 * SYNOPSIS
 * #include "ComplexFFT.h"
 *
 * DESCRIPTION
 *
 * DIAGNOSTICS
 *
 *-----------------------------------------------------------------------
 */

#ifndef _COMPLEXFFT_H
#define _COMPLEXFFT_H

#include "LALDatatypes.h"

#ifdef  __cplusplus
extern "C" {
#endif

NRCSID( COMPLEXFFTH, "$Id$" );

#define COMPLEXFFT_ENULL 1
#define COMPLEXFFT_ENNUL 2
#define COMPLEXFFT_ESIZE 4
#define COMPLEXFFT_ESZMM 8
#define COMPLEXFFT_ESLEN 16
#define COMPLEXFFT_ESAME 32

#define COMPLEXFFT_MSGENULL "Null pointer"
#define COMPLEXFFT_MSGENNUL "Non-null pointer"
#define COMPLEXFFT_MSGESIZE "Invalid input size"
#define COMPLEXFFT_MSGESZMM "Size mismatch"
#define COMPLEXFFT_MSGESLEN "Invalid/mismatched sequence lengths"
#define COMPLEXFFT_MSGESAME "Input/Output data vectors are the same"

typedef struct
tagComplexFFTPlan
{
  INT4   sign;
  UINT4  size;
  void  *plan;
}
ComplexFFTPlan;


void
EstimateFwdComplexFFTPlan (
    Status          *stat,
    ComplexFFTPlan **plan,
    UINT4            size
    );

void
EstimateInvComplexFFTPlan (
    Status          *stat,
    ComplexFFTPlan **plan,
    UINT4            size
    );

void
MeasureFwdComplexFFTPlan (
    Status          *stat,
    ComplexFFTPlan **plan,
    UINT4            size
    );

void
MeasureInvComplexFFTPlan (
    Status          *stat,
    ComplexFFTPlan **plan,
    UINT4            size
    );

void
DestroyComplexFFTPlan (
    Status          *stat,
    ComplexFFTPlan **plan
    );


void
COMPLEX8VectorFFT (
    Status         *stat,
    COMPLEX8Vector *vout,
    COMPLEX8Vector *vinp,
    ComplexFFTPlan *plan
    );


#ifdef  __cplusplus
}
#endif

#endif /* _COMPLEXFFT_H */
