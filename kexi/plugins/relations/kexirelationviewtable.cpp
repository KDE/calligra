/* This file is part of the KDE project
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qheader.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcursor.h>
#include <qpoint.h>

#include <kdebug.h>
#include <kiconloader.h>

#include <kexiDB/kexidbtable.h>
#include <kexiDB/kexidbfield.h>
#include "kexirelationviewtable.h"
#include "kexirelationview.h"

#include <kexidragobjects.h>

#include <stdlib.h>

KexiRelationViewTableContainer::KexiRelationViewTableContainer(KexiRelationView *parent, QString table, const KexiDBTable *t)
 : QFrame(parent,"tv", QFrame::Panel | QFrame::Raised)
{
//	setFixedSize(100, 150);
	resize(100, 150);
	//setMouseTracking(true);
	m_mousePressed = false;

	QGridLayout *g = new QGridLayout(this);
	g->setMargin(3);

	QLabel *l = new KexiRelationViewTableContainerHeader(table, this);
	l->setPaletteBackgroundColor(colorGroup().highlight());

//	QPushButton *btnClose = new QPushButton("x", this, "x");
//	btnClose->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
//	btnClose->setFixedSize(15, 15);
//	btnClose->setFlat(true);

	m_tableView = new KexiRelationViewTable(this, parent, table, t, "tbl-list");
	connect(m_tableView, SIGNAL(tableScrolling()), this, SLOT(moved()));

//	g->addWidget(btnClose, 0, 1);
	g->addWidget(l, 0, 0);
	g->addWidget(m_tableView, 1, 0);

	m_tbHeight = l->height();


	setFrameStyle(QFrame::WinPanel | QFrame::Raised);
	connect(l,SIGNAL(moved()),this,SLOT(moved()));
}

void KexiRelationViewTableContainer::moved() {
//	kdDebug()<<"finally emitting moved"<<endl;
	emit moved(this);
}

int KexiRelationViewTableContainer::globalY(const QString &field)
{
//	m_ta
//	kdDebug() << "KexiRelationViewTableContainer::globalY()" << endl;
	QPoint o = mapFromGlobal(QPoint(0, (m_tableView->globalY(field))));
//	kdDebug() << "KexiRelationViewTableContainer::globalY() db2" << endl;
	return mapToParent(o).y();
}

void
KexiRelationViewTableContainer::mousePressEvent(QMouseEvent *ev)
{
	m_mousePressed = true;
	m_bX = ev->x();
	m_bY = ev->y();

	QFrame::mousePressEvent(ev);
}

void
KexiRelationViewTableContainer::mouseMoveEvent(QMouseEvent *ev)
{
	return;
	if(m_mousePressed)
	{
//		move(ev->x() - m_bX, ev->y() - m_bY);
		if(m_bY < m_tbHeight)
		{
			QPoint movePoint(ev->x() - m_bX, ev->y() - m_bY);
			move(mapToParent(movePoint));
		}
		else if(ev->x() >= width() - 3)
		{
			if(ev->y() >= height() - 6)
				resize(ev->x(), ev->y());
			else
				resize(ev->x(), height());
		}

		emit moved(this);
	}
	else
	{
		if(ev->x() >= width() - 3)
		{
			if(ev->y() >= height() - 3)
				setCursor(QCursor(SizeFDiagCursor));
			else
				setCursor(QCursor(SizeHorCursor));
		}
		else
		{
			setCursor(QCursor(ArrowCursor));
		}
	}

	QFrame::mouseMoveEvent(ev);
}

void
KexiRelationViewTableContainer::mouseReleaseEvent(QMouseEvent *ev)
{
	m_mousePressed = false;
	QFrame::mouseMoveEvent(ev);
}

const QString
KexiRelationViewTableContainer::table()
{
	return m_tableView->table();
}

KexiRelationViewTableContainer::~KexiRelationViewTableContainer()
{
}


//END KexiRelationViewTableContainer

//BEGIN KexiRelatoinViewTableContainerHeader

KexiRelationViewTableContainerHeader::KexiRelationViewTableContainerHeader(const QString& text,QWidget *parent)
	:QLabel(text,parent),m_dragging(false) {
	installEventFilter(this);
}

KexiRelationViewTableContainerHeader::~KexiRelationViewTableContainerHeader() {
}

bool KexiRelationViewTableContainerHeader::eventFilter(QObject *obj, QEvent *ev) {
	if (ev->type()==QEvent::MouseMove)
	{
		if (m_dragging) {
			int diffX,diffY;
			diffX=static_cast<QMouseEvent*>(ev)->globalPos().x()-m_grabX;
			diffY=static_cast<QMouseEvent*>(ev)->globalPos().y()-m_grabY;
			if ((abs(diffX)>2) || (abs(diffY)>2))
			{
				QPoint newPos=parentWidget()->pos()+QPoint(diffX,diffY);

//correct the x position
				if (newPos.x()<0) {
					m_offsetX+=newPos.x();
					newPos.setX(0);
				}
				else
				if (m_offsetX<0) {
					m_offsetX+=newPos.x();
					if (m_offsetX>0) {
						newPos.setX(m_offsetX);
						m_offsetX=0;
					}
					else newPos.setX(0);
				}
//correct the y position
				if (newPos.y()<0) {
					m_offsetY+=newPos.y();
					newPos.setY(0);
				}
				else
				if (m_offsetY<0) {
					m_offsetY+=newPos.y();
					if (m_offsetY>0) {
						newPos.setY(m_offsetY);
						m_offsetY=0;
					}
					else newPos.setY(0);
				}
//move and update helpers

				parentWidget()->move(newPos);
				m_grabX=static_cast<QMouseEvent*>(ev)->globalPos().x();
				m_grabY=static_cast<QMouseEvent*>(ev)->globalPos().y();
//				kdDebug()<<"HEADER:emitting moved"<<endl;
				emit moved();
			}
			return true;
		}
	}
	return false;
}

void KexiRelationViewTableContainerHeader::mousePressEvent(QMouseEvent *ev) {
	kdDebug()<<"KexiRelationViewTableContainerHeader::Mouse Press Event"<<endl;
	m_dragging=true;
	m_grabX=ev->globalPos().x();
	m_grabY=ev->globalPos().y();
	m_offsetX=0;
	m_offsetY=0;
}

void KexiRelationViewTableContainerHeader::mouseReleaseEvent(QMouseEvent *ev) {
	kdDebug()<<"KexiRelationViewTableContainerHeader::Mouse Release Event"<<endl;
	m_dragging=false;
}

//END KexiRelatoinViewTableContainerHeader




KexiRelationViewTable::KexiRelationViewTable(QWidget *parent, KexiRelationView *view, QString table,
                                             const KexiDBTable *t, const char *name)
 : KListView(parent)
{
//	m_fieldList = t.;
	m_table = table;
//	m_parent = parent;

	setAcceptDrops(true);
	viewport()->setAcceptDrops(true);
	setDropVisualizer(false);

	addColumn("", 0);
	addColumn("fields");
	setResizeMode(QListView::LastColumn);
//	setResizeMode(QListView::AllColumns);
	header()->hide();

	setSorting(0, true); // disable sorting

	int order=0;

	for(uint i=0; i < t->fieldCount(); i++)
	{
		KexiDBField f = t->field(i);
		KListViewItem *i = new KListViewItem(this, QString::number(order), f.name());
		if(f.primary_key() || f.unique_key())
			i->setPixmap(1, SmallIcon("key"));

		i->setDragEnabled(true);
		i->setDropEnabled(true);
		order++;
	}

//	setDragEnabled
	connect(this, SIGNAL(dropped(QDropEvent *, QListViewItem *)), this, SLOT(slotDropped(QDropEvent *)));
	connect(this, SIGNAL(contentsMoving(int, int)), this, SLOT(slotContentsMoving(int,int)));

	m_view = view;
}

void KexiRelationViewTable::setReadOnly(bool b)
{
	setAcceptDrops(!b);
	viewport()->setAcceptDrops(!b);
}

int
KexiRelationViewTable::globalY(const QString &item)
{
	QListViewItem *i = findItem(item, 1);
	if(i)
	{
		int y=itemRect(i).y() + (itemRect(i).height() / 2);
		return mapToGlobal(QPoint(0, y)).y();
	}
	return -1;
}

QDragObject *
KexiRelationViewTable::dragObject()
{
//	qDebug("KexiRelationViewTable::dragObject()");
	if(selectedItem())
	{
		KexiFieldDrag *drag = new KexiFieldDrag(
			"kexi/table",m_table,selectedItem()->text(1), this, "metaDrag");
		return drag;
	}

	return 0;
}

bool
KexiRelationViewTable::acceptDrag(QDropEvent *ev) const
{
//	kdDebug() << "KexiRelationViewTable::acceptDrag()" << endl;
	if(ev->provides("kexi/field") && ev->source() != (QWidget*)this)
		return true;

	return false;
}

void
KexiRelationViewTable::slotDropped(QDropEvent *ev)
{
//	qDebug("KexiRelationViewTable::slotDropped()");

	QListViewItem *recever = itemAt(ev->pos());
	if(recever)
	{
		QString srcTable;
		QString dummy;
		QString srcField;
		//better check later if the source is really a table
		KexiFieldDrag::decode(ev,dummy,srcTable,srcField);
//		kdDebug() << "KexiRelationViewTable::slotDropped() srcfield: " << srcField << endl;

		QString rcvField = recever->text(1);

		SourceConnection s;
		s.srcTable = srcTable;
		s.rcvTable = m_table;
		s.srcField = srcField;
		s.rcvField = rcvField;

//		m_parent->addConnection(s);
		m_view->addConnection(s, false);

		kdDebug() << "KexiRelationViewTable::slotDropped() " << srcTable << ":" << srcField << " " << m_table << ":" << rcvField << endl;
		ev->accept();
		return;
	}
	ev->ignore();

}

void
KexiRelationViewTable::slotContentsMoving(int,int)
{
	emit tableScrolling();
}

KexiRelationViewTable::~KexiRelationViewTable()
{
}

#include "kexirelationviewtable.moc"
