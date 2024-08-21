/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "FloodEffectConfigWidget.h"
#include "FloodEffect.h"
#include "KoFilterEffect.h"

#include <KoColorPopupAction.h>

#include <KLocalizedString>

#include <QGridLayout>
#include <QLabel>
#include <QToolButton>

FloodEffectConfigWidget::FloodEffectConfigWidget(QWidget *parent)
    : KoFilterEffectConfigWidgetBase(parent)
    , m_effect(nullptr)
{
    QGridLayout *g = new QGridLayout(this);

    g->addWidget(new QLabel(i18n("Flood color"), this), 0, 0);
    QToolButton *button = new QToolButton(this);
    g->addWidget(button, 0, 1);
    m_actionStopColor = new KoColorPopupAction(this);
    button->setDefaultAction(m_actionStopColor);
    setLayout(g);

    connect(m_actionStopColor, &KoColorPopupAction::colorChanged, this, &FloodEffectConfigWidget::colorChanged);
}

bool FloodEffectConfigWidget::editFilterEffect(KoFilterEffect *filterEffect)
{
    m_effect = dynamic_cast<FloodEffect *>(filterEffect);
    if (!m_effect)
        return false;

    m_actionStopColor->setCurrentColor(m_effect->floodColor());
    return true;
}

void FloodEffectConfigWidget::colorChanged()
{
    if (!m_effect)
        return;

    m_effect->setFloodColor(m_actionStopColor->currentColor());
    Q_EMIT filterChanged();
}
