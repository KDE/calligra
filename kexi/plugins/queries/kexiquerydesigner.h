/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@gmx.at>

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

#ifndef KEXIQUERYDESIGNER_H
#define KEXIQUERYDESIGNER_H

#include <kexidialogbase.h>

class QTabWidget;
class KexiQueryDesignerGuiEditor;
class KexiQueryDesignerSQL;
class KexiDataTableView;
class KexiMainWindow;

namespace KexiPart
{
	class Item;
};

class KexiQueryDesigner : public KexiDialogBase
{
	Q_OBJECT

	public:
		KexiQueryDesigner(KexiMainWindow *parent, const KexiPart::Item &i);
		~KexiQueryDesigner();

	public slots:
		void	sqlQuery();
		void	fastQuery();

	protected:
		virtual QWidget*		mainWidget();

	signals:
		void	queryExecuted(QString statement, bool succeed, const QString &err);

	protected slots:
		virtual bool	beforeSwitch(int mode);

	private:
		QTabWidget			*m_tab;

		int				m_currentView;
		QString				m_statement;
		KexiQueryDesignerSQL		*m_sql;
		KexiDataTableView		*m_queryView;
		KexiQueryDesignerGuiEditor	*m_editor;
};

#endif

