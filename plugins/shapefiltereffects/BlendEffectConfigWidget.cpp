/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "BlendEffectConfigWidget.h"
#include "BlendEffect.h"
#include "KoFilterEffect.h"

#include <KComboBox>
#include <KLocalizedString>

#include <QGridLayout>
#include <QLabel>

BlendEffectConfigWidget::BlendEffectConfigWidget(QWidget *parent)
    : KoFilterEffectConfigWidgetBase(parent)
    , m_effect(nullptr)
{
    QGridLayout *g = new QGridLayout(this);

    g->addWidget(new QLabel(i18n("Blend mode"), this), 0, 0);
    m_mode = new KComboBox(this);
    m_mode->addItem(i18n("Normal"));
    m_mode->addItem(i18n("Multiply"));
    m_mode->addItem(i18n("Screen"));
    m_mode->addItem(i18n("Darken"));
    m_mode->addItem(i18n("Lighten"));
    g->addWidget(m_mode, 0, 1);
    g->addItem(new QSpacerItem(0, 1, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding), 1, 0);

    setLayout(g);

    connect(m_mode, QOverload<int>::of(&KComboBox::currentIndexChanged), this, &BlendEffectConfigWidget::modeChanged);
}

bool BlendEffectConfigWidget::editFilterEffect(KoFilterEffect *filterEffect)
{
    m_effect = dynamic_cast<BlendEffect *>(filterEffect);
    if (!m_effect)
        return false;

    m_mode->blockSignals(true);

    switch (m_effect->blendMode()) {
    case BlendEffect::Normal:
        m_mode->setCurrentIndex(0);
        break;
    case BlendEffect::Multiply:
        m_mode->setCurrentIndex(1);
        break;
    case BlendEffect::Screen:
        m_mode->setCurrentIndex(2);
        break;
    case BlendEffect::Darken:
        m_mode->setCurrentIndex(3);
        break;
    case BlendEffect::Lighten:
        m_mode->setCurrentIndex(4);
        break;
    }

    m_mode->blockSignals(false);

    return true;
}

void BlendEffectConfigWidget::modeChanged(int index)
{
    if (!m_effect)
        return;

    m_effect->setBlendMode(static_cast<BlendEffect::BlendMode>(index));

    Q_EMIT filterChanged();
}
