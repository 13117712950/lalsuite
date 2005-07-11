/********* <lalVerbatim file="PrintTFTileListCV"> ********
Author: Eanna Flanagan, and Cannon, K.
$Id$
********* </lalVerbatim> ********/

#include <math.h>
#include <stdio.h>
#include <lal/LALRCSID.h>

NRCSID(PRINTTFTILELISTC, "$Id$");

#include <lal/ExcessPower.h>
#include <lal/Thresholds.h>


static void PrintTFTile(FILE *file, const TFTile *tile, const REAL4TimeFrequencyPlane *plane)
{
	INT4 t1 = tile->tstart;
	INT4 t2 = tile->tstart + tile->tbins;
	INT4 f1 = tile->fstart;
	INT4 f2 = tile->fstart + tile->fbins;
	REAL8 flow = plane->params.flow;
	REAL8 epoch = plane->epoch.gpsSeconds + plane->epoch.gpsNanoSeconds / (REAL8) 1e-9;
	REAL8 deltaT = plane->params.deltaT;

	fprintf(file, "\n");
	fprintf(file, " Frequency interval: %f Hz to %f Hz, i.e.,  bins %d to %d of %d\n", flow + f1 / deltaT, flow + (f2 + 1) / deltaT, f1, f2, plane->params.freqBins);
	fprintf(file, " Time interval    :  %f s to %f s, i.e.,  bins %d to %d of %d\n", epoch + t1 * deltaT, epoch + (t2 + 1) * deltaT, t1, t2, plane->params.timeBins);
	fprintf(file, " Total number of degrees of freedom:  %f\n", XLALTFTileDegreesOfFreedom(tile));
	fprintf(file, " Excess power:  %f,   1 / alpha    :  %f\n", tile->excessPower, exp(-tile->lnalpha));
	/* print out effective number of sigma */
	fprintf(file, " Effective number of sigma:  %f\n", sqrt(XLALChi2Threshold(1.0, exp(tile->lnalpha))));
}


/******** <lalVerbatim file="PrintTFTileListCP"> ********/
void XLALPrintTFTileList(
	FILE *file,
	const TFTiling *tiling,
	const REAL4TimeFrequencyPlane *plane,
	size_t maxTiles
)
/******** </lalVerbatim> ********/
{
	TFTile *tile = tiling->tile;

	if(maxTiles > tiling->numtiles)
		maxTiles = tiling->numtiles;

	while(maxTiles--)
		PrintTFTile(file, tile++, plane);
}
