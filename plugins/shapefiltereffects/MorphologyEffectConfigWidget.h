/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef MORPHOLOGYEFFECTCONFIGWIDGET_H
#define MORPHOLOGYEFFECTCONFIGWIDGET_H

#include "KoFilterEffectConfigWidgetBase.h"

class KoFilterEffect;
class MorphologyEffect;
class QDoubleSpinBox;
class QButtonGroup;

class MorphologyEffectConfigWidget : public KoFilterEffectConfigWidgetBase
{
    Q_OBJECT
public:
    explicit MorphologyEffectConfigWidget(QWidget *parent = nullptr);

    /// reimplemented from KoFilterEffectConfigWidgetBase
    bool editFilterEffect(KoFilterEffect *filterEffect) override;

private Q_SLOTS:
    void radiusXChanged(double x);
    void radiusYChanged(double y);
    void operatorChanged(int op);

private:
    MorphologyEffect *m_effect;
    QButtonGroup *m_operator;
    QDoubleSpinBox *m_radiusX;
    QDoubleSpinBox *m_radiusY;
};

#endif // MORPHOLOGYEFFECTCONFIGWIDGET_H
