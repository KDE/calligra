/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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

#include <kdebug.h>

#include <qstringlist.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qheader.h>
#include <qevent.h>
#include <qpainter.h>
#include <qstyle.h>
#include <qlineedit.h>
//#include <qbytearray.h>

#include <koApplication.h>

#include "kexirelationview.h"
#include "kexirelationviewtable.h"
#include "kexirelationviewconnection.h"
#include "kexirelation.h"

KexiRelationView::KexiRelationView(QWidget *parent, const char *name,KexiRelation *relation)
 : QScrollView(parent, name)
{
	m_relation=relation;
	m_relation->incUsageCount();
	m_readOnly=false;
	setFrameStyle(QFrame::WinPanel | QFrame::Sunken);

	m_tableCount = 0;


	viewport()->setPaletteBackgroundColor(colorGroup().mid());
	
//	c->resize(250,250);
//	c->show(); 
//	addChild(c, 100,100);
}

void
KexiRelationView::addTable(const QString &table, QStringList columns)
{
	kdDebug() << "KexiRelationView::addTable(): " << table << endl;

	if(m_tables.contains(table))
		return;

//	RelationSource s;
//	s.table = table;

//	int widest = -15;
//	for(TableList::Iterator it=m_tables.begin(); it != m_tables.end(); it++)
//	{
//		if((*it).geometry.x() + (*it).geometry.width() > widest)
//		{
//			widest = (*it).geometry.x() + (*it).geometry.width();
//		}
//	}

//	s.geometry = QRect(widest + 20, 5, 100, 150);

	KexiRelationViewTableContainer *c = new KexiRelationViewTableContainer(this, table, columns);
//	c->show(); 
	addChild(c, 100,100);
	c->show();

	connect(c, SIGNAL(moved(KexiRelationViewTableContainer *)), this,
	 SLOT(containerMoved(KexiRelationViewTableContainer *)));

	m_tables.insert(table, c);
/*	KexiRelationViewTable *tableView = new KexiRelationViewTable(this, table, columns, "someTable");
	tableView->setReadOnly(m_readOnly);
	s.columnView = tableView;

	tableView->setFixedSize(s.geometry.width() - 5, s.geometry.height() - 19);
	moveChild(tableView, s.geometry.x() + 2, s.geometry.y() + 16);

	updateContents(s.geometry);
	tableView->show();

	m_tables.insert(table, s);
	
	connect(tableView, SIGNAL(tableScrolling(QString)), this, SLOT(slotTableScrolling(QString)));
*/
}

