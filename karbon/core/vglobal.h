/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VGLOBAL_H__
#define __VGLOBAL_H__

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
	const double one_7		=  0.14285714285714285714;	// 1/7

	/**
	 * Constants used to decide if a number is equal zero or nearly the same
	 * as another number.
	 */
	const double veryLargeNumber = 1.0e8;
	const double verySmallNumber = 1.0e-8;

	/**
	 * A bezier with this flatness is considered "flat". Used in subdividing.
	 */
	const double flatnessTolerance = 0.5;

	/**
	 * A tolerance used to approximate bezier lengths.
	 */
	const double lengthTolerance = 0.01;

	/**
	 * A range for KoPoint::isNear() check, to decide if a KoPoint "is the same"
	 * as another.
	 */
	const double isNearRange = 0.01;


	const short copyOffset = 10;        /// the amount at which copied objects get offset in x and y direction
	const short maxRecentFiles = 10;    /// default max. number of files shown in open recent menu item
}

#endif
