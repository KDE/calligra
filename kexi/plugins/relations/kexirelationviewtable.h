/* This file is part of the KDE project
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>

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

#ifndef KEXIRELATIONVIEWTABLE_H
#define KEXIRELATIONVIEWTABLE_H

#include <qframe.h>
#include <qstringlist.h>
#include <qlabel.h>
#include <klistview.h>

class KexiRelationView;
class KexiRelationViewTable;
//class KexiRelationViewTableContainer;

class KexiRelationViewTableContainer : public QFrame
{
	Q_OBJECT
	
	public:
		KexiRelationViewTableContainer(KexiRelationView *parent, QString table, QStringList fields);
		~KexiRelationViewTableContainer();

		int			globalY(const QString &field);
		const QString		table();

	signals:
		void			moved(KexiRelationViewTableContainer *);

	protected:
		void			mouseMoveEvent(QMouseEvent *ev);
		void			mousePressEvent(QMouseEvent *ev);
		void			mouseReleaseEvent(QMouseEvent *ev);

		bool			m_mousePressed;
		int			m_bX;
		int			m_bY;
		int			m_grabX;
		int			m_grabY;

		int			m_tbHeight;

	private:
		KexiRelationViewTable	*m_tableView;

	protected slots:
		void moved();
};


class KexiRelationViewTable : public KListView
{
	Q_OBJECT

	public:
		KexiRelationViewTable(QWidget *parent, KexiRelationView *view, QString table, QStringList fields, const char *name=0);
		~KexiRelationViewTable();

		QString			table() const { return m_table; };
		int			globalY(const QString &item);
		void setReadOnly(bool);

	signals:
		void			tableScrolling();

	protected:
		virtual bool		acceptDrag(QDropEvent *e) const;
		virtual QDragObject	*dragObject();

	protected slots:
		void			slotDropped(QDropEvent *e);
		void			slotContentsMoving(int, int);

	private:
		QStringList		m_fieldList;
		QString			m_table;

		KexiRelationView	*m_view;
};

class KexiRelationViewTableContainerHeader : public QLabel
{
	Q_OBJECT
	public:
		KexiRelationViewTableContainerHeader(const QString& text,QWidget *parent);
		virtual ~KexiRelationViewTableContainerHeader();
	
	protected:
		bool			eventFilter(QObject *obj, QEvent *ev);
		void			mousePressEvent(QMouseEvent *ev);
		void			mouseReleaseEvent(QMouseEvent *ev);

		bool			m_dragging;
		int			m_grabX;
		int			m_grabY;
		int			m_offsetX;
		int			m_offsetY;
	signals:
		void			moved();		
};

#endif
