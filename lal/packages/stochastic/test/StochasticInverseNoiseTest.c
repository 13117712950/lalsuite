/************************* <lalVerbatim file="StochasticInverseNoiseTestCV">
Author: UTB Relativity Group; contact whelan@oates.utb.edu
$Id$
********************************* </lalVerbatim> */

/********************************************************** <lalLaTeX>
\subsection{Program \texttt{StochasticInverseNoiseTest.c}}
\label{stochastic:ss:StochasticInverseNoiseTest.c}

Test suite for \texttt{LALStochasticInverseNoise()}.

\subsubsection*{Usage}

\begin{verbatim}
./StochasticInverseNoiseTest [options]
Options:
  -h             print usage message
  -q             quiet: run silently
  -v             verbose: print extra information
  -d level       set lalDebugLevel to level
  -n length      frequency series contain length points
  -w filename    read whitened noise PSD from file filename
  -f filename    read whitening filter from file filename 
  -u filename    print unwhitened inverse noise PSD to file filename
  -m filename    print half-whitened inverse noise PSD to file filename
\end{verbatim}

\subsubsection*{Description}

This program tests the function \texttt{LALStochasticInverseNoise()},
which outputs an unwhitened and "half-whitened" inverse noise spectra
from a whitened data stream and a whitening filter.

First, it tests that the correct error codes 
(\textit{cf.}\ Sec.~\ref{stochastic:s:StochasticCrossCorrelation.h})
are generated for the following error conditions (tests in
\textit{italics} are not performed if \verb+LAL_NDEBUG+ is set, as
the corresponding checks in the code are made using the ASSERT macro):
\begin{itemize}
\item \textit{null pointer to output structure}
\item \textit{null pointer to input structure}
\item \textit{null pointer to whitened noise}
\item \textit{null pointer to whitening filter}
\item \textit{null pointer to unwhitened inverse noise}
\item \textit{null pointer to half-whitened inverse noise}
\item \textit{null pointer to data member of whitened noise}
\item \textit{null pointer to data member of whitening filter}
\item \textit{null pointer to data member of unwhitened inverse noise}
\item \textit{null pointer to data member of half-whitened inverse noise}
\item \textit{null pointer to data member of data member of whitened noise}
\item \textit{null pointer to data member of data member of whitening filter}
\item \textit{null pointer to data member of data member of unwhitened inverse noise}
\item \textit{null pointer to data member of data member of half-whitened inverse noise}
\item \textit{zero length}
\item \textit{negative frequency spacing}
\item \textit{zero frequency spacing}
\item negative start frequency
\item length mismatch between whitened noise and whitening filter
\item length mismatch between whitened noise and unwhitened inverse noise
\item length mismatch between whitened noise and half-whitened inverse noise
\item frequency spacing mismatch between whitened noise and whitening filter
\item start frequency mismatch between whitened noise and whitening filter
\end{itemize}

It then verifies that the correct unwhitened and half-whitened inverse
noise are generated for a simple test case:
\begin{enumerate}
\item $\tilde{R}(f)=(1+i)f^2$, $P^{\scriptstyle{\rm W}}(f)=f^3$.  The
  expected results are $1/P(f)=2f$, $1/P^{\scriptstyle{\rm
      HW}}(f)=(1-i)f^{-1}$.
\end{enumerate}

For each successful test (both of these valid data and the invalid ones
described above), it prints ``\texttt{PASS}'' to standard output; if a
test fails, it prints ``\texttt{FAIL}''.

If the four \texttt{filename} arguments are present, it also
calculates a spectrum based on user-specified data and it prints the
noise spectra to the files specified by the user.

\subsubsection*{Exit codes}
\input{StochasticInverseNoiseTestCE}

\subsubsection*{Uses}

\begin{verbatim}
getopt()
LALStochasticInverseNoise()
LALSCreateVector()
LALCCreateVector()
LALSDestroyVector()
LALCDestroyVector()
LALSReadFrequencySeries()
LALCReadFrequencySeries()
LALSPrintFrequencySeries()
LALCPrintFrequencySeries()
LALUnitAsString()
LALUnitCompare()
LALCheckMemoryLeaks()
\end{verbatim}

\subsubsection*{Notes}
\begin{itemize}
\item No specific error checking is done on user-specified data.  If
  the \texttt{length} argument missing, the resulting defaults
  will cause a bad data error.
\item The length of the user-provided series must be specified, even
  though it could in principle be deduced from the input file, because
  the data sequences must be allocated before the
  \texttt{LALCReadFrequencySeries()} function is called.
\item If one \texttt{filename} argument, but not both, is present,
  the user-specified data will be silently ignored.
\end{itemize}

\vfill{\footnotesize\input{StochasticInverseNoiseTestCV}}

******************************************************* </lalLaTeX> */

#include <lal/LALStdlib.h>

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#include <lal/StochasticCrossCorrelation.h>
#include <lal/AVFactories.h>
#include <lal/ReadFTSeries.h>
#include <lal/PrintFTSeries.h>
#include <lal/Units.h>

#include "CheckStatus.h"

NRCSID(STOCHASTICINVERSENOISETESTC, "$Id$");

#define STOCHASTICINVERSENOISETESTC_TRUE     1
#define STOCHASTICINVERSENOISETESTC_FALSE    0
#define STOCHASTICINVERSENOISETESTC_DELTAF   1.0
#define STOCHASTICINVERSENOISETESTC_F0       0.0
#define STOCHASTICINVERSENOISETESTC_LENGTH   8
#define STOCHASTICINVERSENOISETESTC_TOL      1e-6

extern char *optarg;
extern int   optind;

/* int lalDebugLevel  = LALMSGLVL3; */
int lalDebugLevel = LALNDEBUG;

BOOLEAN optVerbose = STOCHASTICINVERSENOISETESTC_FALSE;
UINT4 optLength    = 0;
CHAR  optWNoiseFile[LALNameLength]   = "";
CHAR  optWFilterFile[LALNameLength]     = "";
CHAR  optInvNoiseFile[LALNameLength] = "";
CHAR  optHWInvNoiseFile[LALNameLength] = "";

