/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#include <kdebug.h>

#include <kexidb/connection.h>
#include <kexidb/parser/parser.h>

#include <kexiproject.h>
#include <keximainwindow.h>

#include "kexiquerydesigner.h"
#include "kexiquerydesignersqleditor.h"
#include "kexiquerydesignersqlhistory.h"
#include "kexiquerydesignersql.h"
#include "kexiquerydocument.h"
#include "kexisectionheader.h"

KexiQueryDesignerSQLView::KexiQueryDesignerSQLView(KexiMainWindow *mainWin, QWidget *parent, KexiQueryDocument *doc, const char *name)
 : KexiViewBase(mainWin, parent, name)
{
	m_doc = doc;
	QSplitter *l = new QSplitter(this);
	l->setOrientation(Vertical);

	m_history = new KexiQueryDesignerSQLHistory(l, "sqlh");
	m_head = new KexiSectionHeader(i18n("SQL Text"), Vertical, l);
	m_editor = new KexiQueryDesignerSQLEditor(mainWin, m_head, "sqle");
	addChildView(m_editor);

	QHBoxLayout *b = new QHBoxLayout(this);
	b->addWidget(l);

	connect(parent, SIGNAL(queryExecuted(QString, bool, const QString &)), m_history, SLOT(addEvent(QString, bool, const QString &)));
	connect(m_history, SIGNAL(editRequested(const QString &)), m_editor, SLOT(setText(const QString &)));
	m_history->setHistory(doc->history());

//	connect(m_editor, SIGNAL(execQ()), parent, SLOT(fastQuery()));
}

bool
KexiQueryDesignerSQLView::beforeSwitchTo(int mode, bool &cancelled, bool &dontStore)
{
	//TODO
	return true;
	
	if (m_doc) {
		KexiDB::Parser *parser = new KexiDB::Parser(mainWin()->project()->dbConnection());
		parser->parse(getQuery());
		m_doc->setSchema(parser->select());

		if(parser->operation() == KexiDB::Parser::OP_Error)
		{
			m_history->addEvent(getQuery(), false, parser->error().error());
			kdDebug() << "KexiQueryDesignerSQLView::beforeSwitchTo(): syntax error!" << endl;
			return false;
		}
		delete parser;
	}

	setDirty(true);
	return true;
}

bool
KexiQueryDesignerSQLView::afterSwitchFrom(int mode, bool &cancelled)
{
	kdDebug() << "KexiQueryDesignerSQLView::afterSwitchFrom()" << endl;
	if (m_doc && m_doc->schema()) {
		m_editor->setText(m_doc->schema()->connection()->selectStatement(*m_doc->schema()));
	}
	return true;
}

QString
KexiQueryDesignerSQLView::getQuery()
{
	return m_editor->getText();
}

KexiQueryDesignerSQLView::~KexiQueryDesignerSQLView()
{
}

#include "kexiquerydesignersql.moc"

