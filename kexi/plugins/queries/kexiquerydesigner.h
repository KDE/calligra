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

#ifndef KEXIQUERYDESIGNER_H
#define KEXIQUERYDESIGNER_H

#include <qintdict.h>

#include <ktexteditor/document.h>
#include <ktexteditor/view.h>

#include "kexidialogbase.h"

class KAction;
class KoStore;
class KexiQueryDesignerGuiEditor;
class KexiDataTable;
class QWidgetStack;
class KMultiTabBar;
class KexiProject;

typedef QIntDict<QWidget> WidgetIndex;

class KexiQueryDesigner : public KexiDialogBase
{
	Q_OBJECT

	public:


		KexiQueryDesigner(KexiView *view,QWidget *parent, QString indentifier, const char *name=0);
		~KexiQueryDesigner();

		virtual KXMLGUIClient	*guiClient();
		virtual void		deactivateActions();
		virtual void		activateActions();

		void			setCurrentQuery(const QString &query);

	protected:
		void			addTab(QPixmap pixmap, const QString &caption, QWidget *assosiated,int ID);

	private:
		class			EditGUIClient;
		friend class		EditGUIClient;
		static EditGUIClient	*m_editGUIClient;

		KexiQueryDesignerGuiEditor *m_editor;
		KTextEditor::Document	*m_sqlDoc;
		KTextEditor::View	*m_sqlView;
		KexiDataTable		*m_view;
		QWidgetStack		*m_widgetStack;

		QString			m_query;

		QString			m_identifier;

		KMultiTabBar		*m_tb;
		int			m_partCount;
		int			m_activeTab;

	signals:
		void			queryChanged();

	protected slots:
		void			slotEditState();
		void			slotSQLState();
		void			slotViewState();

		void			slotTabActivated(int tab);

		void			slotSave(KoStore *store);
};

#endif
