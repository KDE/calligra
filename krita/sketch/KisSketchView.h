/* This file is part of the KDE project
 * Copyright (C) 2012 Boudewijn Rempt <boud@kogmbh.com>
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
 */
#ifndef KRITA_SKETCH_VIEW_H
#define KRITA_SKETCH_VIEW_H

#include <QDeclarativeItem>

class KisSketchView : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QObject* document READ doc)
    Q_PROPERTY(QObject* view READ view NOTIFY viewChanged)
    Q_PROPERTY(QObject* settings READ settings WRITE setSettings)
    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)

public:
    KisSketchView(QDeclarativeItem* parent = 0);
    virtual ~KisSketchView();

    QObject* doc() const;
    QObject* view() const;
    QString file() const;

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

    virtual void componentComplete();
    virtual void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);

    void setFile(const QString &file);

public Q_SLOTS:

    /// XXX: add parameter options... Or how does QML interact with a class like this?
    void createDocument();
    void resetDocumentPosition();

    void undo();
    void redo();

    QObject *settings();
    void setSettings(QObject  *settings);

    void save();
    void saveAs(const QString& fileName);

Q_SIGNALS:
    void viewChanged();
    void fileChanged();

protected:
    virtual bool sceneEvent(QEvent* event);

private:
    class Private;
    Private * const d;

    Q_PRIVATE_SLOT(d, void update())
};

#endif // KRITA_SKETCH_CANVAS_H
