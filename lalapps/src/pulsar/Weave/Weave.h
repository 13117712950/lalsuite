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

#ifndef _WEAVE_H
#define _WEAVE_H

///
/// \defgroup lalapps_pulsar_Weave Weave Search Application
/// \ingroup lalapps_pulsar_Apps
/// \author Karl Wette
///

///
/// \file
/// \ingroup lalapps_pulsar_Weave
///

#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include <lal/LALStdlib.h>
#include <lal/LALStdio.h>
#include <lal/LALString.h>
#include <lal/PulsarDataTypes.h>
#include <lal/GSLHelpers.h>
#include <lal/FITSFileIO.h>

#include <LALAppsVCSInfo.h>

#ifdef __GNUC__
#define UNUSED __attribute__ ((unused))
#else
#define UNUSED
#endif

///
/// Compare two quantities, and return a sort order value if they are unequal
///
#define WEAVE_COMPARE_BY( x, y ) do { \
    if ( (x) < (y) ) return -1; \
    if ( (x) > (y) ) return +1; \
  } while(0)

#ifdef __cplusplus
extern "C" {
#endif

///
/// Bitflags representing search simulation levels
///
typedef enum {
  /// Simulate search (implicitly with full memory allocation)
  WEAVE_SIMULATE                        = 0001,
  /// Simulate search with minimal memory allocation
  WEAVE_SIMULATE_MIN_MEM                = 0002,
} WeaveSimulationLevel;

///
/// Function which transforms a point from physical coordinates to lattice tiling coordinates
///
typedef int ( *WeavePhysicalToLattice )( gsl_vector *out_latt, const PulsarDopplerParams *in_phys, const void *transf_data );

///
/// Function which transforms a point from lattice tiling coordinates to physical coordinates
///
typedef int ( *WeaveLatticeToPhysical )( PulsarDopplerParams *out_phys, const gsl_vector *in_latt, const gsl_vector *ref_latt, const void *transf_data );

///
/// Output result item
///
typedef struct tagWeaveOutputResultItem {
  /// Physical right ascension of semicoherent template
  REAL8 semi_alpha;
  /// Physical right ascension of coherent templates (only needed for per-segment output)
  REAL8 *coh_alpha;
  /// Physical declination of semicoherent template
  REAL8 semi_delta;
  /// Physical declination of coherent templates (only needed for per-segment output)
  REAL8 *coh_delta;
  /// Physical frequency and spindowns of semicoherent template
  REAL8 semi_fkdot[PULSAR_MAX_SPINS];
  /// Physical frequency and spindowns of coherent templates (only needed for per-segment output)
  REAL8 *coh_fkdot[PULSAR_MAX_SPINS];
  /// Mean multi-detector F-statistic
  REAL4 mean2F;
  /// Coherent multi-detector F-statistics (only needed for per-segment output)
  REAL4 *coh2F;
  /// Mean per-detector F-statistic (only needed for per-detector output)
  REAL4 mean2F_det[PULSAR_MAX_DETECTORS];
  /// Coherent per-detector F-statistics (only needed for per-detector and per-segment output)
  REAL4 *coh2F_det[PULSAR_MAX_DETECTORS];
} WeaveOutputResultItem;

///
/// Output miscellaneous per-segment information
///
typedef struct tagWeaveOutputMiscPerSegInfo {
  /// Start time of segment
  LIGOTimeGPS segment_start;
  /// End time of segment
  LIGOTimeGPS segment_end;
  /// Timestamp of first SFT from each detector
  LIGOTimeGPS sft_first[PULSAR_MAX_DETECTORS];
  /// Timestamp of last SFT from each detector
  LIGOTimeGPS sft_last[PULSAR_MAX_DETECTORS];
  /// Number of SFTs from each detector
  INT4 sft_count[PULSAR_MAX_DETECTORS];
  /// Minimum of frequency range covered by SFTs
  REAL8 sft_min_cover_freq;
  /// Maximum of frequency range covered by SFTs
  REAL8 sft_max_cover_freq;
  /// Number of coherent results computed once
  INT4 coh_n1comp;
  /// Number of recomputed coherent results
  INT4 coh_nrecomp;
} WeaveOutputMiscPerSegInfo;

#ifdef __cplusplus
}
#endif

#endif // _WEAVE_H
