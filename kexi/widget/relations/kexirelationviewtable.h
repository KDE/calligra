/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2007 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIRELATIONVIEWTABLE_H
#define KEXIRELATIONVIEWTABLE_H

#include <q3frame.h>
#include <qstringlist.h>
#include <qlabel.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QEvent>
#include <QDropEvent>
#include <k3listview.h>

#include <widget/kexifieldlistview.h>

class KexiRelationView;
class KexiRelationViewTable;
class KexiRelationViewTableContainerHeader;

namespace KexiDB
{
	class TableOrQuerySchema;
}

class KEXIRELATIONSVIEW_EXPORT KexiRelationViewTableContainer : public Q3Frame
{
	Q_OBJECT

	public:
//		KexiRelationViewTableContainer(KexiRelationView *parent, KexiDB::TableSchema *t);
		KexiRelationViewTableContainer(
			KexiRelationView *parent, KexiDB::TableOrQuerySchema *schema);

		virtual ~KexiRelationViewTableContainer();

		int globalY(const QString &field);
//		KexiDB::TableSchema *table();

		KexiRelationViewTable* tableView() const { return m_tableView; }
		KexiDB::TableOrQuerySchema* schema() const;
		
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
		void slotContextMenu(K3ListView *lv, Q3ListViewItem *i, const QPoint& p);

	protected:
//		KexiDB::TableSchema *m_table;
		KexiRelationViewTableContainerHeader *m_tableHeader;
		KexiRelationViewTable *m_tableView;
		KexiRelationView *m_parent;

		friend class KexiRelationViewTableContainerHeader;
};

/*
class KEXIRELATIONSVIEW_EXPORT KexiRelationViewTableItem : public K3ListViewItem
{
	public:
		KexiRelationViewTableItem(QListView *parent, QListViewItem *after,
			QString key, QString field);
		virtual void paintFocus ( QPainter * p, const QColorGroup & cg, const QRect & r );
};*/


class KEXIRELATIONSVIEW_EXPORT KexiRelationViewTable : public KexiFieldListView
{
	Q_OBJECT

	public:
		KexiRelationViewTable(KexiDB::TableOrQuerySchema* tableOrQuerySchema, 
			KexiRelationView *view, QWidget *parent, const char *name = 0);
//		KexiRelationViewTable(QWidget *parent, KexiRelationView *view, KexiDB::TableSchema *t, const char *name=0);
		virtual ~KexiRelationViewTable();

//		KexiDB::TableSchema *table() const { return m_table; };
		int globalY(const QString &item);
//		void setReadOnly(bool);
		virtual QSize sizeHint() const;

	signals:
		void tableScrolling();

	protected slots:
		void slotDropped(QDropEvent *e);
		void slotContentsMoving(int, int);
//		void slotItemDoubleClicked( QListViewItem *i, const QPoint &, int );

	protected:
		virtual void contentsMousePressEvent( QMouseEvent * e );
		virtual bool acceptDrag(QDropEvent *e) const;
//moved		virtual QDragObject *dragObject();
		virtual QRect drawItemHighlighter(QPainter *painter, Q3ListViewItem *item); 

	private:
//		QStringList m_fieldList;
//		KexiDB::TableSchema *m_table;
		KexiRelationView *m_view;
//		QPixmap m_keyIcon, m_noIcon;
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
		bool eventFilter(QObject *obj, QEvent *ev);
		void mousePressEvent(QMouseEvent *ev);
		void mouseReleaseEvent(QMouseEvent *ev);

		bool m_dragging;
		int m_grabX;
		int m_grabY;
		int m_offsetX;
		int m_offsetY;

		QColor m_activeBG, m_activeFG, m_inactiveBG, m_inactiveFG;
};

#endif
