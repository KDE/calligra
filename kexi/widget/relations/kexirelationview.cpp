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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <kdebug.h>

#include <qstringlist.h>
#include <qlayout.h>
#include <qlabel.h>
#include <q3header.h>
#include <qevent.h>
#include <qpainter.h>
#include <qstyle.h>
#include <qlineedit.h>
#include <q3popupmenu.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <Q3Frame>
#include <QKeyEvent>

#include <klocale.h>
#include <kaction.h>
#include <kmenu.h>
#include <kglobalsettings.h>
#include <kmessagebox.h>

#include <kexidb/tableschema.h>
#include <kexidb/indexschema.h>
#include <kexidb/utils.h>

#include "kexirelationview.h"
#include "kexirelationviewtable.h"
#include "kexirelationviewconnection.h"
#include <kexi.h>

KexiRelationView::KexiRelationView(QWidget *parent, const char *name)
 : Q3ScrollView(parent, name, Qt::WStaticContents)
{
//	m_relation=relation;
//	m_relation->incUsageCount();
	m_selectedConnection = 0;
	m_readOnly=false;
	m_focusedTableView = 0;
	setFrameStyle(Q3Frame::WinPanel | Q3Frame::Sunken);

//	connect(relation, SIGNAL(relationListUpdated(QObject *)), this, SLOT(slotListUpdate(QObject *)));

	viewport()->setPaletteBackgroundColor(colorGroup().mid());
	setFocusPolicy(Qt::WheelFocus);
	setResizePolicy(Manual);
/*MOVED TO KexiRelationDialog
	//actions
	m_tableQueryPopup = new KMenu(this, "m_popup");
	m_tableQueryPopup->insertTitle(i18n("Table"));
	m_connectionPopup = new KMenu(this, "m_connectionPopup");
	m_connectionPopup->insertTitle(i18n("Relation"));
	m_areaPopup = new KMenu(this, "m_areaPopup");
	
	plugSharedAction("edit_delete", i18n("Hide Table"), m_tableQueryPopup);
	plugSharedAction("edit_delete",m_connectionPopup);
	plugSharedAction("edit_delete",this, SLOT(removeSelectedObject()));
*/	
#if 0
	m_removeSelectedTableQueryAction = new KAction(i18n("&Hide Selected Table/Query"), "edit-delete", "",
		this, SLOT(removeSelectedTableQuery()), parent->actionCollection(), "relationsview_removeSelectedTableQuery");
	m_removeSelectedConnectionAction = new KAction(i18n("&Remove Selected Relationship"), "dialog-cancel", "",
		this, SLOT(removeSelectedConnection()), parent->actionCollection(), "relationsview_removeSelectedConnection");
	m_openSelectedTableQueryAction = new KAction(i18n("&Open Selected Table/Query"), "", "",
		this, SLOT(openSelectedTableQuery()), 0/*parent->actionCollection()*/, "relationsview_openSelectedTableQuery");
#endif

//	invalidateActions();

#if 0


	m_popup = new KMenu(this, "m_popup");
	m_openSelectedTableQueryAction->plug( m_popup );
	m_removeSelectedTableQueryAction->plug( m_popup );
	m_removeSelectedConnectionAction->plug( m_popup );

	invalidateActions();
#endif

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding, true);
}

KexiRelationView::~KexiRelationView()
{
}

/*KexiRelationViewTableContainer*
KexiRelationView::containerForTable(KexiDB::TableSchema* tableSchema)
{
	if (!tableSchema)
		return 0;
	for (TablesDictIterator it(m_tables); it.current(); ++it) {
		if (it.current()->schema()->table()==tableSchema)
			return it.current();
	}
	return 0;
}*/

KexiRelationViewTableContainer *
KexiRelationView::tableContainer(KexiDB::TableSchema *t) const
{
	return t ? m_tables.find(t->name()) : 0;
}

