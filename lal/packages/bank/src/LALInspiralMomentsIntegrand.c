/*  <lalVerbatim file="LALInspiralMomentsIntegrandCV">
Authors: Brown, D. A., and Sathyaprakash, B. S.
$Id$
</lalVerbatim>  */

#if 0
<lalLaTeX>
\subsection{Module \texttt{LALInspiralMomentsIntegrand.c}}

\subsubsection*{Prototypes}
\vspace{0.1in}
\input{LALInspiralMomentsIntegrandCP}
\idx{LALInspiralMomentsIntegrand()}
\begin{itemize}
   \item \texttt{integrand,} Output, the value of the integrand
   \item \texttt{x,} Input, the point where the integrand is required
   \item \texttt{params,} Input, of type \texttt{InspiralMomentsIn} containing the details
   required in moments calculation
\end{itemize}

\subsubsection*{Description}

The moments of the noise curve are defined as 
\begin{equation}
I(q)  \equiv S_{h}(f_{0}) \int^{f_{c}/f_{0}}_{f_{s}/f_{0}}
\frac{x^{-q/3}}{S_{h}(x f_{0})} \, dx \,.  
\end{equation}
This function calculates the integrand of this integral, i.e.\ for a given $x$
it calculates 
\begin{equation}
\frac{x^{-q/3}}{S_{h}(xf_{0})} \,\,.
\end{equation}
by interpolating the frequency series containing $S_h(f)$.

\subsubsection*{Algorithm}

\subsubsection*{Uses}
LALDPolynomialInterpolation

\subsubsection*{Notes}

\vfill{\footnotesize\input{LALInspiralMomentsIntegrandCV}}

</lalLaTeX>
#endif

#include <lal/Interpolate.h>
#include <lal/LALInspiralBank.h>

NRCSID(LALINSPIRALMOMENTSINTEGRANDC, "$Id$");

/*  <lalVerbatim file="LALInspiralMomentsIntegrandCP"> */
void
LALInspiralMomentsIntegrand(
    LALStatus  *status,
    REAL8      *integrand,
    REAL8       x,
    void       *params
    )
/* </lalVerbatim> */
{ 
   InspiralMomentsIn   *integrandParams;

   DInterpolateOut      interpOutput;
   DInterpolatePar      interpParams;

   UINT4                numInterpPts = 4;
   REAL8                f[4];
   REAL8                fMin;
   REAL8                fMax;
   REAL8                deltaF;
   UINT8                freqIndex;

   INITSTATUS( status, "LALInspiralMomentsIntegrand", 
       LALINSPIRALMOMENTSINTEGRANDC );
   ATTATCHSTATUSPTR( status );

   ASSERT( params, status, 
       LALINSPIRALBANKH_ENULL, LALINSPIRALBANKH_MSGENULL );

   integrandParams = (InspiralMomentsIn *) params;

   /* check that we have a pointer to a frequency series and it has data */
   ASSERT( integrandParams->shf, status, 
       LALINSPIRALBANKH_ENULL, LALINSPIRALBANKH_MSGENULL );
   ASSERT( integrandParams->shf->data, status, 
       LALINSPIRALBANKH_ENULL, LALINSPIRALBANKH_MSGENULL );
   ASSERT( integrandParams->shf->data->data, status, 
       LALINSPIRALBANKH_ENULL, LALINSPIRALBANKH_MSGENULL );

   /* the minimum and maximum frequency where we have four points */
   deltaF = integrandParams->shf->deltaF;
   fMin = integrandParams->shf->f0 + deltaF;
   fMax = integrandParams->shf->f0 +
     ((REAL8) integrandParams->shf->data->length - 2 ) * deltaF;

   /* locate the nearest point in the frequency series to the desired point */
   if ( x <= fMin )
   {
     freqIndex = 1;
   }
   else if ( x >= fMax )
   {
     freqIndex = integrandParams->shf->data->length - 3;
   }
   else
   {
     freqIndex = (UINT8) floor( (x - integrandParams->shf->f0) / deltaF );
   }

   /* set up the frequency values for interpolation */
   f[0] = (REAL8)(freqIndex - 1) * deltaF;
   f[1] = (REAL8)(freqIndex) * deltaF;
   f[2] = (REAL8)(freqIndex + 1) * deltaF;
   f[3] = (REAL8)(freqIndex + 2) * deltaF;

   /* set up the interpolation parameters */
   interpParams.n = numInterpPts;
   interpParams.x = f;
   interpParams.y = integrandParams->shf->data->data + freqIndex - 1;
   
   /* perform the interpolation... */
   LALDPolynomialInterpolation( status->statusPtr, &interpOutput, x,
       &interpParams );
   CHECKSTATUSPTR( status );

   /* ...and check for a negative value of shf */
   if ( interpOutput.y < 0 )
   {
     ABORT( status, 999, "interpolation output is negative" );
   }

   /* now compute the integrand using shf */
   *integrand = pow( x, -(integrandParams->ndx) ) / interpOutput.y;

   DETATCHSTATUSPTR(status);
   RETURN(status);
}
