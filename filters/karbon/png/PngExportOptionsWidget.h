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

#ifndef PNGEXPORTOPTIONSWIDGET_H
#define PNGEXPORTOPTIONSWIDGET_H

#include "ui_PngExportOptionsWidget.h"

#include <QtGui/QWidget>

class PngExportOptionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PngExportOptionsWidget(QSizeF pointSize, QWidget *parent = 0);

    /// Sets the initial unit to use
    void setUnit(const KoUnit &unit);

    /// Returns the chosen export size in pixel
    QSize pixelSize() const;

    /// Returns the chosen export size in point
    QSizeF pointSize() const;

    /// Sets the background color
    void setBackgroundColor(const QColor &color);

    /// Returns the chosen background color
    QColor backgroundColor() const;

private slots:
    void unitWidthChanged(qreal);
    void unitHeightChanged(qreal);
    void pxWidthChanged(int);
    void pxHeightChanged(int);
    void dpiChanged(int);
    void unitChanged(int);
    void aspectChanged(bool);
private:
    void updateFromPointSize(const QSizeF &pointSize);
    void updateFromPixelSize(const QSize &pixelSize);

    void blockChildSignals(bool block);

    Ui_PngExportOptionsWidget widget;
    QSizeF m_pointSize;
};

#endif // PNGEXPORTOPTIONSWIDGET_H
