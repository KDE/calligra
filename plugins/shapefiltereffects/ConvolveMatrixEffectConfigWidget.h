/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef CONVOLVEMATRIXEFFECTCONFIGWIDGET_H
#define CONVOLVEMATRIXEFFECTCONFIGWIDGET_H

#include "KoFilterEffectConfigWidgetBase.h"

class QDoubleSpinBox;
class KoFilterEffect;
class ConvolveMatrixEffect;
class KComboBox;
class QSpinBox;
class QCheckBox;
class MatrixDataModel;

class ConvolveMatrixEffectConfigWidget : public KoFilterEffectConfigWidgetBase
{
    Q_OBJECT
public:
    explicit ConvolveMatrixEffectConfigWidget(QWidget *parent = nullptr);

    /// reimplemented from KoFilterEffectConfigWidgetBase
    bool editFilterEffect(KoFilterEffect *filterEffect) override;

private Q_SLOTS:
    void orderChanged(int value);
    void targetChanged(int value);
    void divisorChanged(double divisor);
    void biasChanged(double bias);
    void edgeModeChanged(int mode);
    void preserveAlphaChanged(bool checked);
    void editKernel();
    void kernelChanged();

private:
    ConvolveMatrixEffect *m_effect;
    KComboBox *m_edgeMode;
    QSpinBox *m_orderX;
    QSpinBox *m_orderY;
    QSpinBox *m_targetX;
    QSpinBox *m_targetY;
    QDoubleSpinBox *m_divisor;
    QDoubleSpinBox *m_bias;
    QCheckBox *m_preserveAlpha;
    MatrixDataModel *m_matrixModel;
};

#endif // CONVOLVEMATRIXEFFECTCONFIGWIDGET_H
