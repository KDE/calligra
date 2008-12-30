/* This file is part of the KDE libraries
   Copyright (C) 2002 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KFE_RESIZEHANDLER_H
#define KFE_RESIZEHANDLER_H

#include <QHash>
#include <qpointer.h>
#include <qwidget.h>

#include <kexi_export.h>

namespace KFormDesigner
{

class Form;
class ResizeHandleSet;

/**
* a single widget which represents a dot for resizing a widget
* @author Joseph Wenninger
*/
class KFORMEDITOR_EXPORT ResizeHandle : public QWidget
{
    Q_OBJECT

public:
    enum HandlePos {
        TopLeftCorner = 0,
        TopCenter = 2,
        TopRightCorner = 4,
        LeftCenter = 8,
        RightCenter = 16,
        BottomLeftCorner = 32,
        BottomCenter = 64,
        BottomRightCorner = 128
    };
    ResizeHandle(ResizeHandleSet *set, HandlePos pos, bool editing = false);
    virtual ~ResizeHandle();
    void setEditingMode(bool editing);

protected:
    virtual void mousePressEvent(QMouseEvent *ev);
    virtual void mouseMoveEvent(QMouseEvent *ev);
    virtual void mouseReleaseEvent(QMouseEvent *ev);
    virtual void paintEvent(QPaintEvent *ev);

protected slots:
    bool eventFilter(QObject *obj, QEvent *ev);
    void updatePos();

private:
    ResizeHandleSet *m_set;
    HandlePos m_pos;
    //QWidget *m_buddy;
    bool m_dragging;
    //bool m_editing;
    int m_x;
    int m_y;
};

/**
* a set of resize handles (for resizing widgets)
* @author Joseph Wenninger
*/
class KFORMEDITOR_EXPORT ResizeHandleSet: public QObject
{
    Q_OBJECT

public:
    typedef QHash<QString, ResizeHandleSet*> Hash;

    ResizeHandleSet(QWidget *modify, Form *form, bool editing = false);
    ~ResizeHandleSet();

    void setWidget(QWidget *modify, bool editing = false);
    QWidget *widget() const {
        return m_widget;
    }
    void raise();
    void setEditingMode(bool editing);

private:
    QPointer<ResizeHandle> m_handles[8];
    QPointer<QWidget> m_widget;
    QPointer<Form>   m_form;
    bool  m_editing;

    friend class ResizeHandle;
};

}

#endif
