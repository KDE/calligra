/*
 *  SPDX-FileCopyrightText: 2010 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KO_DUMMY_COLOR_PROFILE_H_
#define _KO_DUMMY_COLOR_PROFILE_H_

#include "KoColorProfile.h"

class KoDummyColorProfile : public KoColorProfile
{
public:
    KoDummyColorProfile();
    ~KoDummyColorProfile() override;
    KoColorProfile *clone() const override;
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
    QVector<qreal> getColorantsXYZ() const override;
    QVector<qreal> getColorantsxyY() const override;
    QVector<qreal> getWhitePointXYZ() const override;
    QVector<qreal> getWhitePointxyY() const override;
    QVector<qreal> getEstimatedTRC() const override;
    void linearizeFloatValue(QVector<qreal> &Value) const override;
    void delinearizeFloatValue(QVector<qreal> &Value) const override;
    void linearizeFloatValueFast(QVector<qreal> &Value) const override;
    void delinearizeFloatValueFast(QVector<qreal> &Value) const override;
    bool operator==(const KoColorProfile &) const override;
};

#endif
