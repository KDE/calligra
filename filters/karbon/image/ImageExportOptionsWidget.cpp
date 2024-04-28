/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2019 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ImageExportOptionsWidget.h"

#include <KarbonDocument.h>
#include <KoPAPageBase.h>

#include <KoDpi.h>
#include <KoShapePainter.h>
#include <KoUnit.h> // for POINT_TO_INCH
#include <KoZoomHandler.h>

ImageExportOptionsWidget::ImageExportOptionsWidget(KarbonDocument *doc, QWidget *parent)
    : QWidget(parent)
    , m_doc(doc)
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
    widget.unit->addItems(KoUnit::listOfUnitNameForUi(KoUnit::HidePixel));
    widget.unit->setCurrentIndex(unit.indexInListForUi(KoUnit::HidePixel));
    widget.backColor->setColor(Qt::white);
    widget.opacity->setMinimum(0.0);
    widget.opacity->setMaximum(100.0);
    widget.opacity->setValue(0.0);

    for (int i = 0; i < doc->pageCount(); ++i) {
        widget.pageCombo->addItem(i18n("Page %1", i + 1));
    }
    setPage(0);

    widget.unitWidth->changeValue(m_pointSize.width());
    widget.unitHeight->changeValue(m_pointSize.height());

    connect(widget.unitWidth, &KoUnitDoubleSpinBox::valueChangedPt, this, &ImageExportOptionsWidget::unitWidthChanged);
    connect(widget.unitHeight, &KoUnitDoubleSpinBox::valueChangedPt, this, &ImageExportOptionsWidget::unitHeightChanged);
    connect(widget.pxWidth, QOverload<int>::of(&QSpinBox::valueChanged), this, &ImageExportOptionsWidget::pxWidthChanged);
    connect(widget.pxHeight, QOverload<int>::of(&QSpinBox::valueChanged), this, &ImageExportOptionsWidget::pxHeightChanged);
    connect(widget.dpi, QOverload<int>::of(&QSpinBox::valueChanged), this, &ImageExportOptionsWidget::dpiChanged);
    connect(widget.unit, QOverload<int>::of(&QComboBox::activated), this, &ImageExportOptionsWidget::unitChanged);
    connect(widget.pxAspect, &KoAspectButton::keepAspectRatioChanged, this, &ImageExportOptionsWidget::aspectChanged);
    connect(widget.unitAspect, &KoAspectButton::keepAspectRatioChanged, this, &ImageExportOptionsWidget::aspectChanged);
    connect(widget.pageCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImageExportOptionsWidget::setPage);
}

void ImageExportOptionsWidget::setPage(int idx)
{
    KoPAPageBase *page = m_doc->pages().value(idx);
    if (!page) {
        page = m_doc->pages().first();
    }
    KoShapePainter painter;
    painter.setShapes(page->shapes());
    // get the bounding rect of the content
    QRectF shapesRect = painter.contentRect();
    // get the size in point
    m_pointSize = shapesRect.size();
    updateFromPointSize(m_pointSize);
}

KoPAPageBase *ImageExportOptionsWidget::page() const
{
    return m_doc->pages().value(widget.pageCombo->currentIndex());
}

void ImageExportOptionsWidget::setUnit(const KoUnit &unit)
{
    widget.unitWidth->setUnit(unit);
    widget.unitHeight->setUnit(unit);
    widget.unit->setCurrentIndex(unit.indexInListForUi(KoUnit::HidePixel));
}

QSize ImageExportOptionsWidget::pixelSize() const
{
    return QSize(widget.pxWidth->value(), widget.pxHeight->value());
}

QSizeF ImageExportOptionsWidget::pointSize() const
{
    return QSizeF(widget.unitWidth->value(), widget.unitHeight->value());
}

void ImageExportOptionsWidget::setBackgroundColor(const QColor &color)
{
    blockChildSignals(true);

    widget.backColor->setColor(color);
    widget.opacity->setValue(color.alphaF() * 100.0);

    blockChildSignals(false);
}

QColor ImageExportOptionsWidget::backgroundColor() const
{
    QColor color = widget.backColor->color();
    color.setAlphaF(0.01 * widget.opacity->value());
    return color;
}

void ImageExportOptionsWidget::updateFromPointSize(const QSizeF &pointSize)
{
    blockChildSignals(true);
    widget.pxWidth->setValue(qRound(POINT_TO_INCH(pointSize.width()) * widget.dpi->value()));
    widget.pxHeight->setValue(qRound(POINT_TO_INCH(pointSize.height()) * widget.dpi->value()));
    blockChildSignals(false);
}

void ImageExportOptionsWidget::updateFromPixelSize(const QSize &pixelSize)
{
    blockChildSignals(true);
    double inchWidth = static_cast<double>(pixelSize.width()) / static_cast<double>(widget.dpi->value());
    double inchHeight = static_cast<double>(pixelSize.height()) / static_cast<double>(widget.dpi->value());
    widget.unitWidth->changeValue(INCH_TO_POINT(inchWidth));
    widget.unitHeight->changeValue(INCH_TO_POINT(inchHeight));
    blockChildSignals(false);
}

void ImageExportOptionsWidget::blockChildSignals(bool block)
{
    widget.pxWidth->blockSignals(block);
    widget.pxHeight->blockSignals(block);
    widget.unitWidth->blockSignals(block);
    widget.unitHeight->blockSignals(block);
    widget.dpi->blockSignals(block);
    widget.backColor->blockSignals(block);
    widget.opacity->blockSignals(block);
}

void ImageExportOptionsWidget::unitWidthChanged(qreal newWidth)
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

void ImageExportOptionsWidget::unitHeightChanged(qreal newHeight)
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

void ImageExportOptionsWidget::pxWidthChanged(int newWidth)
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

void ImageExportOptionsWidget::pxHeightChanged(int newHeight)
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

void ImageExportOptionsWidget::dpiChanged(int)
{
    blockChildSignals(true);

    updateFromPointSize(QSizeF(widget.unitWidth->value(), widget.unitHeight->value()));

    blockChildSignals(false);
}

void ImageExportOptionsWidget::unitChanged(int newUnit)
{
    blockChildSignals(true);

    const KoUnit unit = KoUnit::fromListForUi(newUnit, KoUnit::HidePixel);
    widget.unitWidth->setUnit(unit);
    widget.unitHeight->setUnit(unit);

    blockChildSignals(false);
}

void ImageExportOptionsWidget::aspectChanged(bool keepAspect)
{
    blockChildSignals(true);

    widget.pxAspect->setKeepAspectRatio(keepAspect);
    widget.unitAspect->setKeepAspectRatio(keepAspect);

    blockChildSignals(false);

    if (keepAspect)
        unitWidthChanged(widget.unitWidth->value());
}

void ImageExportOptionsWidget::enableBackgroundOpacity(bool enable)
{
    widget.opacity->setVisible(enable);
    widget.labelOpacity->setVisible(enable);
}
