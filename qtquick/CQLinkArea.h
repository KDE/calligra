/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#ifndef CQLINKAREA_H
#define CQLINKAREA_H

#include <QDeclarativeItem>

class CQLinkArea : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QVariantList links READ links WRITE setLinks NOTIFY linksChanged)
    Q_PROPERTY(QSizeF sourceSize READ sourceSize WRITE setSourceSize NOTIFY sourceSizeChanged)
    Q_PROPERTY(QColor linkColor READ linkColor WRITE setLinkColor NOTIFY linkColorChanged)

public:
    explicit CQLinkArea(QDeclarativeItem* parent = 0);
    virtual ~CQLinkArea();
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* );

    QVariantList links() const;
    void setLinks(const QVariantList& newLinks);

    QSizeF sourceSize() const;
    void setSourceSize( const QSizeF& size );

    QColor linkColor() const;
    void setLinkColor( const QColor& color );

Q_SIGNALS:
    void linksChanged();
    void clicked();
    void doubleClicked();
    void linkClicked(QUrl linkTarget);
    void sourceSizeChanged();
    void linkColorChanged();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

private:
    class Private;
    Private* const d;
};

#endif // CQLINKAREA_H
