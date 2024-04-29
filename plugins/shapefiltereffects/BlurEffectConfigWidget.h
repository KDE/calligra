/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef BLUREFFECTCONFIGWIDGET_H
#define BLUREFFECTCONFIGWIDGET_H

#include "KoFilterEffectConfigWidgetBase.h"

class KoFilterEffect;
class BlurEffect;
class QDoubleSpinBox;

class BlurEffectConfigWidget : public KoFilterEffectConfigWidgetBase
{
    Q_OBJECT
public:
    explicit BlurEffectConfigWidget(QWidget *parent = nullptr);

    /// reimplemented from KoFilterEffectConfigWidgetBase
    bool editFilterEffect(KoFilterEffect *filterEffect) override;

private Q_SLOTS:
    void stdDeviationChanged(double stdDeviation);

private:
    BlurEffect *m_effect;
    QDoubleSpinBox *m_stdDeviation;
};

#endif // BLUREFFECTCONFIGWIDGET_H
