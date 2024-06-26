/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001-2002 Lennart Kudling <kudling@kde.org>
   SPDX-FileCopyrightText: 2002-2003 Rob Buis <buis@kde.org>
   SPDX-FileCopyrightText: 2005 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2006 Tim Beaulen <tbscope@gmail.com>
   SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KARBONGLOBAL_H
#define KARBONGLOBAL_H

#include <karboncommon_export.h>

#include <QPointF>

// define some often used mathematical constants et al:

// TODO: optimize those values.

namespace KarbonGlobal
{
const qreal pi = 3.14159265358979323846; // pi
const qreal twopi = 6.28318530717958647692; // 2pi
const qreal pi_2 = 1.57079632679489661923; // pi/2
const qreal pi_180 = 0.01745329251994329576; // pi/180
const qreal one_pi_180 = 57.29577951308232087684; // 180/pi
const qreal sqrt2 = 1.41421356237309504880; // sqrt(2)
const qreal one_3 = 0.33333333333333333333; // 1/3
const qreal two_3 = 0.66666666666666666667; // 2/3
const qreal one_6 = 0.16666666666666666667; // 1/6
const qreal one_7 = 0.14285714285714285714; // 1/7

/**
 * Constants used to decide if a number is equal zero or nearly the same
 * as another number.
 */
const qreal veryBigNumber = 1.0e8;
const qreal verySmallNumber = 1.0e-8;

/**
 * A bezier with this flatness is considered "flat". Used in subdividing.
 */
const qreal flatnessTolerance = 0.01;

/**
 * A tolerance used to approximate bezier lengths. If the relative difference
 * between chordlength and polylength (length of the controlpolygon) is smaller
 * than this value, the length of the bezier is 1/2 chordlength + 1/2 polylength.
 */
const qreal lengthTolerance = 0.005;

/**
 * A tolerance used to calculate param t on a segment at a given arc
 * length (counting from t=0).
 * If the relative difference between a length approximation and the given
 * length is smaller than this value, they are assumed to be identical.
 */
const qreal paramLengthTolerance = 0.001;

/**
 * A range for the isNear() check, to decide if a QPointF "is the same"
 * as another.
 */
const qreal isNearRange = 0.001;

/**
 * A tolerance for multiplying normalized (length=1) vectors. A result of
 * >= parallelTolerance indicates parallel vectors.
 */
const qreal parallelTolerance = 0.99;

/**
 * Returns the sign of parameter a.
 */
inline int sign(qreal a)
{
    return a < 0.0 ? -1 : 1;
}

/**
 * Calculates the binomial coefficient n! / ( k! * ( n - k)! ).
 */
int binomialCoeff(unsigned n, unsigned k);

/**
 * Calculates the value ln( n! ).
 */
qreal factorialLn(unsigned n);

/**
 * Calculates the value ln| Gamma(x) | for x > 0.
 */
qreal gammaLn(qreal x);

/// Returns scalar product of two given vectors
KARBONCOMMON_EXPORT qreal scalarProduct(const QPointF &p1, const QPointF &p2);

bool pointsAreNear(const QPointF &p1, const QPointF &p2, qreal range);

/// Returns the cross product of two given vectors
QPointF crossProduct(const QPointF &v1, const QPointF &v2);
}

#endif // KARBONGLOBAL_H
