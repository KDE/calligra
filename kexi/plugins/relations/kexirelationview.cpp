/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#include <kdebug.h>

#include <qstringlist.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qheader.h>
#include <qevent.h>
#include <qpainter.h>
#include <qstyle.h>
#include <qlineedit.h>
#include <qpopupmenu.h>

#include <kdebug.h>
#include <klocale.h>
#include <kaction.h>
#include <kpopupmenu.h>
#include <kglobalsettings.h>

#include <koApplication.h>

#include <kexidb/tableschema.h>

#include "kexirelationview.h"
#include "kexirelationviewtable.h"
#include "kexirelationviewconnection.h"

KexiRelationView::KexiRelationView(QWidget *parent, const char *name)
 : QScrollView(parent, name, WStaticContents)
 , KexiActionProxy(this)
{
//	m_relation=relation;
//	m_relation->incUsageCount();
	m_selectedConnection = 0;
	m_readOnly=false;
	m_focusedTableView = 0;
	setFrameStyle(QFrame::WinPanel | QFrame::Sunken);

//	connect(relation, SIGNAL(relationListUpdated(QObject *)), this, SLOT(slotListUpdate(QObject *)));

	viewport()->setPaletteBackgroundColor(colorGroup().mid());
	setFocusPolicy(StrongFocus);
	setResizePolicy(Manual);

	m_popup = new KPopupMenu(this, "m_popup");
	plugSharedAction("edit_delete", m_popup);
	plugSharedAction("edit_delete",this, SLOT(removeSelectedObject()));


#if 0
	//actions
	m_openSelectedTableQueryAction = new KAction(i18n("&Open Selected Table/Query"), "", "",
		this, SLOT(openSelectedTableQuery()), parent->actionCollection(), "relationsview_openSelectedTableQuery");
	m_removeSelectedTableQueryAction = new KAction(i18n("&Hide Selected Table/Query"), "editdelete", "",
		this, SLOT(removeSelectedTableQuery()), parent->actionCollection(), "relationsview_removeSelectedTableQuery");
	m_removeSelectedConnectionAction = new KAction(i18n("&Remove Selected Relation"), "button_cancel", "",
		this, SLOT(removeSelectedConnection()), parent->actionCollection(), "relationsview_removeSelectedConnection");


	m_popup = new KPopupMenu(this, "m_popup");
	m_openSelectedTableQueryAction->plug( m_popup );
	m_removeSelectedTableQueryAction->plug( m_popup );
	m_removeSelectedConnectionAction->plug( m_popup );

	invalidateActions();
#endif
}

void
KexiRelationView::addTable(KexiDB::TableSchema *t)
{
	if(!t)
		return;

	kdDebug() << "KexiRelationView::addTable(): " << t->name() << ", " << viewport() << endl;

	/*
	if(m_tables.contains(t->name()))
	{
		kdDebug() << "KexiRelationView::addTable(): table already exists" << endl;
//		return;
	}
	*/

	KexiRelationViewTableContainer *c = new KexiRelationViewTableContainer(this, t);
	connect(c, SIGNAL(endDrag()), this, SLOT(tableViewEndDrag()));
	connect(c, SIGNAL(gotFocus()), this, SLOT(tableViewGotFocus()));

	addChild(c, 100,100);
//	c->setFixedSize(110, 160);
	c->show();
	updateGeometry();
	c->updateGeometry();
	c->resize(c->sizeHint());
	int x, y;

	if(m_tables.count() > 0)
	{
		int place = -10;
		QDictIterator<KexiRelationViewTableContainer> it(m_tables);
		for(; it.current(); ++it)
		{
			int right = (*it)->x() + (*it)->width();
			if(right > place)
				place = right;
		}

		x = place + 15;
	}
	else
	{
		x = 5;
	}

	y = 5;
	QPoint p = viewportToContents(QPoint(x, y));
	recalculateSize(p.x() + c->width(), p.y() + c->height());
	moveChild(c, x, y);

	m_tables.insert(t->name(), c);

	connect(c, SIGNAL(moved(KexiRelationViewTableContainer *)), this,
            SLOT(containerMoved(KexiRelationViewTableContainer *)));
}

