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

#ifndef KEXIRELATIONVIEW_H
#define KEXIRELATIONVIEW_H

#include <qscrollview.h>
#include <qptrlist.h>
#include <qmap.h>

#include <kexirelation.h>
#include "kexirelationviewconnection.h"


class QFrame;

class KexiRelationViewTable;
class KexiRelationViewTableContainer;
class KexiRelation;

/*typedef struct RelationSource
{
	QString			table;
	QRect			geometry;
	KexiRelationViewTable	*columnView;
};*/

typedef QMap<QString, KexiRelationViewTableContainer*> TableList;
typedef QPtrList<KexiRelationViewConnection> ConnectionList;

class KexiRelationView : public QScrollView
{
	Q_OBJECT

	public:
		KexiRelationView(QWidget *parent, const char *name=0,KexiRelation* =0);
		~KexiRelationView();

		void		addTable(const QString &table, QStringList columns);
		void		addConnection(SourceConnection con, bool interactive=true);

		RelationList	getConnections()const { return m_connections; };
		void		setReadOnly(bool);

	public slots:
		void		slotTableScrolling(QString);

	protected slots:
		void		containerMoved(KexiRelationViewTableContainer *c);

	protected:
		void		drawContents(QPainter *p, int cx, int cy, int cw, int ch);
//		void		drawSource(QPainter *p, RelationSource src);
//		void		drawConnection(QPainter *p, SourceConnection *conn, bool paint=true);

//		void		contentsMouseReleaseEvent(QMouseEvent *ev);
//		void		contentsMouseMoveEvent(QMouseEvent *ev);

//		QRect		recalculateConnectionRect(SourceConnection *conn);

	private:
		int		m_tableCount;

//		RelationSource	*m_floatingSource;

		TableList	m_tables;
		RelationList	m_connections;
		bool		m_readOnly;
		KexiRelation    *m_relation;
		ConnectionList	m_connectionViews;
};

#endif
