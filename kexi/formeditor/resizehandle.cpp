/* This file is part of the KDE libraries
   Copyright (C) 2002 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2009 Jarosław Staniek <staniek@kde.org>

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

#include "form.h"
#include "resizehandle.h"
#include "container.h"
#include "widgetfactory.h"
#include "widgetlibrary.h"


#include <kdebug.h>
#include <klocale.h>

#include <QCursor>
#include <QMouseEvent>
#include <QPaintEvent>

#define MINIMUM_WIDTH 10
#define MINIMUM_HEIGHT 10

using namespace KFormDesigner;

namespace KFormDesigner {
/**
* A single widget which represents a small rectangle for resizing a form widget.
*/
class KFORMEDITOR_EXPORT ResizeHandle : public QWidget
{
public:
    enum HandlePos {
        TopLeftCorner = 1,
        TopCenter = 2,
        TopRightCorner = 4,
        LeftCenter = 8,
        RightCenter = 16,
        BottomLeftCorner = 32,
        BottomCenter = 64,
        BottomRightCorner = 128
    };

    ResizeHandle(ResizeHandleSet *set, HandlePos pos);

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
    class Private;
    Private* const d;
};

class ResizeHandle::Private
{
public:
    Private(ResizeHandleSet* set_, HandlePos pos_)
        : set(set_), pos(pos_), dragging(false)
    {}
    ~Private() {}

    ResizeHandleSet *set;
    HandlePos pos;
    bool dragging;
    int x;
    int y;
};
}

ResizeHandle::ResizeHandle(ResizeHandleSet *set, HandlePos pos)
    : QWidget(set->widget()->parentWidget()), d(new Private(set, pos))
{
    setFixedSize(6, 6);
    d->set->widget()->installEventFilter(this);
    setAutoFillBackground(true);
    updatePos();
    setEditingMode(false);
    show();
}

ResizeHandle::~ResizeHandle()
{
    delete d;
}

void ResizeHandle::setEditingMode(bool editing)
{
    QPalette pal(palette());
    pal.setBrush(backgroundRole(), editing ? QBrush(Qt::blue) : pal.text());
    setPalette(pal);
}

void ResizeHandle::updatePos()
{
    switch (d->pos) {
    case TopLeftCorner:
        move(d->set->widget()->x() - 3, d->set->widget()->y() - 3);
        setCursor(QCursor(Qt::SizeFDiagCursor));
        break;
    case TopCenter:
        move(d->set->widget()->x() + d->set->widget()->width() / 2 - 3, d->set->widget()->y() - 3);
        setCursor(QCursor(Qt::SizeVerCursor));
        break;
    case TopRightCorner:
        move(d->set->widget()->x() + d->set->widget()->width() - 3, d->set->widget()->y() - 3);
        setCursor(QCursor(Qt::SizeBDiagCursor));
        break;
    case LeftCenter:
        move(d->set->widget()->x() - 3, d->set->widget()->y() + d->set->widget()->height() / 2 - 3);
        setCursor(QCursor(Qt::SizeHorCursor));
        break;
    case RightCenter:
        move(d->set->widget()->x() + d->set->widget()->width() - 3, d->set->widget()->y() + d->set->widget()->height() / 2 - 3);
        setCursor(QCursor(Qt::SizeHorCursor));
        break;
    case BottomLeftCorner:
        move(d->set->widget()->x() - 3, d->set->widget()->y() + d->set->widget()->height() - 3);
        setCursor(QCursor(Qt::SizeBDiagCursor));
        break;
    case BottomCenter:
        move(d->set->widget()->x() + d->set->widget()->width() / 2 - 3, d->set->widget()->y() + d->set->widget()->height() - 3);
        setCursor(QCursor(Qt::SizeVerCursor));
        break;
    case BottomRightCorner:
        move(d->set->widget()->x() + d->set->widget()->width() - 3, d->set->widget()->y() + d->set->widget()->height() - 3);
        setCursor(QCursor(Qt::SizeFDiagCursor));
        break;
    }
}

bool ResizeHandle::eventFilter(QObject *o, QEvent *ev)
{
    if (((ev->type() == QEvent::Move) || (ev->type() == QEvent::Resize)) && o == d->set->widget()) {
        updatePos();
    }
    return false;
}

void ResizeHandle::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() != Qt::LeftButton)
        return;

    const bool startDragging = !d->dragging;
    d->dragging = true;
    d->x = ev->x();
    d->y = ev->y();
    if (startDragging) {
        d->set->resizeStarted();
        d->set->form()->resetInlineEditor();
        emit d->set->geometryChangeStarted();
    }
}