void
KexiRelationView::drawContents(QPainter *p, int cx, int cy, int cw, int ch)
{
	KexiRelationViewConnection *cview;

	QRect clipping(cx, cy, cw, ch);
	for(cview = m_connectionViews.first(); cview; cview = m_connectionViews.next())
	{
//		p->drawRect(cview->connectionRect());
		if(clipping.intersects(cview->connectionRect()))
			cview->drawConnection(p, this);
	}
/*
	QRect clip(cx, cy, cw, ch);
	for(TableList::Iterator it=m_tables.begin(); it != m_tables.end(); it++)
	{
		RelationSource s = (*it);
		if(clip.intersects(s.geometry))
		{
			drawSource(p, s);
		}
	}

	for(RelationList::Iterator itC=m_connections.begin(); itC != m_connections.end(); itC++)
	{
		SourceConnection *c = &(*itC);

		if(clip.intersects(c->geometry))
			drawConnection(p, c, true);
	}
*/
}
/*
void
KexiRelationView::drawSource(QPainter *p, RelationSource src)
{

	kapp->style().drawPrimitive(QStyle::PE_PanelPopup, p, src.geometry, colorGroup());
	p->setBrush(QBrush(colorGroup().background()));
	p->setBrush(QBrush(colorGroup().highlight()));
	p->setPen(QPen(colorGroup().highlight()));
	p->drawRect(src.geometry.x() + 2, src.geometry.y() + 2, src.geometry.width() - 3, 15);
	p->setPen(QPen(colorGroup().highlightedText()));
	p->drawText(src.geometry.x() + 3, src.geometry.y() + 3, src.geometry.width() - 4, 14, AlignLeft | SingleLine | AlignVCenter, src.table);
}


void
KexiRelationView::drawConnection(QPainter *p, SourceConnection *conn, bool paint)
{

	KexiRelationViewTable *tSrc = m_tables[(*conn).srcTable].columnView;
	if(!tSrc)
		return;

	KexiRelationViewTable *tRcv = m_tables[(*conn).rcvTable].columnView;
	if(!tRcv)
		return;

	int srcY = mapFromGlobal(QPoint(0, (tSrc->globalY((*conn).srcField)))).y();
	int rcvY = mapFromGlobal(QPoint(0, (tRcv->globalY((*conn).rcvField)))).y();

	int srcX = m_tables[(*conn).srcTable].geometry.x();
	int srcW = m_tables[(*conn).srcTable].geometry.width();

	int rcvX = m_tables[(*conn).rcvTable].geometry.x();
	int rcvW = m_tables[(*conn).rcvTable].geometry.width();

	if(paint)
		p->setPen(black);
//		p->setPen(QColor(KApplication::random() % 255, KApplication::random() % 255, KApplication::random() % 255));

	if(srcX < rcvX)
	{
		if(paint)
		{
			p->drawLine(srcX + srcW, srcY - 1, srcX + srcW + 6, srcY - 1);
			p->drawLine(srcX + srcW, srcY, srcX + srcW + 7, srcY);
			p->drawLine(srcX + srcW, srcY + 1, srcX + srcW + 6, srcY + 1);

			p->drawLine(rcvX - 3, rcvY - 2, rcvX - 3, rcvY + 2);
			p->drawLine(rcvX - 2, rcvY - 1, rcvX - 2, rcvY + 1);
			p->drawLine(rcvX - 1, rcvY, rcvX - 1, rcvY);


			p->drawLine(srcX + srcW + 7, srcY, rcvX - 4, rcvY);
		}

		if(srcY < rcvY)
		{
			(*conn).geometry = QRect(srcX + srcW, srcY - 4, rcvX, rcvY + 4);
		}
		else
		{
			(*conn).geometry = QRect(srcX + srcW, rcvY - 4, rcvX, srcY + 4);
		}
	}
	else
	{
		if(paint)
		{
			p->drawLine(rcvX + rcvW, rcvY, rcvX + rcvW, rcvY);
			p->drawLine(rcvX + rcvW + 1, rcvY - 1, rcvX + rcvW + 1, rcvY + 1);
			p->drawLine(rcvX + rcvW + 2, rcvY - 2, rcvX + rcvW + 2, rcvY + 2);

			p->drawLine(srcX - 6, srcY - 1, srcX, srcY - 1);
			p->drawLine(srcX - 7, srcY, srcX, srcY);
			p->drawLine(srcX - 6, srcY + 1, srcX, srcY + 1);

			p->drawLine(rcvX + rcvW + 3, rcvY, srcX - 7, srcY);
		}

		if(rcvY < srcY)
		{
			(*conn).geometry = QRect(rcvX + rcvW, rcvY - 4, srcX, srcY + 4);
		}
		else
		{
			(*conn).geometry = QRect(rcvX + rcvW, srcY - 4, srcX, rcvY + 4);
		}
	}

//	kdDebug() << "KexiRelationView::drawConnection(): geometry: " << (*conn).geometry.x() << ":" << (*conn).geometry.width() << endl;

}

void
KexiRelationView::contentsMouseMoveEvent(QMouseEvent *ev)
{
	if(!m_floatingSource)
	{
		for(TableList::Iterator it=m_tables.begin(); it != m_tables.end(); it++)
		{
			int itemX = (*it).geometry.x();
			int itemY = (*it).geometry.y();
			if(ev->x() > itemX - 20 && ev->x() < itemX + (*it).geometry.width() && ev->y() > itemY - 20 && ev->y() < itemY + 20)
			{
				m_floatingSource = &(*it);

				m_grabOffsetX = ev->x() - (*m_floatingSource).geometry.x();
				m_grabOffsetY = ev->y() - (*m_floatingSource).geometry.y();

				break;
			}
		}
	}
	else
	{
		QRect old = (*m_floatingSource).geometry;

		int realX = ev->x() - m_grabOffsetX;
		int realY = ev->y() - m_grabOffsetY;

		(*m_floatingSource).geometry.moveBy(realX, realY);

		(*m_floatingSource).geometry = QRect(realX, realY, (*m_floatingSource).geometry.width(), (*m_floatingSource).geometry.height());

//		moveChild((*m_floatingSource).columnView, realX + 2, realY + 16);
//		updateContents(old);
//		updateContents((*m_floatingSource).geometry);

		for(RelationList::Iterator itC=m_connections.begin(); itC != m_connections.end(); itC++)
		{
			if((*itC).srcTable == (*m_floatingSource).table || (*itC).rcvTable == (*m_floatingSource).table)
			{
//				updateContents((*itC).geometry);
//				updateContents(recalculateConnectionRect(&(*itC)));
			}
		}

	}
}

void
KexiRelationView::contentsMouseReleaseEvent(QMouseEvent *ev)
{
	if(m_floatingSource)
	{
		m_floatingSource = 0;
		m_grabOffsetX = 0;
		m_grabOffsetY = 0;
	}
}
*/
void
KexiRelationView::slotTableScrolling(QString table)
{
	for(RelationList::Iterator itC=m_connections.begin(); itC != m_connections.end(); itC++)
	{
		if((*itC).srcTable == table || (*itC).rcvTable == table)
		{
//			updateContents((*itC).geometry);
//			updateContents(recalculateConnectionRect(&(*itC)));
		}
	}
}


