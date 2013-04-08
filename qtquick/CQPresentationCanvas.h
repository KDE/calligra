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

#ifndef CQPRESENTATIONCANVAS_H
#define CQPRESENTATIONCANVAS_H

#include "CQCanvasBase.h"

class KPrDocument;
class KoCanvasBase;
class CQPresentationCanvas : public CQCanvasBase
{
    Q_OBJECT
    Q_PROPERTY(int currentSlide READ currentSlide WRITE setCurrentSlide NOTIFY currentSlideChanged)

public:
    explicit CQPresentationCanvas(QDeclarativeItem* parent = 0);
    virtual ~CQPresentationCanvas();

    int currentSlide() const;
    void setCurrentSlide(int slide);

    KPrDocument* document() const;

Q_SIGNALS:
    void currentSlideChanged();

protected:
    virtual void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);
    void openFile(const QString& uri);

private Q_SLOTS:
    void updateDocumentSize(const QSize& size);

private:
    void createAndSetCanvasControllerOn(KoCanvasBase* canvas);
    void createAndSetZoomController(KoCanvasBase* canvas);

    class Private;
    Private * const d;
};

#endif // CQPRESENTATIONCANVAS_H
