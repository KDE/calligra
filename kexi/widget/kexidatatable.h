/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
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

#ifndef KEXIDATATABLE_H
#define KEXIDATATABLE_H

#include <kexidialogbase.h>

class KexiMainWindow;
class KexiDataTableView;

namespace KexiDB
{
	class Cursor;
}

class KEXIEXTWIDGETS_EXPORT KexiDataTable : public KexiDialogBase
{
	Q_OBJECT

	public:
		/**
		 * crates a empty table dialog
		 */
		KexiDataTable(KexiMainWindow *win, const QString &caption = QString::null);
		KexiDataTable(KexiMainWindow *win, KexiDB::Cursor *cursor, const QString &caption = QString::null);
		~KexiDataTable();

		void			setData(KexiDB::Cursor *cursor);

		virtual QWidget* mainWidget();
		KexiDataTableView* tableView() const { return m_view; }

		virtual QSize minimumSizeHint() const;
		virtual QSize sizeHint() const;


	protected:
		void init();
		void initActions();

	protected slots:
		void filter();


	private:
		KexiDataTableView	*m_view;
};

#endif

