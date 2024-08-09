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

#include <QQuickPaintedItem>

class CQLinkArea : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QVariantList links READ links WRITE setLinks NOTIFY linksChanged)
    Q_PROPERTY(QSizeF sourceSize READ sourceSize WRITE setSourceSize NOTIFY sourceSizeChanged)
    Q_PROPERTY(QColor linkColor READ linkColor WRITE setLinkColor NOTIFY linkColorChanged)

public:
    explicit CQLinkArea(QQuickItem *parent = nullptr);
    virtual ~CQLinkArea();
    void paint(QPainter *painter) override;

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
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    class Private;
    std::unique_ptr<Private> const d;
};

#endif // CQLINKAREA_H