void
KexiRelationView::addConnection(SourceConnection conn, bool)
{
	kdDebug() << "KexiRelationView::addConnection()" << endl;

	KexiRelationViewTableContainer *rcv = m_tables[conn.rcvTable];
	KexiRelationViewTableContainer *src = m_tables[conn.srcTable];

	/*
	if(!rcv)
	{
		kdDebug() << "KexiRelationView::addConnection(): rcv = 0" << endl;
		kdDebug() << "  " << m_parent << endl;
		kdDebug() << "  " << conn.rcvTable << endl;
		m_parent->chooseTable(conn.rcvTable);
	}

	if(!src)
	{
		kdDebug() << "KexiRelationView::addConnection(): src = 0" << endl;
		kdDebug() << "  " << m_parent << endl;
		kdDebug() << "  " << conn.srcTable << endl;
		m_parent->chooseTable(conn.srcTable);
	}
	*/

	kdDebug() << "KexiRelationView::addConnection(): finalSRC = " << m_tables[conn.srcTable] << endl;

	KexiRelationViewConnection *connView = new KexiRelationViewConnection(src, rcv, conn, this);
	m_connectionViews.append(connView);
	updateContents(connView->connectionRect());

#if 0
	if(!interactive)
	{
		kdDebug() << "KexiRelationView::addConnection: adding self" << endl;
		RelationList l = m_relation->projectRelations();
		l.append(conn);
		m_relation->updateRelationList(this, l);
	}
#endif
}

void
KexiRelationView::drawContents(QPainter *p, int cx, int cy, int cw, int ch)
{
	KexiRelationViewConnection *cview;
//	p->translate(0, (double)contentsY());

	QRect clipping(cx, cy, cw, ch);
	for(cview = m_connectionViews.first(); cview; cview = m_connectionViews.next())
	{
		if(clipping.intersects(cview->connectionRect()))
			cview->drawConnection(p);
	}
}

void
KexiRelationView::slotTableScrolling(QString table)
{
	KexiRelationViewTableContainer *c = m_tables[table];

	if(c)
		containerMoved(c);
}

void
KexiRelationView::containerMoved(KexiRelationViewTableContainer *c)
{
	KexiRelationViewConnection *cview;
	for (cview = m_connectionViews.first(); cview; cview = m_connectionViews.next())
	{
		if(cview->srcTable() == c || cview->rcvTable() == c)
		{
			updateContents(cview->oldRect());
			updateContents(cview->connectionRect());
		}
	}

//	QRect w(c->x() - 5, c->y() - 5, c->width() + 5, c->height() + 5);
//	updateContents(w);

	QPoint p = viewportToContents(QPoint(c->x(), c->y()));
	recalculateSize(p.x() + c->width(), p.y() + c->height());
}

void
KexiRelationView::setReadOnly(bool b)
{
	m_readOnly=b;
	invalidateActions();
/*	for (TableList::iterator it=m_tables.begin();it!=m_tables.end();++it)
	{
//		(*it)->setReadOnly(b);
#ifndef Q_WS_WIN
		#warning readonly needed
#endif
	}*/
}

void
KexiRelationView::slotListUpdate(QObject *)
{
#if 0
	if(s != this)
	{
		m_connectionViews.clear();
		RelationList rl = m_relation->projectRelations();
		if(!rl.isEmpty())
		{
			for(RelationList::Iterator it = rl.begin(); it != rl.end(); it++)
			{
				addConnection((*it), true);
			}
		}
	}

	updateContents();
#endif
}

void
KexiRelationView::contentsMousePressEvent(QMouseEvent *ev)
{
	KexiRelationViewConnection *cview;
	for(cview = m_connectionViews.first(); cview; cview = m_connectionViews.next())
	{
		if(!cview->matchesPoint(ev->pos(), 3))
			continue;
		cview->setSelected(true);
		updateContents(cview->connectionRect());
		m_selectedConnection = cview;
		invalidateActions();

		if(ev->button() == RightButton) {//show popup
			kdDebug() << "KexiRelationView::contentsMousePressEvent(): context" << endl;
//			QPopupMenu m;
//				m_removeSelectedTableQueryAction->plug( &m );
//				m_removeSelectedConnectionAction->plug( &m );
			executePopup( ev->globalPos() );
		}
		return;
	}
	//connection not found
	if(ev->button() == RightButton) {//show popup on view background area
//		QPopupMenu m;
//			m_removeSelectedConnectionAction->plug( &m );
		executePopup(ev->globalPos());
	}
	else if(ev->button() == LeftButton) {//clear connection selection
		if (m_selectedConnection) {
			m_selectedConnection->setSelected(false);
			updateContents(m_selectedConnection->connectionRect());
			m_selectedConnection = 0;
			invalidateActions();
		}
		if (m_focusedTableView) {
			m_focusedTableView->unsetFocus();
			m_focusedTableView = 0;
			setFocus();
			invalidateActions();
		}
	}
}

