/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef COMPONENTTRANSFEREFFECTCONFIGWIDGET_H
#define COMPONENTTRANSFEREFFECTCONFIGWIDGET_H

#include "ComponentTransferEffect.h"
#include "KoFilterEffectConfigWidgetBase.h"

class KoFilterEffect;
class QDoubleSpinBox;
class KComboBox;
class KLineEdit;
class QStackedWidget;

class ComponentTransferEffectConfigWidget : public KoFilterEffectConfigWidgetBase
{
    Q_OBJECT
public:
    explicit ComponentTransferEffectConfigWidget(QWidget *parent = nullptr);

    /// reimplemented from KoFilterEffectConfigWidgetBase
    bool editFilterEffect(KoFilterEffect *filterEffect) override;

private Q_SLOTS:
    void slopeChanged(double slope);
    void interceptChanged(double intercept);
    void amplitudeChanged(double amplitude);
    void exponentChanged(double exponent);
    void offsetChanged(double offset);
    void functionChanged(int index);
    void channelSelected(int channel);
    void tableValuesChanged();
    void discreteValuesChanged();

private:
    void updateControls();

    ComponentTransferEffect *m_effect;
    KComboBox *m_function;
    QStackedWidget *m_stack;
    KLineEdit *m_tableValues;
    KLineEdit *m_discreteValues;
    QDoubleSpinBox *m_slope;
    QDoubleSpinBox *m_intercept;
    QDoubleSpinBox *m_amplitude;
    QDoubleSpinBox *m_exponent;
    QDoubleSpinBox *m_offset;
    ComponentTransferEffect::Channel m_currentChannel;
};

#endif // COMPONENTTRANSFEREFFECTCONFIGWIDGET_H
