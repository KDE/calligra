/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
    explicit ImageExportOptionsWidget(KarbonDocument *doc, QWidget *parent = nullptr);

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
