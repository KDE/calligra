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

#include "kexiapplication.h"
#include "kexirelationview.h"

KexiRelationView::KexiRelationView(QWidget *parent, const char *name)
 : QScrollView(parent, name)
{
	setFrameStyle(QFrame::WinPanel | QFrame::Sunken);

	m_tableCount = 0;

	m_floatingSource = 0;
	m_floatingX = 0;
	m_floatingY = 0;
	
	viewport()->setPaletteBackgroundColor(colorGroup().mid());
}

void
KexiRelationView::addTable(QString table, QStringList columns)
{
	RelationSource s;
	s.table = table;
	s.geometry = QRect(5, 5, 100, 150);
	
	KexiRelationViewTable *tableView = new KexiRelationViewTable(this, table, columns, "someTable");
	s.columnView = tableView;
	
	tableView->setFixedSize(s.geometry.width() - 5, s.geometry.height() - 19);
	moveChild(tableView, s.geometry.x() + 2, s.geometry.y() + 16);

	updateContents(s.geometry);
	tableView->show();

	m_tables.insert(table, s);
}

void
KexiRelationView::drawContents(QPainter *p, int cx, int cy, int cw, int ch)
{
	QRect clip(cx, cy, cw, ch);
	for(TableList::Iterator it=m_tables.begin(); it != m_tables.end(); it++)
	{
		RelationSource s = (*it);
		if(clip.intersects(s.geometry))
		{
			drawSource(p, s);
		}
	}

	for(ConnectionList::Iterator itC=m_connections.begin(); itC != m_connections.end(); itC++)
	{
		SourceConnection *c = &(*itC);

		kdDebug() << "KexiRelationView::addConnection(): rect: " << (*c).geometry.x() << ", " << (*c).geometry.y() << endl;

		if(clip.intersects(c->geometry))
			drawConnection(p, c, true);
	}
}

void
KexiRelationView::drawSource(QPainter *p, RelationSource src)
{
	kexi->style().drawPrimitive(QStyle::PE_PanelPopup, p, src.geometry, colorGroup());
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

//	p->drawLine(0, srcY, 0, rcvY);

	kdDebug() << "KexiRelationView::drawConnection() srcY=" << srcY << endl;
	kdDebug() << "KexiRelationView::drawConnection() rcvY=" << rcvY << endl;

	int srcX = m_tables[(*conn).srcTable].geometry.x();
	int srcW = m_tables[(*conn).srcTable].geometry.width();

	int rcvX = m_tables[(*conn).rcvTable].geometry.x();
	int rcvW = m_tables[(*conn).rcvTable].geometry.width();

	if(paint)
		p->setPen(black);

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
			kdDebug() << "KexiRelationView::drawConnection() using 1" << endl;

		}
		else
		{
			(*conn).geometry = QRect(srcX + srcW, rcvY - 4, rcvX, srcY + 4);
			kdDebug() << "KexiRelationView::drawConnection() using 2" << endl;
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
			kdDebug() << "KexiRelationView::drawConnection() using 3" << endl;
		}
		else
		{
			(*conn).geometry = QRect(rcvX + rcvW, srcY - 4, srcX, rcvY + 4);
			kdDebug() << "KexiRelationView::drawConnection() using 4" << endl;
		}
	}
	
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
				m_floatingX = ev->x();
				m_floatingY = ev->y();

				break;
			}
		}
	}
	else
	{
		QRect old = (*m_floatingSource).geometry;

		int grepOffsetX = ev->x() - (*m_floatingSource).geometry.x();

		int realX = ev->x();
		int realY = ev->y();

		(*m_floatingSource).geometry.moveBy(realX, realY);

		(*m_floatingSource).geometry = QRect(realX, realY, (*m_floatingSource).geometry.width(), (*m_floatingSource).geometry.height());

		moveChild((*m_floatingSource).columnView, realX + 2, realY + 16);
		updateContents(old);
		updateContents((*m_floatingSource).geometry);

		for(ConnectionList::Iterator itC=m_connections.begin(); itC != m_connections.end(); itC++)
		{
			if((*itC).srcTable == (*m_floatingSource).table || (*itC).rcvTable == (*m_floatingSource).table)
			{
				updateContents((*itC).geometry);
				updateContents(recalculateConnectionRect(&(*itC)));
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
		m_floatingX = 0;
		m_floatingY = 0;
	}
}

void
KexiRelationView::addConnection(SourceConnection connection)
{
	SourceConnection *conn = &connection;

	recalculateConnectionRect(conn);

	m_connections.append((*conn));

	updateContents((*conn).geometry);
	kdDebug() << "KexiRelationView::addConnection(): rect: " << (*conn).geometry.x() << ", " << (*conn).geometry.y() << endl;
}

QRect
KexiRelationView::recalculateConnectionRect(SourceConnection *conn)
{
	drawConnection(0, conn, false);

	return (*conn).geometry;
}

KexiRelationView::~KexiRelationView()
{
}


/// implementation of KexiRelationViewTable

KexiRelationViewTable::KexiRelationViewTable(KexiRelationView *parent, QString table, QStringList fields, const char *name)
 : QListView(parent)
{
	m_fieldList = fields;
	m_table = table;
	m_parent = parent;
	
	viewport()->setAcceptDrops(true);
	
	addColumn("fields");
	header()->hide();
	for(QStringList::Iterator it = m_fieldList.begin(); it != m_fieldList.end(); it++)
	{
		QListViewItem *i = new QListViewItem(this, (*it));
		i->setDragEnabled(true);
		i->setDropEnabled(true);
	}
	
//	setDragEnabled
	connect(this, SIGNAL(dropped(QDropEvent *)), this, SLOT(slotDropped(QDropEvent *)));
}

int
KexiRelationViewTable::globalY(QString item)
{
	QListViewItem *i = findItem(item, 0);
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
		KexiFieldMetaDrag *drag = new KexiFieldMetaDrag(QCString(selectedItem()->text(0).latin1()), this, "metaDrag");
		return drag;
	}

	return 0;
}

