/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIRELATIONVIEW_H
#define KEXIRELATIONVIEW_H

#include <qscrollview.h>
#include <qptrlist.h>
#include <qdict.h>

#include "kexirelationviewconnection.h"

class QFrame;

class KexiRelationViewTable;
class KexiRelationViewTableContainer;
class KAction;
class KPopupMenu;

namespace KexiDB
{
	class TableSchema;
	class Reference;
	class Connection;
}

typedef QDict<KexiRelationViewTableContainer> TableList;
typedef QPtrList<KexiRelationViewConnection> ConnectionList;

struct SourceConnection
{
	QString	srcTable;
	QString rcvTable;
	QString srcField;
	QString rcvField;

};

class KEXIRELATIONSVIEW_EXPORT KexiRelationView : public QScrollView
{
	Q_OBJECT

	public:
		KexiRelationView(QWidget *parent, KexiDB::Connection *conn, const char *name=0);
		~KexiRelationView();

		void		addTable(KexiDB::TableSchema *t);
		void		addConnection(SourceConnection con, bool interactive=true);

//		RelationList	getConnections()const { return m_connections; };
		void setReadOnly(bool);

		inline KexiRelationViewConnection* selectedConnection() const { return m_selectedConnection; }
		inline KexiRelationViewTableContainer* focusedTableView() const { return m_focusedTableView; }

	signals:
		void tableContextMenuRequest( const QPoint& pos );
		void connectionContextMenuRequest( const QPoint& pos );
		void emptyAreaContextMenuRequest( const QPoint& pos );
		void tableViewGotFocus();

	public slots:
		//! Clears current selection - table/query or connection
		void clearSelection();

		void		slotTableScrolling(QString);
//		void		removeSelectedConnection();
//		void		removeSelectedTableQuery();

		//! removes selected table or connection
		void removeSelectedObject();

	protected slots:
		void		containerMoved(KexiRelationViewTableContainer *c);
		void		slotListUpdate(QObject *s);
		void		tableViewEndDrag();
		void		slotTableViewGotFocus();
//		void		tableHeaderContextMenuRequest(const QPoint& pos);

	protected:

		/*! executes popup menu at \a pos, or, 
		 if \a pos not specified: at center of selected table view (if any selected),
		 or at center point of the relations view. */
//		void executePopup( QPoint pos = QPoint(-1,-1) );

		void		drawContents(QPainter *p, int cx, int cy, int cw, int ch);
		void		contentsMousePressEvent(QMouseEvent *ev);
//		virtual void	keyPressEvent(QKeyEvent *ev);

		void		recalculateSize(int width, int height);
		void		stretchExpandSize();
//		void		invalidateActions();

//		void clearTableSelection();
//		void clearConnSelection();

	private:
		TableList		m_tables;
//		RelationList		m_connections;
		bool			m_readOnly;
//		KexiRelation    	*m_relation;
		ConnectionList		m_connectionViews;
		KexiRelationViewConnection *m_selectedConnection;
		KexiRelationViewTableContainer *m_focusedTableView;
	/*
		KPopupMenu *m_tableQueryPopup //over table/query
			, *m_connectionPopup //over connection
			, *m_areaPopup; //over outer area
		KAction *m_openSelectedTableQueryAction;
*/
};

#endif
