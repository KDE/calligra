/* This file is part of the KDE project
   Copyright (C) 2002 Lennart Kudling <kudling@kde.org>
   Copyright (C) 2004 Laurent Montel <montel@kde.org>
   Copyright (C) 2005-2006 Thomas Zander <zander@kde.org>
   Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2008 Jan Hambrecht <jaham@gmx.net>

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

#include "KarbonGlobal.h"
#include <math.h>

int KarbonGlobal::binomialCoeff(unsigned n, unsigned k)
{
    return
        static_cast<int>(
            0.5 +
            exp(
                factorialLn(n) -
                factorialLn(k) -
                factorialLn(n - k)));
}

qreal KarbonGlobal::factorialLn(unsigned n)
{
    const unsigned cacheSize = 100;

    // A static array is initialized to zero.
    static qreal cache[ cacheSize ];


    if (n <= 1)
        return 0.0;

    if (n <= cacheSize - 1) {
        return cache[ n ]
               ? cache[ n ]
               : (cache[ n ] = gammaLn(n + 1.0));
    } else {
        return gammaLn(n + 1.0);
    }
}

qreal KarbonGlobal::gammaLn(qreal x)
{
    static qreal coeff[ 6 ] = {
        76.18009172947146,
        -86.50532032941677,
        24.01409824083091,
        -1.231739572450155,
        0.1208650973866179e-2,
        -0.5395239384953e-5
    };

    qreal y = x;

    qreal tmp = x + 5.5;
    tmp -= (x + 0.5) * log(tmp);

    qreal ser = 1.000000000190015;

    for (int i = 0; i < 5; ++i) {
        ser += coeff[ i ] / ++y;
    }

    return -tmp + log(2.5066282746310005 * ser / x);
}

qreal KarbonGlobal::scalarProduct(const QPointF &p1, const QPointF &p2)
{
    return p1.x() * p2.x() + p1.y() * p2.y();
}

bool KarbonGlobal::pointsAreNear(const QPointF &p1, const QPointF &p2, qreal range)
{
    return (p2.x() >= p1.x() - range && p2.x() <= p1.x() + range && p2.y() >= p1.y() - range && p2.y() <= p1.y() + range);
}

QPointF KarbonGlobal::crossProduct(const QPointF &v1, const QPointF &v2)
{
    return QPointF(v1.x() * v2.y(), -v1.y()*v2.x());
}
