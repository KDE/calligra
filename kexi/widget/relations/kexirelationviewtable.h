/* This file is part of the KDE project
   Copyright (C) 2002, 2003	Lucijan Busch <lucijan@gmx.at>
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

#ifndef KEXIRELATIONVIEWTABLE_H
#define KEXIRELATIONVIEWTABLE_H

#include <qframe.h>
#include <qstringlist.h>
#include <qlabel.h>
#include <klistview.h>

class KexiRelationView;
class KexiRelationViewTable;
class KexiRelationViewTableContainerHeader;

namespace KexiDB
{
	class TableSchema;
}

class KEXIRELATIONSVIEW_EXPORT KexiRelationViewTableContainer : public QFrame
{
	Q_OBJECT

	public:
		KexiRelationViewTableContainer(KexiRelationView *parent, KexiDB::TableSchema *t);
		~KexiRelationViewTableContainer();

		int			globalY(const QString &field);
		KexiDB::TableSchema *table();

//		virtual QSize sizeHint();
		
		int right() { return x() + width() - 1; }
		int bottom() { return y() + height() - 1; }

	signals:
		void moved(KexiRelationViewTableContainer *);
		void endDrag();
		void gotFocus();
		void contextMenuRequest(const QPoint& pos);

	public slots:
		virtual void setFocus();
		virtual void unsetFocus();

	protected slots:
		void moved();
		void slotContextMenu(KListView *lv, QListViewItem *i, const QPoint& p);

	protected:
//js		virtual void			mouseMoveEvent(QMouseEvent *ev);
//js		virtual void			mousePressEvent(QMouseEvent *ev);
//js		virtual void			mouseReleaseEvent(QMouseEvent *ev);

//		bool			m_mousePressed;
//		int			m_bX;
//		int			m_bY;
//		int			m_grabX;
//		int			m_grabY;
		KexiDB::TableSchema *m_table;
		KexiRelationViewTableContainerHeader *m_tableHeader;
		KexiRelationViewTable	*m_tableView;
		KexiRelationView	*m_parent;

		friend class KexiRelationViewTableContainerHeader;
};


class KEXIRELATIONSVIEW_EXPORT KexiRelationViewTableItem : public KListViewItem
{
	public:
		KexiRelationViewTableItem(QListView *parent, QListViewItem *after,
			QString key, QString field);
		virtual void paintFocus ( QPainter * p, const QColorGroup & cg, const QRect & r );
};


class KEXIRELATIONSVIEW_EXPORT KexiRelationViewTable : public KListView
{
	Q_OBJECT

	public:
		KexiRelationViewTable(QWidget *parent, KexiRelationView *view, KexiDB::TableSchema *t, const char *name=0);
		~KexiRelationViewTable();

		QString			table() const { return m_table; };
		int			globalY(const QString &item);
		void setReadOnly(bool);

		virtual QSize sizeHint();

	signals:
		void			tableScrolling();

	protected slots:
		void			slotDropped(QDropEvent *e);
		void			slotContentsMoving(int, int);

	protected:
		virtual void contentsMousePressEvent( QMouseEvent * e );
		virtual bool		acceptDrag(QDropEvent *e) const;
		virtual QDragObject	*dragObject();
		virtual QRect drawItemHighlighter(QPainter *painter, QListViewItem *item); 

	private:
		QStringList		m_fieldList;
		QString			m_table;

		KexiRelationView	*m_view;
		QPixmap m_keyIcon, m_noIcon;
};

class KEXIRELATIONSVIEW_EXPORT KexiRelationViewTableContainerHeader : public QLabel
{
	Q_OBJECT
	public:
		KexiRelationViewTableContainerHeader(const QString& text,QWidget *parent);
		virtual ~KexiRelationViewTableContainerHeader();

		virtual void setFocus();
		virtual void unsetFocus();

	signals:
		void moved();
		void endDrag();

	protected:
		bool			eventFilter(QObject *obj, QEvent *ev);
		void			mousePressEvent(QMouseEvent *ev);
		void			mouseReleaseEvent(QMouseEvent *ev);

		bool			m_dragging;
		int			m_grabX;
		int			m_grabY;
		int			m_offsetX;
		int			m_offsetY;

		QColor m_activeBG, m_activeFG, m_inactiveBG, m_inactiveFG;
};

#endif
