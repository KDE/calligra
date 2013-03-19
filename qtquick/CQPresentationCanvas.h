/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Shantanu Tushar <shantanu@kde.org>
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

#include <QtDeclarative/QDeclarativeItem>

class KoZoomController;
class CQCanvasController;
class KoCanvasBase;

class CQPresentationCanvas : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
public:
    QString source() const;
    void setSource(const QString &source);

signals:
    void sourceChanged();

protected:
    virtual void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);

private Q_SLOTS:
    void updateDocumentSize(const QSize& size);

private:
    bool openFile(const QString& uri);
    void createAndSetCanvasControllerOn(KoCanvasBase* canvas);
    void createAndSetZoomController(KoCanvasBase* canvas);

    QString m_source;
    KoCanvasBase* m_canvasBase;
    CQCanvasController* m_canvasController;
    KoZoomController* m_zoomController;
};

#endif // CQPRESENTATIONCANVAS_H
