/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef CQCANVASBASE_H
#define CQCANVASBASE_H

#include <QQuickPaintedItem>

class KoZoomController;
class CQCanvasController;
class CQCanvasBase : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(qreal shapeTransparency READ shapeTransparency WRITE setShapeTransparency NOTIFY shapeTransparencyChanged)

public:
    explicit CQCanvasBase(QQuickItem *parent);
    virtual ~CQCanvasBase();

    CQCanvasController *canvasController() const;
    KoZoomController *zoomController() const;
    QString source() const;

    virtual qreal shapeTransparency() const;
    virtual void setShapeTransparency(qreal newTransparency);
public Q_SLOTS:
    virtual void setSource(const QString &source);
    virtual void render(QPainter *painter, const QRectF &target) = 0;

Q_SIGNALS:
    void sourceChanged();
    void shapeTransparencyChanged();
    void canvasControllerChanged();
    void positionShouldChange(const QPoint &pos);

    void loadingBegun();
    void loadingFinished();

protected:
    virtual void setCanvasController(CQCanvasController *controller);
    virtual void setZoomController(KoZoomController *controller);

    virtual void openFile(const QString &file) = 0;

private:
    class Private;
    Private *const d;
};

Q_DECLARE_METATYPE(CQCanvasBase *);

#endif // CQCANVASBASE_H
