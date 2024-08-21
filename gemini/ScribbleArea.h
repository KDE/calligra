/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 */

#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H

#include <QImage>
#include <QQuickPaintedItem>

class ScribbleArea : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(int penWidth READ penWidth WRITE setPenWidth NOTIFY penWidthChanged)

public:
    explicit ScribbleArea(QQuickItem *parent = nullptr);
    ~ScribbleArea() override;

    Q_INVOKABLE void clear();

    QColor color() const;
    void setColor(const QColor &newColor);

    int penWidth() const;
    void setPenWidth(const int &newWidth);

Q_SIGNALS:
    void colorChanged();
    void penWidthChanged();
    void paintingStopped();
    void paintingStarted();

protected:
    bool event(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paint(QPainter *painter) override;

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
