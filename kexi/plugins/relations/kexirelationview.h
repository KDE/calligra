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
}

typedef QDict<KexiRelationViewTableContainer> TableList;
typedef QPtrList<KexiRelationViewConnection> ConnectionList;

class KexiRelationView : public QScrollView
{
	Q_OBJECT

	public:
		KexiRelationView(QWidget *parent, const char *name=0);
		~KexiRelationView();

		void		addTable(KexiDB::TableSchema *t);
//		void		addConnection(SourceConnection con, bool interactive=true);

//		RelationList	getConnections()const { return m_connections; };
		void setReadOnly(bool);
		void executePopup( QPoint pos = QPoint(-1,-1) );

	public slots:
		void		slotTableScrolling(QString);
		void		removeSelectedConnection();
		void		removeSelectedTableQuery();

	protected slots:
		void		containerMoved(KexiRelationViewTableContainer *c);
		void		slotListUpdate(QObject *s);
		void		tableViewEndDrag();
		void		tableViewGotFocus();

	protected:
		void		drawContents(QPainter *p, int cx, int cy, int cw, int ch);
		void		contentsMousePressEvent(QMouseEvent *ev);
		virtual void	keyPressEvent(QKeyEvent *ev);

		void		recalculateSize(int width, int height);
		void		stretchExpandSize();
		void		invalidateActions();

	private:
		TableList		m_tables;
//		RelationList		m_connections;
		bool			m_readOnly;
//		KexiRelation    	*m_relation;
		ConnectionList		m_connectionViews;
		KexiRelationViewConnection *m_selectedConnection;

		KexiRelationViewTableContainer *m_focusedTableView;

		KPopupMenu *m_popup;
		KAction *m_openSelectedTableQueryAction;
		KAction *m_removeSelectedTableQueryAction;
		KAction *m_removeSelectedConnectionAction;
};

#endif
