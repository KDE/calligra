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
#include <qvaluelist.h>
#include <qmap.h>
#include <qdragobject.h>
#include <qcstring.h>

#include <klistview.h>

//#include <qlistview.h>

//class KListView;
class QFrame;

class KexiRelationViewTable;

typedef struct RelationSource
{
	QString			table;
	QRect			geometry;
	KexiRelationViewTable	*columnView;
};

typedef struct SourceConnection
{
	QString	srcTable;
	QString	rcvTable;
	QString	srcField;
	QString	rcvField;
	QRect	geometry;
};

typedef QMap<QString, RelationSource> TableList;
typedef QValueList<SourceConnection> ConnectionList;

class KexiRelationView : public QScrollView
{
	Q_OBJECT
	
	public:
		KexiRelationView(QWidget *parent, const char *name=0);
		~KexiRelationView();

		void		addTable(QString table, QStringList columns);
		void		addConnection(SourceConnection con);

		ConnectionList	getConnections() { return m_connections; };

	protected:
		void		drawContents(QPainter *p, int cx, int cy, int cw, int ch);
		void		drawSource(QPainter *p, RelationSource src);
		void		drawConnection(QPainter *p, SourceConnection *conn, bool paint=true);

//		void		contentsMousePressEvent(QMouseEvent *ev);
		void		contentsMouseReleaseEvent(QMouseEvent *ev);
		void		contentsMouseMoveEvent(QMouseEvent *ev);

		QRect		recalculateConnectionRect(SourceConnection *conn);

	private:
		int		m_tableCount;
		
		RelationSource	*m_floatingSource;
		int		m_grabOffsetX;
		int		m_grabOffsetY;

		TableList	m_tables;
		ConnectionList	m_connections;
};

class KexiRelationViewTable : public KListView
{
	Q_OBJECT
	
	public:
		KexiRelationViewTable(KexiRelationView *parent, QString table, QStringList fields, const char *name=0);
		~KexiRelationViewTable();

		QString			table() { return m_table; };
		int			globalY(QString item);

	protected:
		QDragObject		*dragObject();
		virtual bool		acceptDrag(QDropEvent *e) const;

	protected slots:
		void			slotDropped(QDropEvent *e);

	private:
		QStringList		m_fieldList;
		QString			m_table;

		KexiRelationView	*m_parent;
};

class KexiFieldMetaDrag : public QStoredDrag
{
	public:
		KexiFieldMetaDrag(QCString meta, QWidget *parent=0, const char *name=0);
		~KexiFieldMetaDrag() { };
		
		static bool canDecode( QDragMoveEvent* e);
		static bool decode( QDropEvent* e, QString& s);
};

#endif
