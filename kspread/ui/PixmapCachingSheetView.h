/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

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

namespace KSpread {

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
    ~PixmapCachingSheetView();

    virtual void invalidateRegion(const Region& region);
    virtual void invalidate();
    virtual void paintCells(QPainter& painter, const QRectF& paintRect, const QPointF& topLeft, const CanvasBase* canvas);
private:
    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // PIXMAPCACHINGSHEETVIEW_H
