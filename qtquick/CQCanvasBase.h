/*
 * This file is part of the KDE project
 * Copyright (C) 2013 Arjen-Wander Hiemstra <aw.hiemstra@gmail.com>
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

public:
    explicit CQCanvasBase(QDeclarativeItem* parent);
    virtual ~CQCanvasBase();

    CQCanvasController *canvasController() const;
    KoZoomController *zoomController() const;
    QString source() const;

public Q_SLOTS:
    virtual void setSource(const QString &source);

Q_SIGNALS:
    void sourceChanged();
    void canvasControllerChanged();

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
