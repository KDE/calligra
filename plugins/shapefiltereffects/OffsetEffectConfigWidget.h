/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef OFFSETEFFECTCONFIGWIDGET_H
#define OFFSETEFFECTCONFIGWIDGET_H

#include "KoFilterEffectConfigWidgetBase.h"

class KoFilterEffect;
class OffsetEffect;
class QDoubleSpinBox;

class OffsetEffectConfigWidget : public KoFilterEffectConfigWidgetBase
{
    Q_OBJECT
public:
    explicit OffsetEffectConfigWidget(QWidget *parent = nullptr);

    /// reimplemented from KoFilterEffectConfigWidgetBase
    bool editFilterEffect(KoFilterEffect *filterEffect) override;

private Q_SLOTS:
    void offsetChanged(double offset);

private:
    OffsetEffect *m_effect;
    QDoubleSpinBox *m_offsetX;
    QDoubleSpinBox *m_offsetY;
};

#endif // OFFSETEFFECTCONFIGWIDGET_H
