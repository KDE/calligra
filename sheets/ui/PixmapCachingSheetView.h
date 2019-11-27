/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
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
