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
#include "kexidataprovider.h"

class QTabWidget;
class KexiView;
class KexiQueryPartItem;
class KexiQueryDesignerGuiEditor;
class KexiQueryDesignerSQL;
class KexiDataTable;

class KEXI_HAND_QUERY_EXPORT KexiQueryDesigner : public KexiDialogBase
{
	Q_OBJECT

	public:
		KexiQueryDesigner(KexiView *view,QWidget *parent, const char *name, KexiQueryPartItem *item,
		 bool modeview=true);
		~KexiQueryDesigner();

		virtual	KXMLGUIClient *guiClient(){return new KXMLGUIClient();}

		void	saveBack();

	public slots:
		void	query();
		void	slotContextHelp(const QString &, const QString &);

	signals:
		void	queryExecuted(QString statement, bool succeed);

	protected:
#ifndef KEXI_NO_PRINT
		virtual void print(KPrinter &p);
#endif

	protected slots:
		void	viewChanged(QWidget *);
		void	slotClosing(KexiDialogBase *);

	private:
		QTabWidget			*m_tab;

		int				m_currentView;
		QString				m_statement;
		KexiDataProvider::ParameterList m_parameters;

		KexiQueryDesignerGuiEditor	*m_editor;
		KexiQueryDesignerSQL		*m_sql;
		KexiDataTable			*m_view;

		KexiQueryPartItem		*m_item;
};

#endif
