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

#ifndef IMAGEEXPORTOPTIONSWIDGET_H
#define IMAGEEXPORTOPTIONSWIDGET_H

#include "ui_ImageExportOptionsWidget.h"

#include <QWidget>

class KarbonDocument;
class KoPAPageBase;

class ImageExportOptionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ImageExportOptionsWidget(KarbonDocument *doc, QWidget *parent = 0);

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

    /// Enables/disables setting the background opacity
    void enableBackgroundOpacity(bool enable);

    KoPAPageBase *page() const;

private Q_SLOTS:
    void unitWidthChanged(qreal);
    void unitHeightChanged(qreal);
    void pxWidthChanged(int);
    void pxHeightChanged(int);
    void dpiChanged(int);
    void unitChanged(int);
    void aspectChanged(bool);
    void setPage(int idx);
private:
    void updateFromPointSize(const QSizeF &pointSize);
    void updateFromPixelSize(const QSize &pixelSize);

    void blockChildSignals(bool block);

    Ui_ImageExportOptionsWidget widget;
    QSizeF m_pointSize;

    KarbonDocument *m_doc;
};

#endif // IMAGEEXPORTOPTIONSWIDGET_H
