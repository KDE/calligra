/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003   Joseph Wenninger<jowenn@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qlayout.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qdom.h>
#include <qtimer.h>

#include <klocale.h>
#include <kdebug.h>

#include <koStore.h>

#include "kexiDB/kexidb.h"
#include "kexiDB/kexidbrecordset.h"
#include "kexiDB/kexidbtable.h"

#include "kexiview.h"
#include "kexiproject.h"
#include "kexirelationview.h"
#include "kexirelationdialog.h"

#define TESTING_KexiRelationDialog

KexiRelationDialog::KexiRelationDialog(KexiView *view, QString identifier, QWidget *parent, const char *name, bool embedd)
	: KexiDialogBase(view, identifier, parent)
	, m_db(kexiProject()->db())
{
	setCustomCaption(i18n("Relations"));

	QHBoxLayout *hlyr = new QHBoxLayout(0);
	gridLayout()->addLayout( hlyr, 0, 0 );

	m_tableCombo = new QComboBox(this);
	hlyr->addWidget(m_tableCombo);
	m_tableCombo->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred));
	m_tableCombo->insertStringList(m_db->tableNames());
	QStringList tmp=m_db->tableNames();
	for (QStringList::iterator it=tmp.begin();it!=tmp.end();++it)
		kdDebug()<<"KexiRelationDialog::KexiRelationDialog: Adding table: "<<(*it)<<endl;

	QPushButton *btnAdd = new QPushButton(i18n("&Add"), this);
	hlyr->addWidget(btnAdd);
	hlyr->addStretch(1);
	connect(btnAdd, SIGNAL(clicked()), this, SLOT(slotAddTable()));

	m_relationView = new KexiRelationView(this, 0, kexiProject()->relationManager());
	gridLayout()->addWidget(m_relationView, 1, 0);
	m_relationView->setFocus();
	if(!embedd)
		setFocusProxy(m_relationView);

	connect(kexiProject(), SIGNAL(saving(KoStore *)), this, SLOT(slotSave(KoStore *)));
	RelationList rl = kexiProject()->relationManager()->projectRelations();
	if(!rl.isEmpty())
	{
		for(RelationList::Iterator it = rl.begin(); it != rl.end(); it++)
		{
			chooseTable((*it).srcTable);
			chooseTable((*it).rcvTable);

			m_relationView->addConnection((*it),true);
		}
	}

	if(!embedd)
	{
		setContextHelp(i18n("Relations"), i18n("To create a relation simply drag the source field onto the targetfield. "
			"An arrowhead is used to show which table is the parent (master) and which table is the child (slave) in the relation."));
	}
//	else
//js: while embedding means read-only?		m_relationView->setReadOnly(true);

	setXMLFile("kexirelationsview.rc");
	view->insertChildClient(this);


#ifdef TESTING_KexiRelationDialog
	for (int i=0;i<(int)m_db->tableNames().count();i++)
		QTimer::singleShot(100,this,SLOT(slotAddTable()));
#endif
}

void
KexiRelationDialog::slotAddTable()
{
	if (m_tableCombo->currentItem()!=-1) //(m_tableCombo->count() > 0)
	{
		QString tname = m_tableCombo->text(m_tableCombo->currentItem());
		const KexiDBTable * const  t=m_db->table(tname);
		if (t)
		{
//			QStringList fields;
//			for(uint i=0; i < t->fieldCount(); i++)
//				fields.append(t->field(i).name());
			m_relationView->addTable(tname, t);
			kdDebug() << "KexiRelationDialog::slotAddTable(): adding table " << tname << endl;
		}

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
KexiRelationDialog::keyPressEvent(QKeyEvent *ev)
{
	kdDebug() << "KexiRelationDialog::keyPressEvent()" << endl;
//	m_relationView->removeSelected();
}

void
KexiRelationDialog::slotSave(KoStore *store)
{
	kdDebug() << "KexiRelationDialog::slotSave()" << endl;
//	storeRelations(m_relationView->getConnections(), store);
}

KexiRelationDialog::~KexiRelationDialog()
{
}


#include "kexirelationdialog.moc"
