/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qlayout.h>
#include <qsplitter.h>

#include <kdebug.h>
#include <klocale.h>

#include <kexidb/field.h>

#include <keximainwindow.h>
#include <kexirelationpart.h>
#include <kexitableview.h>
#include <kexitableviewdata.h>

#include "kexiquerydesignerguieditor.h"

KexiQueryDesignerGuiEditor::KexiQueryDesignerGuiEditor(QWidget *parent, KexiMainWindow *win)
 : QWidget(parent)
{
	QSplitter *s = new QSplitter(Vertical, this);
	KexiRelationPart *p = win->relationPart();
	if(p)
		p->createWidget(s);

	m_data = new KexiTableViewData();
	KexiDB::Field *f = new KexiDB::Field(i18n("Table"), KexiDB::Field::Text);
	KexiTableViewColumn *col = new KexiTableViewColumn(*f);
	m_data->addColumn(col);
	kdDebug() << "KexiQueryDesignerGuiEditor::KexiQueryDesignerGuiEditor() data = " << m_data << endl;
	m_table = new KexiTableView(m_data, s, "designer");
	QVBoxLayout *l = new QVBoxLayout(this);
	l->addWidget(s);
}

KexiQueryDesignerGuiEditor::~KexiQueryDesignerGuiEditor()
{
}

#include "kexiquerydesignerguieditor.moc"

