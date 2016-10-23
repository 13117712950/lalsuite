//
// Copyright (C) 2016 Karl Wette
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with with program; see the file COPYING. If not, write to the
// Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
// MA 02111-1307 USA
//

#ifndef _OUTPUT_RESULTS_H
#define _OUTPUT_RESULTS_H

///
/// \file
/// \ingroup lalapps_pulsar_Weave
///

#include "Weave.h"
#include "SetupData.h"
#include "ComputeResults.h"

#include <lal/LFTandTSutils.h>

#ifdef __cplusplus
extern "C" {
#endif

///
/// Output results from a search
///
typedef struct tagWeaveOutputResults WeaveOutputResults;

///
/// Various varameters required to output results
///
typedef struct tagWeaveOutputParams {
  /// Reference time at which search is conducted
  LIGOTimeGPS ref_time;
  /// Number of spindown parameters to output
  size_t nspins;
  /// If outputting per-detector quantities, list of detectors
  LALStringVector *per_detectors;
  /// Number of per-segment items to output (may be zero)
  UINT4 per_nsegments;
} WeaveOutputParams;

///
/// \name Routines which handle the output results
///
/// @{

WeaveOutputResultItem *XLALWeaveOutputResultItemCreate(
  const WeaveOutputParams *par
  );
void XLALWeaveOutputResultItemDestroy(
  WeaveOutputResultItem *item
  );
WeaveOutputResults *XLALWeaveOutputResultsCreate(
  const LIGOTimeGPS *ref_time,
  const size_t nspins,
  const LALStringVector *per_detectors,
  const UINT4 per_nsegments,
  const int toplist_limit
  );
void XLALWeaveOutputResultsDestroy(
  WeaveOutputResults *out
  );
int XLALWeaveOutputResultsAdd(
  WeaveOutputResults *out,
  const WeaveSemiResults *semi_res,
  const UINT4 semi_nfreqs
  );
int XLALWeaveOutputResultsWrite(
  FITSFile *file,
  const WeaveOutputResults *out
  );
int XLALWeaveOutputResultsReadAppend(
  FITSFile *file,
  WeaveOutputResults **out
  );
int XLALWeaveOutputResultsCompare(
  BOOLEAN *equal,
  const WeaveSetupData *setup,
  const REAL8 param_tol_mism,
  const VectorComparison *result_tol,
  const WeaveOutputResults *out_1,
  const WeaveOutputResults *out_2
  );
int XLALWeaveOutputMiscPerSegInfoWrite(
  FITSFile *file,
  const WeaveSetupData *setup,
  const BOOLEAN write_SFT_info,
  const UINT4 nsegments,
  const WeaveOutputMiscPerSegInfo *per_seg_info
  );

/// @}

#ifdef __cplusplus
}
#endif

#endif // _OUTPUT_RESULTS_H
