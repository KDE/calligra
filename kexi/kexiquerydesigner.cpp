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
#include <qdom.h>

#include <klocale.h>
#include <kaction.h>
#include <kdebug.h>

#include <koStore.h>

#include <ktexteditor/editorchooser.h>
#include <ktexteditor/highlightinginterface.h>
#include <ktexteditor/editinterface.h>

#include "kexiapplication.h"
#include "kexiproject.h"
#include "kexiquerydesignerguieditor.h"
#include "kexidatatable.h"
#include "kexitableview.h"
#include "kexitableitem.h"
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

		KToggleAction *m_actionEdit;
		KToggleAction *m_actionSQL;
		KToggleAction *m_actionView;
};

KexiQueryDesigner::EditGUIClient *KexiQueryDesigner::m_editGUIClient=0;

KexiQueryDesigner::KexiQueryDesigner(QWidget *parent, QString identifier, const char *name)
 : KexiDialogBase(parent, name)
{
	m_identifier = identifier;

	setCaption(i18n("Query"));
	registerAs(DocumentWindow);

	m_editor = new KexiQueryDesignerGuiEditor(this);
	
	m_sqlDoc = KTextEditor::EditorChooser::createDocument(this, "sqlDoc");
	m_sqlView = m_sqlDoc->createView(this, 0L);

	m_view = new KexiDataTable(this, "query", "query-result", true);

	KTextEditor::HighlightingInterface *hl = KTextEditor::highlightingInterface(m_sqlDoc);
	for(uint i=0; i < hl->hlModeCount(); i++)
	{
		if(hl->hlModeName(i) == "SQL")
		{
			hl->setHlMode(i);
			break;
		}
		i++;
	}

	m_editor->show();
	m_sqlView->hide();
	m_view->hide();

//	activateActions();
//	connect(kexi->project(), SIGNAL(saving()), this, SLOT(slotSave()));
	connect(kexi->project(), SIGNAL(saving(KoStore *)), this, SLOT(slotSave(KoStore *)));

	QGridLayout *g = new QGridLayout(this);
	g->addWidget(m_editor,	0,	0);
	g->addWidget(m_sqlView,	0,	0);
	g->addWidget(m_view,	0,	0);
}

KXMLGUIClient *KexiQueryDesigner::guiClient()
{
        if (!m_editGUIClient)
                m_editGUIClient=new EditGUIClient();
        return m_editGUIClient;
}


void KexiQueryDesigner::activateActions()
{
        m_editGUIClient->activate(this);
}

void KexiQueryDesigner::deactivateActions()
{
        m_editGUIClient->deactivate(this);
}

void
KexiQueryDesigner::setCurrentQuery(QString query)
{
	if(query != m_query)
	{
		m_query = query;
		emit queryChanged();
	}
}

void
KexiQueryDesigner::slotEditState()
{
	m_editGUIClient->m_actionView->setChecked(false);
	m_editGUIClient->m_actionSQL->setChecked(false);

	m_editor->show();
	m_sqlView->hide();
	m_view->hide();

	m_currentPart = EditorPart;
}

void
KexiQueryDesigner::slotSQLState()
{
	m_editGUIClient->m_actionEdit->setChecked(false);
	m_editGUIClient->m_actionView->setChecked(false);

	m_editor->hide();
	m_sqlView->show();
	m_view->hide();
	
	m_currentPart = SqlPart;
}

void
KexiQueryDesigner::slotViewState()
{
	m_editGUIClient->m_actionEdit->setChecked(false);
	m_editGUIClient->m_actionSQL->setChecked(false);

	m_editor->hide();
	
	m_sqlView->hide();
	m_view->show();

	switch(m_currentPart)
	{
		case EditorPart:
			if(m_editor->getQuery() == "")
				break;
			
			m_view->executeQuery(m_editor->getQuery());
			break;
			
		case SqlPart:
			KTextEditor::EditInterface *eIface = KTextEditor::editInterface(m_sqlDoc);
			kdDebug() << "KexiQueryDesigner::slotViewState() sql-edit: " << eIface->text() << endl;

			/*
			 * here we should grep for ; and new lines and split all parts into new
			 * queries if needed
			 */
			if(eIface->text() != "")
				m_view->executeQuery(eIface->text());

			break;
	}
}

void
KexiQueryDesigner::slotSave(KoStore *store)
{
	kdDebug() << "KexiQueryDesigner::slotSave()" << endl;

	if(m_editor->table()->rows() > 1)
	{
		kdDebug() << "KexiQueryDesigner::slotSave() processing information" << endl;

		QDomDocument domDoc("Query");
		domDoc.appendChild(domDoc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));

		QDomElement itemsElement = domDoc.createElement("Items");
		domDoc.appendChild(itemsElement);

		kdDebug() << "KexiQueryDesigner::slotSave() have to process " << m_editor->table()->rows() << "items" << endl;


		for(int i=0; m_editor->table()->rows(); i++)
		{
			KexiTableItem *it = m_editor->table()->itemAt(i);
//			KexiTableItem *it = new KexiTableItem(m_editor->table());
			
			if(!it)
				return;
			
			if(!it->isInsertItem())
			{
				QDomElement table = domDoc.createElement("table");
				itemsElement.appendChild(table);
				QDomText tTable = domDoc.createTextNode(it->getValue(1).toString());
				table.appendChild(tTable);
			}

			i++;
		}

		kdDebug() << "KexiQueryDesigner::slotSave() XML:\n" << domDoc.toString() << endl;

	}
}

KexiQueryDesigner::~KexiQueryDesigner()
{
}

#include "kexiquerydesigner.moc"
