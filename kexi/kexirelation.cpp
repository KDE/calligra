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
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qdom.h>
#include <qhbox.h>

#include <klocale.h>
#include <kdebug.h>

#include <koStore.h>

#include "kexiDB/kexidb.h"
#include "kexiDB/kexidbrecord.h"

#include "kexiproject.h"
#include "kexirelationview.h"
#include "kexirelation.h"

KexiRelation::KexiRelation(KexiView *view,QWidget *parent, const char *name, bool embedd)
 : KexiDialogBase(view,parent, name)
{
	setCaption(i18n("Relations"));


	m_db = kexiProject()->db();

	QHBox *hbox = new QHBox(this);

	m_tableCombo = new QComboBox(hbox);
	m_tableCombo->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
	m_tableCombo->insertStringList(kexiProject()->db()->tables());
	m_tableCombo->show();

	QPushButton *btnAdd = new QPushButton(i18n("&Add"), hbox);
	btnAdd->show();
	connect(btnAdd, SIGNAL(clicked()), this, SLOT(slotAddTable()));

	m_view = new KexiRelationView(this);
	m_view->show();

	connect(kexiProject(), SIGNAL(saving(KoStore *)), this, SLOT(slotSave(KoStore *)));

	RelationList *rl = projectRelations();
	if(rl)
	{
		for(RelationList::Iterator it = rl->begin(); it != rl->end(); it++)
		{
			m_tableCombo->setCurrentText((*it).srcTable);
			slotAddTable();
			m_tableCombo->setCurrentText((*it).rcvTable);
			slotAddTable();

			m_view->addConnection((*it));
		}
	}

	QVBoxLayout *g = new QVBoxLayout(this);
	g->addWidget(hbox);
	g->addWidget(m_view);

	if(!embedd)
		registerAs(DocumentWindow);
}

void
KexiRelation::slotAddTable()
{
	if (m_tableCombo->count() > 0)
	{
		KexiDBRecord *r = m_db->queryRecord("select * from " + m_tableCombo->currentText() + " limit 1");
		QStringList fields;
		for(uint i=0; i < r->fieldCount(); i++)
		{
			fields.append(r->fieldInfo(i)->name());
		}
		m_view->addTable(m_tableCombo->currentText(), fields);

		delete r;
		m_tableCombo->removeItem(m_tableCombo->currentItem());
	}
}

RelationList*
KexiRelation::projectRelations()
{
#if 0
	KoStore* store = KoStore::createStore(project()->url(), KoStore::Read, "application/x-kexi");

	if(!store)
	{
		return 0;
	}

	if(!store->open("/relations.xml"))
		return 0;

	QDomDocument inBuf;

	//error reporting
	QString errorMsg;
	int errorLine;
	int errorCol;

	bool parsed = inBuf.setContent(store->device(), false, &errorMsg, &errorLine, &errorCol);
	store->close();
	delete store;

	if(!parsed)
	{
		kdDebug() << "coudn't parse:" << endl;
		kdDebug() << "error: " << errorMsg << " line: " << errorLine << " col: " << errorCol << endl;
		return 0;
	}

	RelationList *list = new RelationList();

	QDomElement element = inBuf.namedItem("Relations").toElement();
	for(QDomNode n = element.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		SourceConnection con;
		QDomElement sourceTable = n.namedItem("sourcetable").toElement();
		QDomElement sourceField = n.namedItem("sourcefield").toElement();
		QDomElement targetTable = n.namedItem("targettable").toElement();
		QDomElement targetField = n.namedItem("targetfield").toElement();

		con.srcTable = sourceTable.text();
		con.srcField = sourceField.text();
		con.rcvTable = targetTable.text();
		con.rcvField = targetField.text();
		list->append(con);
	}

	return list;
#endif
}

bool
KexiRelation::storeRelations(RelationList relations, KoStore *store)
{
	QDomDocument domDoc("Relations");
	domDoc.appendChild(domDoc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));


	QDomElement relationsElement = domDoc.createElement("Relations");
	domDoc.appendChild(relationsElement);

	for(RelationList::Iterator it = relations.begin(); it != relations.end(); it++)
	{
		QDomElement relationElement = domDoc.createElement("Relation");
		relationsElement.appendChild(relationElement);

		QDomElement sourceTable = domDoc.createElement("sourcetable");
		relationElement.appendChild(sourceTable);
		QDomText tSourceTable = domDoc.createTextNode((*it).srcTable);
		sourceTable.appendChild(tSourceTable);

		QDomElement sourceField = domDoc.createElement("sourcefield");
		relationElement.appendChild(sourceField);
		QDomText tSourceField = domDoc.createTextNode((*it).srcField);
		sourceField.appendChild(tSourceField);

		QDomElement targetTable = domDoc.createElement("targettable");
		relationElement.appendChild(targetTable);
		QDomText tTargetTable = domDoc.createTextNode((*it).rcvTable);
		targetTable.appendChild(tTargetTable);

		QDomElement targetField = domDoc.createElement("targetfield");
		relationElement.appendChild(targetField);
		QDomText tTargetField = domDoc.createTextNode((*it).rcvField);
		targetField.appendChild(tTargetField);

		kdDebug() << "KexiRelationView::storeRelatoins() srcTable: " << (*it).srcTable << endl;
		kdDebug() << "KexiRelationView::storeRelatoins() srcField: " << (*it).srcField << endl;
		kdDebug() << "KexiRelationView::storeRelatoins() rcvTable: " << (*it).rcvTable << endl;
		kdDebug() << "KexiRelationView::storeRelatoins() rcvField: " << (*it).rcvField << endl;
	}

	QByteArray data = domDoc.toCString();
	data.resize(data.size()-1);

	if(store)
	{
		store->open("/relations.xml");
		store->write(data);
		store->close();
	}

	return false;
}

void
KexiRelation::slotSave(KoStore *store)
{
	kdDebug() << "KexiRelation::slotSave()" << endl;
	storeRelations(m_view->getConnections(), store);
}

KexiRelation::~KexiRelation()
{
}

#include "kexirelation.moc"