KexiRelationViewTableContainer*
KexiRelationView::addTableContainer(KexiDB::TableSchema *t, const QRect &rect)
{
	if(!t)
		return 0;

	kDebug() << "KexiRelationView::addTable(): " << t->name() << ", " << viewport() << endl;

	KexiRelationViewTableContainer* c = tableContainer(t);
	if (c) {
		kWarning() << "KexiRelationView::addTable(): table already added" << endl;
		return c;
	}

	c = new KexiRelationViewTableContainer(this, 
/*! @todo what about query? */
		new KexiDB::TableOrQuerySchema(t)
	);
	connect(c, SIGNAL(endDrag()), this, SLOT(slotTableViewEndDrag()));
	connect(c, SIGNAL(gotFocus()), this, SLOT(slotTableViewGotFocus()));
//	connect(c, SIGNAL(headerContextMenuRequest(const QPoint&)), 
//		this, SLOT(tableHeaderContextMenuRequest(const QPoint&)));
	connect(c, SIGNAL(contextMenuRequest(const QPoint&)), 
		this, SIGNAL(tableContextMenuRequest(const QPoint&)));
	
	addChild(c, 100,100);
	if (rect.isValid()) {//predefined size
		QSize finalSize = c->size().expandedTo( c->sizeHint() );
		QRect r = rect;
		r.setSize( finalSize + QSize(0,10) );
		moveChild( c, rect.left(), rect.top() );
		//we're doing this instead of setGeometry(rect)
		//because the geomenty might be saved on other system with bigger fonts :)
		c->resize(c->sizeHint());
//		c->setGeometry(r);
//TODO

//		moveChild( c, rect.left(), rect.top() ); // setGeometry(rect);
//		c->resize( finalSize );
//		c->updateGeometry();
	}
	c->show();
	updateGeometry();
	if (!rect.isValid()) {
		c->updateGeometry();
		c->resize(c->sizeHint());
	}
	int x, y;

	if(m_tables.count() > 0)
	{
		int place = -10;
		Q3DictIterator<KexiRelationViewTableContainer> it(m_tables);
		for(; it.current(); ++it)
		{
			int right = (*it)->x() + (*it)->width();
			if(right > place)
				place = right;
		}

		x = place + 30;
	}
	else
	{
		x = 5;
	}

	y = 5;
	QPoint p = viewportToContents(QPoint(x, y));
	recalculateSize(p.x() + c->width(), p.y() + c->height());
	if (!rect.isValid()) {
		moveChild(c, x, y);
	}

	m_tables.insert(t->name(), c);

	connect(c, SIGNAL(moved(KexiRelationViewTableContainer *)), this,
            SLOT(containerMoved(KexiRelationViewTableContainer *)));
	
	if (hasFocus()) //ok?
		c->setFocus();

	return c;
}

void
KexiRelationView::addConnection(const SourceConnection& _conn)
{
	SourceConnection conn = _conn;
	kDebug() << "KexiRelationView::addConnection()" << endl;

	KexiRelationViewTableContainer *master = m_tables[conn.masterTable];
	KexiRelationViewTableContainer *details = m_tables[conn.detailsTable];
	if (!master || !details)
		return;

/*! @todo what about query? */
	KexiDB::TableSchema *masterTable = master->schema()->table();
/*! @todo what about query? */
	KexiDB::TableSchema *detailsTable = details->schema()->table();
	if (!masterTable || !detailsTable)
		return;

	// ok, but we need to know where is the 'master' and where is the 'details' side:
	KexiDB::Field *masterFld = masterTable->field(conn.masterField);
	KexiDB::Field *detailsFld = detailsTable->field(conn.detailsField);
	if (!masterFld || !detailsFld)
		return;
	
	if (!masterFld->isUniqueKey()) {
		if (detailsFld->isUniqueKey()) {
			//SWAP:
			KexiDB::Field *tmpFld = masterFld;
			masterFld = detailsFld;
			detailsFld = tmpFld;
			KexiDB::TableSchema *tmpTable = masterTable;
			masterTable = detailsTable;
			detailsTable = tmpTable;
			KexiRelationViewTableContainer *tmp = master;
			master = details;
			details = tmp;
			QString tmp_masterTable = conn.masterTable;
			conn.masterTable = conn.detailsTable;
			conn.detailsTable = tmp_masterTable;
			QString tmp_masterField = conn.masterField;
			conn.masterField = conn.detailsField;
			conn.detailsField = tmp_masterField;
		}
	}

//	kDebug() << "KexiRelationView::addConnection(): finalSRC = " << m_tables[conn.srcTable] << endl;

	KexiRelationViewConnection *connView = new KexiRelationViewConnection(master, details, conn, this);
	m_connectionViews.append(connView);
	updateContents(connView->connectionRect());

/*js: will be moved up to relation/query part as this is only visual class
	KexiDB::TableSchema *mtable = m_conn->tableSchema(conn.srcTable);
	KexiDB::TableSchema *ftable = m_conn->tableSchema(conn.rcvTable);
	KexiDB::IndexSchema *forign = new KexiDB::IndexSchema(ftable);

	forign->addField(mtable->field(conn.srcField));
	new KexiDB::Reference(forign, mtable->primaryKey());
*/
#if 0
	if(!interactive)
	{
		kDebug() << "KexiRelationView::addConnection: adding self" << endl;
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
		if(clipping.intersects(cview->oldRect() | cview->connectionRect()))
			cview->drawConnection(p);
	}
}

