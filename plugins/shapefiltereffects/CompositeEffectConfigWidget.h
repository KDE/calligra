/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef COMPOSITEEFFECTCONFIGWIDGET_H
#define COMPOSITEEFFECTCONFIGWIDGET_H

#include "KoFilterEffectConfigWidgetBase.h"

class KoFilterEffect;
class CompositeEffect;
class QDoubleSpinBox;
class KComboBox;

class CompositeEffectConfigWidget : public KoFilterEffectConfigWidgetBase
{
    Q_OBJECT
public:
    explicit CompositeEffectConfigWidget(QWidget *parent = nullptr);

    /// reimplemented from KoFilterEffectConfigWidgetBase
    bool editFilterEffect(KoFilterEffect *filterEffect) override;

private Q_SLOTS:
    void valueChanged();
    void operationChanged(int index);

private:
    CompositeEffect *m_effect;
    KComboBox *m_operation;
    QDoubleSpinBox *m_k[4];
    QWidget *m_arithmeticWidget;
};

#endif // COMPOSITEEFFECTCONFIGWIDGET_H
