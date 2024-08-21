/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "MorphologyEffectConfigWidget.h"
#include "KoFilterEffect.h"
#include "MorphologyEffect.h"

#include <KLocalizedString>
#include <QSpinBox>

#include <QButtonGroup>
#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>

MorphologyEffectConfigWidget::MorphologyEffectConfigWidget(QWidget *parent)
    : KoFilterEffectConfigWidgetBase(parent)
    , m_effect(nullptr)
{
    QGridLayout *g = new QGridLayout(this);

    m_operator = new QButtonGroup(this);
    QRadioButton *erode = new QRadioButton(i18n("Erode"), this);
    QRadioButton *dilate = new QRadioButton(i18n("Dilate"), this);
    m_operator->addButton(erode, MorphologyEffect::Erode);
    m_operator->addButton(dilate, MorphologyEffect::Dilate);
    g->addWidget(new QLabel(i18n("Operator:"), this), 0, 0);
    g->addWidget(erode, 0, 1);
    g->addWidget(dilate, 0, 2);

    g->addWidget(new QLabel(i18n("Radius x:"), this), 1, 0);
    m_radiusX = new QDoubleSpinBox(this);
    m_radiusX->setRange(0.0, 100);
    m_radiusX->setSingleStep(0.5);
    g->addWidget(m_radiusX, 1, 1, 1, 2);

    g->addWidget(new QLabel(i18n("Radius y:"), this), 2, 0);
    m_radiusY = new QDoubleSpinBox(this);
    m_radiusY->setRange(0.0, 100);
    m_radiusY->setSingleStep(0.5);
    g->addWidget(m_radiusY, 2, 1, 1, 2);

    setLayout(g);

    connect(m_operator, &QButtonGroup::idClicked, this, &MorphologyEffectConfigWidget::operatorChanged);
    connect(m_radiusX, &QDoubleSpinBox::valueChanged, this, &MorphologyEffectConfigWidget::radiusXChanged);
    connect(m_radiusY, &QDoubleSpinBox::valueChanged, this, &MorphologyEffectConfigWidget::radiusYChanged);
}

bool MorphologyEffectConfigWidget::editFilterEffect(KoFilterEffect *filterEffect)
{
    m_effect = dynamic_cast<MorphologyEffect *>(filterEffect);
    if (!m_effect)
        return false;

    m_operator->blockSignals(true);
    m_operator->button(m_effect->morphologyOperator())->setChecked(true);
    m_operator->blockSignals(false);
    m_radiusX->blockSignals(true);
    m_radiusX->setValue(m_effect->morphologyRadius().x() * 100);
    m_radiusX->blockSignals(false);
    m_radiusY->blockSignals(true);
    m_radiusY->setValue(m_effect->morphologyRadius().y() * 100);
    m_radiusY->blockSignals(false);

    return true;
}

void MorphologyEffectConfigWidget::operatorChanged(int id)
{
    if (!m_effect)
        return;

    switch (id) {
    case MorphologyEffect::Erode:
        m_effect->setMorphologyOperator(MorphologyEffect::Erode);
        break;
    case MorphologyEffect::Dilate:
        m_effect->setMorphologyOperator(MorphologyEffect::Dilate);
        break;
    }
    Q_EMIT filterChanged();
}

void MorphologyEffectConfigWidget::radiusXChanged(double x)
{
    if (!m_effect)
        return;

    QPointF radius = m_effect->morphologyRadius();
    if (radius.x() != x)
        m_effect->setMorphologyRadius(QPointF(x * 0.01, radius.y()));

    Q_EMIT filterChanged();
}

void MorphologyEffectConfigWidget::radiusYChanged(double y)
{
    if (!m_effect)
        return;

    QPointF radius = m_effect->morphologyRadius();
    if (radius.y() != y)
        m_effect->setMorphologyRadius(QPointF(radius.x(), y * 0.01));

    Q_EMIT filterChanged();
}
