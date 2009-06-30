/*
*  Copyright (C) 2007 Thomas Cokelaer
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
*  You should have received a copy of the GNU General Public License
*  along with with program; see the file COPYING. If not, write to the
*  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
*  MA  02111-1307  USA
*/

/*  <lalVerbatim file="LALInspiralHybridHexagonalBankCV">
Author: Cokelaer Thomas
$Id$
</lalVerbatim>  */

/*  <lalLaTeX>

\subsection{Module \texttt{LALInspiralHybridHexagonalBank.c}}

\subsubsection*{Prototypes}
\vspace{0.1in}
\input{LALInspiralHybridHexagonalBankCP}
\idx{LALInspiralHybridHexagonalBank()}

\subsubsection*{Description}
This code does almost the same as the standard Hexagonal Bank code. However,
once the templates cover both the equal line and an other line ($m_1={\rm mMin}$
or $m_2 ={\rm mMax}$), then there is no need to carry on any square/hexagonal
placement. One can simply populate templates along a bissectrice.

\subsubsection*{Algorithm}
The algorithm is identical to the hexagonal placement. However, once a template covers
both the equal mass line and the upper boundary, then the hexagonal placement stops.
So, an additional placement is needed to finalise the bank. In principle the placement
needs to be completed on bothe side of the template bank, at low mass and high mass. So,
we should start from the two templates which covers the two boundaries and populate the
parameter space along a bissectrice.


The coordinates of the bissectrice at a given $tau_0$ coordinate is estimated by tracing a vertical
line in the $\tau_0/tau_3$ plane, estimate the vlue of $tau_3$ on the upper boundary and low
boundary ($\eta=1/4$ line), and finally take the mean of the two values. Although,  is is an approximation
since we should also take into account the orientation of the ellipse, we
think this is good enough.  The vertical line crosses the parameter space on the
$\eta=1/4$ line and the other parameter space boundary which is define either by (1)
$m_1=variable$ and $m_2=mMin$ or (2) $m_1=variable$ and $m_2=mMax$. Concerning (1),
$\eta=1/4$, this is a trivial computation, since
\begin{equation}
\tau_3 =    \frac{  A3}{\eta}  \left( \frac{\eta \tau_0}{A0} \right)^{2/5},
\end{equation}
which in the case of $\eta=1/4$ simply becomes :
\begin{equation}
\tau_3 =     4 A3  \left( \frac{\tau_0}{4 A0} \right)^{2/5}.
\end{equation}
In the case (2), if $\tau_0$ is provided, if we can extract the total mass and $\eta$ parameter,
then $tau_3$ is given by
\begin{equation}
\tau_3 =    \frac{  A3}{\eta}  M^{-2/3}.
\end{equation}
So, we need $M$ and $\eta$. Starting from
\begin{equation}
\tau_0 =    \frac{  A0}{\eta}  \left( M \right)^{-5/3},
\end{equation}
we can extract a cubic equation
\begin{equation}
x^3 - px+q=0
\end{equation}
where $x = M^{1/3}$, $p = -\frac{A0}{\tau_0/m_{\rm Extreme}}$ and $q= - m_{\rm Extreme}=0$. $ m_{\rm Extreme}$
is either set to mMin or mMax depending on which side of the parameter space we are.

The solution for $x$ is standard and takes the expression :
\begin{equation}
x =  \left(-\frac{q}{2}-\frac{1}{2}*\sqrt{\frac{27 q^2 + 4 p^3}{27}}\right)^{\frac{1}{3}}
   + \left(-\frac{q}{2}+\frac{1}{2}*\sqrt{\frac{27 q^2 + 4 p^3}{27}}\right)^{\frac{1}{3}};
\end{equation}

\begin{figure}[]
\begin{center}
\includegraphics[angle=-180, width=-3.5 true in]{LALInspiralHybridHexa2}
\label{fig:hybridhexa1}
\end{center}
\caption{Example of hybrid hexagonal placement. Once an ellipse covers the upper and lower boundary, then the hexagonal
placement stops. This occurs neccesseraly at low and high mass range. }
\end{figure}


\begin{figure}[]
\begin{center}
\includegraphics[angle=-180, width=-3.5 true in]{LALInspiralHybridHexa1}
\label{fig:hybridhexa1}
\end{center}
\caption{Example of hybrid hexagonal placement. Once the ellipses covers the upper and lower part of the parameter space (at
tau0=3.6 and tau0=0.4), then the placement is switched from the hexagonal to a placement along the bissectric of the
upper/lower boundaries as described in the text.}
\end{figure}


\subsubsection*{Uses}
\begin{verbatim}
LALPopulateNarrowEdge()
XLALInspiralBissectionLine ()
\end{verbatim}

\subsubsection*{Notes}

\vspace{0.1in}
\vfill{\footnotesize\input{LALInspiralHybridHexagonalBankCV}}
</lalLaTeX>  */

