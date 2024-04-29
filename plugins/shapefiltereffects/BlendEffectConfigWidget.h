/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef BLENDEFFECTCONFIGWIDGET_H
#define BLENDEFFECTCONFIGWIDGET_H

#include "KoFilterEffectConfigWidgetBase.h"

class BlendEffect;
class KoFilterEffect;
class KComboBox;

class BlendEffectConfigWidget : public KoFilterEffectConfigWidgetBase
{
    Q_OBJECT
public:
    explicit BlendEffectConfigWidget(QWidget *parent = nullptr);

    /// reimplemented from KoFilterEffectConfigWidgetBase
    bool editFilterEffect(KoFilterEffect *filterEffect) override;

private Q_SLOTS:
    void modeChanged(int index);

private:
    KComboBox *m_mode;
    BlendEffect *m_effect;
};

#endif // BLENDEFFECTCONFIGWIDGET_H
