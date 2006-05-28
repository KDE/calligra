/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __VGLOBAL_H__
#define __VGLOBAL_H__

#include <kglobal.h>
#include <QPointF>
#include <QMatrix>

// define some often used mathematical constants et al:

// TODO: optimize those values.

namespace VGlobal
{
	const double pi			=  3.14159265358979323846;	// pi
	const double twopi		=  6.28318530717958647692;	// 2pi
	const double pi_2		=  1.57079632679489661923;	// pi/2
	const double pi_180		=  0.01745329251994329576;	// pi/180
	const double one_pi_180	= 57.29577951308232087684;	// 180/pi
	const double sqrt2		=  1.41421356237309504880;	// sqrt(2)
	const double one_3		=  0.33333333333333333333;	// 1/3
	const double two_3		=  0.66666666666666666667;	// 2/3
	const double one_6		=  0.16666666666666666667;	// 1/6
	const double one_7		=  0.14285714285714285714;	// 1/7

	/**
	 * Constants used to decide if a number is equal zero or nearly the same
	 * as another number.
	 */
	const double veryBigNumber = 1.0e8;
	const double verySmallNumber = 1.0e-8;

	/**
	 * A bezier with this flatness is considered "flat". Used in subdividing.
	 */
	const double flatnessTolerance = 0.01;

	/**
	 * A tolerance used to approximate bezier lengths. If the relative difference
	 * between chordlength and polylength (length of the controlpolygon) is smaller
	 * than this value, the length of the bezier is 1/2 chordlength + 1/2 polylength.
	 */
	const double lengthTolerance = 0.005;

	/**
	 * A tolerance used to calculate param t on a segment at a given arc
	 * length (counting from t=0).
	 * If the relative difference between a length approximation and the given
	 * length is smaller than this value, they are assumed to be identical.
	 */
	const double paramLengthTolerance = 0.001;

	/**
	 * A range for the isNear() check, to decide if a QPointF "is the same"
	 * as another.
	 */
	const double isNearRange = 0.001;

	/**
	 * A tolerance for multiplying normalized (length=1) vectors. A result of
	 * >= parallelTolerance indicates parallel vectors.
	 */
	const double parallelTolerance = 0.99;

	/**
	 * Returns the sign of paramater a.
	 */
	inline int sign( double a )
	{
		return a < 0.0
			   ? -1
			   : 1;
	}

	/**
	 * Calculates the binomial coefficient n! / ( k! * ( n - k)! ).
	 */
	int binomialCoeff( unsigned n, unsigned k );

	/**
	 * Calculates the value ln( n! ).
	 */
	double factorialLn( unsigned n );

	/**
	 * Calculates the value ln| Gamma(x) | for x > 0.
	 */
	double gammaLn( double x );

	QPointF transformPoint(const QPointF &p, const QMatrix &m);
	double multiplyPoints(const QPointF &p1, const QPointF &p2);
	bool pointsAreNear(const QPointF &p1, const QPointF &p2, double range);
}

#endif

