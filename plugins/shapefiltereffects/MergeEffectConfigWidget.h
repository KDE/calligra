/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef MERGEEFFECTCONFIGWIDGET_H
#define MERGEEFFECTCONFIGWIDGET_H

#include "KoFilterEffectConfigWidgetBase.h"

class KoFilterEffect;
class MergeEffect;

class MergeEffectConfigWidget : public KoFilterEffectConfigWidgetBase
{
    Q_OBJECT
public:
    explicit MergeEffectConfigWidget(QWidget *parent = nullptr);

    /// reimplemented from KoFilterEffectConfigWidgetBase
    bool editFilterEffect(KoFilterEffect *filterEffect) override;

private Q_SLOTS:

private:
    MergeEffect *m_effect;
};

#endif // MERGEEFFECTCONFIGWIDGET_H