void
KexiRelationView::addConnection(SourceConnection conn, bool interactive)
{
	kdDebug() << "KexiRelationView::addConnection()" << endl;
//	m_connection.insert(new KexiRelationViewConnection(
//	kdDebug() << "KexiRelationView::addConnection() source: " << (&conn)->srcTable << endl;
//	QString srcS = (&conn)->srcTable;
//	KexiRelationViewTable *src = m_tables[srcS];
//	SourceConnection *c = &conn;
	KexiRelationViewTableContainer *rcv = m_tables[conn.rcvTable];
	KexiRelationViewTableContainer *src = m_tables[conn.srcTable];
	

	KexiRelationViewConnection *connView = new KexiRelationViewConnection(src, rcv, conn.srcField, conn.rcvField);
	m_connectionViews.append(connView);
	updateContents(connView->connectionRect());
}
/*
//	SourceConnection *conn = &connection;
//	bool add=true;
	for (RelationList::iterator it=m_connections.begin();it!=m_connections.end();++it)
	{
		if (
			(connection.srcTable==(*it).srcTable) &&
			(connection.rcvTable==(*it).rcvTable) &&
			(connection.srcField==(*it).srcField) &&
			(connection.rcvField==(*it).rcvField)
		)
		{	
			add=false;
			m_connections.remove(it);
			break;
		}
	}
	kdDebug() << "KexiRelationView::addConnection()" << conn->srcTable << ":" << conn->rcvTable << endl;


//	recalculateConnectionRect(conn);

//	if (add) m_connections.append((*conn));
//	if (interactive) m_relation->updateRelationList(this,m_connections);

//	updateContents((*conn).geometry);
//	kdDebug() << "KexiRelationView::addConnection(): rect: " << (*conn).geometry.x() << ", " << (*conn).geometry.y() << endl;


QRect
KexiRelationView::recalculateConnectionRect(SourceConnection *conn)
{
	drawConnection(0, conn, false);

//	return (*conn).geometry;
}
*/

void
KexiRelationView::containerMoved(KexiRelationViewTableContainer *c)
{
	KexiRelationViewConnection *cview;
	for(cview = m_connectionViews.first(); cview; cview = m_connectionViews.next())
	{
		if(cview->srcTable() == c || cview->rcvTable() == c)
		{
			updateContents(cview->oldRect());
			updateContents(cview->connectionRect());
		}
	}
}

void
KexiRelationView::setReadOnly(bool b) 
{
	m_readOnly=b;
	for (TableList::iterator it=m_tables.begin();it!=m_tables.end();++it)
	{
//		(*it)->setReadOnly(b);
		#warning readonly needed
	}
}


KexiRelationView::~KexiRelationView()
{
	m_relation->decUsageCount();
}

#include "kexirelationview.moc"
