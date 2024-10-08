/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "StarShapeConfigWidget.h"
#include "StarShape.h"
#include "StarShapeConfigCommand.h"

StarShapeConfigWidget::StarShapeConfigWidget()
{
    widget.setupUi(this);

    connect(widget.corners, &QSpinBox::valueChanged, this, &KoShapeConfigWidgetBase::propertyChanged);
    connect(widget.innerRadius, &QAbstractSpinBox::editingFinished, this, &KoShapeConfigWidgetBase::propertyChanged);
    connect(widget.outerRadius, &QAbstractSpinBox::editingFinished, this, &KoShapeConfigWidgetBase::propertyChanged);
    connect(widget.convex, &QCheckBox::stateChanged, this, &KoShapeConfigWidgetBase::propertyChanged);
    connect(widget.convex, &QAbstractButton::clicked, this, &StarShapeConfigWidget::typeChanged);
}

void StarShapeConfigWidget::setUnit(const KoUnit &unit)
{
    widget.innerRadius->setUnit(unit);
    widget.outerRadius->setUnit(unit);
}

void StarShapeConfigWidget::open(KoShape *shape)
{
    m_star = dynamic_cast<StarShape *>(shape);
    if (!m_star)
        return;

    widget.corners->blockSignals(true);
    widget.innerRadius->blockSignals(true);
    widget.outerRadius->blockSignals(true);
    widget.convex->blockSignals(true);

    widget.corners->setValue(m_star->cornerCount());
    widget.innerRadius->changeValue(m_star->baseRadius());
    widget.outerRadius->changeValue(m_star->tipRadius());
    widget.convex->setCheckState(m_star->convex() ? Qt::Checked : Qt::Unchecked);
    typeChanged();

    widget.corners->blockSignals(false);
    widget.innerRadius->blockSignals(false);
    widget.outerRadius->blockSignals(false);
    widget.convex->blockSignals(false);
}

void StarShapeConfigWidget::save()
{
    if (!m_star)
        return;

    m_star->setCornerCount(widget.corners->value());
    m_star->setBaseRadius(widget.innerRadius->value());
    m_star->setTipRadius(widget.outerRadius->value());
    m_star->setConvex(widget.convex->checkState() == Qt::Checked);
}

KUndo2Command *StarShapeConfigWidget::createCommand()
{
    if (!m_star)
        return nullptr;
    else
        return new StarShapeConfigCommand(m_star,
                                          widget.corners->value(),
                                          widget.innerRadius->value(),
                                          widget.outerRadius->value(),
                                          widget.convex->checkState() == Qt::Checked);
}

void StarShapeConfigWidget::typeChanged()
{
    if (widget.convex->checkState() == Qt::Checked) {
        widget.innerRadius->setEnabled(false);
    } else {
        widget.innerRadius->setEnabled(true);
    }
}
