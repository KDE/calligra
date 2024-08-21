/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "OffsetEffectConfigWidget.h"
#include "KoFilterEffect.h"
#include "OffsetEffect.h"
#include <KLocalizedString>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>

const qreal OffsetLimit = 100.0;

OffsetEffectConfigWidget::OffsetEffectConfigWidget(QWidget *parent)
    : KoFilterEffectConfigWidgetBase(parent)
    , m_effect(nullptr)
{
    QGridLayout *g = new QGridLayout(this);

    g->addWidget(new QLabel(i18n("dx"), this), 0, 0);
    m_offsetX = new QDoubleSpinBox(this);
    m_offsetX->setRange(-OffsetLimit, OffsetLimit);
    m_offsetX->setSingleStep(1.0);
    g->addWidget(m_offsetX, 0, 1);

    g->addWidget(new QLabel(i18n("dy"), this), 0, 2);
    m_offsetY = new QDoubleSpinBox(this);
    m_offsetY->setRange(-OffsetLimit, OffsetLimit);
    m_offsetY->setSingleStep(1.0);
    g->addWidget(m_offsetY, 0, 3);
    setLayout(g);

    connect(m_offsetX, &QDoubleSpinBox::valueChanged, this, &OffsetEffectConfigWidget::offsetChanged);
    connect(m_offsetY, &QDoubleSpinBox::valueChanged, this, &OffsetEffectConfigWidget::offsetChanged);
}

bool OffsetEffectConfigWidget::editFilterEffect(KoFilterEffect *filterEffect)
{
    m_effect = dynamic_cast<OffsetEffect *>(filterEffect);
    if (!m_effect)
        return false;

    m_offsetX->blockSignals(true);
    m_offsetY->blockSignals(true);
    m_offsetX->setValue(m_effect->offset().x() * 100.0);
    m_offsetY->setValue(m_effect->offset().y() * 100.0);
    m_offsetX->blockSignals(false);
    m_offsetY->blockSignals(false);

    return true;
}

void OffsetEffectConfigWidget::offsetChanged(double /*offset*/)
{
    if (!m_effect)
        return;

    m_effect->setOffset(0.01 * QPointF(m_offsetX->value(), m_offsetY->value()));
    Q_EMIT filterChanged();
}
