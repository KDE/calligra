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

#include "kexiDB/kexidb.h"

#include "kexiapplication.h"
#include "kexitableview.h"
#include "kexitableitem.h"
#include "kexiquerydesignerguitables.h"
#include "kexiquerydesignerguieditor.h"

KexiQueryDesignerGuiEditor::KexiQueryDesignerGuiEditor(KexiQueryDesigner *parent, const char *name)
 : QWidget(parent, name)
{
	m_tables = new KexiQueryDesignerGuiTables(this);
	m_tables->show();
	
	m_designTable = new KexiTableView(this);
	
	m_designTable->addColumn(i18n("source"), QVariant::StringList, true);
//	m_designTable->addColumn(i18n("column"), QVariant::StringList, true);
	m_designTable->addColumn(i18n("field"), QVariant::String, true);
	m_designTable->addColumn(i18n("hidden"), QVariant::Bool, true);
	m_designTable->addColumn(i18n("condition"), QVariant::String, true);
	
	KexiTableItem *i = new KexiTableItem(m_designTable);

	QGridLayout *g = new QGridLayout(this);
	g->addWidget(m_tables,		0, 0);
	g->addWidget(m_designTable,	1, 0);
}

KexiQueryDesignerGuiEditor::~KexiQueryDesignerGuiEditor()
{
}


#include "kexiquerydesignerguieditor.moc"
