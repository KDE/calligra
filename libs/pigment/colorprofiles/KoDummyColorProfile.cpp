/*
 *  Copyright (c) 2010 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KoDummyColorProfile.h"

KoDummyColorProfile::KoDummyColorProfile()
{
    setName("default");
}

KoDummyColorProfile::~KoDummyColorProfile()
{
}

KoColorProfile* KoDummyColorProfile::clone() const
{
    return new KoDummyColorProfile();
}

bool KoDummyColorProfile::valid() const
{
    return true;
}
float KoDummyColorProfile::version() const
{
    return 0.0;
}
bool KoDummyColorProfile::isSuitableForOutput() const
{
    return true;
}

bool KoDummyColorProfile::isSuitableForPrinting() const
{
    return true;
}

bool KoDummyColorProfile::isSuitableForDisplay() const
{
    return true;
}
bool KoDummyColorProfile::supportsPerceptual() const
{
    return true;
}
bool KoDummyColorProfile::supportsSaturation() const
{
    return true;
}
bool KoDummyColorProfile::supportsAbsolute() const
{
    return true;
}
bool KoDummyColorProfile::supportsRelative() const
{
    return true;
}
bool KoDummyColorProfile::hasColorants() const
{
    return true;
}
bool KoDummyColorProfile::hasTRC() const
{
    return true;
}
QVector<qreal> KoDummyColorProfile::getColorantsXYZ() const
{
    QVector<qreal> d50Dummy(3);
    d50Dummy<<0.34773<<0.35952<<1.0;
    return d50Dummy;
}

QVector<qreal> KoDummyColorProfile::getColorantsxyY() const
{
    QVector<qreal> d50Dummy(3);
    d50Dummy<<0.34773<<0.35952<<1.0;
    return d50Dummy;
}

QVector<qreal> KoDummyColorProfile::getWhitePointXYZ() const
{
    QVector<qreal> d50Dummy(3);
    d50Dummy<<0.9642<<1.0000<<0.8249;
    return d50Dummy;
}

QVector<qreal> KoDummyColorProfile::getWhitePointxyY() const
{
    QVector<qreal> d50Dummy(3);
    d50Dummy<<0.34773<<0.35952<<1.0;
    return d50Dummy;
}

QVector <qreal> KoDummyColorProfile::getEstimatedTRC() const

{
    QVector<qreal> Dummy(3);
    Dummy.fill(2.2);
    return Dummy;
}

void KoDummyColorProfile::linearizeFloatValue(QVector <qreal> & ) const
{
}

void KoDummyColorProfile::delinearizeFloatValue(QVector <qreal> & ) const
{
}
void KoDummyColorProfile::linearizeFloatValueFast(QVector <qreal> & ) const
{
}

void KoDummyColorProfile::delinearizeFloatValueFast(QVector <qreal> & ) const
{
}
bool KoDummyColorProfile::operator==(const KoColorProfile& rhs) const
{
    return dynamic_cast<const KoDummyColorProfile*>(&rhs);
}

