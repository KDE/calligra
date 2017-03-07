/* This file is part of the KDE project
 * Copyright (C) 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H

#include <QQuickPaintedItem>
#include <QImage>

class ScribbleArea : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(int penWidth READ penWidth WRITE setPenWidth NOTIFY penWidthChanged)

public:
    explicit ScribbleArea(QQuickItem* parent = 0);
    virtual ~ScribbleArea();

    Q_INVOKABLE void clear();

    QColor color() const;
    void setColor(const QColor& newColor);

    int penWidth() const;
    void setPenWidth(const int& newWidth);

Q_SIGNALS:
    void colorChanged();
    void penWidthChanged();
    void paintingStopped();
    void paintingStarted();

protected:
    virtual bool event(QEvent* event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void paint(QPainter* painter);

private:
    void drawLineTo(const QPointF &endPoint);
    void resizeImage(QImage *image, const QSize &newSize);

    bool scribbling;
    int myPenWidth;
    QColor myPenColor;
    QImage image;
    QPointF lastPoint;
};

#endif // SCRIBBLEAREA_H
