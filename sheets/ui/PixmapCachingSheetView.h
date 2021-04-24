/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PIXMAPCACHINGSHEETVIEW_H
#define PIXMAPCACHINGSHEETVIEW_H

#include "SheetView.h"

namespace ThreadWeaver {
    class Job;
}

namespace Calligra {
namespace Sheets {

class PixmapCachingSheetView : public SheetView
{
    Q_OBJECT
public:
    /**
     * Constructor.
     */
    explicit PixmapCachingSheetView(const Sheet* sheet);

    /**
     * Destructor.
     */
    ~PixmapCachingSheetView() override;

    void invalidate() override;
    void paintCells(QPainter& painter, const QRectF& paintRect, const QPointF& topLeft, CanvasBase* canvas, const QRect& visibleRect) override;
protected:
    void invalidateRange(const QRect &range) override;
private Q_SLOTS:
    void jobDone(ThreadWeaver::Job* job);
private:
    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // PIXMAPCACHINGSHEETVIEW_H