#include <stdio.h>
#include <lal/LALInspiralBank.h>
#include <lal/AVFactories.h>
#include <lal/SeqFactories.h>
#include <lal/LALStdio.h>
#include <lal/FindRoot.h>





void
LALPopulateNarrowEdge(LALStatus       	*status,
		      InspiralMomentsEtc      *moments,
		      InspiralCell            **cell,
		      INT4                     headId,
		      InspiralTemplate        *paramsIn,
		      HexaGridParam           *gridParam,
		      CellEvolution           *cellEvolution,
		      CellList **cellList,
		      INT4 flag
		      );


NRCSID(LALINSPIRALHYBRIDHEXAGONALBANKC, "$Id$");

/*  <lalVerbatim file="LALInspiralHybridHexagonalBankCP"> */
void
LALInspiralCreatePNCoarseBankHybridHexa(
    LALStatus            *status,
    InspiralTemplateList **list,
    INT4                 *nlist,
    InspiralCoarseBankIn coarseIn
    )
{  /*  </lalVerbatim>  */
  INT4                  i;
  INT4 			firstId = 0;
  REAL4 		A0, A3;
  REAL4                 piFl;
  InspiralBankParams    bankPars;
  InspiralTemplate      *tempPars;
  InspiralMomentsEtc    moments;
  InspiralCell          *cells;
  HexaGridParam         gridParam;
  CellEvolution         cellEvolution;
  CellList 		*cellList = NULL;

  INITSTATUS( status, "LALInspiralHybridHexagonalBank",
      LALINSPIRALHYBRIDHEXAGONALBANKC );
  ATTATCHSTATUSPTR( status );

  ASSERT( coarseIn.mMin > 0., status,
      LALINSPIRALBANKH_ESIZE, LALINSPIRALBANKH_MSGESIZE );
  ASSERT( coarseIn.mMax > 0., status,
      LALINSPIRALBANKH_ESIZE, LALINSPIRALBANKH_MSGESIZE );
  ASSERT( coarseIn.MMax >= 2.*coarseIn.mMin, status,
      LALINSPIRALBANKH_ESIZE, LALINSPIRALBANKH_MSGESIZE );

  /* Set the elements of the metric and tempPars structures in  */
  /* conformity with the coarseIn structure                     */
  if ( !(tempPars = (InspiralTemplate *)
  			LALCalloc( 1, sizeof(InspiralTemplate))))
  {
    LALFree(tempPars);
    LALFree(cells);
    ABORT( status, LALINSPIRALBANKH_EMEM, LALINSPIRALBANKH_MSGEMEM );
  }


  LALInspiralSetParams( status->statusPtr, tempPars, coarseIn );
  CHECKSTATUSPTR( status );

  /* Identify the boundary of search and parameters for the     */
  /* first lattice point                                        */
  LALInspiralSetSearchLimits( status->statusPtr, &bankPars, coarseIn );
  CHECKSTATUSPTR( status );

  tempPars->totalMass   = coarseIn.MMax;
  tempPars->eta         = 0.25;
  tempPars->ieta        = 1.L;
  tempPars->fLower      = coarseIn.fLower;
  tempPars->massChoice  = m1Andm2;
  tempPars->mass1       = coarseIn.mMin;
  tempPars->mass2       = coarseIn.mMax;

  LALInspiralParameterCalc( status->statusPtr, tempPars );
  CHECKSTATUSPTR( status );

  /* Get the moments of the PSD integrand and other parameters */
  /* required in the computation of the metric  once for all.   */
  LALGetInspiralMoments(
  		status->statusPtr,
  		&moments,
   		&coarseIn.shf,
   	 	tempPars );
  CHECKSTATUSPTR( status );

  /* Allocate memory for one cell */
  cells = (InspiralCell*)
      LALCalloc(1,   sizeof(InspiralCell) );

  /*define gridParam*/
  gridParam.mm 		= coarseIn.mmCoarse;
  gridParam.x0Min     	= bankPars.x0Min;
  gridParam.x0Max     	= bankPars.x0Max;
  gridParam.x1Min     	= bankPars.x1Min;
  gridParam.x1Max     	= bankPars.x1Max;
  gridParam.mMin      	= coarseIn.mMin;
  gridParam.mMax      	= coarseIn.mMax;
  gridParam.MMin      	= coarseIn.MMin;
  gridParam.MMax      	= coarseIn.MMax;
  gridParam.etaMin    	= coarseIn.etamin;
  gridParam.space     	= coarseIn.space;
  gridParam.massRange 	= coarseIn.massRange;
  gridParam.gridSpacing = coarseIn.gridSpacing;
  gridParam.fLower 	= coarseIn.fLower;


  cellEvolution.nTemplate 		= 1;
  cellEvolution.nTemplateMax 	= 1;
  cellEvolution.fertile 		= 0;

  /* initialise that first cell */
  tempPars->massChoice  = t03;
  cells[0].t0           = tempPars->t0;
  cells[0].t3           = tempPars->t3;

  /* some aliases */
  piFl  = LAL_PI * tempPars->fLower;
  A0    = 5. / pow(piFl, 8./3.) / 256.;
  A3    = LAL_PI / pow(piFl, 5./3.)/8.;


  /* Initialise the first template */
  LALInitHexagonalBank(
			status->statusPtr,
		       	&cells, firstId,
		       	&moments, tempPars,
		       	&gridParam, &cellEvolution,
		       	&cellList);
  CHECKSTATUSPTR( status );


  {
    INT4 k, kk; /*some indexes*/
    INT4 *new_list 		= NULL;
    CellList *ptr 	= NULL;
    INT4 length 	= 1; /* default size of the bank when we
    						start the bank generation. */

    /* we re-allocate an array which size equals the
     * template bank size. */
    if (! (new_list =  LALMalloc(length*sizeof(INT4))))
    {
      ABORT( status, LALINSPIRALBANKH_EMEM, LALINSPIRALBANKH_MSGEMEM );
    }

    /* while there are cells/template which can propagate, we carry on the loop.*/
    while (cellEvolution.fertile)
    {
      length = LALListLength(cellList);
      /*realloc some memory for the next template*/
      if (! (new_list =  LALRealloc(new_list, length*sizeof(INT4))))
      {
		ABORT( status, LALINSPIRALBANKH_EMEM, LALINSPIRALBANKH_MSGEMEM );
		/* freeing memory here ? */
      }
      ptr = cellList;
      /* we extract the ids which might change within the LALPopulateCell
       * function. Indeed the bank might grow and then we will lost track
       * of ids/bank size and so on. */
      for ( k = 0; k < length; k++)
      {
		new_list[k] = ptr->id;
		ptr = ptr->next;
      }
      /* look at all the template/ids in the current bank to search for fertile cells */
      for (kk = 0; kk < length; kk++)
	  {
		k = new_list[kk];
		if ( cells[k].status == Fertile)
		{
          LALPopulateCell(status->statusPtr, &moments, &cells,
              k,  tempPars, &gridParam, &cellEvolution, &cellList);
		  CHECKSTATUSPTR( status );
	  	  /* now the bank might have grown, but we only look at the
	  	   * template created before this for loop, when we entered
	  	   * in the while loop
	  	   * */
        }
      }
    }
    LALFree(new_list);
  }

#if 1
  {



    INT4 edge1=0, edge2=0;

    	gridParam.gridSpacing = Hexagonal;

    i=0;
    while (i<cellEvolution.nTemplate){
      if (cells[i].status == Edge){
	edge1 = i;
	cells[i].status = In;
	i=cellEvolution.nTemplate;
      }
      i++;
    }
    i=0;
    while (i<cellEvolution.nTemplate){
      if (cells[i].status == Edge){
	edge2=i;
	cells[i].status = In;
	i=cellEvolution.nTemplate;
      }
      i++;
    }



    if (cells[edge1].t0 > cells[edge2].t0){
      LALPopulateNarrowEdge(status->statusPtr, &moments, &cells,
			    edge1,  tempPars, &gridParam, &cellEvolution, &cellList, 0);
      CHECKSTATUSPTR( status );
      LALPopulateNarrowEdge(status->statusPtr, &moments, &cells,
			    edge2,  tempPars, &gridParam, &cellEvolution, &cellList, 1);
      CHECKSTATUSPTR( status );
    }
    else
    {
      LALPopulateNarrowEdge(status->statusPtr, &moments, &cells,
			    edge1,  tempPars, &gridParam, &cellEvolution, &cellList, 1);
      CHECKSTATUSPTR( status );
      LALPopulateNarrowEdge(status->statusPtr, &moments, &cells,
			    edge2,  tempPars, &gridParam, &cellEvolution, &cellList, 0);
      CHECKSTATUSPTR( status );
    }
  }


#endif



  if (cellList != NULL)
    ABORT(status, LALINSPIRALBANKH_EHEXAINIT,LALINSPIRALBANKH_MSGEHEXAINIT);
	/* Here is the current number of template generated. Now, we need
	 * to clean some of them which might be redundant.
	 * */
  *nlist = cellEvolution.nTemplate;

  {
    INT4 k ;
    INT4 length;
    length = cellEvolution.nTemplate;

    for ( k = 0; k < length; k++)
    {
      REAL4 a;
      REAL4 b;
      REAL4 x0;
      REAL4 tempA3;
      SFindRootIn input;
      INT4 valid;

      PRIN  prin;

      tempA3              = pow(A3, -5./2.)/pow(0.25,-1.5);
      tempPars->t0        = cells[k].t0;
      tempPars->t3        = cells[k].t3;
      /* if non physical parameter i.e below eta=0.25*/
      if(cells[k].RectPosition[0] == Below )
      {
        INT4 above=0, below=0, in=0, out=0;

		/*first, we define the line which is along the long semi-axis of the
		 * ambiguity function, defined by the angle theta and the position of
		 * the template.
		 * */
		a = tan(cells[k].metric.theta);
		b = cells[k].t3 - a * cells[k].t0;
		/* and call a function to search for a solution along eta=1/4 */
		input.function 	= LALSPAF;
		input.xmin 		= cells[k].t3-1e-3;
		input.xmax 		= 1000;
		input.xacc 		= 1e-6;

		prin.ct = a * A0 * tempA3;
		prin.b = b;

		LALSBisectionFindRoot(status->statusPtr,
			&x0, &input, (void *)&prin);
		CHECKSTATUSPTR( status );

		tempPars->t3 = x0 + 1e-3; /* to be sure it is physical */
		tempPars->t0 = (tempPars->t3 - b)/a;
		if (tempPars->t0 > 0)
		{
	  	  LALInspiralParameterCalc(status->statusPtr, tempPars);
	  	  CHECKSTATUSPTR( status );
        	}
		else
		{
			LALWarning(status,"HybridHexagonal placement: nothing to be done since t0<=0\n");
		}

		cells[k].t0  = tempPars->t0;
		cells[k].t3  = tempPars->t3;

		/* update its position values */
		valid = 1;
		GetPositionRectangle(status->statusPtr, &cells, k,  tempPars ,
			     &gridParam,
			     &cellEvolution,
			     &cellList,
			     &valid);
		{

	  		switch (cells[k].RectPosition[1]){
			  case In:    in    +=1; break;
			  case Below: below +=1; break;
			  case Above: above +=1; break;
			  case Out:   out   +=1; break;
			  }
			  switch (cells[k].RectPosition[2]){
			  case In:    in    +=1; break;
			  case Below: below +=1; break;
			  case Above: above +=1; break;
			  case Out:   out   +=1; break;
			  }
			  switch (cells[k].RectPosition[3]){
			  case In:    in    +=1; break;
			  case Below: below +=1; break;
			  case Above: above +=1; break;
			  case Out:   out   +=1; break;
			  }
			  switch (cells[k].RectPosition[4]){
			  case In:    in    +=1; break;
			  case Below: below +=1; break;
			  case Above: above +=1; break;
			  case Out:   out   +=1; break;
			  }
			}


		  if (above == 2 && cells[k].position == In)
		  {
		  if (cells[k].child[0] >=0 )
		   {
		     cells[cells[k].child[0]].position = Out;
		   }
		   else
		   {
		     /* nothing to be done, the child is not valid anyway*/
		   }

		  }
		  else
		  {

		  }
	}
	else
	 {

	 }
    }
  }

  for (i=0; i<cellEvolution.nTemplate; i++) {
    if (cells[i].position == In ) {
      *nlist = *nlist +1;
    }
  }



  /* allocate appropriate memory and fill the output bank */
  *list = (InspiralTemplateList*)
    LALRealloc( *list, sizeof(InspiralTemplateList) * (*nlist+1) );
  if ( ! *list )
  {
    LALFree( tempPars );
    ABORT( status, LALINSPIRALBANKH_EMEM, LALINSPIRALBANKH_MSGEMEM );
  }
  memset( *list + *nlist, 0, sizeof(InspiralTemplateList) );
  {
    *nlist = 0 ;
    for (i=0; i<cellEvolution.nTemplate; i++)
    {
      if (cells[i].position == In & cells[i].t0>0)
      {
        tempPars->t0  = cells[i].t0;
        tempPars->t3  = cells[i].t3;
        tempPars->massChoice = t03;
        tempPars->fLower = coarseIn.fLower;
        LALInspiralParameterCalc( status->statusPtr, tempPars );
        CHECKSTATUSPTR( status );

        (*list)[*nlist].ID            = *nlist;
        (*list)[*nlist].params        = *tempPars;
        (*list)[*nlist].metric        = cells[i].metric;
        ++(*nlist);
      }
    }
  }


  LALFree( cells );
  LALFree( tempPars );

  DETATCHSTATUSPTR( status );
  RETURN ( status );
}








