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
#include "kexirelationdialog.h"

KexiRelationDialog::KexiRelationDialog(KexiView *view,QWidget *parent, const char *name, bool embedd)
 : KexiDialogBase(view,parent, name)
{
	setCaption(i18n("Relations"));


	m_db = kexiProject()->db();

	QHBox *hbox = new QHBox(this);

	m_tableCombo = new QComboBox(hbox);
	m_tableCombo->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
	m_tableCombo->insertStringList(kexiProject()->db()->tables());
	QStringList tmp=kexiProject()->db()->tables();
	for (QStringList::iterator it=tmp.begin();it!=tmp.end();++it)
		kdDebug()<<"KexiRelationDialog::KexiRelationDialog: Adding table: "<<(*it)<<endl;
	m_tableCombo->show();

	QPushButton *btnAdd = new QPushButton(i18n("&Add"), hbox);
	btnAdd->show();
	connect(btnAdd, SIGNAL(clicked()), this, SLOT(slotAddTable()));

	m_view = new KexiRelationView(this, 0, kexiProject()->relationManager());
	m_view->show();

	connect(kexiProject(), SIGNAL(saving(KoStore *)), this, SLOT(slotSave(KoStore *)));
	RelationList rl = kexiProject()->relationManager()->projectRelations();
	if(!rl.isEmpty())
	{
		for(RelationList::Iterator it = rl.begin(); it != rl.end(); it++)
		{
			chooseTable((*it).srcTable);
			chooseTable((*it).rcvTable);

			m_view->addConnection((*it),true);
		}
	}
	QVBoxLayout *g = new QVBoxLayout(this);
	g->addWidget(hbox);
	g->addWidget(m_view);

	if(!embedd)
	{
		registerAs(DocumentWindow);
		setContextHelp(i18n("Relations"), i18n("To create a relation simply drag the source field onto the targetfield.\
		 An arrowhead is used to show which table is the parent (master) and which table is the child (slave) in the relation."));
	}
	else
		m_view->setReadOnly(true);
}

void
KexiRelationDialog::slotAddTable()
{
	if (m_tableCombo->currentItem()!=-1) //(m_tableCombo->count() > 0)
	{
		QString tname = m_tableCombo->text(m_tableCombo->currentItem());
		KexiDBRecord *r = m_db->queryRecord("select * from " + tname + " limit 1");
		QStringList fields;
		for(uint i=0; i < r->fieldCount(); i++)
		{
			fields.append(r->fieldInfo(i)->name());
		}
		m_view->addTable(tname, fields);
		kdDebug() << "KexiRelationDialog::slotAddTable(): adding table " << tname << endl;

		delete r;
		int oi=m_tableCombo->currentItem();
		kdDebug()<<"KexiRelationDialog::slotAddTable(): removing a table from the combo box"<<endl;
		m_tableCombo->removeItem(m_tableCombo->currentItem());
		if (m_tableCombo->count()>0)
		{
			if (oi>=m_tableCombo->count()) oi=m_tableCombo->count()-1;
			m_tableCombo->setCurrentItem(oi);
		}
	}
}


void
KexiRelationDialog::chooseTable(QString t)
{
	for(int i=0; i < m_tableCombo->count(); i++)
	{
		if(m_tableCombo->text(i) == t)
		{
			m_tableCombo->setCurrentItem(i);
			slotAddTable();
		}
	}
}

void
KexiRelationDialog::slotSave(KoStore *store)
{
	kdDebug() << "KexiRelationDialog::slotSave()" << endl;
//	storeRelations(m_view->getConnections(), store);
}

KexiRelationDialog::~KexiRelationDialog()
{
}

#include "kexirelationdialog.moc"
