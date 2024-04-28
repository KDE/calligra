/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Silvio Heinrich <plassy@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef H_SELECTION_RECT_H
#define H_SELECTION_RECT_H

#include <QPointF>
#include <QRectF>
#include <QSizeF>
#include <limits>

/**
 * This class represents a rectangle that can be moved and resized
 * inbetween a constraining rectangle.
 */
class SelectionRect
{
public:
    typedef int HandleFlags;

    enum PositionInfo {
        INSIDE_RECT = 0x01,
        TOP_HANDLE = 0x02,
        BOTTOM_HANDLE = 0x04,
        LEFT_HANDLE = 0x08,
        RIGHT_HANDLE = 0x10,
        TOP_LEFT_HANDLE = TOP_HANDLE | LEFT_HANDLE,
        TOP_RIGHT_HANDLE = TOP_HANDLE | RIGHT_HANDLE,
        BOTTOM_LEFT_HANDLE = BOTTOM_HANDLE | LEFT_HANDLE,
        BOTTOM_RIGHT_HANDLE = BOTTOM_HANDLE | RIGHT_HANDLE
    };

    explicit SelectionRect(const QRectF &rect = QRectF(), qreal handleSize = 10.0);

    void setRect(const QRectF &rect);
    void setHandleSize(qreal size);
    void setAspectRatio(qreal aspect);
    void setConstrainingRect(const QRectF &rect);
    void setConstrainingAspectRatio(qreal aspect);
    bool beginDragging(const QPointF &pos);
    void doDragging(const QPointF &pos);
    void finishDragging();

    int getNumHandles() const
    {
        return 8;
    }
    QRectF getRect() const
    {
        return m_rect;
    }

    HandleFlags getHandleFlags(const QPointF &pos) const;
    HandleFlags getHandleFlags(int handleIndex) const;
    QRectF getHandleRect(HandleFlags handle) const;

private:
    void fixAspect(HandleFlags handle);

private:
    QPointF m_tempPos;
    QRectF m_rect;
    qreal m_aspectRatio;
    qreal m_lConstr;
    qreal m_rConstr;
    qreal m_tConstr;
    qreal m_bConstr;
    qreal m_aConstr;
    qreal m_handleSize;
    HandleFlags m_currentHandle;
};

#endif // H_SELECTION_RECT_H
