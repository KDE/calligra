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

#include <qlayout.h>

#include <klocale.h>
#include <kaction.h>

#include "kexiquerydesignerguieditor.h"
#include "kexiquerydesigner.h"

class KexiQueryDesigner::EditGUIClient: public KXMLGUIClient
{
	public:
		EditGUIClient():KXMLGUIClient()
		{
			m_actionEdit = new KToggleAction(i18n("Edit"), "state_edit", Key_F5, actionCollection(), "stateEdit");
			m_actionSQL  = new KToggleAction(i18n("SQL"), "state_sql", Key_F5, actionCollection(), "stateSQL");
			m_actionView = new KToggleAction(i18n("View"), "state_view", Key_F5, actionCollection(), "stateView");

			setXMLFile("kexiquerydesignerui.rc");
		}
		virtual ~EditGUIClient(){;}
		
		void activate(QObject* o)
		{
			connect(m_actionEdit, SIGNAL(activated()), o, SLOT(slotEditState()));
			connect(m_actionSQL, SIGNAL(activated()), o, SLOT(slotSQLState()));
			connect(m_actionView, SIGNAL(activated()), o, SLOT(slotViewState()));
		}
		void deactivate(QObject* o)
		{
			m_actionEdit->disconnect(o);
			m_actionSQL->disconnect(o);
			m_actionView->disconnect(o);
		}
	
	private:
		KToggleAction *m_actionEdit;
		KToggleAction *m_actionSQL;
		KToggleAction *m_actionView;
};

//KexiQueryDesigner::EditGUIClient *KexiQueryDesigner::m_editGuiClient=0;

KexiQueryDesigner::KexiQueryDesigner(QWidget *parent, const char *name)
 : KexiDialogBase(parent, name)
{
	setCaption(i18n("Query"));
	registerAs(DocumentWindow);

	KexiQueryDesignerGuiEditor *editor = new KexiQueryDesignerGuiEditor(this);
	editor->show();

	QGridLayout *g = new QGridLayout(this);
	g->addWidget(editor, 0, 0);
}

void
KexiQueryDesigner::slotEditState()
{
}

void
KexiQueryDesigner::slotSQLState()
{
}

void
KexiQueryDesigner::slotViewState()
{
}

KexiQueryDesigner::~KexiQueryDesigner()
{
}

#include "kexiquerydesigner.moc"