void
KexiRelationViewTable::contentsDropEvent(QDropEvent *ev)
{
	kdDebug() << "KexiRelationViewTable::dropEvent()" << endl;

	QListViewItem *recever = itemAt(ev->pos());
	if(recever)
	{
		KexiRelationViewTable *sourceTable = static_cast<KexiRelationViewTable *>(ev->source());
		
		QString srcTable = sourceTable->table();
		QString srcField(ev->encodedData("kexi/field"));

		QString rcvField = recever->text(0);

		SourceConnection s;
		s.srcTable = srcTable;
		s.rcvTable = m_table;
		s.srcField = srcField;
		s.rcvField = rcvField;

		m_parent->addConnection(s);

		kdDebug() << "KexiRelationViewTable::dropEvent() " << srcTable << ":" << srcField << " " << m_table << ":" << rcvField << endl;
		ev->accept();
		return;
	}
	ev->ignore();
}

void
KexiRelationViewTable::slotDropped(QDropEvent *ev)
{
	qDebug("KexiRelationViewTable::slotDropped()");
}

KexiRelationViewTable::~KexiRelationViewTable()
{
}


/// implementation of KexiFieldMetaDrag

KexiFieldMetaDrag::KexiFieldMetaDrag(QCString meta, QWidget *parent, const char *name)
 : QStoredDrag("kexi/field", parent, name)
{
//	QByteArray data(1);
//	data[0] = meta;
	setEncodedData(meta);
}


bool
KexiFieldMetaDrag::canDecode(QDragMoveEvent *e)
{
	return e->provides("kexi/field");
}

bool
KexiFieldMetaDrag::decode( QDropEvent* e, QString& str )
{
	QByteArray payload = e->data("kexi/field");
	if(payload.size())
	{
		e->accept();
//		str = payload[0];
		for(uint i=0; i < payload.size(); i++)
		{
			str += payload[i];
		}

		kdDebug() << "KexiFieldMetaDrag::decode() decoded: " << str << endl;
		return true;
	}
	return false;
}

#include "kexirelationview.moc"
