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

#include <klocale.h>

#include <qlayout.h>
#include <qcombobox.h>
#include <qpushbutton.h>

#include "kexiDB/kexidb.h"
#include "kexiDB/kexidbrecord.h"

#include "kexiapplication.h"
#include "kexirelationview.h"
#include "kexirelation.h"

KexiRelation::KexiRelation(QWidget *parent, const char *name)
 : KexiDialogBase(parent, name)
{
	setCaption(i18n("Relations"));
	registerAs(DocumentWindow);

	m_db = kexi->project()->db();

	m_tableCombo = new QComboBox(this);
	m_tableCombo->insertStringList(kexi->project()->db()->tables());
	m_tableCombo->show();

	QPushButton *btnAdd = new QPushButton(i18n("&Add"), this);
	btnAdd->show();
	connect(btnAdd, SIGNAL(clicked()), this, SLOT(slotAddTable()));

	m_view = new KexiRelationView(this);
	m_view->show();

	QGridLayout *g = new QGridLayout(this);
	g->addWidget(m_tableCombo,	0,	0);
	g->addWidget(btnAdd,		0,	1);
	g->addMultiCellWidget(m_view,	1,	1,	0,	1);
}

void
KexiRelation::slotAddTable()
{
	KexiDBRecord *r = m_db->queryRecord("select * from " + m_tableCombo->currentText() + " limit 1");
	QStringList fields;
	for(uint i=0; i < r->fieldCount(); i++)
	{
		fields.append(r->fieldInfo(i)->name());
	}
	m_view->addTable(m_tableCombo->currentText(), fields);
	
	delete r;
}

KexiRelation::~KexiRelation()
{
}

#include "kexirelation.moc"
