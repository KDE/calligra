/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@gmx.at>

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

#ifndef KEXIQUERYDESIGNER_H
#define KEXIQUERYDESIGNER_H

#include "kexidialogbase.h"

class QTabWidget;
class KexiView;
class KexiQueryPartItem;
class KexiQueryDesignerGuiEditor;
class KexiQueryDesignerSQL;
class KexiDataTable;

class KexiQueryDesigner : public KexiDialogBase
{
	Q_OBJECT

	public:
		KexiQueryDesigner(KexiView *view,QWidget *parent, const char *name, KexiQueryPartItem *item);
		~KexiQueryDesigner();

		virtual	KXMLGUIClient *guiClient(){return new KXMLGUIClient();}

	public slots:
		void	query();
		void	slotContextHelp(const QString &, const QString &);

	signals:
		void	queryExecuted(QString statement, bool succeed);

	protected:
		virtual void print(KPrinter &p);

	protected slots:
		void	viewChanged(QWidget *);

	private:
		QTabWidget			*m_tab;

		int				m_currentView;
		QString				m_statement;

		KexiQueryDesignerGuiEditor	*m_editor;
		KexiQueryDesignerSQL		*m_sql;
		KexiDataTable			*m_view;
};

#endif
