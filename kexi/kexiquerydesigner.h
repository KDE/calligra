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

#include <ktexteditor/document.h>
#include <ktexteditor/view.h>

#include <kexidialogbase.h>

class KAction;
class KoStore;
class KexiQueryDesignerGuiEditor;
class KexiDataTable;

class KexiQueryDesigner : public KexiDialogBase
{
	Q_OBJECT
	
	public:
	
	enum ActivePart
	{
		EditorPart,
		SqlPart,
		ViewPart
	};
	
	
		KexiQueryDesigner(QWidget *parent, QString indentifier, const char *name=0);
		~KexiQueryDesigner();
		
		virtual KXMLGUIClient	*guiClient();
		virtual void		deactivateActions();
		virtual void		activateActions();

		void			setCurrentQuery(QString query);

	private:
		class			EditGUIClient;
		friend class		EditGUIClient;
		static EditGUIClient	*m_editGUIClient;
		
		KexiQueryDesignerGuiEditor *m_editor;
		KTextEditor::Document	*m_sqlDoc;
		KTextEditor::View	*m_sqlView;
		KexiDataTable		*m_view;

		QString			m_query;

		QString			m_identifier;
		
		ActivePart		m_currentPart;

	signals:
		void			queryChanged();

	protected slots:
		void			slotEditState();
		void			slotSQLState();
		void			slotViewState();

		void			slotSave(KoStore *store);
};

#endif
