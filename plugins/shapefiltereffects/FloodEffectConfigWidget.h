/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef FLOODEFFECTCONFIGWIDGET_H
#define FLOODEFFECTCONFIGWIDGET_H

#include "KoFilterEffectConfigWidgetBase.h"

class KoFilterEffect;
class FloodEffect;
class KoColorPopupAction;

class FloodEffectConfigWidget : public KoFilterEffectConfigWidgetBase
{
    Q_OBJECT
public:
    explicit FloodEffectConfigWidget(QWidget *parent = nullptr);

    /// reimplemented from KoFilterEffectConfigWidgetBase
    bool editFilterEffect(KoFilterEffect *filterEffect) override;

private Q_SLOTS:
    void colorChanged();

private:
    FloodEffect *m_effect;
    KoColorPopupAction *m_actionStopColor;
};

#endif // FLOODEFFECTCONFIGWIDGET_H
