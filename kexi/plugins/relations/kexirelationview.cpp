/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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

#include <koApplication.h>

#include "kexirelationview.h"
#include "kexirelationviewtable.h"
#include "kexirelationviewconnection.h"
#include "kexirelation.h"
#include "kexirelationdialog.h"

KexiRelationView::KexiRelationView(KexiRelationDialog *parent, const char *name,KexiRelation *relation)
 : QScrollView(parent, name)
{
	m_parent=parent;
	m_relation=relation;
	m_relation->incUsageCount();
	m_selected = 0;
	m_readOnly=false;
	setFrameStyle(QFrame::WinPanel | QFrame::Sunken);

	m_tableCount = 0;

	connect(relation, SIGNAL(relationListUpdated(QObject *)), this, SLOT(slotListUpdate(QObject *)));

	viewport()->setPaletteBackgroundColor(colorGroup().mid());
	setFocusPolicy(StrongFocus);
}

void
KexiRelationView::addTable(const QString &table, const KexiDBTable *t)
{
	kdDebug() << "KexiRelationView::addTable(): " << table << endl;

	if(m_tables.contains(table))
	{
		kdDebug() << "KexiRelationView::addTable(): table already exists" << endl;
//		return;
	}

	KexiRelationViewTableContainer *c = new KexiRelationViewTableContainer(this, table, t);
	addChild(c, 100,100);
	c->show();
	c->setFixedSize(110, 160);

	if(m_tables.count() > 0)
	{
		int place = -10;
		for(TableList::Iterator it = m_tables.begin(); it != m_tables.end(); ++it)
		{
			int right = (*it)->x() + (*it)->width();
			if(right > place)
				place = right;
		}

		moveChild(c, place + 15, 5);
	}
	else
		moveChild(c, 5, 5);

	m_tables.insert(table, c);

	kdDebug() << "KexiRelationView::addTable(): finalTable = " << m_tables[table] << endl;


	connect(c, SIGNAL(moved(KexiRelationViewTableContainer *)), this,
            SLOT(containerMoved(KexiRelationViewTableContainer *)));

}

void
KexiRelationView::addConnection(SourceConnection conn, bool interactive)
{
	kdDebug() << "KexiRelationView::addConnection()" << endl;

	KexiRelationViewTableContainer *rcv = m_tables[conn.rcvTable];
	KexiRelationViewTableContainer *src = m_tables[conn.srcTable];

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

	kdDebug() << "KexiRelationView::addConnection(): finalSRC = " << m_tables[conn.srcTable] << endl;


	KexiRelationViewConnection *connView = new KexiRelationViewConnection(m_tables[conn.srcTable],
	 m_tables[conn.rcvTable], conn);
	m_connectionViews.append(connView);
	updateContents(connView->connectionRect());

	if(!interactive)
	{
		kdDebug() << "KexiRelationView::addConnection: adding self" << endl;
		RelationList l = m_relation->projectRelations();
		l.append(conn);
		m_relation->updateRelationList(this, l);
	}
}

void
KexiRelationView::drawContents(QPainter *p, int cx, int cy, int cw, int ch)
{
	KexiRelationViewConnection *cview;

	QRect clipping(cx, cy, cw, ch);
	for(cview = m_connectionViews.first(); cview; cview = m_connectionViews.next())
	{
		if(clipping.intersects(cview->connectionRect()))
			cview->drawConnection(p, this);
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
#ifndef Q_WS_WIN
		#warning readonly needed
#endif
	}
}

void
KexiRelationView::slotListUpdate(QObject *s)
{
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
}

void
KexiRelationView::contentsMousePressEvent(QMouseEvent *ev)
{
	if(m_selected)
	{
		m_selected->setSelected(false);
		updateContents(m_selected->connectionRect());
		m_selected = 0;
	}

	KexiRelationViewConnection *cview;
	for(cview = m_connectionViews.first(); cview; cview = m_connectionViews.next())
	{
		if(cview->matchesPoint(ev->pos(), 3))
		{
			cview->setSelected(true);
			updateContents(cview->connectionRect());
			m_selected = cview;

			if(ev->button() == RightButton)
			{
				kdDebug() << "KexiRelationView::contentsMousePressEvent(): context" << endl;
				QPopupMenu m;
				m.insertItem(i18n("Remove"), this, SLOT(removeSelected()));
				m.exec(ev->globalPos());
			}

			return;
		}
	}
}

void
KexiRelationView::keyPressEvent(QKeyEvent *ev)
{
	kdDebug() << "KexiRelationView::keyPressEvent()" << endl;

	if(ev->key() == Key_Delete)
		removeSelected();
}

void
KexiRelationView::removeSelected()
{
	if(!m_selected)
		return;

	m_connectionViews.remove(m_selected);
	updateContents(m_selected->connectionRect());

	kdDebug() << "KexiRelationView::removeSelected()" << endl;

	RelationList l = m_relation->projectRelations();
	RelationList nl;
	for(RelationList::Iterator it = l.begin(); it != l.end(); ++it)
	{
		if((*it).srcTable == m_selected->connection().srcTable && (*it).rcvTable == m_selected->connection().rcvTable &&
		 (*it).srcField == m_selected->connection().srcField && (*it).rcvField == m_selected->connection().rcvField)
		{
			kdDebug() << "KexiRelationView::removeSelected(): matching found!" << endl;
//			l.remove(it);
		}
		else
		{
			nl.append(*it);
		}
	}

	kdDebug() << "KexiRelationView::removeSelected(): d2" << endl;
	m_relation->updateRelationList(this, nl);
	kdDebug() << "KexiRelationView::removeSelected(): d3" << endl;

	delete m_selected;
	m_selected = 0;
}

KexiRelationView::~KexiRelationView()
{
	m_relation->decUsageCount();
}

#include "kexirelationview.moc"
