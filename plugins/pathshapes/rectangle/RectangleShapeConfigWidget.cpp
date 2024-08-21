/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "RectangleShapeConfigWidget.h"
#include "RectangleShape.h"
#include "RectangleShapeConfigCommand.h"

RectangleShapeConfigWidget::RectangleShapeConfigWidget()
{
    widget.setupUi(this);

    connect(widget.cornerRadiusX, &QAbstractSpinBox::editingFinished, this, &KoShapeConfigWidgetBase::propertyChanged);
    connect(widget.cornerRadiusY, &QAbstractSpinBox::editingFinished, this, &KoShapeConfigWidgetBase::propertyChanged);
}

void RectangleShapeConfigWidget::setUnit(const KoUnit &unit)
{
    widget.cornerRadiusX->setUnit(unit);
    widget.cornerRadiusY->setUnit(unit);
}

void RectangleShapeConfigWidget::open(KoShape *shape)
{
    m_rectangle = dynamic_cast<RectangleShape *>(shape);
    if (!m_rectangle)
        return;

    widget.cornerRadiusX->blockSignals(true);
    widget.cornerRadiusY->blockSignals(true);

    QSizeF size = m_rectangle->size();

    widget.cornerRadiusX->setMaximum(0.5 * size.width());
    widget.cornerRadiusX->changeValue(0.01 * m_rectangle->cornerRadiusX() * 0.5 * size.width());
    widget.cornerRadiusY->setMaximum(0.5 * size.height());
    widget.cornerRadiusY->changeValue(0.01 * m_rectangle->cornerRadiusY() * 0.5 * size.height());

    widget.cornerRadiusX->blockSignals(false);
    widget.cornerRadiusY->blockSignals(false);
}

void RectangleShapeConfigWidget::save()
{
    if (!m_rectangle)
        return;

    QSizeF size = m_rectangle->size();

    m_rectangle->setCornerRadiusX(100.0 * widget.cornerRadiusX->value() / (0.5 * size.width()));
    m_rectangle->setCornerRadiusY(100.0 * widget.cornerRadiusY->value() / (0.5 * size.height()));
}

KUndo2Command *RectangleShapeConfigWidget::createCommand()
{
    if (!m_rectangle)
        return nullptr;
    QSizeF size = m_rectangle->size();

    qreal cornerRadiusX = 100.0 * widget.cornerRadiusX->value() / (0.5 * size.width());
    qreal cornerRadiusY = 100.0 * widget.cornerRadiusY->value() / (0.5 * size.height());

    return new RectangleShapeConfigCommand(m_rectangle, cornerRadiusX, cornerRadiusY);
}
