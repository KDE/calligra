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
#include <qtimer.h>

#include <klocale.h>
#include <kdebug.h>

#include <kexidb/connection.h>

#include <kexiproject.h>
#include <keximainwindow.h>
#include "kexirelationview.h"
#include "kexirelationdialog.h"

//#define TESTING_KexiRelationDialog

KexiRelationDialog::KexiRelationDialog(KexiMainWindow *win, QWidget *parent, 
	const char *name)
	: QWidget(parent, name)
{
	m_conn = win->project()->dbConnection();

	QHBoxLayout *hlyr = new QHBoxLayout(0);
	QGridLayout *g = new QGridLayout(this);
	g->addLayout( hlyr, 0, 0 );

	m_tableCombo = new QComboBox(this);
	hlyr->addWidget(m_tableCombo);
	m_tableCombo->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred));
	m_tableCombo->insertStringList(m_conn->tableNames());
	QStringList tmp=m_conn->tableNames();

	QPushButton *btnAdd = new QPushButton(i18n("&Add"), this);
	hlyr->addWidget(btnAdd);
	hlyr->addStretch(1);
	connect(btnAdd, SIGNAL(clicked()), this, SLOT(slotAddTable()));

	m_relationView = new KexiRelationView(this);
	g->addWidget(m_relationView, 1, 0);
	m_relationView->setFocus();

#if 0
	if(!embedd)
	{
		setContextHelp(i18n("Relations"), i18n("To create a relation simply drag the source field onto the targetfield. "
			"An arrowhead is used to show which table is the parent (master) and which table is the child (slave) in the relation."));
	}
#endif
//	else
//js: while embedding means read-only?		m_relationView->setReadOnly(true);

#ifdef TESTING_KexiRelationDialog
	for (int i=0;i<(int)m_db->tableNames().count();i++)
		QTimer::singleShot(100,this,SLOT(slotAddTable()));
#endif
}

KexiRelationDialog::~KexiRelationDialog()
{
}

void
KexiRelationDialog::slotAddTable()
{
	if (m_tableCombo->currentItem()!=-1) //(m_tableCombo->count() > 0)
	{
		QString tname = m_tableCombo->text(m_tableCombo->currentItem());
		KexiDB::TableSchema *t = m_conn->tableSchema(tname);
		if (t)
		{
			m_relationView->addTable(t);
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

#if 0//js
void
KexiRelationDialog::keyPressEvent(QKeyEvent *ev)
{
	kdDebug() << "KexiRelationDialog::keyPressEvent()" << endl;
//	m_relationView->removeSelected();
}
#endif


#include "kexirelationdialog.moc"
