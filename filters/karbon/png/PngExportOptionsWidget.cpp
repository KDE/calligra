/* This file is part of the KDE project
 * Copyright (C) 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "PngExportOptionsWidget.h"

#include <KoUnit.h> // for POINT_TO_INCH
#include <KoDpi.h>

PngExportOptionsWidget::PngExportOptionsWidget(QSizeF pointSize, QWidget * parent)
        : QWidget(parent), m_pointSize(pointSize)
{
    KoUnit unit;

    widget.setupUi(this);

    widget.pxWidth->setRange(1, 10000);
    widget.pxWidth->setAlignment(Qt::AlignRight);
    widget.pxWidth->setSuffix(" px");
    widget.pxHeight->setRange(1, 10000);
    widget.pxHeight->setAlignment(Qt::AlignRight);
    widget.pxHeight->setSuffix(" px");
    widget.unitWidth->setMinMaxStep(0, 10000, 1);
    widget.unitHeight->setMinMaxStep(0, 10000, 1);
    widget.dpi->setRange(1, 10000);
    widget.dpi->setValue(KoDpi::dpiX());
    widget.dpi->setAlignment(Qt::AlignRight);
    widget.dpi->setSuffix(" DPI");
    widget.pxAspect->setKeepAspectRatio(true);
    widget.unitAspect->setKeepAspectRatio(true);
    widget.unit->addItems(KoUnit::listOfUnitName());
    widget.unit->setCurrentIndex(unit.indexInList());
    widget.backColor->setColor(Qt::white);
    widget.opacity->setMinimum(0.0);
    widget.opacity->setMaximum(100.0);
    widget.opacity->setValue(100.0);
    widget.unitWidth->changeValue(pointSize.width());
    widget.unitHeight->changeValue(pointSize.height());
    updateFromPointSize(pointSize);

    connect(widget.unitWidth, SIGNAL(valueChangedPt(qreal)), this, SLOT(unitWidthChanged(qreal)));
    connect(widget.unitHeight, SIGNAL(valueChangedPt(qreal)), this, SLOT(unitHeightChanged(qreal)));
    connect(widget.pxWidth, SIGNAL(valueChanged(int)), this, SLOT(pxWidthChanged(int)));
    connect(widget.pxHeight, SIGNAL(valueChanged(int)), this, SLOT(pxHeightChanged(int)));
    connect(widget.dpi, SIGNAL(valueChanged(int)), this, SLOT(dpiChanged(int)));
    connect(widget.unit, SIGNAL(activated(int)), this, SLOT(unitChanged(int)));
    connect(widget.pxAspect, SIGNAL(keepAspectRatioChanged(bool)), this, SLOT(aspectChanged(bool)));
    connect(widget.unitAspect, SIGNAL(keepAspectRatioChanged(bool)), this, SLOT(aspectChanged(bool)));
}

void PngExportOptionsWidget::setUnit(const KoUnit &unit)
{
    widget.unitWidth->setUnit(unit);
    widget.unitHeight->setUnit(unit);
    widget.unit->setCurrentIndex(unit.indexInList());
}

QSize PngExportOptionsWidget::pixelSize() const
{
    return QSize(widget.pxWidth->value(), widget.pxHeight->value());
}

QSizeF PngExportOptionsWidget::pointSize() const
{
    return QSizeF(widget.unitWidth->value(), widget.unitHeight->value());
}

void PngExportOptionsWidget::setBackgroundColor(const QColor &color)
{
    blockChildSignals(true);

    widget.backColor->setColor(color);
    widget.opacity->setValue(color.alphaF() * 100.0);

    blockChildSignals(false);
}

QColor PngExportOptionsWidget::backgroundColor() const
{
    QColor color = widget.backColor->color();
    color.setAlphaF(0.01 * widget.opacity->value());
    return color;
}

void PngExportOptionsWidget::updateFromPointSize(const QSizeF &pointSize)
{
    blockChildSignals(true);
    widget.pxWidth->setValue(qRound(POINT_TO_INCH(pointSize.width()) * widget.dpi->value()));
    widget.pxHeight->setValue(qRound(POINT_TO_INCH(pointSize.height()) * widget.dpi->value()));
    blockChildSignals(false);
}

void PngExportOptionsWidget::updateFromPixelSize(const QSize &pixelSize)
{
    blockChildSignals(true);
    double inchWidth = static_cast<double>(pixelSize.width()) / static_cast<double>(widget.dpi->value());
    double inchHeight = static_cast<double>(pixelSize.height()) / static_cast<double>(widget.dpi->value());
    widget.unitWidth->changeValue(INCH_TO_POINT(inchWidth));
    widget.unitHeight->changeValue(INCH_TO_POINT(inchHeight));
    blockChildSignals(false);
}

void PngExportOptionsWidget::blockChildSignals(bool block)
{
    widget.pxWidth->blockSignals(block);
    widget.pxHeight->blockSignals(block);
    widget.unitWidth->blockSignals(block);
    widget.unitHeight->blockSignals(block);
    widget.dpi->blockSignals(block);
    widget.backColor->blockSignals(block);
    widget.opacity->blockSignals(block);
}

void PngExportOptionsWidget::unitWidthChanged(qreal newWidth)
{
    blockChildSignals(true);

    double newHeight = widget.unitHeight->value();
    if (widget.unitAspect->keepAspectRatio()) {
        newHeight = newWidth * m_pointSize.height() / m_pointSize.width();
        widget.unitHeight->changeValue(newHeight);
    }
    updateFromPointSize(QSizeF(newWidth, newHeight));

    blockChildSignals(false);
}

void PngExportOptionsWidget::unitHeightChanged(qreal newHeight)
{
    blockChildSignals(true);

    double newWidth = widget.unitWidth->value();
    if (widget.unitAspect->keepAspectRatio()) {
        newWidth = newHeight * m_pointSize.width() / m_pointSize.height();
        widget.unitWidth->changeValue(newWidth);
    }
    updateFromPointSize(QSizeF(newWidth, newHeight));

    blockChildSignals(false);
}

void PngExportOptionsWidget::pxWidthChanged(int newWidth)
{
    blockChildSignals(true);

    int newHeight = widget.pxHeight->value();
    if (widget.pxAspect->keepAspectRatio()) {
        newHeight = qRound(newWidth * m_pointSize.height() / m_pointSize.width());
        widget.pxHeight->setValue(newHeight);
    }
    updateFromPixelSize(QSize(newWidth, newHeight));

    blockChildSignals(false);
}

void PngExportOptionsWidget::pxHeightChanged(int newHeight)
{
    blockChildSignals(true);

    int newWidth = widget.pxWidth->value();
    if (widget.pxAspect->keepAspectRatio()) {
        newWidth = qRound(newHeight * m_pointSize.width() / m_pointSize.height());
        widget.pxWidth->setValue(newWidth);
    }
    updateFromPixelSize(QSize(newWidth, newHeight));

    blockChildSignals(false);
}

void PngExportOptionsWidget::dpiChanged(int)
{
    blockChildSignals(true);

    updateFromPointSize(QSizeF(widget.unitWidth->value(), widget.unitHeight->value()));

    blockChildSignals(false);
}

void PngExportOptionsWidget::unitChanged(int newUnit)
{
    KoUnit unit((KoUnit::Unit) newUnit);
    blockChildSignals(true);
    widget.unitWidth->setUnit(unit);
    widget.unitHeight->setUnit(unit);
    blockChildSignals(false);
}

void PngExportOptionsWidget::aspectChanged(bool keepAspect)
{
    blockChildSignals(true);

    widget.pxAspect->setKeepAspectRatio(keepAspect);
    widget.unitAspect->setKeepAspectRatio(keepAspect);

    blockChildSignals(false);

    if (keepAspect)
        unitWidthChanged(widget.unitWidth->value());
}
#include "PngExportOptionsWidget.moc"
