/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qobjectlist.h>
#include <qpainter.h>
#include <qcursor.h>
#include <qapplication.h>
#include <qfocusdata.h>

#include <kdebug.h>

#include "kexidbform.h"

#include <formeditor/objecttree.h>

class KexiDBForm::Private
{
	public:
		Private()
		 : orderedFocusWidgetsIterator(orderedFocusWidgets)
		 , autoTabStops(false)
		{
		}
		QPtrList<QWidget> orderedFocusWidgets;
		QPtrListIterator<QWidget> orderedFocusWidgetsIterator;
		bool autoTabStops : 1;
};

//========================

KexiDBForm::KexiDBForm(QWidget *parent, const char *name/*, KexiDB::Connection *conn*/)
 : KexiGradientWidget(parent, name)
 , KexiDataItemInterface()
 , d(new Private())
{
//test	setDisplayMode( KexiGradientWidget::SimpleGradient );

	setFocusPolicy(NoFocus);

	//m_conn = conn;
	kexipluginsdbg << "KexiDBForm::KexiDBForm(): " << endl;
	setCursor(QCursor(Qt::ArrowCursor)); //to avoid keeping Size cursor when moving from form's boundaries
}

KexiDBForm::~KexiDBForm()
{
	kexipluginsdbg << "KexiDBForm::~KexiDBForm(): close" << endl;
	delete d;
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
KexiDBForm::drawRect(const QRect& r, int type)
{
	QValueList<QRect> l;
	l.append(r);
	drawRects(l, type);
}

void
KexiDBForm::drawRects(const QValueList<QRect> &list, int type)
{
	QPainter p;
	p.begin(this, true);
	bool unclipped = testWFlags( WPaintUnclipped );
	setWFlags( WPaintUnclipped );

	if (prev_rect.isValid()) {
		//redraw prev. selection's rectangle
		p.drawPixmap( QPoint(prev_rect.x()-2, prev_rect.y()-2), buffer, QRect(prev_rect.x()-2, prev_rect.y()-2, prev_rect.width()+4, prev_rect.height()+4));
	}
	p.setBrush(QBrush::NoBrush);
	if(type == 1) // selection rect
		p.setPen(QPen(white, 1, Qt::DotLine));
	else if(type == 2) // insert rect
		p.setPen(QPen(white, 2));
	p.setRasterOp(XorROP);

	prev_rect = QRect();
	QValueList<QRect>::ConstIterator endIt = list.constEnd();
	for(QValueList<QRect>::ConstIterator it = list.constBegin(); it != endIt; ++it) {
		p.drawRect(*it);
		prev_rect = prev_rect.unite(*it);
	}

	if (!unclipped)
		clearWFlags( WPaintUnclipped );
	p.end();
}

void
KexiDBForm::initBuffer()
{
	repaintAll(this);
	buffer.resize( width(), height() );
	buffer = QPixmap::grabWindow( winId() );
	prev_rect = QRect();
}

void
KexiDBForm::clearForm()
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
KexiDBForm::highlightWidgets(QWidget *from, QWidget *to)//, const QPoint &point)
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
KexiDBForm::sizeHint() const
{
	//todo: find better size (user configured?)
	return QSize(400,300);
}

void KexiDBForm::setInvalidState( const QString& displayText )
{
	//! @todo draw "invalid data source" text on the surface?
}

bool KexiDBForm::autoTabStops() const
{
	return d->autoTabStops;
}

void KexiDBForm::setAutoTabStops(bool set)
{
	d->autoTabStops = set;
}

QPtrList<QWidget>* KexiDBForm::orderedFocusWidgets() const
{
	return &d->orderedFocusWidgets;
}

void KexiDBForm::updateTabStopsOrder(KFormDesigner::Form* form)
{
	QWidget *fromWidget = 0;
	QWidget *topLevelWidget = form->widget()->topLevelWidget();
	form->updateTabStopsOrder(); //certain widgets can have now updated focusPolicy properties, fix this
	if (d->orderedFocusWidgets.isEmpty()) {
		//generate a new list
		for (KFormDesigner::ObjectTreeListIterator it(form->tabStopsIterator()); it.current(); ++it) {
			it.current()->widget()->installEventFilter(this);
			if (fromWidget) {
				kdDebug() << "KexiDBForm::updateTabStopsOrder() tab order: " << fromWidget->name() 
					<< " -> " << it.current()->widget()->name() << endl;
//				setTabOrder( fromWidget, it.current()->widget() );
			}
			fromWidget = it.current()->widget();
			d->orderedFocusWidgets.append( it.current()->widget() );
		}
	}
/*	else {
		//restore ordering
		for (QPtrListIterator<QWidget> it(d->orderedFocusWidgets); it.current(); ++it) {
			if (fromWidget) {
				kdDebug() << "KexiDBForm::updateTabStopsOrder() tab order: " << fromWidget->name() 
					<< " -> " << it.current()->name() << endl;
				setTabOrder( fromWidget, it.current() );
			}
			fromWidget = it.current();
		}
//		SET_FOCUS_USING_REASON(focusWidget(), QFocusEvent::Tab);
	}*/
 }

bool KexiDBForm::eventFilter ( QObject * watched, QEvent * e )
{
	if (e->type()==QEvent::KeyPress && static_cast<QWidget*>(watched)) {
		if (static_cast<QKeyEvent*>(e)->key() == Key_Tab
			|| static_cast<QKeyEvent*>(e)->key() == Key_Backtab)
		{
			if (d->orderedFocusWidgetsIterator.current() != static_cast<QWidget*>(watched)) {
				d->orderedFocusWidgetsIterator.toFirst();
				while (d->orderedFocusWidgetsIterator.current() && d->orderedFocusWidgetsIterator.current()!=static_cast<QWidget*>(watched)) {
					QWidget *ww = d->orderedFocusWidgetsIterator.current();
					++d->orderedFocusWidgetsIterator;
				}
			}
			kdDebug() << watched->name() << endl;
			if (static_cast<QKeyEvent*>(e)->key() == Key_Tab) {
				if (d->orderedFocusWidgets.first() && watched == d->orderedFocusWidgets.last()) {
					d->orderedFocusWidgetsIterator.toFirst();
				}
				else if (watched == d->orderedFocusWidgetsIterator.current()) {
					++d->orderedFocusWidgetsIterator; //next
				}
				else
					return true; //ignore
				//set focus, but don't use just setFocus() because certain widgets
				//behaves differently (e.g. QLineEdit calls selectAll()) when 
				//focus event's reason is QFocusEvent::Tab
				SET_FOCUS_USING_REASON(d->orderedFocusWidgetsIterator.current(), QFocusEvent::Tab);
				kdDebug() << "focusing " << d->orderedFocusWidgetsIterator.current()->name() << endl;
				return true;
			}	else if (static_cast<QKeyEvent*>(e)->key() == Key_BackTab) {
				if (d->orderedFocusWidgets.last() && watched == d->orderedFocusWidgets.first()) {
					d->orderedFocusWidgetsIterator.toLast();
				}
				else if (watched == d->orderedFocusWidgetsIterator.current()) {
					--d->orderedFocusWidgetsIterator; //prev
				}
				else
					return true; //ignore
				//set focus, see above note
				SET_FOCUS_USING_REASON(d->orderedFocusWidgetsIterator.current(), QFocusEvent::Backtab);
				kdDebug() << "focusing " << d->orderedFocusWidgetsIterator.current()->name() << endl;
				return true;
			}
		}
	}
	return KexiGradientWidget::eventFilter(watched, e);
}

#include "kexidbform.moc"