INT4 code;

static void
Usage (const char *program, int exitflag);

static void
ParseOptions (int argc, char *argv[]);

/***************************** <lalErrTable file="StochasticInverseNoiseTestCE"> */
#define STOCHASTICINVERSENOISETESTC_ENOM 0
#define STOCHASTICINVERSENOISETESTC_EARG 1
#define STOCHASTICINVERSENOISETESTC_ECHK 2
#define STOCHASTICINVERSENOISETESTC_EFLS 3
#define STOCHASTICINVERSENOISETESTC_EUSE 4

#define STOCHASTICINVERSENOISETESTC_MSGENOM "Nominal exit"
#define STOCHASTICINVERSENOISETESTC_MSGEARG "Error parsing command-line arguments"
#define STOCHASTICINVERSENOISETESTC_MSGECHK "Error checking failed to catch bad data"
#define STOCHASTICINVERSENOISETESTC_MSGEFLS "Incorrect answer for valid data"
#define STOCHASTICINVERSENOISETESTC_MSGEUSE "Bad user-entered data"
/***************************** </lalErrTable> */

int main(int argc, char *argv[])
{

  static LALStatus                status;

  StochasticInverseNoiseInput        input;
  StochasticInverseNoiseOutput       output;

  REAL4FrequencySeries     realBadData;
  COMPLEX8FrequencySeries  complexBadData;
  REAL4                   *sPtr;
  COMPLEX8                *cPtr;
  LIGOTimeGPS              epoch = {1234,56789};

  REAL4FrequencySeries     wNoise;
  COMPLEX8FrequencySeries  wFilter;
  REAL4FrequencySeries     invNoise;
  COMPLEX8FrequencySeries  hwInvNoise;

  REAL8      f;
  INT4       i;
  REAL4      expectedReal;
  REAL4      expectedImag;

  LALUnitPair              unitPair;
  LALUnit                  expectedUnit;
  BOOLEAN                  result;

  CHARVector               *unitString = NULL;

  ParseOptions (argc, argv);

  /* define valid parameters */ 
  wNoise.f0     = STOCHASTICINVERSENOISETESTC_F0;
  wNoise.deltaF = STOCHASTICINVERSENOISETESTC_DELTAF;
  wNoise.epoch  = epoch;
  wNoise.data   = NULL;
  invNoise.data = NULL;

  realBadData = wNoise;
 
  wFilter.f0     = wNoise.f0;
  wFilter.deltaF  = wNoise.deltaF;
  wFilter.epoch   = wNoise.epoch;
  wFilter.data    = NULL;
  hwInvNoise.data = NULL;

  complexBadData  = wFilter;

  /******** Set Testing  Units ********/
  /* whitening filter */
  wFilter.sampleUnits = lalDimensionlessUnit;
  wFilter.sampleUnits.unitNumerator[LALUnitIndexADCCount] = 1;
  wFilter.sampleUnits.unitNumerator[LALUnitIndexStrain] = -1;
  
  /* whitened noise */
  wNoise.sampleUnits = lalDimensionlessUnit;
  wNoise.sampleUnits.unitNumerator[LALUnitIndexADCCount] = 2;
  wNoise.sampleUnits.unitNumerator[LALUnitIndexSecond] = 1;
  /**************************************************/

  /* allocate memory */
  /* printf("About to create\n"); */
  LALSCreateVector(&status, &(wNoise.data),
		   STOCHASTICINVERSENOISETESTC_LENGTH);
  if ( ( code = CheckStatus(&status, 0 , "", 
			    STOCHASTICINVERSENOISETESTC_EFLS,
			    STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
  {
    return code;
  }
  /* printf("Just created\n"); */

  LALSCreateVector(&status, &(invNoise.data), 
		   STOCHASTICINVERSENOISETESTC_LENGTH);
  if ( ( code = CheckStatus(&status, 0 , "", 
			    STOCHASTICINVERSENOISETESTC_EFLS,
			    STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
  {
    return code;
  }
  LALCCreateVector(&status, &(wFilter.data),
		   STOCHASTICINVERSENOISETESTC_LENGTH);
  if ( ( code = CheckStatus(&status, 0 , "", 
			    STOCHASTICINVERSENOISETESTC_EFLS,
			    STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
  {
    return code;
  }
  LALCCreateVector(&status, &(hwInvNoise.data),
		   STOCHASTICINVERSENOISETESTC_LENGTH);
  if ( ( code = CheckStatus(&status, 0 , "", 
			    STOCHASTICINVERSENOISETESTC_EFLS,
			    STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
  {
    return code;
  }

  input.whitenedNoisePSD = &wNoise;
  input.whiteningFilter = &wFilter;
  output.unWhitenedInverseNoisePSD = &invNoise;
  output.halfWhitenedInverseNoisePSD = &hwInvNoise;

 /* TEST INVALID DATA HERE -------------------------------------- */

#ifndef LAL_NDEBUG
  if ( ! lalNoDebug )
  {
    /* test behavior for null pointer to input structure */
    LALStochasticInverseNoise(&status, &output, NULL);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENULLPTR,
			      STOCHASTICCROSSCORRELATIONH_MSGENULLPTR,
			      STOCHASTICINVERSENOISETESTC_ECHK,
			      STOCHASTICINVERSENOISETESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: null pointer to input structure results in error:\n \"%s\"\n",STOCHASTICCROSSCORRELATIONH_MSGENULLPTR);
    
    /* test behavior for null pointer to output structure */
    LALStochasticInverseNoise(&status, NULL, &input);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENULLPTR,
			      STOCHASTICCROSSCORRELATIONH_MSGENULLPTR,
			      STOCHASTICINVERSENOISETESTC_ECHK,
			      STOCHASTICINVERSENOISETESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: null pointer to output structure results in error:\n \"%s\"\n",STOCHASTICCROSSCORRELATIONH_MSGENULLPTR);
    
    /* test behavior for null pointer to wNoise member of input structure */ 
    input.whitenedNoisePSD = NULL;
    LALStochasticInverseNoise(&status, &output, &input);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENULLPTR,
			      STOCHASTICCROSSCORRELATIONH_MSGENULLPTR,
			      STOCHASTICINVERSENOISETESTC_ECHK,
			      STOCHASTICINVERSENOISETESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: null pointer to whitened noise results in error:\n       \"%s\"\n",
	   STOCHASTICCROSSCORRELATIONH_MSGENULLPTR);
    input.whitenedNoisePSD = &wNoise;
    /* test behavior for null pointer to wFitler member of input structure */
    input.whiteningFilter = NULL;
    LALStochasticInverseNoise(&status, &output, &input);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENULLPTR,
			      STOCHASTICCROSSCORRELATIONH_MSGENULLPTR,
			      STOCHASTICINVERSENOISETESTC_ECHK,
			      STOCHASTICINVERSENOISETESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: null pointer to whitening filter results in error:\n       \"%s\"\n", STOCHASTICCROSSCORRELATIONH_MSGENULLPTR);
    input.whiteningFilter = &wFilter;
    
    /* test behavior for null pointer to invNoise member of output structure */
    output.unWhitenedInverseNoisePSD = NULL;
    LALStochasticInverseNoise(&status, &output, &input);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENULLPTR,
			      STOCHASTICCROSSCORRELATIONH_MSGENULLPTR,
			      STOCHASTICINVERSENOISETESTC_ECHK,
			      STOCHASTICINVERSENOISETESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: null pointer to unwhitened inverse noise results in error:\n       \"%s\"\n", STOCHASTICCROSSCORRELATIONH_MSGENULLPTR);
    output.unWhitenedInverseNoisePSD = &invNoise;
    
    /* test behavior for null pointer to half-whitened inverse noise member */
    /* of output structure */
    output.halfWhitenedInverseNoisePSD = NULL;
    LALStochasticInverseNoise(&status, &output, &input);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENULLPTR,
			      STOCHASTICCROSSCORRELATIONH_MSGENULLPTR,
			      STOCHASTICINVERSENOISETESTC_ECHK,
			      STOCHASTICINVERSENOISETESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: null pointer to half-whitened inverse noise results in error:\n       \"%s\"\n", STOCHASTICCROSSCORRELATIONH_MSGENULLPTR);
    output.halfWhitenedInverseNoisePSD = &hwInvNoise;
    
    /* test behavior for null pointer to data member of wnoise */
    input.whitenedNoisePSD = &realBadData;
    LALStochasticInverseNoise(&status, &output, &input);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENULLPTR,
			      STOCHASTICCROSSCORRELATIONH_MSGENULLPTR,
			      STOCHASTICINVERSENOISETESTC_ECHK,
			      STOCHASTICINVERSENOISETESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: null pointer to data member of whitened noise results in error:\n       \"%s\"\n", STOCHASTICCROSSCORRELATIONH_MSGENULLPTR);
    input.whitenedNoisePSD = &wNoise;
    
    /* test behavior for null pointer to data member of wFilter */
    input.whiteningFilter = &complexBadData;
    LALStochasticInverseNoise(&status, &output, &input);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENULLPTR,
			      STOCHASTICCROSSCORRELATIONH_MSGENULLPTR,
			      STOCHASTICINVERSENOISETESTC_ECHK,
			      STOCHASTICINVERSENOISETESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: null pointer to data member of whitening filter results in error:\n       \"%s\"\n", STOCHASTICCROSSCORRELATIONH_MSGENULLPTR);
    input.whiteningFilter = &wFilter;
    
    /* test behavior for null pointer to data member of invNoise  */
    output.unWhitenedInverseNoisePSD = &realBadData;
    LALStochasticInverseNoise(&status, &output, &input);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENULLPTR,
			      STOCHASTICCROSSCORRELATIONH_MSGENULLPTR,
			      STOCHASTICINVERSENOISETESTC_ECHK,
			      STOCHASTICINVERSENOISETESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: null pointer to data member of unwhitened inverse noise results in error:\n       \"%s\"\n", STOCHASTICCROSSCORRELATIONH_MSGENULLPTR);
    output.unWhitenedInverseNoisePSD = &invNoise;
    
    /* test behavior for null pointer to data member of half-whitened */
    /* inverse noise */
    output.halfWhitenedInverseNoisePSD = &complexBadData;
    LALStochasticInverseNoise(&status, &output, &input);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENULLPTR,
			      STOCHASTICCROSSCORRELATIONH_MSGENULLPTR,
			      STOCHASTICINVERSENOISETESTC_ECHK,
			      STOCHASTICINVERSENOISETESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: null pointer to data member of half-whitened noise results in error:\n       \"%s\"\n", STOCHASTICCROSSCORRELATIONH_MSGENULLPTR);
    output.halfWhitenedInverseNoisePSD = &hwInvNoise;
    
    /* Create a vector for testing REAL4 null data-data pointers */
    LALSCreateVector(&status, &(realBadData.data), STOCHASTICINVERSENOISETESTC_LENGTH);
    if ( ( code = CheckStatus(&status, 0 , "", 
			      STOCHASTICINVERSENOISETESTC_EFLS,
			      STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
    {
      return code;
    }
    sPtr = realBadData.data->data;
    realBadData.data->data = NULL;
    
    /* test behavior for null pointer to data-data member of wNoise */
    input.whitenedNoisePSD = &realBadData;
    LALStochasticInverseNoise(&status, &output, &input);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENULLPTR,
			      STOCHASTICCROSSCORRELATIONH_MSGENULLPTR,
			      STOCHASTICINVERSENOISETESTC_ECHK,
			      STOCHASTICINVERSENOISETESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: null pointer to data-data member of whitened noise results in error:\n       \"%s\"\n", STOCHASTICCROSSCORRELATIONH_MSGENULLPTR);
    input.whitenedNoisePSD = &wNoise;
    
    /* test behavior for null pointer to data-data member of invNoise */
    output.unWhitenedInverseNoisePSD = &realBadData;
    LALStochasticInverseNoise(&status, &output, &input);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENULLPTR,
			      STOCHASTICCROSSCORRELATIONH_MSGENULLPTR,
			      STOCHASTICINVERSENOISETESTC_ECHK,
			      STOCHASTICINVERSENOISETESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: null pointer to data-data member of unwhitened inverse noise results in error:\n       \"%s\"\n", STOCHASTICCROSSCORRELATIONH_MSGENULLPTR);
    output.unWhitenedInverseNoisePSD = &invNoise;
    
    /* Create a vector for testing COMPLEX8 null data-data pointers */ 
    LALCCreateVector(&status, &(complexBadData.data), STOCHASTICINVERSENOISETESTC_LENGTH);
    if ( ( code = CheckStatus(&status, 0 , "", 
			      STOCHASTICINVERSENOISETESTC_EFLS,
			      STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
    {
      return code;
    }
    cPtr = complexBadData.data->data;
    complexBadData.data->data = NULL;
    
    /* test behavior for null pointer to data-data member of wFilter */
    input.whiteningFilter = &complexBadData;
    LALStochasticInverseNoise(&status, &output, &input);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENULLPTR,
			      STOCHASTICCROSSCORRELATIONH_MSGENULLPTR,
			      STOCHASTICINVERSENOISETESTC_ECHK,
			      STOCHASTICINVERSENOISETESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: null pointer to data-data member of whitening filter results in error:\n       \"%s\"\n", STOCHASTICCROSSCORRELATIONH_MSGENULLPTR);
    input.whiteningFilter = &wFilter;
    
    /* test behavior for null pointer to data-data member of hwInvNoise */
    output.halfWhitenedInverseNoisePSD = &complexBadData;
    LALStochasticInverseNoise(&status, &output, &input);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENULLPTR,
			      STOCHASTICCROSSCORRELATIONH_MSGENULLPTR,
			      STOCHASTICINVERSENOISETESTC_ECHK,
			      STOCHASTICINVERSENOISETESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: null pointer to data-data member of half-whitened inverse noise results in error:\n       \"%s\"\n", STOCHASTICCROSSCORRELATIONH_MSGENULLPTR);
    output.halfWhitenedInverseNoisePSD = &hwInvNoise;
    
    /** clean up **/
    realBadData.data->data = sPtr;
    LALSDestroyVector(&status, &(realBadData.data));
    if ( ( code = CheckStatus(&status, 0 , "", 
			      STOCHASTICINVERSENOISETESTC_EFLS,
			      STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
    {
      return code;
    }
    
    complexBadData.data->data = cPtr;
    LALCDestroyVector(&status, &(complexBadData.data));
    if ( ( code = CheckStatus(&status, 0 , "", 
			      STOCHASTICINVERSENOISETESTC_EFLS,
			      STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
    {
      return code;
    }
    
    /* test behavior for zero length */
    wNoise.data->length = 
      wFilter.data->length = 
      invNoise.data->length = 
      hwInvNoise.data->length = 0;
    
    LALStochasticInverseNoise(&status, &output, &input);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_EZEROLEN,
			      STOCHASTICCROSSCORRELATIONH_MSGEZEROLEN,
			      STOCHASTICINVERSENOISETESTC_ECHK,
			      STOCHASTICINVERSENOISETESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: zero length results in error:\n       \"%s\"\n",
	   STOCHASTICCROSSCORRELATIONH_MSGEZEROLEN);
    /* reassign valid length */
    wNoise.data->length = 
      wFilter.data->length = 
      invNoise.data->length = 
      hwInvNoise.data->length =STOCHASTICINVERSENOISETESTC_LENGTH;
    
    /* test behavior for negative frequency spacing */
    wNoise.deltaF = 
      wFilter.deltaF = - STOCHASTICINVERSENOISETESTC_DELTAF;
    
    LALStochasticInverseNoise(&status, &output, &input);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENONPOSDELTAF,
			      STOCHASTICCROSSCORRELATIONH_MSGENONPOSDELTAF,
			      STOCHASTICINVERSENOISETESTC_ECHK,
			      STOCHASTICINVERSENOISETESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: negative frequency spacing results in error:\n       \"%s\"\n",
	   STOCHASTICCROSSCORRELATIONH_MSGENONPOSDELTAF);
    /* reassign valid frequency spacing */
    wNoise.deltaF = 
      wFilter.deltaF = STOCHASTICINVERSENOISETESTC_DELTAF;
    
    /* test behavior for zero frequency spacing */
    wNoise.deltaF = 
    wFilter.deltaF = 0.0;
  
    LALStochasticInverseNoise(&status, &output, &input);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENONPOSDELTAF,
			      STOCHASTICCROSSCORRELATIONH_MSGENONPOSDELTAF,
			      STOCHASTICINVERSENOISETESTC_ECHK,
			      STOCHASTICINVERSENOISETESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: zero frequency spacing results in error:\n       \"%s\"\n",
	   STOCHASTICCROSSCORRELATIONH_MSGENONPOSDELTAF);
    /* reassign valid frequency spacing */
    wNoise.deltaF = 
      wFilter.deltaF = STOCHASTICINVERSENOISETESTC_DELTAF;
  } /* if ( ! lalNoDebug ) */  
#endif /* LAL_NDEBUG */
  
  /* test behavior for negative start frequency */
  wNoise.f0 = wFilter.f0 = -3.0;
  
  LALStochasticInverseNoise(&status, &output, &input);
  if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENEGFMIN,
			    STOCHASTICCROSSCORRELATIONH_MSGENEGFMIN,
			    STOCHASTICINVERSENOISETESTC_ECHK,
			    STOCHASTICINVERSENOISETESTC_MSGECHK) ) )
    {
      return code;
    }
  printf("  PASS: negative start  frequency results in error:\n     \"%s\"\n",
	 STOCHASTICCROSSCORRELATIONH_MSGENEGFMIN);
  /* reasign valid f0 */
  wNoise.f0 = 
    wFilter.f0 = STOCHASTICINVERSENOISETESTC_F0;
  
  /* test behavior for length mismatch between wNoise and wFilter */
  wFilter.data->length 
    = STOCHASTICINVERSENOISETESTC_LENGTH - 1;
  LALStochasticInverseNoise(&status, &output, &input);
  if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_EMMLEN,
			    STOCHASTICCROSSCORRELATIONH_MSGEMMLEN,
			    STOCHASTICINVERSENOISETESTC_ECHK,
			    STOCHASTICINVERSENOISETESTC_MSGECHK) ) )
  {
    return code;
  }
  printf("  PASS: length mismatch between whitened noise and whitening filter results in error:\n       \"%s\"\n",
	 STOCHASTICCROSSCORRELATIONH_MSGEMMLEN);
  wFilter.data->length = STOCHASTICINVERSENOISETESTC_LENGTH;
  
  /* test behavior length mismatch between wNoise and invNoise */
  invNoise.data->length 
    = STOCHASTICINVERSENOISETESTC_LENGTH - 1;
  LALStochasticInverseNoise(&status, &output, &input);
  if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_EMMLEN,
			    STOCHASTICCROSSCORRELATIONH_MSGEMMLEN,
			    STOCHASTICINVERSENOISETESTC_ECHK,
			    STOCHASTICINVERSENOISETESTC_MSGECHK) ) )
  {
    return code;
  }
   printf("  PASS: length mismatch between whitened inverse noise and unwhitened inverse noise results in error:\n       \"%s\"\n",
          STOCHASTICCROSSCORRELATIONH_MSGEMMLEN);
   invNoise.data->length = STOCHASTICINVERSENOISETESTC_LENGTH;

   /* test behavior length mismatch between wNoise and hwInvNoise */
   hwInvNoise.data->length 
    = STOCHASTICINVERSENOISETESTC_LENGTH - 1;
   LALStochasticInverseNoise(&status, &output, &input);
   if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_EMMLEN,
			     STOCHASTICCROSSCORRELATIONH_MSGEMMLEN,
			     STOCHASTICINVERSENOISETESTC_ECHK,
			     STOCHASTICINVERSENOISETESTC_MSGECHK) ) )
   {
       return code;
   }
   printf("  PASS: length mismatch between whtiened inverse noise and half-whitened inverse noise results in error:\n       \"%s\"\n",
          STOCHASTICCROSSCORRELATIONH_MSGEMMLEN);
   hwInvNoise.data->length = STOCHASTICINVERSENOISETESTC_LENGTH;

   /* test behavior for initial frequency mismatch between wNoise and wFilter*/
   wFilter.f0 = 30;
   LALStochasticInverseNoise(&status, &output, &input);
   if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_EMMFMIN,
			     STOCHASTICCROSSCORRELATIONH_MSGEMMFMIN,
			     STOCHASTICINVERSENOISETESTC_ECHK,
			     STOCHASTICINVERSENOISETESTC_MSGECHK) ) )
   {
       return code;
   }
   printf("  PASS: initial frequency mismatch between whitened noise and whitening filter results in error:\n       \"%s\"\n",
          STOCHASTICCROSSCORRELATIONH_MSGEMMFMIN);
   wFilter.f0 = STOCHASTICINVERSENOISETESTC_F0;

   /* test behavior for frequency spacing mismatch between wNoise and wFilter*/
   wFilter.deltaF = 
     2.0 * STOCHASTICINVERSENOISETESTC_DELTAF;
   LALStochasticInverseNoise(&status, &output, &input);
   if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_EMMDELTAF,
			     STOCHASTICCROSSCORRELATIONH_MSGEMMDELTAF,
			     STOCHASTICINVERSENOISETESTC_ECHK,
			     STOCHASTICINVERSENOISETESTC_MSGECHK) ) )
   {
       return code;
   }
   printf("  PASS: frequency spacing mismatch between whitened noise and whitening filter results in error:\n       \"%s\"\n",
          STOCHASTICCROSSCORRELATIONH_MSGEMMDELTAF);
   wFilter.deltaF = STOCHASTICINVERSENOISETESTC_DELTAF;


 /* VALID TEST DATA HERE ----------------------------------------- */

   /* create input to test */
   for (i=0; i < STOCHASTICINVERSENOISETESTC_LENGTH; i++)
   {
     f = i*STOCHASTICINVERSENOISETESTC_DELTAF;
     
     wNoise.data->data[i]     = f*f*f;
     wFilter.data->data[i].re = f*f;
     wFilter.data->data[i].im = f*f;
   }

   /* fill inverse noise input and output */
   input.whitenedNoisePSD      = &(wNoise);
   input.whiteningFilter                 = &(wFilter);
   output.unWhitenedInverseNoisePSD         = &(invNoise);
   output.halfWhitenedInverseNoisePSD       = &(hwInvNoise);
   
   /*calculate the unwhitened inverse noise and half-whitened inverse noise*/
   LALStochasticInverseNoise(&status, &output, &input );
   if ( ( code = CheckStatus (&status, 0 , "",
			      STOCHASTICINVERSENOISETESTC_EFLS,
			      STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
   {
     return code;
   }

   if (optVerbose) 
   {
     printf("  Valid Data Test:\n");
     printf("  Checking half-whitened inverse noise...\n");
   }
   /* check output f0 */
   if (optVerbose)
   {
     printf("f0=%g, should be %g\n", hwInvNoise.f0,
	    STOCHASTICINVERSENOISETESTC_F0);
   }
   if ( fabs(hwInvNoise.f0-STOCHASTICINVERSENOISETESTC_F0)
	> STOCHASTICINVERSENOISETESTC_TOL )
   {
     printf("  FAIL: Valid data test\n");
     if (optVerbose)
     {
       printf("Exiting with error: %s\n", STOCHASTICINVERSENOISETESTC_MSGEFLS);
     }
     return STOCHASTICINVERSENOISETESTC_EFLS;
   }

   /* check output deltaF */
   if (optVerbose)
   {
     printf("deltaF=%g, should be %g\n", hwInvNoise.deltaF,
            STOCHASTICINVERSENOISETESTC_DELTAF);
   }
   if ( fabs(hwInvNoise.deltaF-STOCHASTICINVERSENOISETESTC_DELTAF)
        / STOCHASTICINVERSENOISETESTC_DELTAF 
	> STOCHASTICINVERSENOISETESTC_TOL )
   {
     printf("  FAIL: Valid data test\n");
     if (optVerbose)
     {
       printf("Exiting with error: %s\n", STOCHASTICINVERSENOISETESTC_MSGEFLS);
     }
     return STOCHASTICINVERSENOISETESTC_EFLS;
   }

   /* check output units */

   expectedUnit = lalDimensionlessUnit;
   expectedUnit.unitNumerator[LALUnitIndexADCCount] = -1;
   expectedUnit.unitNumerator[LALUnitIndexStrain] = -1;
   expectedUnit.unitNumerator[LALUnitIndexSecond] = -1;
   unitPair.unitOne = &expectedUnit;
   unitPair.unitTwo = &(hwInvNoise.sampleUnits);
   LALUnitCompare(&status, &result, &unitPair);
   if ( ( code = CheckStatus(&status, 0 , "",
			     STOCHASTICINVERSENOISETESTC_EFLS,
			     STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
   {
     return code;
   }

   if (optVerbose) 
   {
     LALCHARCreateVector(&status, &unitString, LALUnitTextSize);
     if ( ( code = CheckStatus(&status, 0 , "",
			       STOCHASTICINVERSENOISETESTC_EFLS,
			       STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
     {
       return code;
     }
     
     LALUnitAsString( &status, unitString, unitPair.unitTwo );
     if ( ( code = CheckStatus(&status, 0 , "",
			       STOCHASTICINVERSENOISETESTC_EFLS,
			       STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
     {
       return code;
     }
     printf( "Units of 1/PHW(f) are \"%s\", ", unitString->data );
     
     LALUnitAsString( &status, unitString, unitPair.unitOne );
     if ( ( code = CheckStatus(&status, 0 , "",
			       STOCHASTICINVERSENOISETESTC_EFLS,
			       STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
     {
       return code;
     }
     printf( "should be \"%s\"\n", unitString->data );
     
     LALCHARDestroyVector(&status, &unitString);
     if ( ( code = CheckStatus(&status, 0 , "",
			       STOCHASTICINVERSENOISETESTC_EFLS,
			       STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
     {
       return code;
     }
   }
   
   if (!result)
   {
     printf("  FAIL: Valid data test #1\n");
     if (optVerbose)
     {
       printf("Exiting with error: %s\n", 
	      STOCHASTICINVERSENOISETESTC_MSGEFLS);
     }
     return STOCHASTICINVERSENOISETESTC_EFLS;
   }

   /* check output values */
   if (optVerbose) 
   {
     printf("1/PHW(0)=%g + %g i, should be 0\n",
            hwInvNoise.data->data[0].re, hwInvNoise.data->data[0].im);
   }
   if ( fabs(hwInvNoise.data->data[0].re) > STOCHASTICINVERSENOISETESTC_TOL
        || fabs(hwInvNoise.data->data[0].im) 
	> STOCHASTICINVERSENOISETESTC_TOL )
   {
     printf("  FAIL: Valid data test\n");
     if (optVerbose)
     {
       printf("Exiting with error: %s\n", STOCHASTICINVERSENOISETESTC_MSGEFLS);
     }
     return STOCHASTICINVERSENOISETESTC_EFLS;
   }

  for (i=1; i < STOCHASTICINVERSENOISETESTC_LENGTH; i++) 
  {
    f = i*STOCHASTICINVERSENOISETESTC_DELTAF;
    expectedReal = 1/f;
    expectedImag = - expectedReal;
    if (optVerbose) 
    {
      printf("1/PHW(%f Hz)=%g + %g i, should be %g + %g i\n",
	     f, hwInvNoise.data->data[i].re, hwInvNoise.data->data[i].im,
	     expectedReal, expectedImag);
    }
    if (fabs(hwInvNoise.data->data[i].re - expectedReal)/expectedReal
	> STOCHASTICINVERSENOISETESTC_TOL
	|| fabs(hwInvNoise.data->data[i].im - expectedImag)/expectedImag
	> STOCHASTICINVERSENOISETESTC_TOL)
    {
      printf("  FAIL: Valid data test\n");
      if (optVerbose)
      {
	printf("Exiting with error: %s\n", STOCHASTICINVERSENOISETESTC_MSGEFLS);
      }
      return STOCHASTICINVERSENOISETESTC_EFLS;
    }
  }
  /****** check valid unwhitened inverse noise *******/
   if (optVerbose) 
   {
     printf("  Checking unwhitened inverse noise...\n");
   }
   /* check output f0 */
   if (optVerbose)
   {
     printf("f0=%g, should be %g\n", invNoise.f0,
	    STOCHASTICINVERSENOISETESTC_F0);
   }
   if ( fabs(invNoise.f0-STOCHASTICINVERSENOISETESTC_F0)
	> STOCHASTICINVERSENOISETESTC_TOL )
   {
     printf("  FAIL: Valid data test\n");
     if (optVerbose)
     {
       printf("Exiting with error: %s\n", STOCHASTICINVERSENOISETESTC_MSGEFLS);
     }
     return STOCHASTICINVERSENOISETESTC_EFLS;
   }

   /* check output deltaF */
   if (optVerbose)
   {
     printf("deltaF=%g, should be %g\n", invNoise.deltaF,
            STOCHASTICINVERSENOISETESTC_DELTAF);
   }
   if ( fabs(invNoise.deltaF-STOCHASTICINVERSENOISETESTC_DELTAF)
        / STOCHASTICINVERSENOISETESTC_DELTAF 
	> STOCHASTICINVERSENOISETESTC_TOL )
   {
     printf("  FAIL: Valid data test\n");
     if (optVerbose)
     {
       printf("Exiting with error: %s\n", STOCHASTICINVERSENOISETESTC_MSGEFLS);
     }
     return STOCHASTICINVERSENOISETESTC_EFLS;
   }

   /* check output units */
  expectedUnit = lalDimensionlessUnit;
  expectedUnit.unitNumerator[LALUnitIndexStrain] = -2;
  expectedUnit.unitNumerator[LALUnitIndexSecond] = -1;
  unitPair.unitOne = &expectedUnit;
  unitPair.unitTwo = &(invNoise.sampleUnits);
  LALUnitCompare(&status, &result, &unitPair);
  if ( ( code = CheckStatus(&status, 0 , "",
			    STOCHASTICINVERSENOISETESTC_EFLS,
			    STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
  {
    return code;
  }

  if (optVerbose) 
  {
    LALCHARCreateVector(&status, &unitString, LALUnitTextSize);
    if ( ( code = CheckStatus(&status, 0 , "",
			      STOCHASTICINVERSENOISETESTC_EFLS,
			      STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
    {
      return code;
    }
    
    LALUnitAsString( &status, unitString, unitPair.unitTwo );
  if ( ( code = CheckStatus(&status, 0 , "",
			    STOCHASTICINVERSENOISETESTC_EFLS,
			    STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
    {
      return code;
    }
    printf( "Units of 1/P(f) are \"%s\", ", unitString->data );
    
    LALUnitAsString( &status, unitString, unitPair.unitOne );
  if ( ( code = CheckStatus(&status, 0 , "",
			    STOCHASTICINVERSENOISETESTC_EFLS,
			    STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
    {
      return code;
    }
    printf( "should be \"%s\"\n", unitString->data );
    
    LALCHARDestroyVector(&status, &unitString);
  if ( ( code = CheckStatus(&status, 0 , "",
			    STOCHASTICINVERSENOISETESTC_EFLS,
			    STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
    {
      return code;
    }
  }

  if (!result)
  {
    printf("  FAIL: Valid data test #1\n");
    if (optVerbose)
    {
      printf("Exiting with error: %s\n", 
             STOCHASTICINVERSENOISETESTC_MSGEFLS);
    }
    return STOCHASTICINVERSENOISETESTC_EFLS;
  }

   /* check output values */
   if (optVerbose) 
   {
     printf("1/P(0)=%g, should be 0\n",
            invNoise.data->data[0]);
   }
   if ( fabs(invNoise.data->data[0]) > STOCHASTICINVERSENOISETESTC_TOL )
   {
     printf("  FAIL: Valid data test\n");
     if (optVerbose)
     {
       printf("Exiting with error: %s\n", STOCHASTICINVERSENOISETESTC_MSGEFLS);
     }
     return STOCHASTICINVERSENOISETESTC_EFLS;
   }

  for (i=1; i < STOCHASTICINVERSENOISETESTC_LENGTH; i++) 
  {
    f = i*STOCHASTICINVERSENOISETESTC_DELTAF;
    expectedReal = 2*f;
    if (optVerbose) 
    {
      printf("1/P(%f Hz)=%g, should be %g\n",
	     f, invNoise.data->data[i], expectedReal);
    }
    if ( fabs(invNoise.data->data[i] - expectedReal)/expectedReal
	 > STOCHASTICINVERSENOISETESTC_TOL )
    {
      printf("  FAIL: Valid data test\n");
      if (optVerbose)
      {
	printf("Exiting with error: %s\n", STOCHASTICINVERSENOISETESTC_MSGEFLS);
      }
      return STOCHASTICINVERSENOISETESTC_EFLS;
    }
  }

  printf("  PASS: Valid data test #1\n");

  /******* clean up valid data ******/
  LALSDestroyVector(&status, &(wNoise .data));
  if ( ( code = CheckStatus (&status, 0 , "", 
			     STOCHASTICINVERSENOISETESTC_EFLS,
			     STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
  {
    return code;
  }
  LALSDestroyVector(&status, &(invNoise.data));
  if ( ( code = CheckStatus (&status, 0 , "", 
			     STOCHASTICINVERSENOISETESTC_EFLS,
			     STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
  {
    return code;
  }
  LALCDestroyVector(&status, &(wFilter.data));
  if ( ( code = CheckStatus (&status, 0 , "", 
			     STOCHASTICINVERSENOISETESTC_EFLS,
			     STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
  {
    return code;
  }
  LALCDestroyVector(&status, &(hwInvNoise.data));
  if ( ( code = CheckStatus (&status, 0 , "", 
			     STOCHASTICINVERSENOISETESTC_EFLS,
			     STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
  {
    return code;
  }

   LALCheckMemoryLeaks();

   printf("PASS: all tests\n");


 /* VALID USER TEST DATA HERE ----------------------------------------- */

  if (optWNoiseFile[0] && optWFilterFile[0] && 
      optInvNoiseFile[0] && optHWInvNoiseFile[0])
  { 
    /* allocate memory */
    wNoise.data      = NULL;
    wFilter.data     = NULL;
    invNoise.data    = NULL;
    hwInvNoise.data  = NULL;

    LALSCreateVector(&status, &(wNoise.data), optLength);
    if ( ( code = CheckStatus(&status, 0 , "", 
			      STOCHASTICINVERSENOISETESTC_EUSE,
			      STOCHASTICINVERSENOISETESTC_MSGEUSE) ) )
    {
      return code;
    }
    LALSCreateVector(&status, &(invNoise.data), optLength);
    if ( ( code = CheckStatus(&status, 0 , "", 
			      STOCHASTICINVERSENOISETESTC_EUSE,
			      STOCHASTICINVERSENOISETESTC_MSGEUSE) ) )
    {
      return code;
    }
    LALCCreateVector(&status, &(wFilter.data), optLength);
    if ( ( code = CheckStatus(&status, 0 , "", 
			      STOCHASTICINVERSENOISETESTC_EUSE,
			      STOCHASTICINVERSENOISETESTC_MSGEUSE) ) )
    {
      return code;
    }
    LALCCreateVector(&status, &(hwInvNoise.data), optLength);
    if ( ( code = CheckStatus(&status, 0 , "", 
			      STOCHASTICINVERSENOISETESTC_EUSE,
			      STOCHASTICINVERSENOISETESTC_MSGEUSE) ) )
    {
      return code;
    }

    /* Read input files */
    LALSReadFrequencySeries(&status, &wNoise, optWNoiseFile);
    LALCReadFrequencySeries(&status, &wFilter, optWFilterFile);
    
    /* fill inverse noise input and output */
    input.whitenedNoisePSD = &(wNoise);
    input.whiteningFilter            = &(wFilter);
    output.unWhitenedInverseNoisePSD    = &(invNoise);
    output.halfWhitenedInverseNoisePSD  = &(hwInvNoise);

    /*calculate the unwhitened inverse noise and half-whitened inverse noise*/
    LALStochasticInverseNoise(&status, &output, &input );
    if ( ( code = CheckStatus (&status, 0 , "", 
			       STOCHASTICINVERSENOISETESTC_EUSE,
			       STOCHASTICINVERSENOISETESTC_MSGEUSE) ) )
    {
      return code;
    }

    /* print output files */
    LALSPrintFrequencySeries(output.unWhitenedInverseNoisePSD, 
                             optInvNoiseFile);
    printf("====== Unwhitened Inverse Noise PSD Written to File %s ======\n",
	   optInvNoiseFile);
    LALCPrintFrequencySeries(output.halfWhitenedInverseNoisePSD,
                             optHWInvNoiseFile);
    printf("===== Half-Whitened Inverse Noise PSD Written to File %s =====\n",
	   optHWInvNoiseFile);


    /* clean up valid data */
    LALSDestroyVector(&status, &(wNoise .data));
    if ( ( code = CheckStatus (&status, 0 , "", 
			       STOCHASTICINVERSENOISETESTC_EFLS,
			       STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
    {
      return code;
    }
    LALSDestroyVector(&status, &(invNoise.data));
    if ( ( code = CheckStatus (&status, 0 , "", 
			       STOCHASTICINVERSENOISETESTC_EFLS,
			       STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
    {
      return code;
    }
    LALCDestroyVector(&status, &(wFilter.data));
    if ( ( code = CheckStatus (&status, 0 , "", 
			       STOCHASTICINVERSENOISETESTC_EFLS,
			       STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
    {
      return code;
    }
    LALCDestroyVector(&status, &(hwInvNoise.data));
    if ( ( code = CheckStatus (&status, 0 , "", 
			       STOCHASTICINVERSENOISETESTC_EFLS,
			       STOCHASTICINVERSENOISETESTC_MSGEFLS) ) )
    {
      return code;
    }

  }
  LALCheckMemoryLeaks();
  return 0;

} 


/*----------------------------------------------------------------------*/


/* Usage () Message */
/* Prints a usage message for program program and exits with code exitcode.*/

static void Usage (const char *program, int exitcode)
{
  fprintf (stderr, "Usage: %s [options]\n", program);
  fprintf (stderr, "Options:\n");
  fprintf (stderr, "  -h             print this message\n");
  fprintf (stderr, "  -q             quiet: run silently\n");
  fprintf (stderr, "  -v             verbose: print extra information\n");
  fprintf (stderr, "  -d level       set lalDebugLevel to level\n");
  fprintf (stderr, "  -n length      frequency series contain length points\n");
  fprintf (stderr, "  -w filename    read whitened noise PSD from file filename\n");
  fprintf (stderr, "  -f filename    read whitening filter from file filename \n");
  fprintf (stderr, "  -u filename    print unwhitened inverse noise PSD to file filename\n");
  fprintf (stderr, "  -m filename    print half-whitened inverse noise PSD to file filename\n");
  exit (exitcode);
}

/*
 * ParseOptions ()
 *
 * Parses the argc - 1 option strings in argv[].
 *
 */
static void
ParseOptions (int argc, char *argv[])
{
  while (1)
  {
    int c = -1;

    c = getopt (argc, argv, "hqvd:n:w:f:u:m:");
    if (c == -1)
    {
      break;
    }

    switch (c)
    {
        
      case 'n': /* specify number of points in frequency series */
        optLength = atoi (optarg);
        break;
        
      case 'w': /* specify whitened noise file */
        strncpy (optWNoiseFile, optarg, LALNameLength);
        break;
        
      case 'f': /* specify whitening filter file */
        strncpy (optWFilterFile, optarg, LALNameLength);
        break;
        
      case 'u': /* specify unwhitened inverse noise file */
        strncpy (optInvNoiseFile, optarg, LALNameLength);
        break;

      case 'm': /* specify hwInvNoise file */
        strncpy (optHWInvNoiseFile, optarg, LALNameLength);
        break;

      case 'd': /* set debug level */
        lalDebugLevel = atoi (optarg);
        break;

      case 'v': /* optVerbose */
        optVerbose = STOCHASTICINVERSENOISETESTC_TRUE;
        break;

      case 'q': /* quiet: run silently (ignore error messages) */
        freopen ("/dev/null", "w", stderr);
        freopen ("/dev/null", "w", stdout);
        break;

      case 'h':
        Usage (argv[0], 0);
        break;

      default:
        Usage (argv[0], 1);
    }

  }

  if (optind < argc)
  {
    Usage (argv[0], 1);
  }

  return;
}

