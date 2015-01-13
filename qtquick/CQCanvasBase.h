/*
 * This file is part of the KDE project
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef CQCANVASBASE_H
#define CQCANVASBASE_H

#include <QDeclarativeItem>

class KoZoomController;
class CQCanvasController;
class CQCanvasBase : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(qreal shapeTransparency READ shapeTransparency WRITE setShapeTransparency NOTIFY shapeTransparencyChanged)

public:
    explicit CQCanvasBase(QDeclarativeItem* parent);
    virtual ~CQCanvasBase();

    CQCanvasController *canvasController() const;
    KoZoomController *zoomController() const;
    QString source() const;

    virtual qreal shapeTransparency() const;
    virtual void setShapeTransparency(qreal newTransparency);
public Q_SLOTS:
    virtual void setSource(const QString &source);
    virtual void render(QPainter* painter, const QRectF& target) = 0;

Q_SIGNALS:
    void sourceChanged();
    void shapeTransparencyChanged();
    void canvasControllerChanged();
    void positionShouldChange(const QPoint& pos);

    void loadingBegun();
    void loadingFinished();

protected:
    virtual void setCanvasController(CQCanvasController *controller);
    virtual void setZoomController(KoZoomController *controller);

    virtual void openFile(const QString &file) = 0;

private:
    class Private;
    Private * const d;
};

Q_DECLARE_METATYPE(CQCanvasBase*);

#endif // CQCANVASBASE_H
