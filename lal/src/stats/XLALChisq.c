/*
 * Copyright (C) 2005,2007,2008,2010,2015  Kipp Cannon
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#include <math.h>


#include <gsl/gsl_cdf.h>
#include <gsl/gsl_sf.h>


#include <lal/LALChisq.h>
#include <lal/XLALGSL.h>
#include <lal/XLALError.h>


/**
 * Natural logarithm of complementary cumulative probability function for
 * Chi Squared distribution.
 *
 * Returns natural logarithm of probability that \f$x_{1}^{2} + \cdots +
 * x_{\mathrm{dof}}^{2} \geq \chi^{2}\f$, where \f$x_{1}, \ldots,
 * x_{\mathrm{dof}}\f$ are independent zero mean unit variance Gaussian
 * random variables.  The integral expression is \f$\ln Q = \ln
 * \int_{\chi^{2}/2}^{\infty} x^{\frac{n}{2}-1} \mathrm{e}^{-x} /
 * \Gamma(n/2) \, \mathrm{d}x\f$, where \f$n = \mathrm{dof} =\f$ number of
 * degrees of freedom.
 */
double XLALLogChisqCCDF(
	double chi2,
	double dof
)
{
	/*
	 * Notes:
	 *
	 * Q(chi^2, dof) = Gamma(dof/2, chi^2/2) / Gamma(dof/2)
	 *
	 * The incomplete gamma function, Gamma(a, x), can by approximated by
	 * computing the first few terms of
	 *
	 * Gamma(a, x) = e^-x x^(a-1) [ 1 + (a-1)/x + (a-1)(a-2)/x^2 + ... ]
	 *
	 * See Abramowitz and Stegun, (6.5.32).
	 */

	double ln_prob;

	if((chi2 < 0.0) || (dof <= 0.0))
		XLAL_ERROR_REAL8(XLAL_EDOM);

	/* start with a high precision technique for large probabilities */
	XLAL_CALLGSL(ln_prob = log(gsl_cdf_chisq_Q(chi2, dof)));

	/* if the result turned out to be very small, then recompute it using
	 * an asymptotic approximation */
	if(ln_prob < -600.) {
		const double a = dof / 2.;
		const double x = chi2 / 2.;
		double term;
		int i;

		/* calculate the log of the numerator */
		for(i = 1, ln_prob = term = 1.; (i < a) && fabs(term/ln_prob) > 1e-17; i++) {
			term *= (a - i) / x;
			ln_prob += term;
		}
		ln_prob = (a - 1) * log(x) - x + log(ln_prob);

		/* subtract the log of the denominator */
		XLAL_CALLGSL(ln_prob -= gsl_sf_lngamma(a));
	}

	/* check that the final answer is the log of a legal probability */
	if(ln_prob > 0.0)
		XLAL_ERROR_REAL8(XLAL_ERANGE);

	return ln_prob;
}