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
#include <qdragobject.h>

#include <qlistview.h>

typedef QPtrList<QFrame> TableList;

//class KListView;
class QFrame;

class KexiRelationViewTable;

typedef struct Connection
{
	KexiRelationViewTable	*sourceTable;
	KexiRelationViewTable	*receverTable;
	QString			*sourceField;
	QString			*receverField;
};

class KexiRelationView : public QScrollView
{
	Q_OBJECT
	
	public:
		KexiRelationView(QWidget *parent, const char *name=0);
		~KexiRelationView();

		void		addTable(QString table, QStringList columns);
		void		addConnection(Connection con);

	private:
		int		m_tableCount;

		TableList	m_tables;
};

class KexiRelationViewTable : public QListView
{
	Q_OBJECT
	
	public:
		KexiRelationViewTable(QWidget *parent, QString table, QStringList fields, const char *name=0);
		~KexiRelationViewTable();

	protected:
		QDragObject	*dragObject();

	protected slots:
		void		slotDropped(QDropEvent *e);

	private:
		QStringList	m_fieldList;
};

class KexiFieldMetaDrag : public QStoredDrag
{
	public:
		KexiFieldMetaDrag(uchar meta, QWidget *parent=0, const char *name=0);
		~KexiFieldMetaDrag() { };
		
		static bool canDecode( QDragMoveEvent* e);
		static bool decode( QDropEvent* e, QString& s);
};

#endif
