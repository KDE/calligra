/* This file is part of the KDE project
   Copyright 2011 Silvio Heinrich <plassy@web.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef H_SELECTION_RECT_H
#define H_SELECTION_RECT_H

#include <QRectF>
#include <QPointF>
#include <QSizeF>
#include <limits>

class SelectionRect
{
public:
    typedef int HandleFlags;
    
    enum PositionInfo
    {
        INSIDE_RECT   = 0x01,
        TOP_HANDLE    = 0x02,
        BOTTOM_HANDLE = 0x04,
        LEFT_HANDLE   = 0x08,
        RIGHT_HANDLE  = 0x10
    };
    
    SelectionRect(const QRectF& rect=QRectF(), qreal handleSize=10.0):
        m_rect(rect),
        m_aspectRatio(1),
        m_aConstr (0),
        m_minSize(0,0),
        m_handleSize(handleSize),
        m_currentHandle(0)
    {
        m_lConstr = -std::numeric_limits<qreal>::infinity();
        m_rConstr =  std::numeric_limits<qreal>::infinity();
        m_tConstr = -std::numeric_limits<qreal>::infinity();
        m_bConstr =  std::numeric_limits<qreal>::infinity();
    }

    void setRect(const QRectF& rect)
    {
        m_rect = rect;
    }

    void setHandleSize(qreal size)
    {
        m_handleSize = size;
    }

    void setAspectRatio(qreal aspect)
    {
        m_aspectRatio = aspect;
    }

    void setConstrainingRect(const QRectF& rect)
    {
        m_lConstr = rect.left();
        m_rConstr = rect.right();
        m_tConstr = rect.top();
        m_bConstr = rect.bottom();
    }

    void setConstrainingAspectRatio(qreal aspect)
    {
        m_aConstr = aspect;

        if (m_aConstr != 0.0) {
            fixAspect(TOP_HANDLE);
        }
    }

    bool beginDragging(const QPointF& pos)
    {
        m_tempPos       = pos;
        m_currentHandle = getHandleFlags(pos);
        return bool(m_currentHandle);
    }
    
    void doDragging(const QPointF& pos)
    {
        if (m_currentHandle & INSIDE_RECT) {
            m_rect.moveTo(m_rect.topLeft() + (pos - m_tempPos));
            m_tempPos = pos;
            
            if (m_rect.left() < m_lConstr) {
                m_rect.moveLeft(m_lConstr);
            }
            if (m_rect.right() > m_rConstr) {
                m_rect.moveRight(m_rConstr);
            }
            if (m_rect.top() < m_tConstr) {
                m_rect.moveTop(m_tConstr);
            }
            if (m_rect.bottom() > m_bConstr) {
                m_rect.moveBottom(m_bConstr);
            }
        }
        else {
            if (m_currentHandle & TOP_HANDLE) {
                m_rect.setTop(qBound(m_tConstr, pos.y(), m_bConstr));
            }
            if (m_currentHandle & BOTTOM_HANDLE) {
                m_rect.setBottom(qBound(m_tConstr, pos.y(), m_bConstr));
            }
            if (m_currentHandle & LEFT_HANDLE) {
                m_rect.setLeft(qBound(m_lConstr, pos.x(), m_rConstr));
            }
            if (m_currentHandle & RIGHT_HANDLE) {
                m_rect.setRight(qBound(m_lConstr, pos.x(), m_rConstr));
            }

            if(m_aConstr != 0.0) {
                fixAspect(m_currentHandle);
            }
        }
    }

    void finishDragging()
    {
        m_currentHandle = 0;
        m_rect          = m_rect.normalized();
    }

    int    getNumHandles() const { return 8;      }
    QRectF getRect      () const { return m_rect; }

    HandleFlags getHandleFlags(const QPointF& pos) const
    {
        for(int i=0; i<getNumHandles(); ++i) {
            if(getHandleRect(getHandleFlags(i)).contains(pos))
                return getHandleFlags(i);
        }

        return m_rect.contains(pos) ? INSIDE_RECT : 0;
    }

    HandleFlags getHandleFlags(int handleIndex) const
    {
        switch(handleIndex)
        {
            case 0: return TOP_HANDLE|LEFT_HANDLE;
            case 1: return TOP_HANDLE;
            case 2: return TOP_HANDLE|RIGHT_HANDLE;
            case 3: return RIGHT_HANDLE;
            case 4: return BOTTOM_HANDLE|RIGHT_HANDLE;
            case 5: return BOTTOM_HANDLE;
            case 6: return BOTTOM_HANDLE|LEFT_HANDLE;
            case 7: return LEFT_HANDLE;
        }
        
        return 0;
    }
    
    QRectF getHandleRect(HandleFlags handle) const
    {
        qreal x = (m_rect.left() + m_rect.right()) / 2.0;
        qreal y = (m_rect.top()  + m_rect.bottom()) / 2.0;
        qreal w = m_handleSize;
        qreal h = m_handleSize * m_aspectRatio;
        
        x = (handle & LEFT_HANDLE  ) ? m_rect.left()   : x;
        y = (handle & TOP_HANDLE   ) ? m_rect.top()    : y;
        x = (handle & RIGHT_HANDLE ) ? m_rect.right()  : x;
        y = (handle & BOTTOM_HANDLE) ? m_rect.bottom() : y;

        return QRectF(x-(w/2.0), y-(h/2.0), w, h);
    }

private:
    void fixAspect(HandleFlags handle)
    {
        QRectF oldRect = m_rect;
        
        switch(handle)
        {
        case TOP_HANDLE:
        case BOTTOM_HANDLE:
            m_rect.setWidth((m_rect.height() * m_aConstr) / m_aspectRatio);
            break;

        case LEFT_HANDLE:
        case RIGHT_HANDLE:
        case RIGHT_HANDLE|BOTTOM_HANDLE:
            m_rect.setHeight((m_rect.width() / m_aConstr) * m_aspectRatio);
            break;

        case RIGHT_HANDLE|TOP_HANDLE:
            m_rect.setHeight((m_rect.width() / m_aConstr) * m_aspectRatio);
            m_rect.moveBottomLeft(oldRect.bottomLeft());
            break;

        case LEFT_HANDLE|BOTTOM_HANDLE:
            m_rect.setHeight((m_rect.width() / m_aConstr) * m_aspectRatio);
            m_rect.moveTopRight(oldRect.topRight());
            break;

        case LEFT_HANDLE|TOP_HANDLE:
            m_rect.setHeight((m_rect.width() / m_aConstr) * m_aspectRatio);
            m_rect.moveBottomRight(oldRect.bottomRight());
            break;
        }

        if (m_rect.top() < m_tConstr || m_rect.top() > m_bConstr) {
            m_rect.setTop(qBound(m_tConstr, m_rect.top(), m_bConstr));
            fixAspect(TOP_HANDLE);
        }

        if (m_rect.bottom() < m_tConstr || m_rect.bottom() > m_bConstr) {
            m_rect.setBottom(qBound(m_tConstr, m_rect.bottom(), m_bConstr));
            fixAspect(BOTTOM_HANDLE);
            
            if(handle & LEFT_HANDLE)  { m_rect.moveTopRight(oldRect.topRight()); }
            if(handle & RIGHT_HANDLE) { m_rect.moveTopLeft(oldRect.topLeft());   }
        }

        if (m_rect.left() < m_lConstr || m_rect.left() > m_rConstr) {
            m_rect.setLeft(qBound(m_lConstr, m_rect.left(), m_rConstr));
            fixAspect(LEFT_HANDLE);
        }

        if (m_rect.right() < m_lConstr || m_rect.right() > m_rConstr) {
            m_rect.setRight(qBound(m_lConstr, m_rect.right(), m_rConstr));
            fixAspect(RIGHT_HANDLE);
            m_rect.moveBottomRight(oldRect.bottomRight());
        }
    }
    
private:
    QPointF m_tempPos;
    QRectF m_rect;
    qreal m_aspectRatio;
    qreal m_lConstr;
    qreal m_rConstr;
    qreal m_tConstr;
    qreal m_bConstr;
    qreal m_aConstr;
    QSizeF m_minSize;
    qreal m_handleSize;
    HandleFlags m_currentHandle;
};

#endif // H_SELECTION_RECT_H
