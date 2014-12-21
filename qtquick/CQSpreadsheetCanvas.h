/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Shantanu Tushar <shantanu@kde.org>
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
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
 *
 */

#ifndef CQSPREADSHEETCANVAS_H
#define CQSPREADSHEETCANVAS_H

#include "CQCanvasBase.h"

namespace Calligra {
    namespace Sheets {
        class Map;
    }
}

class KoCanvasBase;

class CQSpreadsheetCanvas : public CQCanvasBase
{
    Q_OBJECT
    Q_PROPERTY(int currentSheet READ currentSheet WRITE setCurrentSheet NOTIFY currentSheetChanged)
    Q_PROPERTY(QObjectList linkTargets READ linkTargets NOTIFY linkTargetsChanged)

public:
    explicit CQSpreadsheetCanvas(QDeclarativeItem* parent = 0);
    virtual ~CQSpreadsheetCanvas();

    int currentSheet() const;
    Calligra::Sheets::Map* documentMap() const;

    QObjectList linkTargets() const;

    void setCurrentSheet(int sheet);

    virtual void render(QPainter* painter, const QRectF& target);

Q_SIGNALS:
    void currentSheetChanged();
    void linkTargetsChanged();

protected:
    virtual void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);
    virtual void openFile(const QString& file);

private Q_SLOTS:
    void updateDocumentSize(const QSize& size);

private:
    void createAndSetCanvasControllerOn(KoCanvasBase* canvas);
    void createAndSetZoomController(KoCanvasBase* canvas);

    class Private;
    Private * const d;
};

#endif // CQSPREADSHEETCANVAS_H
