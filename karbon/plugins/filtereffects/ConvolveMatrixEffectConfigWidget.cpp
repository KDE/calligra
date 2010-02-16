/* This file is part of the KDE project
 * Copyright (c) 2010 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "ConvolveMatrixEffectConfigWidget.h"
#include "ConvolveMatrixEffect.h"
#include "KoFilterEffect.h"

#include <KNumInput>
#include <KLocale>

#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QRadioButton>
#include <QtGui/QButtonGroup>

ConvolveMatrixEffectConfigWidget::ConvolveMatrixEffectConfigWidget(QWidget *parent)
        : KoFilterEffectConfigWidgetBase(parent), m_effect(0)
{
    QGridLayout * g = new QGridLayout(this);

    m_edgeMode = new QButtonGroup(this);
    QHBoxLayout * buttons = new QHBoxLayout();
    QRadioButton * duplicate = new QRadioButton(i18n("Duplicate"), this);
    QRadioButton * wrap = new QRadioButton(i18n("Wrap"), this);
    QRadioButton * none = new QRadioButton(i18n("None"), this);
    m_edgeMode->addButton(duplicate, ConvolveMatrixEffect::Duplicate);
    m_edgeMode->addButton(wrap, ConvolveMatrixEffect::Wrap);
    m_edgeMode->addButton(none, ConvolveMatrixEffect::None);
    g->addWidget(new QLabel(i18n("Edge mode:"), this), 0, 0);
    buttons->addWidget(duplicate);
    buttons->addWidget(wrap);
    buttons->addWidget(none);
    g->addLayout(buttons, 0, 1);

    g->addWidget(new QLabel(i18n("Kernel size:"), this), 1, 0);
    QHBoxLayout * order = new QHBoxLayout();
    m_orderX = new KIntNumInput(this);
    m_orderX->setRange(1, 100, 1, false);
    m_orderY = new KIntNumInput(this);
    m_orderY->setRange(1, 100, 1, false);
    order->addWidget(m_orderX);
    order->addWidget(new QLabel("X", this));
    order->addWidget(m_orderY);
    g->addLayout(order, 1, 1);
    setLayout(g);

    connect(m_edgeMode, SIGNAL(buttonClicked(int)), this, SLOT(edgeModeChanged(int)));
    connect(m_orderX, SIGNAL(valueChanged(int)), this, SLOT(orderXChanged(int)));
    connect(m_orderY, SIGNAL(valueChanged(int)), this, SLOT(orderYChanged(int)));
}

bool ConvolveMatrixEffectConfigWidget::editFilterEffect(KoFilterEffect * filterEffect)
{
    m_effect = dynamic_cast<ConvolveMatrixEffect*>(filterEffect);
    if (!m_effect)
        return false;

    m_edgeMode->blockSignals(true);
    m_edgeMode->button(m_effect->edgeMode())->setChecked(true);
    m_edgeMode->blockSignals(false);
    m_orderX->blockSignals(true);
    m_orderX->setValue(m_effect->order().x());
    m_orderX->blockSignals(false);
    m_orderY->blockSignals(true);
    m_orderY->setValue(m_effect->order().y());
    m_orderY->blockSignals(false);

    return true;
}

void ConvolveMatrixEffectConfigWidget::edgeModeChanged(int id)
{
    if (!m_effect)
        return;

    switch(id) {
        case ConvolveMatrixEffect::Duplicate:
            m_effect->setEdgeMode(ConvolveMatrixEffect::Duplicate);
            break;
        case ConvolveMatrixEffect::Wrap:
            m_effect->setEdgeMode(ConvolveMatrixEffect::Wrap);
            break;
        case ConvolveMatrixEffect::None:
            m_effect->setEdgeMode(ConvolveMatrixEffect::None);
            break;
    }
    emit filterChanged();
}

void ConvolveMatrixEffectConfigWidget::orderXChanged(int x)
{
    if (!m_effect)
        return;

    QPoint order = m_effect->order();
    if (order.x() != x)
        m_effect->setOrder(QPoint(x, order.y()));

    emit filterChanged();
}

void ConvolveMatrixEffectConfigWidget::orderYChanged(int y)
{
    if (!m_effect)
        return;

    QPoint order = m_effect->order();
    if (order.y() != y)
        m_effect->setOrder(QPoint(order.x(), y));

    emit filterChanged();
}

#include "ConvolveMatrixEffectConfigWidget.moc"
