/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef COLORMATRIXEFFECTCONFIGWIDGET_H
#define COLORMATRIXEFFECTCONFIGWIDGET_H

#include "KoFilterEffectConfigWidgetBase.h"

class ColorMatrixEffect;
class KoFilterEffect;
class KComboBox;
class QStackedWidget;
class QDoubleSpinBox;
class MatrixDataModel;

class ColorMatrixEffectConfigWidget : public KoFilterEffectConfigWidgetBase
{
    Q_OBJECT
public:
    explicit ColorMatrixEffectConfigWidget(QWidget *parent = nullptr);

    /// reimplemented from KoFilterEffectConfigWidgetBase
    bool editFilterEffect(KoFilterEffect *filterEffect) override;

private Q_SLOTS:
    void matrixChanged();
    void saturateChanged(double saturate);
    void hueRotateChanged(double angle);
    void typeChanged(int index);

private:
    KComboBox *m_type;
    ColorMatrixEffect *m_effect;
    MatrixDataModel *m_matrixModel;
    QStackedWidget *m_stack;
    QDoubleSpinBox *m_saturate;
    QDoubleSpinBox *m_hueRotate;
};

#endif // COLORMATRIXEFFECTCONFIGWIDGET_H
