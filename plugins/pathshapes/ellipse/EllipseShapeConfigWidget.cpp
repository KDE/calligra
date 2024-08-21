/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "EllipseShapeConfigWidget.h"
#include "EllipseShapeConfigCommand.h"
#include <KLocalizedString>

EllipseShapeConfigWidget::EllipseShapeConfigWidget()
{
    widget.setupUi(this);

    widget.ellipseType->clear();
    widget.ellipseType->addItem(i18n("Arc"));
    widget.ellipseType->addItem(i18n("Pie"));
    widget.ellipseType->addItem(i18n("Chord"));

    widget.startAngle->setMinimum(0.0);
    widget.startAngle->setMaximum(360.0);

    widget.endAngle->setMinimum(0.0);
    widget.endAngle->setMaximum(360.0);

    connect(widget.ellipseType, &QComboBox::currentIndexChanged, this, &KoShapeConfigWidgetBase::propertyChanged);
    connect(widget.startAngle, &QAbstractSpinBox::editingFinished, this, &KoShapeConfigWidgetBase::propertyChanged);
    connect(widget.endAngle, &QAbstractSpinBox::editingFinished, this, &KoShapeConfigWidgetBase::propertyChanged);
    connect(widget.closeEllipse, &QAbstractButton::clicked, this, &EllipseShapeConfigWidget::closeEllipse);
}

void EllipseShapeConfigWidget::open(KoShape *shape)
{
    m_ellipse = dynamic_cast<EllipseShape *>(shape);
    if (!m_ellipse)
        return;

    widget.ellipseType->blockSignals(true);
    widget.startAngle->blockSignals(true);
    widget.endAngle->blockSignals(true);

    widget.ellipseType->setCurrentIndex(m_ellipse->type());
    widget.startAngle->setValue(m_ellipse->startAngle());
    widget.endAngle->setValue(m_ellipse->endAngle());

    widget.ellipseType->blockSignals(false);
    widget.startAngle->blockSignals(false);
    widget.endAngle->blockSignals(false);
}

void EllipseShapeConfigWidget::save()
{
    if (!m_ellipse)
        return;

    m_ellipse->setType(static_cast<EllipseShape::EllipseType>(widget.ellipseType->currentIndex()));
    m_ellipse->setStartAngle(widget.startAngle->value());
    m_ellipse->setEndAngle(widget.endAngle->value());
}

KUndo2Command *EllipseShapeConfigWidget::createCommand()
{
    if (!m_ellipse) {
        return nullptr;
    } else {
        EllipseShape::EllipseType type = static_cast<EllipseShape::EllipseType>(widget.ellipseType->currentIndex());
        return new EllipseShapeConfigCommand(m_ellipse, type, widget.startAngle->value(), widget.endAngle->value());
    }
}

void EllipseShapeConfigWidget::closeEllipse()
{
    widget.startAngle->blockSignals(true);
    widget.endAngle->blockSignals(true);

    widget.startAngle->setValue(0.0);
    widget.endAngle->setValue(360.0);

    widget.startAngle->blockSignals(false);
    widget.endAngle->blockSignals(false);

    Q_EMIT propertyChanged();
}
