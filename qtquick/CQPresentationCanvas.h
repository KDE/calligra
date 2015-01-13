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

class KoCanvasBase;
class KPrDocument;
class CQPresentationCanvas : public CQCanvasBase
{
    Q_OBJECT
    Q_PROPERTY(int currentSlide READ currentSlide WRITE setCurrentSlide NOTIFY currentSlideChanged)
    Q_PROPERTY(QSizeF pageSize READ pageSize NOTIFY currentSlideChanged)
    Q_PROPERTY(QObjectList linkTargets READ linkTargets NOTIFY linkTargetsChanged)
    Q_PROPERTY(QObject* document READ doc NOTIFY documentChanged);
    Q_PROPERTY(QObject* textEditor READ textEditor NOTIFY textEditorChanged)

public:
    explicit CQPresentationCanvas(QDeclarativeItem* parent = 0);
    virtual ~CQPresentationCanvas();

    int currentSlide() const;
    Q_INVOKABLE int slideCount() const;
    KPrDocument* document() const;
    Q_INVOKABLE QObject* doc() const;
    Q_INVOKABLE QObject* part() const;
    QSizeF pageSize() const;

    QObjectList linkTargets() const;

    void setCurrentSlide(int slide);

    virtual void render(QPainter* painter, const QRectF& target);

    QObject* textEditor() const;
    // Deselects any text selection present in the document, and deselects all shapes
    // This is highly useful, as it makes navigation prettier.
    Q_INVOKABLE void deselectEverything();

    virtual qreal shapeTransparency() const;
    virtual void setShapeTransparency(qreal newTransparency);

Q_SIGNALS:
    void currentSlideChanged();
    void linkTargetsChanged();
    void documentChanged();
    void textEditorChanged();

protected:
    virtual bool event( QEvent* event );
    virtual void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);
    virtual void openFile(const QString& uri);

private Q_SLOTS:
    void updateDocumentSize(const QSize& size);

private:
    void createAndSetCanvasControllerOn(KoCanvasBase* canvas);
    void createAndSetZoomController(KoCanvasBase* canvas);

    class Private;
    Private * const d;
};

#endif // CQPRESENTATIONCANVAS_H