void
KexiRelationView::slotTableScrolling(const QString& table)
{
	KexiRelationViewTableContainer *c = m_tables[table];

	if(c)
		containerMoved(c);
}

void
KexiRelationView::containerMoved(KexiRelationViewTableContainer *c)
{
	KexiRelationViewConnection *cview;
	QRect r;
	for (ConnectionListIterator it(m_connectionViews); ((cview=it.current())); ++it) {
//! @todo	optimize
		if(cview->masterTable() == c || cview->detailsTable() == c
			|| cview->connectionRect().intersects(r)) 
		{
			r |= cview->oldRect();
			kDebug() << r << endl;
			r |= cview->connectionRect();
			kDebug() << r << endl;
		}
//			updateContents(cview->oldRect());
//			updateContents(cview->connectionRect());
//		}
	}
//! @todo	optimize!
//didn't work well:	updateContents(r);
	updateContents();

//	QRect w(c->x() - 5, c->y() - 5, c->width() + 5, c->height() + 5);
//	updateContents(w);

	QPoint p = viewportToContents(QPoint(c->x(), c->y()));
	recalculateSize(p.x() + c->width(), p.y() + c->height());

	emit tablePositionChanged(c);
}

void
KexiRelationView::setReadOnly(bool b)
{
	m_readOnly=b;
//TODO
//	invalidateActions();
/*	TableList::Iterator it, end( m_tables.end() );	
 	for ( it=m_tables.begin(); it != end; ++it)
	{
//		(*it)->setReadOnly(b);
#ifndef Q_WS_WIN
#ifdef __GNUC__
		#warning readonly needed
#endif
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
			RelationList::ConstIterator it, end( rl.constEnd() );
			for( it = rl.begin(); it != end; ++it)
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
		clearSelection();
		setFocus();
		cview->setSelected(true);
		updateContents(cview->connectionRect());
		m_selectedConnection = cview;
		emit connectionViewGotFocus();
//		invalidateActions();

		if(ev->button() == Qt::RightButton) {//show popup
			kDebug() << "KexiRelationView::contentsMousePressEvent(): context" << endl;
//			QPopupMenu m;
//				m_removeSelectedTableQueryAction->plug( &m );
//				m_removeSelectedConnectionAction->plug( &m );
			emit connectionContextMenuRequest( ev->globalPos() );
//			executePopup( ev->globalPos() );
		}
		return;
	}
	//connection not found
	clearSelection();
//	invalidateActions();
	if(ev->button() == Qt::RightButton) {//show popup on view background area
//		QPopupMenu m;
//			m_removeSelectedConnectionAction->plug( &m );
		emit emptyAreaContextMenuRequest( ev->globalPos() );
//		executePopup(ev->globalPos());
	}
	else {
		emit emptyAreaGotFocus();
	}
	setFocus();
//	QScrollView::contentsMousePressEvent(ev);
}

void KexiRelationView::clearSelection()
{
	if (m_focusedTableView) {
		m_focusedTableView->unsetFocus();
		m_focusedTableView = 0;
//		setFocus();
//		invalidateActions();
	}
	if (m_selectedConnection) {
		m_selectedConnection->setSelected(false);
		updateContents(m_selectedConnection->connectionRect());
		m_selectedConnection = 0;
//		invalidateActions();
	}
}

void
KexiRelationView::keyPressEvent(QKeyEvent *ev)
{
	kDebug() << "KexiRelationView::keyPressEvent()" << endl;

	if (ev->key()==KGlobalSettings::contextMenuKey()) {
		if (m_selectedConnection) {
			emit connectionContextMenuRequest(
				mapToGlobal(m_selectedConnection->connectionRect().center()) );
		}
//		m_popup->exec( mapToGlobal( m_focusedTableView ? m_focusedTableView->pos() + m_focusedTableView->rect().center() : rect().center() ) );
//		executePopup();
	}
	else {
		if(ev->key() == Qt::Key_Delete)
			removeSelectedObject();
	}
}

void
KexiRelationView::recalculateSize(int width, int height)
{
	kDebug() << "recalculateSize(" << width << ", " << height << ")" << endl;
	int newW = contentsWidth(), newH = contentsHeight();
	kDebug() << "contentsSize(" << newW << ", " << newH << ")" << endl;

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
	Q3DictIterator<KexiRelationViewTableContainer> it(m_tables);
	for (;it.current(); ++it) {
		if (it.current()->right()>max_x)
			max_x = it.current()->right();
		if (it.current()->bottom()>max_y)
			max_y = it.current()->bottom();
	}
	QPoint p = viewportToContents(QPoint(max_x, max_y) + QPoint(3,3)); //3 pixels margin
	resizeContents(p.x(), p.y());
}

void KexiRelationView::slotTableViewEndDrag()
{
	kDebug() << "END DRAG!" <<endl;
	stretchExpandSize();

}

void
KexiRelationView::removeSelectedObject()
{
	if (m_selectedConnection) {
		removeConnection(m_selectedConnection);

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
			kDebug() << "KexiRelationView::removeSelectedConnection(): matching found!" << endl;
//			l.remove(it);
		}
		else
		{
			nl.append(*it);
		}
	}

	kDebug() << "KexiRelationView::removeSelectedConnection(): d2" << endl;
	m_relation->updateRelationList(this, nl);
	kDebug() << "KexiRelationView::removeSelectedConnection(): d3" << endl;
#endif
		delete m_selectedConnection;
		m_selectedConnection = 0;
//		invalidateActions();
	}
	else if (m_focusedTableView) {
		KexiRelationViewTableContainer *tmp = m_focusedTableView;
		m_focusedTableView = 0;
		hideTable(tmp);
	}
}

void
KexiRelationView::hideTable(KexiRelationViewTableContainer* tableView)
{
/*! @todo what about query? */
	KexiDB::TableSchema *ts = tableView->schema()->table();
	//for all connections: find and remove all connected with this table
	Q3PtrListIterator<KexiRelationViewConnection> it(m_connectionViews);
	for (;it.current();) {
		if (it.current()->masterTable() == tableView 
			|| it.current()->detailsTable() == tableView)
		{
			//remove this
			removeConnection(it.current());
		}
		else {
			++it;
		}
	}
	m_tables.take(tableView->schema()->name());
	delete tableView;
	emit tableHidden( *ts );
}

