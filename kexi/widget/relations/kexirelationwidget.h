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

#ifndef KEXIRELATIONWIDGET_H
#define KEXIRELATIONWIDGET_H

#include <qwidget.h>
#include "kexiactionproxy.h"

class QComboBox;
class QPushButton;

class KexiRelationView;
class KexiMainWindow;
class KPopupMenu;
class KAction;

namespace KexiDB
{
	class Connection;
	class TableSchema;
	class Reference;
}

class KEXIRELATIONSVIEW_EXPORT KexiRelationWidget : public QWidget, public KexiActionProxy
{
	Q_OBJECT

	public:
		KexiRelationWidget(KexiMainWindow *win, QWidget *parent, const char *name=0);
		~KexiRelationWidget();

//		KexiRelationView	*relationView() const { return m_relationView; }
		void			addTable(QString t);

//	signals:
//		void openTable(KexiDB::TableSchema* table, bool designMode);

	public slots:
		void slotAddTable();
		void removeSelectedObject();

	protected slots:
		void tableViewGotFocus();
		void connectionViewGotFocus();
		void emptyAreaGotFocus();
		void tableContextMenuRequest(const QPoint& pos);
		void connectionContextMenuRequest(const QPoint& pos);
		void emptyAreaContextMenuRequest( const QPoint& pos );
		void openSelectedTable();
		void designSelectedTable();

	protected:
#if 0//js
		virtual void		keyPressEvent(QKeyEvent *ev);
#endif
		/*! executes popup menu at \a pos, or, 
		 if \a pos not specified: at center of selected table view (if any selected),
		 or at center point of the relations view. */
		void executePopup( QPoint pos = QPoint(-1,-1) );

		void		invalidateActions();

	private:
		KexiMainWindow *m_win;
		QComboBox		*m_tableCombo;
		QPushButton		*m_btnAdd;
		KexiRelationView	*m_relationView;
		KexiDB::Connection	*m_conn;

		KPopupMenu *m_tableQueryPopup //over table/query
			, *m_connectionPopup //over connection
			, *m_areaPopup; //over outer area
		KAction *m_openSelectedTableAction, *m_designSelectedTableAction;
};

#endif
