/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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
 * Boston, MA 02110-1301, USA.
*/

#include <qobject.h>
#include <qpainter.h>
#include <qcursor.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3ValueList>

#include <kdebug.h>

#include "kexireportform.h"

KexiReportForm::KexiReportForm(QWidget *parent, const char *name/*, KexiDB::Connection *conn*/)
 : QWidget(parent, name)
{
  //m_conn = conn;
  kexipluginsdbg << "KexiReportForm::KexiReportForm(): " << endl;
  setCursor(QCursor(Qt::ArrowCursor)); //to avoid keeping Size cursor when moving from form's boundaries
  setBackgroundColor(Qt::white);
}

KexiReportForm::~KexiReportForm()
{
  kexipluginsdbg << "KexiReportForm::~KexiReportForm(): close" << endl;
}

//repaint all children widgets
static void repaintAll(QWidget *w)
{
  QObjectList *list = w->queryList("QWidget");
  QObjectListIt it(*list);
  for (QObject *obj; (obj=it.current()); ++it ) {
    static_cast<QWidget*>(obj)->repaint();
  }
  delete list;
}

void
KexiReportForm::drawRect(const QRect& r, int type)
{
  Q3ValueList<QRect> l;
  l.append(r);
  drawRects(l, type);
}

void
KexiReportForm::drawRects(const Q3ValueList<QRect> &list, int type)
{
  QPainter p;
  p.begin(this, true);
  bool unclipped = testWFlags( WPaintUnclipped );
  setWFlags( WPaintUnclipped );

  if (prev_rect.isValid()) {
    //redraw prev. selection's rectangle
    p.drawPixmap( QPoint(prev_rect.x()-2, prev_rect.y()-2), buffer, QRect(prev_rect.x()-2, prev_rect.y()-2, prev_rect.width()+4, prev_rect.height()+4));
  }
  p.setBrush(Qt::NoBrush);
  if(type == 1) // selection rect
    p.setPen(QPen(Qt::white, 1, Qt::DotLine));
  else if(type == 2) // insert rect
    p.setPen(QPen(Qt::white, 2));
  p.setRasterOp(XorROP);

  prev_rect = QRect();
  Q3ValueList<QRect>::ConstIterator endIt = list.constEnd();
  for(Q3ValueList<QRect>::ConstIterator it = list.constBegin(); it != endIt; ++it) {
    p.drawRect(*it);
    prev_rect = prev_rect.unite(*it);
  }

  if (!unclipped)
    clearWFlags( WPaintUnclipped );
  p.end();
}

void
KexiReportForm::initBuffer()
{
  repaintAll(this);
  buffer.resize( width(), height() );
  buffer = QPixmap::grabWindow( winId() );
  prev_rect = QRect();
}

void
KexiReportForm::clearForm()
{
  QPainter p;
  p.begin(this, true);
  bool unclipped = testWFlags( WPaintUnclipped );
  setWFlags( WPaintUnclipped );

  //redraw entire form surface
  p.drawPixmap( QPoint(0,0), buffer, QRect(0,0,buffer.width(), buffer.height()) );

  if (!unclipped)
    clearWFlags( WPaintUnclipped );
  p.end();

  repaintAll(this);
}

void
KexiReportForm::highlightWidgets(QWidget *from, QWidget *to)//, const QPoint &point)
{
  QPoint fromPoint, toPoint;
  if(from && from->parentWidget() && (from != this))
    fromPoint = from->parentWidget()->mapTo(this, from->pos());
  if(to && to->parentWidget() && (to != this))
    toPoint = to->parentWidget()->mapTo(this, to->pos());

  QPainter p;
  p.begin(this, true);
  bool unclipped = testWFlags( WPaintUnclipped );
  setWFlags( WPaintUnclipped );

  if (prev_rect.isValid()) {
    //redraw prev. selection's rectangle
    p.drawPixmap( QPoint(prev_rect.x(), prev_rect.y()), buffer, QRect(prev_rect.x(), prev_rect.y(), prev_rect.width(), prev_rect.height()));
  }

  p.setPen( QPen(Qt::red, 2) );

  if(to)
  {
    QPixmap pix1 = QPixmap::grabWidget(from);
    QPixmap pix2 = QPixmap::grabWidget(to);

    if((from != this) && (to != this))
      p.drawLine( from->parentWidget()->mapTo(this, from->geometry().center()), to->parentWidget()->mapTo(this, to->geometry().center()) );

    p.drawPixmap(fromPoint.x(), fromPoint.y(), pix1);
    p.drawPixmap(toPoint.x(), toPoint.y(), pix2);

    if(to == this)
      p.drawRoundRect(2, 2, width()-4, height()-4, 4, 4);
    else
      p.drawRoundRect(toPoint.x(), toPoint.y(), to->width(), to->height(), 5, 5);
  }

  if(from == this)
    p.drawRoundRect(2, 2, width()-4, height()-4, 4, 4);
  else
    p.drawRoundRect(fromPoint.x(),  fromPoint.y(), from->width(), from->height(), 5, 5);

  if((to == this) || (from == this))
    prev_rect = QRect(0, 0, buffer.width(), buffer.height());
  else if(to)
  {
    prev_rect.setX( (fromPoint.x() < toPoint.x()) ? (fromPoint.x() - 5) : (toPoint.x() - 5) );
    prev_rect.setY( (fromPoint.y() < toPoint.y()) ? (fromPoint.y() - 5) : (toPoint.y() - 5) );
    prev_rect.setRight( (fromPoint.x() < toPoint.x()) ? (toPoint.x() + to->width() + 10) : (fromPoint.x() + from->width() + 10) );
    prev_rect.setBottom( (fromPoint.y() < toPoint.y()) ? (toPoint.y() + to->height() + 10) : (fromPoint.y() + from->height() + 10) ) ;
  }
  else
    prev_rect = QRect(fromPoint.x()- 5,  fromPoint.y() -5, from->width() + 10, from->height() + 10);

  if (!unclipped)
    clearWFlags( WPaintUnclipped );
  p.end();
}

QSize
KexiReportForm::sizeHint() const
{
  //todo: find better size (user configured?)
  return QSize(400,300);
}

#include "kexireportform.moc"

