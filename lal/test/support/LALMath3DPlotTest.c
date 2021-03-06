/*
*  Copyright (C) 2007 Chad Hanna, Benjamin Owen
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

/*_______________________________________________________________________________________
 *
 * File Name: LALMath3DPlotTest.c
 *
 * Author: Hanna C. R.
 *
 *_______________________________________________________________________________________
 */

/**
 * \author Hanna, C.R.
 * \file
 * \ingroup LALMathematica_h
 *
 * \brief Tests LALMath3DPlot().
 *
 * \code
 * LALMath3DPlotTest
 * \endcode
 *
 * ### Description ###
 *
 * This program generates a set of points simulating a template bank and calls
 * LALMath3DPlot() to generate a MATHEMATICA notebook to display a 3D image of the
 * bank.  Instructions on how to evaluate the notebook appear when it is opened.
 *
 * ### Notes ###
 *
 * For a more interesting test of LALMath3DPlot() see <tt>InspiralSpinBankTest.c</tt> in
 * the LALInspiral <tt>bank</tt> package.
 *
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <lal/AVFactories.h>
#include <lal/LALConfig.h>
#include <lal/LALMalloc.h>
#include <lal/LALStatusMacros.h>
#include <lal/LALStdlib.h>
#include <lal/LALMathematica.h>

/**\name Error Codes */ /** @{ */
#define LALMATH3DPLOTTESTC_ENORM        0       /**< Normal exit */
#define LALMATH3DPLOTTESTC_EMEM         1       /**< Memory allocation error */
#define LALMATH3DPLOTTESTC_ESUB         2       /**< Subroutine error */
/** @} */

/** \cond DONT_DOXYGEN */
#define LALMATH3DPLOTTESTC_MSGENORM     "Normal exit"
#define LALMATH3DPLOTTESTC_MSGEMEM      "Memory allocation error"
#define LALMATH3DPLOTTESTC_MSGESUB      "Subroutine error"


int main(void){
  static LALStatus status;
  INT4 loopx = 0; 			/* loop counters */
  INT4 loopy = 0;
  INT4 loopz = 0;
  INT4 ntiles = 0;
  Math3DPointList *list = NULL; 	/* Pointer to structure for mathematica plot */
  Math3DPointList *first = NULL;

  if ((list = (Math3DPointList *) LALCalloc(1, sizeof(Math3DPointList))) == NULL){
    LALError(&status, LALMATH3DPLOTTESTC_MSGEMEM);
    printf(LALMATH3DPLOTTESTC_MSGEMEM);
    return LALMATH3DPLOTTESTC_EMEM;
  }
  first=list;

  for(loopx=1; loopx <= 20; loopx++){
    for(loopy=1; loopy <= 20; loopy++){
      for(loopz=0; loopz <= 1; loopz++){
        list->x = loopx;
        list->y = loopy;
        list->z = loopz;
        list->grayLevel = 0.0;
        ntiles++;
        if ((list = list->next = (Math3DPointList *) LALCalloc(1, sizeof(Math3DPointList))) == NULL){
          LALError(&status, LALMATH3DPLOTTESTC_MSGEMEM);
          printf(LALMATH3DPLOTTESTC_MSGEMEM);
          return LALMATH3DPLOTTESTC_EMEM;
        }
      }
    }
  }

  for(loopx=1; loopx <= 20; loopx++){
    for(loopy=1; loopy <= 20; loopy++){
      list->x = loopx;
      list->y = loopy;
      list->z = 2;
      list->grayLevel = 1.0;
      ntiles++;
      if ((list = list->next = (Math3DPointList *) LALCalloc(1, sizeof(Math3DPointList))) == NULL){
        LALError(&status, LALMATH3DPLOTTESTC_MSGEMEM);
        printf(LALMATH3DPLOTTESTC_MSGEMEM);
        return LALMATH3DPLOTTESTC_EMEM;
      }
    }
  }

  /*LAL!*/
  for(loopx=1; loopx <= 20; loopx++){
    for(loopy=1; loopy <= 20; loopy++){
      for(loopz=3; loopz <= 4; loopz++){
        if( ((loopx==6)||(loopx==19)) && (loopy<16) && (loopy>5)) continue;
        if((loopy==15)&&(((loopx<20)&&(loopx>14))||((loopx<7)&&(loopx>1)))) continue;
        if((loopx>9)&&(loopx<12)&&(((loopy>6)&&(loopy<10))||(loopy==12))) continue;
        if(((loopx==9)||(loopx==12)) && ((loopy>9)&&(loopy<13))) continue;
        if(((loopx==8)||(loopx==13)) && ((loopy>12)&&(loopy<16))) continue;
	list->x = loopx;
        list->y = loopy;
        list->z = loopz;
        list->grayLevel = 0.0;
        ntiles++;
        if ((list = list->next = (Math3DPointList *) LALCalloc(1, sizeof(Math3DPointList))) == NULL){
          LALError(&status, LALMATH3DPLOTTESTC_MSGEMEM);
          printf(LALMATH3DPLOTTESTC_MSGEMEM);
          return LALMATH3DPLOTTESTC_EMEM;
        }
      }
    }
  }


  list->next = NULL;
  printf("\nCalling LALMath3DPlot()......\n");
  LALMath3DPlot(&status, first, &ntiles, NULL);
  REPORTSTATUS(&status);
  if (status.statusCode){
    LALError(&status, LALMATH3DPLOTTESTC_MSGESUB);
    printf(LALMATH3DPLOTTESTC_MSGESUB);
    return LALMATH3DPLOTTESTC_ESUB;
  }


  /* Clean Up the memory from the MathPlot3D structure */
  list = first;
  while(list->next){
    first = list->next;
    LALFree(list);
    list = first;
  }


  /* free the last (first?) memory allocated for Math3DPlot. */
  LALFree(list);

  if (status.statusCode)
    return LALMATH3DPLOTTESTC_ESUB;
  else
    return LALMATH3DPLOTTESTC_ENORM;

}
/** \endcond */
