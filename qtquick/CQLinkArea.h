/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
    explicit CQLinkArea(QDeclarativeItem *parent = nullptr);
    virtual ~CQLinkArea();
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

    QVariantList links() const;
    void setLinks(const QVariantList &newLinks);

    QSizeF sourceSize() const;
    void setSourceSize(const QSizeF &size);

    QColor linkColor() const;
    void setLinkColor(const QColor &color);

Q_SIGNALS:
    void linksChanged();
    void clicked();
    void doubleClicked();
    void linkClicked(QUrl linkTarget);
    void sourceSizeChanged();
    void linkColorChanged();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    class Private;
    Private *const d;
};

#endif // CQLINKAREA_H
