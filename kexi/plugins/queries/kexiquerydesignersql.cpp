/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@gmx.at>

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

#include <qsplitter.h>
#include <qlayout.h>


#include "kexiquerydesigner.h"
#include "kexiquerydesignersqleditor.h"
#include "kexiquerydesignersqlhistory.h"
#include "kexiquerydesignersql.h"

KexiQueryDesignerSQL::KexiQueryDesignerSQL(KexiQueryDesigner *parent)
 : QWidget(parent)
{
	QSplitter *l = new QSplitter(this);
	l->setOrientation(Vertical);

	m_history = new KexiQueryDesignerSQLHistory(l, "sqlh");
	m_editor = new KexiQueryDesignerSQLEditor(l, "sqle");

	QHBoxLayout *b = new QHBoxLayout(this);
	b->addWidget(l);

	connect(parent, SIGNAL(queryExecuted(QString, bool, const QString &)), m_history, SLOT(addEvent(QString, bool, const QString &)));

	connect(m_editor, SIGNAL(execQ()), parent, SLOT(fastQuery()));
}

QString
KexiQueryDesignerSQL::getQuery()
{
	return m_editor->getText();
}

KexiQueryDesignerSQL::~KexiQueryDesignerSQL()
{
}

#include "kexiquerydesignersql.moc"
