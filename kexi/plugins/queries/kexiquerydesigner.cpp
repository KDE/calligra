/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@gmx.at>
             (C) 2003 by Joseph Wenninger <jowenn@kde.org>

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

#include <qtabwidget.h>
#include <qlayout.h>
#include <qstatusbar.h>
#include <qregexp.h>
#include <qlayout.h>

#include <klocale.h>
#include <kaction.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <klineeditdlg.h>
#include <klistview.h>

#include <kexidb/cursor.h>
#include <kexidb/connection.h>
#include <kexidb/parser/parser.h>

#include <kexipartitem.h>
#include <keximainwindow.h>
#include <kexidatatableview.h>
#include <kexiproject.h>

#include "kexiquerydesignersqleditor.h"
#include "kexiquerydesignerguieditor.h"
#include "kexiquerydesignersql.h"
#include "kexiquerydesigner.h"

KexiQueryDesigner::KexiQueryDesigner(KexiMainWindow *win, const KexiPart::Item &it)
	: KexiDialogBase(win, it.name())
{
	setIcon(SmallIcon("query"));
	QVBoxLayout *l = new QVBoxLayout(this);

	m_tab = new QTabWidget(this);
	m_tab->setTabPosition(QTabWidget::Bottom);
	l->addWidget(m_tab);
//	gridLayout()->addWidget(m_tab, 0, 0);

//	m_editor = new KexiQueryDesignerGuiEditor(view, this, this, item, "design");
//	connect(m_editor, SIGNAL(contextHelp(const QString &, const QString &)), this,
//	 SLOT(slotContextHelp(const QString &, const QString &)));
	m_editor = new KexiQueryDesignerGuiEditor(this, win);
	m_sql = new KexiQueryDesignerSQL(this);
	m_queryView = new KexiDataTableView(this);
//	m_queryView = new KexiDataTable(m_view, "Query View", "query_view", this, true);
//	m_view = new KexiDataTable(view, this, "sql", 0, true);

	m_tab->insertTab(m_editor, SmallIcon("state_edit"), i18n("Composer"));
	m_tab->insertTab(m_sql, SmallIcon("state_sql"), i18n("SQL"));
	m_tab->insertTab(m_queryView, SmallIcon("table"), i18n("View"));

	m_currentView = 0;

	registerDialog();

/*
	item->setClient(this);
m_parameters = item->parameters();
	m_editor->setPrameters(m_parameters);

	if(modeview)
	{
		m_statement = item->sql();
		m_tab->setCurrentPage(2);

		query();
	}
*/

	connect(m_sql, SIGNAL(queryShortcut()), this, SLOT(fastQuery()));
	connect(m_tab, SIGNAL(currentChanged(QWidget *)), this, SLOT(viewChanged(QWidget *)));
//	connect(this, SIGNAL(closing(KexiDialogBase *)), this, SLOT(slotClosing(KexiDialogBase *)));
}

QWidget *
KexiQueryDesigner::mainWidget()
{
	return m_queryView;
}

void
KexiQueryDesigner::sqlQuery()
{
	kdDebug() << "KexiQueryDesigner::query()" << endl;
//	KexiDB::Cursor *rec = mainWin()->project()->dbConnection()->executeQuery(m_statement);

	KexiDB::Cursor *rec;
	KexiDB::Parser *parser = new KexiDB::Parser(mainWin()->project()->dbConnection());
	parser->parse(m_statement);
	if(parser->operation() == KexiDB::Parser::OP_Select)
		rec = mainWin()->project()->dbConnection()->executeQuery(*parser->select());
	else
		rec = mainWin()->project()->dbConnection()->executeQuery(m_statement);

	if (rec)
	{
		m_queryView->setData(rec);
		emit queryExecuted(m_statement, true, "");
	}
	else
	{
		m_tab->setCurrentPage(1);
		m_sql->editor()->jump(parser->error().at());
		if(!parser->error().error().isEmpty())
			emit queryExecuted(m_statement, false, parser->error().error());
		else
			emit queryExecuted(m_statement, true, "");
	}
}

void
KexiQueryDesigner::viewChanged(QWidget *w)
{
/*	int view = m_tab->indexOf(w);

	if(view == 0)
	{
		m_currentView = 0;
	}
	else if(view == 1)
	{
		m_currentView = 1;
	}
	else
	{
		if(m_currentView == 0)
		{
			m_statement = m_editor->getQuery();
			m_editor->getParameters(m_parameters);
			query();
		}
		else
		{
			m_statement = m_sql->getQuery();
			query();
		}
	}
*/

	kdDebug() << "KexiQueryDesigner::viewChanged(): index of W = " << m_tab->indexOf(w) << endl;

	//this is really ugly, however we might switch back to toolbar stuff anyway...
	if(m_tab->indexOf(w) == 2)
	{
		if(m_currentView == 0)
		{
			KexiDB::Cursor *rec = mainWin()->project()->dbConnection()->executeQuery(*m_editor->schema());
			if(rec)
				m_queryView->setData(rec);
		}
		if(m_currentView == 1)
		{
			m_statement = m_sql->getQuery();
			sqlQuery();
		}
	}
	m_currentView = m_tab->indexOf(w);
}

void
KexiQueryDesigner::fastQuery()
{
	kdDebug() << "KexiQueryDesigner::fastQuery()" << endl;
	m_statement = m_sql->getQuery();
	m_tab->setCurrentPage(2);
	sqlQuery();
}

KexiQueryDesigner::~KexiQueryDesigner()
{
}

#include "kexiquerydesigner.moc"