void
KexiRelationView::hideAllTablesExcept( KexiDB::TableSchema::List* tables )
{
//! @todo what about queries?
	for (TablesDictIterator it(m_tables); it.current();) {
		KexiDB::TableSchema *table = it.current()->schema()->table();
		if (!table || tables->findRef( table )!=-1) {
			++it;
			continue;
		}
		hideTable(it.current());
	}
}

void
KexiRelationView::removeConnection(KexiRelationViewConnection *conn)
{
	emit aboutConnectionRemove(conn);
	m_connectionViews.remove(conn);
	updateContents(conn->connectionRect());
	kDebug() << "KexiRelationView::removeConnection()" << endl;
}

void KexiRelationView::slotTableViewGotFocus()
{
	if (m_focusedTableView == sender())
		return;
	kDebug() << "GOT FOCUS!" <<endl;
	clearSelection();
//	if (m_focusedTableView)
//		m_focusedTableView->unsetFocus();
	m_focusedTableView = (KexiRelationViewTableContainer*)sender();
//	invalidateActions();
	emit tableViewGotFocus();
}

QSize KexiRelationView::sizeHint() const
{
	return QSize(Q3ScrollView::sizeHint());//.width(), 600);
}

void KexiRelationView::clear()
{
	removeAllConnections();
	m_tables.setAutoDelete(true);
	m_tables.clear();
	m_tables.setAutoDelete(false);
	updateContents();
}

void KexiRelationView::removeAllConnections()
{
	clearSelection(); //sanity
	m_connectionViews.setAutoDelete(true);
	m_connectionViews.clear();
	m_connectionViews.setAutoDelete(false);
	updateContents();
}

/*

void KexiRelationView::tableHeaderContextMenuRequest(const QPoint& pos)
{
	if (m_focusedTableView != sender())
		return;
	kDebug() << "HEADER CTXT MENU!" <<endl;
	invalidateActions();
	m_tableQueryPopup->exec(pos);	
}

//! Invalidates all actions availability
void KexiRelationView::invalidateActions()
{
	setAvailable("edit_delete", m_selectedConnection || m_focusedTableView);
}

void KexiRelationView::executePopup( QPoint pos )
{
	if (pos==QPoint(-1,-1)) {
		pos = mapToGlobal( m_focusedTableView ? m_focusedTableView->pos() + m_focusedTableView->rect().center() : rect().center() );
	}
	if (m_focusedTableView)
		m_tableQueryPopup->exec(pos);
	else if (m_selectedConnection)
		m_connectionPopup->exec(pos);
}
*/

#include "kexirelationview.moc"