REAL8
XLALInspiralBissectionLine (
	REAL8 tau0,
	REAL8 fL,
	REAL8 mMin,
	REAL8 mMax)
{
  /*REAL8 piFa;*/
  REAL8 tau3_a, tau3_b, M, eta, massSep;

  /* First we solve for the lower (equal mass) limit */
  tau3_a = XLALInspiralTau3FromTau0AndEqualMassLine( tau0, fL);

  /* Figure out the boundary between m1 = mMin and m1 = mMax */
  M = mMin + mMax;
  eta = (mMin*mMax)/pow(M, 2.0);
  massSep = XLALInspiralTau0FromMEta(M, eta, fL);

  /* Next we solve for the upper part */
  if (tau0 >= massSep )
  {
    M = XLALInspiralMFromTau0AndNonEqualMass (tau0,  mMin, fL);
    eta = mMin*(M-mMin)/ pow(M,2.0);
  }
  else
  {
    M = XLALInspiralMFromTau0AndNonEqualMass (tau0,  mMax, fL);
    eta = mMax*(M-mMax)/ pow(M,2.0);
  }

  tau3_b = XLALInspiralTau3FromNonEqualMass(M,eta,fL);

  return (0.5 * (tau3_a + tau3_b));
}



void
LALPopulateNarrowEdge(LALStatus               *status,
		      InspiralMomentsEtc      *moments,
		      InspiralCell            **cell,
		      INT4                     headId,
		      InspiralTemplate        *paramsIn,
		      HexaGridParam           *gridParam,
		      CellEvolution           *cellEvolution,
		      CellList **cellList,
		      INT4                    flag
		      )
{
  REAL4 dx0, dx1;
  REAL4 theta, ctheta,stheta;
  INT4 offSpring;
  INT4 next, iteration;
  REAL4 x_int, y_int,xr_int, yr_int, c,s, dy,theta_min, theta_max, theta_int, a, b, t0, t3;

  INITSTATUS( status, "LALPopulateNarrowEdge",
	      LALINSPIRALHYBRIDHEXAGONALBANKC );
  ATTATCHSTATUSPTR( status );

  /* aliases to get the characteristics of the parent template, that we refer
   * to its ID (headId) */

  while ( (*cell)[headId].t0 < gridParam->x0Max && (*cell)[headId].t0 > gridParam->x0Min) {

    dx0           = (*cell)[headId].dx0/sqrt(2.);
    dx1           = (*cell)[headId].dx1/sqrt(2.);
    theta         = (*cell)[headId].metric.theta;
    ctheta        = cos(theta); /*aliases*/
    stheta        = sin(theta); /*aliases*/
    offSpring     = cellEvolution->nTemplate;

    /* reallocate memory by set of 1000 cells if needed*/
    if ( cellEvolution->nTemplate  >= cellEvolution->nTemplateMax){
      *cell = (InspiralCell*)
	LALRealloc( *cell, sizeof(InspiralCell) * (cellEvolution->nTemplateMax + 1000) );
      if ( ! cell ) {
	ABORT( status, LALINSPIRALBANKH_EMEM, LALINSPIRALBANKH_MSGEMEM );
      }
      cellEvolution->nTemplateMax +=  1000;
    }

    next = cellEvolution->nTemplate;

    theta_min = 0.+.1 +LAL_PI/2.;
    theta_max = 2*LAL_PI-.1+LAL_PI/2.;

    theta_min = 0.+.1 ;
    theta_max = 2*LAL_PI-.1;

    /* we will search the intersection between the bissectrice of the parameter space and
    the ellipse which crosses the center of the ellipses next to this point. Such an ellipse
    has semi axes scaled by sqrt(3).*/
    t0 = (*cell)[headId].t0;
    t3 = (*cell)[headId].t3;
    a  = dx0 * sqrt(3.);
    b  = dx1 * sqrt(3.);
    c = cos(theta);
    s = sin(theta);


    iteration = 1;
    while (fabs(theta_max-theta_min)>(.1/180.*LAL_PI) && iteration<20)
    {
      /*for a given angle, what is the ellipse point whivh is the closest to the bissectrice?*/
      theta_int = (theta_max + theta_min)/2.;

      xr_int = a*cos(theta_int);
      yr_int = b*sin(theta_int);

      /*here are the coordinates of the point we are looking at, which stans on an ellipse of semi axis scaled by sqrt(3)
      (suppose to cross all relevant template center)*/
      x_int = xr_int *c - yr_int * s +t0;
      y_int = xr_int *s + yr_int * c +t3;

      /* how far this point is far away of the bissectrice ? */
      dy = y_int -  XLALInspiralBissectionLine(x_int, gridParam->fLower, gridParam->mMin,gridParam->mMax);
      /* which direction shall we go for the dichotomy ? */
      if (flag==0){
	if (dy>0 )
	  theta_max = theta_int;
	else
	  theta_min = theta_int;
      }
      else{
	if (dy>0 )
	  theta_min = theta_int;
	else
	  theta_max = theta_int;
      }
      iteration++;
    }

    /* let us save this new cell coordinate  */

    (*cell)[next].t0   = x_int;
    (*cell)[next].t3   = y_int;

     /*special case when the new position is outside the parameter space requested. */
    if ( (*cell)[next].t0  > gridParam->x0Max ){
      (*cell)[next].t0 = gridParam->x0Max;
      (*cell)[next].t3 = XLALInspiralBissectionLine(gridParam->x0Max, gridParam->fLower, gridParam->mMin,gridParam->mMax);
    }

    if ( (*cell)[next].t3  > gridParam->x1Max ){
      (*cell)[next].t0 = gridParam->x0Max;
      (*cell)[next].t3 = XLALInspiralBissectionLine(gridParam->x0Max, gridParam->fLower, gridParam->mMin,gridParam->mMax);
    }

    if ( (*cell)[next].t0  < gridParam->x0Min ){
	  (*cell)[next].t0 = gridParam->x0Min;
	  (*cell)[next].t3 = XLALInspiralBissectionLine(gridParam->x0Min, gridParam->fLower, gridParam->mMin,gridParam->mMax);
    }

    /*Finally, we initialise the cell properly*/
    LALInitHexagonalBank(status->statusPtr,  cell,  next,
		moments, paramsIn, gridParam, cellEvolution, cellList);
    /* and change the size of the population accordingly*/
    cellEvolution->nTemplate++;

    /* the parent celle can not populate anymore*/
    (*cell)[next].status = Sterile;
    (cellEvolution->fertile)=cellEvolution->fertile-1;
    LALListDelete(cellList, next);
    headId=next;


  }

  /*Similarly fot the first parent. */
 (*cell)[headId].status = Sterile;
 (cellEvolution->fertile)=cellEvolution->fertile-1;
  LALListDelete(cellList, headId);
  DETATCHSTATUSPTR( status );
  RETURN ( status );

}
