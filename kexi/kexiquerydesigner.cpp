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
#include <qwidgetstack.h>
#include <qcstring.h>

#include <klocale.h>
#include <kaction.h>
#include <kiconloader.h>
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
#include "kexitablelist.h"
#include "kmultitabbar.h"
#include "kexiquerydesigner.h"

class KexiQueryDesigner::EditGUIClient: public KXMLGUIClient
{
	public:
		EditGUIClient():KXMLGUIClient()
		{
			m_actionEdit = new KToggleAction(i18n("Edit"), "state_edit", Key_F5, actionCollection(), "stateEdit");
			m_actionSQL  = new KToggleAction(i18n("SQL"), "state_sql", Key_F6, actionCollection(), "stateSQL");
			m_actionView = new KToggleAction(i18n("View"), "state_view", Key_F7, actionCollection(), "stateView");

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

	QVBoxLayout *l=new QVBoxLayout(this);

	m_widgetStack=new QWidgetStack(this);
	m_widgetStack->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	l->addWidget(m_widgetStack);
	m_widgetStack->addWidget(m_editor = new KexiQueryDesignerGuiEditor(m_widgetStack,this));
	
	m_sqlDoc = KTextEditor::EditorChooser::createDocument(this, "sqlDoc");
	m_widgetStack->addWidget(m_sqlView = m_sqlDoc->createView(this, 0L));

	m_widgetStack->addWidget(m_view = new KexiDataTable(this, "query", "query-result", true));

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

	m_widgetStack->raiseWidget(m_editor);

        KMultiTabBar *tb=new KMultiTabBar(this,KMultiTabBar::Horizontal);
	tb->showActiveTabTexts(true);
        tb->insertTab(SmallIcon("state_edit"),-1,"Graphical Designer");
        tb->insertTab(SmallIcon("state_sql"),-1,"Sql Editor");
        tb->insertTab(SmallIcon("state_view"),-1,"Result");
	l->addWidget(tb);
	l->activate();
//	activateActions();
//	connect(kexi->project(), SIGNAL(saving()), this, SLOT(slotSave()));
	connect(kexi->project(), SIGNAL(saving(KoStore *)), this, SLOT(slotSave(KoStore *)));
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

	m_widgetStack->raiseWidget(m_editor);

	m_currentPart = EditorPart;
}

void
KexiQueryDesigner::slotSQLState()
{
	m_editGUIClient->m_actionEdit->setChecked(false);
	m_editGUIClient->m_actionView->setChecked(false);

	m_widgetStack->raiseWidget(m_sqlView);
	
	m_currentPart = SqlPart;
}

void
KexiQueryDesigner::slotViewState()
{
	m_editGUIClient->m_actionEdit->setChecked(false);
	m_editGUIClient->m_actionSQL->setChecked(false);

	kdDebug()<<"Raising view"<<endl;
	m_widgetStack->raiseWidget(m_view);

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

		KexiTableList *designContent = m_editor->table()->contents();
		if(!designContent)
			return;


		for(KexiTableItem *it=designContent->first(); it; it = designContent->next())
		{
			if(!it)
				return;

			if(!it->isInsertItem())
			{
				QDomElement table = domDoc.createElement("table");
				itemsElement.appendChild(table);
				int tableIndex = it->getValue(0).toInt();
				QDomText tTable = domDoc.createTextNode((*m_editor->sourceList().at(tableIndex)));
				table.appendChild(tTable);

				QDomElement field = domDoc.createElement("field");
				itemsElement.appendChild(field);
				QDomText tField = domDoc.createTextNode(it->getValue(1).toString());
				field.appendChild(tField);
				
				QDomElement shown = domDoc.createElement("shown");
				itemsElement.appendChild(shown);
				QDomText tShown = domDoc.createTextNode(it->getValue(2).toString());
				shown.appendChild(tShown);

				QDomElement andC = domDoc.createElement("andC");
				itemsElement.appendChild(andC);
				QDomText tandC = domDoc.createTextNode(it->getValue(3).toString());
				andC.appendChild(tandC);

				QDomElement orC = domDoc.createElement("orC");
				itemsElement.appendChild(orC);
				QDomText torC = domDoc.createTextNode(it->getValue(4).toString());
				orC.appendChild(torC);
				
			}
		}

		QDomElement preparsed = domDoc.createElement("preparsed");
		domDoc.appendChild(preparsed);
		QDomText tPreparsed = domDoc.createTextNode(m_editor->getQuery());
		preparsed.appendChild(tPreparsed);

//		kdDebug() << "KexiQueryDesigner::slotSave() XML:\n" << domDoc.toString() << endl;

		QByteArray data = domDoc.toCString();
		data.resize(data.size()-1);

		if(store)
		{
			store->open("/query/" + m_identifier + ".query");
			store->write(data);
			store->close();
		}
	}
}

KexiQueryDesigner::~KexiQueryDesigner()
{
}

#include "kexiquerydesigner.moc"
