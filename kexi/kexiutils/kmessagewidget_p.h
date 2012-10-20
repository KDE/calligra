/* This file is part of the KDE libraries
 *
 * Copyright (c) 2011 Aurélien Gâteau <agateau@kde.org>
 * Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */
#ifndef KMESSAGEWIDGET_P_H
#define KMESSAGEWIDGET_P_H

#include "kmessagewidget.h"

#include <QLabel>

class KMessageWidgetFrame : public QFrame
{
    Q_OBJECT
public:
    KMessageWidgetFrame(QWidget* parent = 0);

    virtual void paintEvent(QPaintEvent* event);

    KMessageWidget::CalloutPointerDirection calloutPointerDirection() const;

    void setCalloutPointerDirection(KMessageWidget::CalloutPointerDirection direction);

    QPoint calloutPointerPosition() const;
    
    QPoint pointerPosition() const;

    void setCalloutPointerPosition(const QPoint& globalPos);

    void updateCalloutPointerPosition() const;

    const int radius;
    QBrush bgBrush;
    QBrush borderBrush;

private:
    void paintCalloutPointer();
    void updateCalloutPointerTransformation() const;

    KMessageWidget::CalloutPointerDirection m_calloutPointerDirection;
    mutable QTransform m_calloutPointerTransformation;
    mutable QSize m_sizeForRecentTransformation;
    QPoint m_calloutPointerGlobalPosition;
    QPolygonF m_polyline;
    QPolygonF m_polygon;
};

class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget *parent = 0);
    virtual ~ClickableLabel();
    virtual void mousePressEvent(QMouseEvent *ev);

signals:
    void clicked();
};

#endif /* KMESSAGEWIDGET_P_H */
