/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
             (C) 2002 by Joseph Wenninger <jowenn@kde.org>

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

KexiQueryDesigner::KexiQueryDesigner(KexiView *view,QWidget *parent, QString identifier, const char *name)
 : KexiDialogBase(view,parent, name)
{
	m_identifier = identifier;
	m_partCount = 0;
	m_activeTab = -1;

	setCaption(i18n("Query"));

	QVBoxLayout *l=new QVBoxLayout(this);

	m_widgetStack=new QWidgetStack(this);
	m_widgetStack->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	l->addWidget(m_widgetStack);
	m_widgetStack->addWidget(m_editor = new KexiQueryDesignerGuiEditor(view,m_widgetStack,this));

	m_sqlDoc = KTextEditor::EditorChooser::createDocument(this, "sqlDoc");
	m_widgetStack->addWidget(m_sqlView = m_sqlDoc->createView(this, 0L));

	m_widgetStack->addWidget(m_view = new KexiDataTable(view,this, "query", "query-result", true));

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

        m_tb = new KMultiTabBar(this,KMultiTabBar::Horizontal);
	m_tb->showActiveTabTexts(false);
        addTab(SmallIcon("state_edit"), "Graphical Designer", m_editor,m_widgetStack->id(m_editor));
        addTab(SmallIcon("state_sql"), "Sql Editor", m_sqlView,m_widgetStack->id(m_sqlView));
        addTab(SmallIcon("table"), "Result", m_view,m_widgetStack->id(m_view));
	l->addWidget(m_tb);
	l->activate();
//	activateActions();
//	connect(kexi->project(), SIGNAL(saving()), this, SLOT(slotSave()));
#if 0
	connect(kexi->project(), SIGNAL(saving(KoStore *)), this, SLOT(slotSave(KoStore *)));
#endif
	registerAs(DocumentWindow);
//	showMaximized();

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
KexiQueryDesigner::setCurrentQuery(const QString &query)
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
}

void
KexiQueryDesigner::slotSQLState()
{
	m_editGUIClient->m_actionEdit->setChecked(false);
	m_editGUIClient->m_actionView->setChecked(false);

	m_widgetStack->raiseWidget(m_sqlView);

}

void
KexiQueryDesigner::slotViewState()
{
	m_editGUIClient->m_actionEdit->setChecked(false);
	m_editGUIClient->m_actionSQL->setChecked(false);

	kdDebug()<<"Raising view"<<endl;

	QObject *viswid=m_widgetStack->visibleWidget();
	m_widgetStack->raiseWidget(m_view);

	if (viswid==m_editor) {
			if(m_editor->getQuery() == "")

			m_view->executeQuery(m_editor->getQuery());
	} else
	if (viswid==m_sqlView) {
			KTextEditor::EditInterface *eIface = KTextEditor::editInterface(m_sqlDoc);
			kdDebug() << "KexiQueryDesigner::slotViewState() sql-edit: " << eIface->text() << endl;

			/*
			 * here we should grep for ; and new lines and split all parts into new
			 * queries if needed
			 */
			if(eIface->text() != "")
				m_view->executeQuery(eIface->text());
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

		QDomElement nameElement = domDoc.createElement("Query");
		QDomText attrName = domDoc.createTextNode(m_identifier);
		nameElement.appendChild(attrName);
		domDoc.appendChild(nameElement);


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

		QByteArray data = domDoc.toCString();
		data.resize(data.size()-1);

		if(store)
		{
			FileReference ref;
			ref.group = "queries";
			ref.name = m_identifier;
			ref.location = "/query/" + m_identifier + ".query";
#if 0
			kexi->project()->addFileReference(ref);
#endif
			store->open("/query/" + m_identifier + ".query");
			store->write(data);
			store->close();
		}
	}
}

void
KexiQueryDesigner::addTab(QPixmap pixmap, const QString &caption, QWidget *assosiated,int ID)
{
	m_partCount++;
	kdDebug() << "KexiQueryDesigner::addTab(): adding tab: " << m_partCount << endl;
	m_tb->appendTab(pixmap, ID, caption);

	connect(m_tb->getTab(ID), SIGNAL(clicked(int)), this, SLOT(slotTabActivated(int)));

	if(m_activeTab == -1)
	{
		m_tb->setTab(ID, true);
		m_activeTab = ID;
	}
}

void
KexiQueryDesigner::slotTabActivated(int tab)
{
	if(tab != m_activeTab)
	{
		m_tb->setTab(m_activeTab, false);

		//please close your eyes and go some lines down (blind)
		if(m_widgetStack->widget(tab) == m_view && m_widgetStack->widget(m_activeTab) == m_editor)
		{
			m_view->executeQuery(m_editor->getQuery());
		}

		m_activeTab = tab;
		m_widgetStack->raiseWidget(m_activeTab);
	}
	else
	{
		m_tb->setTab(tab, true);
	}
}

KexiQueryDesigner::~KexiQueryDesigner()
{
}

#include "kexiquerydesigner.moc"
