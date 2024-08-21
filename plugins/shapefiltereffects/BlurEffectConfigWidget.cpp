/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "BlurEffectConfigWidget.h"
#include "BlurEffect.h"
#include "KoFilterEffect.h"

#include <KLocalizedString>
#include <QSpinBox>

#include <QGridLayout>
#include <QLabel>

BlurEffectConfigWidget::BlurEffectConfigWidget(QWidget *parent)
    : KoFilterEffectConfigWidgetBase(parent)
    , m_effect(nullptr)
{
    QGridLayout *g = new QGridLayout(this);

    g->addWidget(new QLabel(i18n("Radius"), this), 0, 0);
    m_stdDeviation = new QDoubleSpinBox(this);
    m_stdDeviation->setRange(0.0, 100);
    m_stdDeviation->setSingleStep(0.5);
    g->addWidget(m_stdDeviation, 0, 1);
    setLayout(g);

    connect(m_stdDeviation, &QDoubleSpinBox::valueChanged, this, &BlurEffectConfigWidget::stdDeviationChanged);
}

bool BlurEffectConfigWidget::editFilterEffect(KoFilterEffect *filterEffect)
{
    m_effect = dynamic_cast<BlurEffect *>(filterEffect);
    if (!m_effect)
        return false;

    m_stdDeviation->setValue(m_effect->deviation().x() * 100.0);
    return true;
}

void BlurEffectConfigWidget::stdDeviationChanged(double stdDeviation)
{
    if (!m_effect)
        return;

    qreal newDev = 0.01 * stdDeviation;
    m_effect->setDeviation(QPointF(newDev, newDev));
    Q_EMIT filterChanged();
}