/*
void
KexiRelationView::keyPressEvent(QKeyEvent *ev)
{
	kdDebug() << "KexiRelationView::keyPressEvent()" << endl;

	if (ev->key()==KGlobalSettings::contextMenuKey()) {
//		m_popup->exec( mapToGlobal( m_focusedTableView ? m_focusedTableView->pos() + m_focusedTableView->rect().center() : rect().center() ) );
		executePopup();
	}

	if(ev->key() == Key_Delete)
		removeSelectedConnection();

}*/

void
KexiRelationView::recalculateSize(int width, int height)
{
	kdDebug() << "recalculateSize(" << width << ", " << height << ")" << endl;
	int newW = contentsWidth(), newH = contentsHeight();
	kdDebug() << "contentsSize(" << newW << ", " << newH << ")" << endl;

	if(newW < width)
		newW = width;

	if(newH < height)
		newH = height;

	resizeContents(newW, newH);
}

/*! Resizes contents to size exactly enough to fit tableViews.
	Executed on every tableView's drop event. 
*/
void
KexiRelationView::stretchExpandSize()
{
	int max_x=-1, max_y=-1;
	QDictIterator<KexiRelationViewTableContainer> it(m_tables);
	for (;it.current(); ++it) {
		if (it.current()->right()>max_x)
			max_x = it.current()->right();
		if (it.current()->bottom()>max_y)
			max_y = it.current()->bottom();
	}
	QPoint p = viewportToContents(QPoint(max_x, max_y) + QPoint(3,3)); //3 pixels margin
	resizeContents(p.x(), p.y());
}

void
KexiRelationView::removeSelectedObject()
{
	if (m_selectedConnection) {
		m_connectionViews.remove(m_selectedConnection);
		updateContents(m_selectedConnection->connectionRect());

		kdDebug() << "KexiRelationView::removeSelectedConnection()" << endl;

#if 0
	RelationList l = m_relation->projectRelations();
	RelationList nl;
	for(RelationList::Iterator it = l.begin(); it != l.end(); ++it)
	{
		if((*it).srcTable == m_selectedConnection->connection().srcTable
			&& (*it).rcvTable == m_selectedConnection->connection().rcvTable
			&& (*it).srcField == m_selectedConnection->connection().srcField
			&& (*it).rcvField == m_selectedConnection->connection().rcvField)
		{
			kdDebug() << "KexiRelationView::removeSelectedConnection(): matching found!" << endl;
//			l.remove(it);
		}
		else
		{
			nl.append(*it);
		}
	}

	kdDebug() << "KexiRelationView::removeSelectedConnection(): d2" << endl;
	m_relation->updateRelationList(this, nl);
	kdDebug() << "KexiRelationView::removeSelectedConnection(): d3" << endl;
#endif
		delete m_selectedConnection;
		m_selectedConnection = 0;
		invalidateActions();
	}
//TODO	else if (
}

/*void KexiRelationView::removeSelectedTableQuery()
{
	//TODO
}*/

void KexiRelationView::tableViewEndDrag()
{
	kdDebug() << "END DRAG!" <<endl;
	stretchExpandSize();
}

void KexiRelationView::tableViewGotFocus()
{
	if (m_focusedTableView == sender())
		return;
	kdDebug() << "GOT FOCUS!" <<endl;
	if (m_focusedTableView)
		m_focusedTableView->unsetFocus();
	m_focusedTableView = (KexiRelationViewTableContainer*)sender();
	invalidateActions();
}

//! Invalidates all actions availability
void KexiRelationView::invalidateActions()
{
#if 0
	m_openSelectedTableQueryAction->setEnabled( m_focusedTableView );
	m_removeSelectedTableQueryAction->setEnabled( !m_readOnly && m_focusedTableView );
	m_removeSelectedConnectionAction->setEnabled( !m_readOnly && m_selectedConnection );
#endif
}

/*! executes popup menu at \a pos, or, 
	if \a pos not specified: at center of selected table view (if any selected),
	or at center point of the relations view. */
void KexiRelationView::executePopup( QPoint pos )
{
	if (pos==QPoint(-1,-1)) {
		pos = mapToGlobal( m_focusedTableView ? m_focusedTableView->pos() + m_focusedTableView->rect().center() : rect().center() );
	}
	m_popup->exec(pos);
}

KexiRelationView::~KexiRelationView()
{
}

#include "kexirelationview.moc"
