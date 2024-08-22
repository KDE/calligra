/*
 *  SPDX-FileCopyrightText: 2010 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KoDummyColorProfile.h"

KoDummyColorProfile::KoDummyColorProfile()
{
    setName("default");
}

KoDummyColorProfile::~KoDummyColorProfile() = default;

KoColorProfile *KoDummyColorProfile::clone() const
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
    d50Dummy << 0.34773 << 0.35952 << 1.0;
    return d50Dummy;
}

QVector<qreal> KoDummyColorProfile::getColorantsxyY() const
{
    QVector<qreal> d50Dummy(3);
    d50Dummy << 0.34773 << 0.35952 << 1.0;
    return d50Dummy;
}

QVector<qreal> KoDummyColorProfile::getWhitePointXYZ() const
{
    QVector<qreal> d50Dummy(3);
    d50Dummy << 0.9642 << 1.0000 << 0.8249;
    return d50Dummy;
}

QVector<qreal> KoDummyColorProfile::getWhitePointxyY() const
{
    QVector<qreal> d50Dummy(3);
    d50Dummy << 0.34773 << 0.35952 << 1.0;
    return d50Dummy;
}

QVector<qreal> KoDummyColorProfile::getEstimatedTRC() const

{
    QVector<qreal> Dummy(3);
    Dummy.fill(2.2);
    return Dummy;
}

void KoDummyColorProfile::linearizeFloatValue(QVector<qreal> &) const
{
}

void KoDummyColorProfile::delinearizeFloatValue(QVector<qreal> &) const
{
}
void KoDummyColorProfile::linearizeFloatValueFast(QVector<qreal> &) const
{
}

void KoDummyColorProfile::delinearizeFloatValueFast(QVector<qreal> &) const
{
}
bool KoDummyColorProfile::operator==(const KoColorProfile &rhs) const
{
    return dynamic_cast<const KoDummyColorProfile *>(&rhs);
}
