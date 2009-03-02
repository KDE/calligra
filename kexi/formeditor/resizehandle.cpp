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

#include <kdebug.h>
#include <klocale.h>

#include <qpainter.h>
#include <qcursor.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QEvent>
#include <QPaintEvent>

#include "form.h"
//#include "formmanager.h"
#include "resizehandle.h"
#include "container.h"
#include "widgetfactory.h"
#include "widgetlibrary.h"

#define MINIMUM_WIDTH 10
#define MINIMUM_HEIGHT 10

using namespace KFormDesigner;

ResizeHandle::ResizeHandle(ResizeHandleSet *set, HandlePos pos, bool editing)
        : QWidget(set->m_widget->parentWidget()), m_set(set), m_pos(pos)
{
// setBackgroundMode(Qt::NoBackground);
    m_dragging = false;
    //m_editing = editing;
    setEditingMode(editing);
    setFixedSize(6, 6);
    //m_buddy = buddy;
    //buddy->installEventFilter(this);
    m_set->m_widget->installEventFilter(this);
    setAutoFillBackground(true);
//js installEventFilter(this);

    updatePos();
    show();
}

ResizeHandle::~ResizeHandle()
{
}

void ResizeHandle::setEditingMode(bool editing)
{
    QPalette pal(palette());
    pal.setBrush(backgroundRole(), editing ? QBrush(Qt::blue) : pal.text());
    setPalette(pal);
}

void ResizeHandle::updatePos()
{
    switch (m_pos) {
    case TopLeftCorner:
        move(m_set->m_widget->x() - 3, m_set->m_widget->y() - 3);
        setCursor(QCursor(Qt::SizeFDiagCursor));
        break;
    case TopCenter:
        move(m_set->m_widget->x() + m_set->m_widget->width() / 2 - 3, m_set->m_widget->y() - 3);
        setCursor(QCursor(Qt::SizeVerCursor));
        break;
    case TopRightCorner:
        move(m_set->m_widget->x() + m_set->m_widget->width() - 3, m_set->m_widget->y() - 3);
        setCursor(QCursor(Qt::SizeBDiagCursor));
        break;
    case LeftCenter:
        move(m_set->m_widget->x() - 3, m_set->m_widget->y() + m_set->m_widget->height() / 2 - 3);
        setCursor(QCursor(Qt::SizeHorCursor));
        break;
    case RightCenter:
        move(m_set->m_widget->x() + m_set->m_widget->width() - 3, m_set->m_widget->y() + m_set->m_widget->height() / 2 - 3);
        setCursor(QCursor(Qt::SizeHorCursor));
        break;
    case BottomLeftCorner:
        move(m_set->m_widget->x() - 3, m_set->m_widget->y() + m_set->m_widget->height() - 3);
        setCursor(QCursor(Qt::SizeBDiagCursor));
        break;
    case BottomCenter:
        move(m_set->m_widget->x() + m_set->m_widget->width() / 2 - 3, m_set->m_widget->y() + m_set->m_widget->height() - 3);
        setCursor(QCursor(Qt::SizeVerCursor));
        break;
    case BottomRightCorner:
        move(m_set->m_widget->x() + m_set->m_widget->width() - 3, m_set->m_widget->y() + m_set->m_widget->height() - 3);
        setCursor(QCursor(Qt::SizeFDiagCursor));
        break;
    }
}

bool ResizeHandle::eventFilter(QObject *o, QEvent *ev)
{
    if (((ev->type() == QEvent::Move) || (ev->type() == QEvent::Resize)) && o == m_set->m_widget) {
        //QTimer::singleShot(0,this,SLOT(updatePos()));
        updatePos();
    }
    /* else if (ev->type() == QEvent::Paint && o == this) {
        QPainter p;
        p.begin(m_set->m_widget, true);
        const bool unclipped = testWFlags( WPaintUnclipped );
        setWFlags( WPaintUnclipped );

        p.setPen(QPen(white, 10));
        p.setRasterOp(XorROP);
        p.drawRect( 20, 20, 100, 100 );//m_set->m_widget->x(), m_set->m_widget->y(), 150, 150 );
        p.drawRect( m_set->m_widget->x(), m_set->m_widget->y(), 150, 150 );
        if (!unclipped)
          clearWFlags( WPaintUnclipped );
        p.end();

        return true;
      }*/
    return false;
}

