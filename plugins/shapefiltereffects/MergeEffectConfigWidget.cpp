/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "MergeEffectConfigWidget.h"
#include "KoFilterEffect.h"
#include "MergeEffect.h"
#include <QGridLayout>

MergeEffectConfigWidget::MergeEffectConfigWidget(QWidget *parent)
    : KoFilterEffectConfigWidgetBase(parent)
    , m_effect(nullptr)
{
    QGridLayout *g = new QGridLayout(this);

    setLayout(g);
}

bool MergeEffectConfigWidget::editFilterEffect(KoFilterEffect *filterEffect)
{
    m_effect = dynamic_cast<MergeEffect *>(filterEffect);
    if (!m_effect)
        return false;

    return true;
}
