/*  <lalVerbatim file="LALMatrixTransformCV">
Author: Sathyaprakash, B. S.
$Id$
</lalVerbatim>  */

/*  <lalLaTeX>

\subsection{Module \texttt{LALMatrixTransform.c}}
A routine to transform a second rank tensor under a given transformation.

\subsubsection*{Prototypes}
\vspace{0.1in}
\input{LALMatrixTransformCP}
\idx{LALMatrixTransform()}
\begin{itemize}
   \item \texttt{n,} Input, dimension of the matrix (currently, and possibly always, only 3)
   \item \texttt{data1,} Input, transformation matrix
   \item \texttt{data2,} Input, matrix whose transformation is required
   \item \texttt{data3,} Output, transformed matrix
\end{itemize}

\subsubsection*{Description}
Given the matrix of transformation in \texttt{data1} and a second rank tensor
\texttt{data2}, this routine computes the transformed tensor in \texttt{data3}.

\subsubsection*{Algorithm}
$$ C_{ij} = A_{im} A_{jl}  B_{ml}.$$

\subsubsection*{Uses}
None.

\subsubsection*{Notes}

\vfill{\footnotesize\input{LALMatrixTransformCV}}

</lalLaTeX>  */



#include <lal/LALInspiralBank.h>

NRCSID(LALMATRIXTRANSFORMC, "$Id$");

/*  <lalVerbatim file="LALMatrixTransformCP"> */

void LALMatrixTransform (LALStatus *status, 
                         INT4 n, 
                         REAL8 **data1, 
                         REAL8 **data2, 
                         REAL8 **data3)
{ /* </lalVerbatim> */

   INT4 i, j, l, m;

   INITSTATUS(status, "LALMatrixTransform", LALMATRIXTRANSFORMC);
   ATTATCHSTATUSPTR(status);
   ASSERT (data1,  status, LALINSPIRALBANKH_ENULL, LALINSPIRALBANKH_MSGENULL);
   ASSERT (data2,  status, LALINSPIRALBANKH_ENULL, LALINSPIRALBANKH_MSGENULL);
   ASSERT (data3,  status, LALINSPIRALBANKH_ENULL, LALINSPIRALBANKH_MSGENULL);

   for (i=0; i<n; i++) {
   for (j=0; j<n; j++) {
      data3[i][j] = 0.0;
      for (l=0; l<n; l++) {
      for (m=0; m<n; m++) {
	 data3[i][j] += data1[i][m]*data2[m][l]*data1[j][l];
      }}
   }}
   DETATCHSTATUSPTR(status);
   RETURN(status);
}