void ResizeHandle::mousePressEvent(QMouseEvent *ev)
{
    const bool startDragging = !m_dragging;
    m_dragging = true;
    m_x = ev->x();
    m_y = ev->y();
    if (startDragging) {
// m_form->resizeHandleDraggingStarted(m_set->widget());
        WidgetFactory *wfactory = m_set->m_form->library()->factoryForClassName(m_set->widget()->metaObject()->className());
        if (wfactory)
            wfactory->resetEditor();
    }
}

void ResizeHandle::mouseMoveEvent(QMouseEvent *ev)
{
    int gridX = m_set->m_form->gridSize();
    int gridY = m_set->m_form->gridSize();

    if (!m_dragging) return;
    //if(m_editing)  return;

    int tmpx = m_set->m_widget->x();
    int tmpy = m_set->m_widget->y();
    int tmpw = m_set->m_widget->width();
    int tmph = m_set->m_widget->height();

    int dummyx = ev->x() - m_x;
    int dummyy = ev->y() - m_y;

    if (m_set->m_form->isSnapWidgetsToGridEnabled() && ev->buttons() == Qt::LeftButton && ev->modifiers() != (Qt::ControlModifier | Qt::AltModifier)) {
        dummyy = (int)(((float)dummyy) / ((float)gridY) + 0.5);
        dummyy *= gridY;
        dummyx = (int)(((float)dummyx) / ((float)gridX) + 0.5);
        dummyx *= gridX;
    }

    switch (m_pos) {
    case TopRightCorner:
        tmpw += dummyx;
        tmpy += dummyy;
        tmph -= dummyy;
        break;
    case RightCenter:
        tmpw += dummyx;
        break;
    case BottomRightCorner:
        tmpw += dummyx;
        tmph += dummyy;
        break;
    case TopCenter:
        tmpy += dummyy;
        tmph -= dummyy;
        break;
    case BottomCenter:
        tmph = tmph + dummyy;
        break;
    case TopLeftCorner:
        tmpx += dummyx;
        tmpw -= dummyx;
        tmpy += dummyy;
        tmph -= dummyy;
        break;
    case LeftCenter:
        tmpx += dummyx;
        tmpw -= dummyx;
        break;
    case BottomLeftCorner:
        tmpx += dummyx;
        tmpw -= dummyx;
        tmph += dummyy;
        break;
    }

    // Not move the top-left corner further than the bottom-right corner
    if (tmpx >= m_set->m_widget->x() + m_set->m_widget->width()) {
        tmpx = m_set->m_widget->x() + m_set->m_widget->width() - MINIMUM_WIDTH;
        tmpw = MINIMUM_WIDTH;
    }

    if (tmpy >= m_set->m_widget->y() + m_set->m_widget->height()) {
        tmpy = m_set->m_widget->y() + m_set->m_widget->height() - MINIMUM_HEIGHT;
        tmph = MINIMUM_HEIGHT;
    }

    // Do not resize a widget outside of parent boundaries
    if (tmpx < 0) {
        tmpw += tmpx;
        tmpx = 0;
    } else if (tmpx + tmpw > m_set->m_widget->parentWidget()->width())
        tmpw = m_set->m_widget->parentWidget()->width() - tmpx;

    if (tmpy < 0) {
        tmph += tmpy;
        tmpy = 0;
    } else if (tmpy + tmph > m_set->m_widget->parentWidget()->height())
        tmph = m_set->m_widget->parentWidget()->height() - tmpy;

    const bool shouldBeMoved = (tmpx != m_set->m_widget->x()) || (tmpy != m_set->m_widget->y());
    const bool shouldBeResized = (tmpw != m_set->m_widget->width()) || (tmph != m_set->m_widget->height());

    if (shouldBeMoved && shouldBeResized)
        m_set->m_widget->hide();

    // Resize it
    if (shouldBeResized) {
        // Keep a QSize(10, 10) minimum size
        tmpw = (tmpw < MINIMUM_WIDTH) ? MINIMUM_WIDTH : tmpw;
        tmph = (tmph < MINIMUM_HEIGHT) ? MINIMUM_HEIGHT : tmph;
        m_set->m_widget->resize(tmpw, tmph);
    }

    // Move the widget if necessary
    if (shouldBeMoved)
        m_set->m_widget->move(tmpx, tmpy);

    if (shouldBeMoved && shouldBeResized)
        m_set->m_widget->show();
}

