/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Gary Cramblitt <garycramblitt@comcast.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KO_PAGE_PREVIEW_WIDGET
#define KO_PAGE_PREVIEW_WIDGET

#include "kowidgets_export.h"

#include <QWidget>

// Needed for building on Windows (cannot use forward declarations)
#include <KoPageLayout.h>
#include <KoColumns.h>

/// A widget to preview the KoPageLayout and KoColumns data structures.
class KOWIDGETS_EXPORT KoPagePreviewWidget : public QWidget {
    Q_OBJECT
public:
    explicit KoPagePreviewWidget(QWidget *parent = nullptr);
    ~KoPagePreviewWidget() override;

protected:
    void paintEvent(QPaintEvent *event) override;

public Q_SLOTS:
    void setPageLayout(const KoPageLayout &layout);
    void setColumns(const KoColumns &columns);

private:
    void drawPage(QPainter &painter, qreal zoom, const QRect &dimensions, bool left);

private:
    class Private;
    Private * const d;
};

#endif