void ResizeHandle::mouseMoveEvent(QMouseEvent *ev)
{
    int gridX = d->set->form()->gridSize();
    int gridY = d->set->form()->gridSize();

    if (!d->dragging)
        return;

    int tmpx = d->set->widget()->x();
    int tmpy = d->set->widget()->y();
    int tmpw = d->set->widget()->width();
    int tmph = d->set->widget()->height();

    int dummyx = ev->x() - d->x;
    int dummyy = ev->y() - d->y;

    if (   d->set->form()->isSnapToGridEnabled() && ev->buttons() == Qt::LeftButton
        && ev->modifiers() != (Qt::ControlModifier | Qt::AltModifier))
    {
        dummyx = alignValueToGrid(dummyx, gridX);
        dummyy = alignValueToGrid(dummyy, gridY);
    }

    switch (d->pos) {
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

    // Do not move the top-left corner further than the bottom-right corner
    if (tmpx >= d->set->widget()->x() + d->set->widget()->width()) {
        tmpx = d->set->widget()->x() + d->set->widget()->width() - MINIMUM_WIDTH;
        tmpw = MINIMUM_WIDTH;
    }

    if (tmpy >= d->set->widget()->y() + d->set->widget()->height()) {
        tmpy = d->set->widget()->y() + d->set->widget()->height() - MINIMUM_HEIGHT;
        tmph = MINIMUM_HEIGHT;
    }

    // Do not resize a widget outside of parent boundaries
    if (tmpx < 0) {
        tmpw += tmpx;
        tmpx = 0;
    } else if (tmpx + tmpw > d->set->widget()->parentWidget()->width()) {
        tmpw = d->set->widget()->parentWidget()->width() - tmpx;
    }

    if (tmpy < 0) {
        tmph += tmpy;
        tmpy = 0;
    } else if (tmpy + tmph > d->set->widget()->parentWidget()->height()) {
        tmph = d->set->widget()->parentWidget()->height() - tmpy;
    }

    const bool shouldBeMoved = (tmpx != d->set->widget()->x()) || (tmpy != d->set->widget()->y());
    const bool shouldBeResized = (tmpw != d->set->widget()->width()) || (tmph != d->set->widget()->height());

    if (shouldBeMoved && shouldBeResized) {
        d->set->widget()->hide();
    }

    // Resize it
    if (shouldBeResized) {
        // Keep a QSize(10, 10) minimum size
        tmpw = (tmpw < MINIMUM_WIDTH) ? MINIMUM_WIDTH : tmpw;
        tmph = (tmph < MINIMUM_HEIGHT) ? MINIMUM_HEIGHT : tmph;
    }

    //kDebug() << "geometry: OLD" << d->set->widget()->geometry() << "NEW" << QRect(tmpx, tmpy, tmpw, tmph);
    emit d->set->geometryChanged(QRect(tmpx, tmpy, tmpw, tmph));

    if (shouldBeMoved && shouldBeResized) {
        d->set->widget()->show();
    }
}

void ResizeHandle::mouseReleaseEvent(QMouseEvent *)
{
    d->dragging = false;
    d->set->resizeFinished();
}

void ResizeHandle::paintEvent(QPaintEvent *)
{
}

/////////////// ResizeHandleSet //////////////////

class ResizeHandleSet::Private
{
public:
    Private();
    ~Private() {}

    QRect origWidgetRect;
    QPointer<ResizeHandle> handles[8];
    QPointer<QWidget> widget;
    QPointer<Form>   form;
    bool  editing;
};

ResizeHandleSet::Private::Private() : widget(0)
{
}

ResizeHandleSet::ResizeHandleSet(QWidget *modify, Form *form)
    : QObject(modify->parentWidget()), d(new Private)
{
    d->form = form;
    setWidget(modify);
}

ResizeHandleSet::~ResizeHandleSet()
{
    for (int i = 0; i < 8; i++)
        delete d->handles[i];
    delete d;
}

void
ResizeHandleSet::setWidget(QWidget *modify)
{
    if (modify == d->widget)
        return;

    if (d->widget) {
        for (int i = 0; i < 8; i++)
            delete d->handles[i];
    }

    d->widget = modify;

    d->handles[0] = new ResizeHandle(this, ResizeHandle::TopLeftCorner);
    d->handles[1] = new ResizeHandle(this, ResizeHandle::TopCenter);
    d->handles[2] = new ResizeHandle(this, ResizeHandle::TopRightCorner);
    d->handles[3] = new ResizeHandle(this, ResizeHandle::LeftCenter);
    d->handles[4] = new ResizeHandle(this, ResizeHandle::RightCenter);
    d->handles[5] = new ResizeHandle(this, ResizeHandle::BottomLeftCorner);
    d->handles[6] = new ResizeHandle(this, ResizeHandle::BottomCenter);
    d->handles[7] = new ResizeHandle(this, ResizeHandle::BottomRightCorner);
}

void
ResizeHandleSet::raise()
{
    for (int i = 0; i < 8; i++)
        d->handles[i]->raise();
}

void ResizeHandleSet::setEditingMode(bool editing)
{
    for (int i = 0; i < 8; i++)
        d->handles[i]->setEditingMode(editing);
}

void ResizeHandleSet::resizeStarted()
{
    d->origWidgetRect = d->widget->geometry();
}

void ResizeHandleSet::resizeFinished()
{
    if (d->widget) {
        //kDebug() << "old:" << d->origWidgetRect << "new:" << d->widget->geometry();
    }
}

QWidget* ResizeHandleSet::widget() const
{
    return d->widget;
}

Form* ResizeHandleSet::form() const
{
    return d->form;
}