void ResizeHandle::mouseReleaseEvent(QMouseEvent *)
{
    m_dragging = false;
}

void ResizeHandle::paintEvent(QPaintEvent *)
{
    //draw XORed background

    /*QPainter p(this);
    p.setRasterOp(XorROP);
    p.fillRect(QRect(0, 0, 6, 6),white);
    bitBlt( this, QPoint(0,0), parentWidget(), rect(), XorROP);*/
}

/////////////// ResizeHandleSet //////////////////

ResizeHandleSet::ResizeHandleSet(QWidget *modify, Form *form, bool editing)
        : QObject(modify->parentWidget()), /*m_widget(modify),*/ m_form(form)
{
    m_widget = 0;
    /*QWidget *parent = modify->parentWidget();

    handles[0] = new ResizeHandle( modify, ResizeHandle::TopLeft, editing);
    handles[1] = new ResizeHandle( modify, ResizeHandle::TopCenter, editing);
    handles[2] = new ResizeHandle( modify, ResizeHandle::TopRight, editing);
    handles[3] = new ResizeHandle( modify, ResizeHandle::LeftCenter, editing);
    handles[4] = new ResizeHandle( modify, ResizeHandle::RightCenter, editing);
    handles[5] = new ResizeHandle( modify, ResizeHandle::BottomLeft, editing);
    handles[6] = new ResizeHandle( modify, ResizeHandle::BottomCenter, editing);
    handles[7] = new ResizeHandle( modify, ResizeHandle::BottomRight, editing);*/
    setWidget(modify, editing);
}

ResizeHandleSet::~ResizeHandleSet()
{
    for (int i = 0; i < 8; i++)
        delete m_handles[i];
}

void
ResizeHandleSet::setWidget(QWidget *modify, bool editing)
{
    if (modify == m_widget)
        return;

    if (m_widget) {
        for (int i = 0; i < 8; i++)
            delete m_handles[i];
    }

    m_widget = modify;

    m_handles[0] = new ResizeHandle(this, ResizeHandle::TopLeftCorner, editing);
    m_handles[1] = new ResizeHandle(this, ResizeHandle::TopCenter, editing);
    m_handles[2] = new ResizeHandle(this, ResizeHandle::TopRightCorner, editing);
    m_handles[3] = new ResizeHandle(this, ResizeHandle::LeftCenter, editing);
    m_handles[4] = new ResizeHandle(this, ResizeHandle::RightCenter, editing);
    m_handles[5] = new ResizeHandle(this, ResizeHandle::BottomLeftCorner, editing);
    m_handles[6] = new ResizeHandle(this, ResizeHandle::BottomCenter, editing);
    m_handles[7] = new ResizeHandle(this, ResizeHandle::BottomRightCorner, editing);
}

void
ResizeHandleSet::raise()
{
    for (int i = 0; i < 8; i++)
        m_handles[i]->raise();
}

void ResizeHandleSet::setEditingMode(bool editing)
{
    for (int i = 0; i < 8; i++)
        m_handles[i]->setEditingMode(editing);
}

#include "resizehandle.moc"
