/* This file is part of the KDE project
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qheader.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcursor.h>

#include <kdebug.h>

#include "kexirelationviewtable.h"
#include "kexirelationview.h"

#include <kexidragobjects.h>

KexiRelationViewTableContainer::KexiRelationViewTableContainer(KexiRelationView *parent, QString table, QStringList fields)
 : QFrame(parent,"tv", QFrame::Panel | QFrame::Raised)
{
//	setFixedSize(100, 150);
	resize(100, 150);
	setMouseTracking(true);
	m_mousePressed = false;

	QGridLayout *g = new QGridLayout(this);
	g->setMargin(3);

	QLabel *l = new QLabel(table, this);
	l->setPaletteBackgroundColor(colorGroup().highlight());
	g->addWidget(l, 0, 0);
	
	QPushButton *btnClose = new QPushButton("x", this, "x");
	btnClose->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	btnClose->setFixedSize(15, 15);
//	btnClose->setFlat(true);

	m_tableView = new KexiRelationViewTable(this, parent, table, fields, "tbl-list");

	g->addWidget(btnClose, 0, 1);
	g->addMultiCellWidget(m_tableView, 1, 1, 0, 1);

	m_tbHeight = l->height();


	setFrameStyle(QFrame::WinPanel | QFrame::Raised);
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
	if(m_mousePressed)
	{
//		move(ev->x() - m_bX, ev->y() - m_bY);
		if(m_bY < m_tbHeight)
		{
			QPoint movePoint(ev->x() - m_bX, ev->y() - m_bY);
			move(mapToParent(movePoint));
		}
		else if(ev-x() >= width() - 3)
		{
			if(ev->y() >= height() - 6)
				resize(ev->x(), ev->y());
			else
				resize(ev->x(), height());
		}
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

KexiRelationViewTableContainer::~KexiRelationViewTableContainer()
{
}

KexiRelationViewTable::KexiRelationViewTable(QWidget *parent, KexiRelationView *view, QString table, QStringList fields, const char *name)
 : KListView(parent)
{
	m_fieldList = fields;
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
	for(QStringList::Iterator it = m_fieldList.begin(); it != m_fieldList.end(); it++)
	{
		KListViewItem *i = new KListViewItem(this, QString::number(order), (*it));
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
	qDebug("KexiRelationViewTable::dragObject()");
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
	kdDebug() << "KexiRelationViewTable::acceptDrag()" << endl;
	if(ev->provides("kexi/field") && ev->source() != this)
		return true;

	return false;
}

void
KexiRelationViewTable::slotDropped(QDropEvent *ev)
{
	qDebug("KexiRelationViewTable::slotDropped()");

	QListViewItem *recever = itemAt(ev->pos());
	if(recever)
	{


		QString srcTable;
		QString dummy;
		QString srcField;
		//better check later if the source is really a table
		KexiFieldDrag::decode(ev,dummy,srcTable,srcField);
		kdDebug() << "KexiRelationViewTable::slotDropped() srcfield: " << srcField << endl;

		QString rcvField = recever->text(1);

		SourceConnection s;
		s.srcTable = srcTable;
		s.rcvTable = m_table;
		s.srcField = srcField;
		s.rcvField = rcvField;

//		m_parent->addConnection(s);
		m_view->addConnection(s);

		kdDebug() << "KexiRelationViewTable::slotDropped() " << srcTable << ":" << srcField << " " << m_table << ":" << rcvField << endl;
		ev->accept();
		return;
	}
	ev->ignore();

}

void
KexiRelationViewTable::slotContentsMoving(int,int)
{
	emit tableScrolling(table());
}

KexiRelationViewTable::~KexiRelationViewTable()
{
}

#include "kexirelationviewtable.moc"
