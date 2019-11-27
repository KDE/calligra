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

#ifndef _KO_DUMMY_COLOR_PROFILE_H_
#define _KO_DUMMY_COLOR_PROFILE_H_

#include "KoColorProfile.h"

class KoDummyColorProfile : public KoColorProfile
{
public:
    KoDummyColorProfile();
    ~KoDummyColorProfile() override;
    KoColorProfile* clone() const override;
    bool valid() const override;
    float version() const override;
    bool isSuitableForOutput() const override;
    bool isSuitableForPrinting() const override;
    bool isSuitableForDisplay() const override;
    bool supportsPerceptual() const override;
    bool supportsSaturation() const override;
    bool supportsAbsolute() const override;
    bool supportsRelative() const override;
    bool hasColorants() const override;
    bool hasTRC() const override;
    QVector <qreal> getColorantsXYZ() const override;
    QVector <qreal> getColorantsxyY() const override;
    QVector <qreal> getWhitePointXYZ() const override;
    QVector <qreal> getWhitePointxyY() const override;
    QVector <qreal> getEstimatedTRC() const override;
    void linearizeFloatValue(QVector <qreal> & Value) const override;
    void delinearizeFloatValue(QVector <qreal> & Value) const override;
    void linearizeFloatValueFast(QVector <qreal> & Value) const override;
    void delinearizeFloatValueFast(QVector <qreal> & Value) const override;
    bool operator==(const KoColorProfile&) const override;
};

#endif
