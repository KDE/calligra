/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <kexiproject.h>
#include <kexidb/connection.h>
#include <kexidb/parser/parser.h>
#include <kexidb/cursor.h>
#include <keximainwindow.h>
#include <kexiutils/utils.h>

#include "kexiqueryview.h"
#include "kexiquerypart.h"
#include "kexitableview.h"

KexiQueryView::KexiQueryView(KexiMainWindow *win, QWidget *parent, const char *name)
 : KexiDataTable(win, parent, name)
 , m_cursor(0)
{
	tableView()->setInsertingEnabled(false); //default
}

KexiQueryView::~KexiQueryView()
{
	if (m_cursor)
		m_cursor->connection()->deleteCursor(m_cursor);
}

bool KexiQueryView::executeQuery(KexiDB::QuerySchema *query)
{
	if (!query)
		return false;
	KexiUtils::WaitCursor wait;
	KexiDB::Cursor *oldCursor = m_cursor;
	m_cursor = mainWin()->project()->dbConnection()->executeQuery(*query);
	if (!m_cursor) {
		parentDialog()->setStatus(parentDialog()->mainWin()->project()->dbConnection(), 
			i18n("Query executing failed."));
		//todo: also provide server result and sql statement
		return false;
	}
	setData(m_cursor);

//! @todo remove close() when dynamic cursors arrive
	m_cursor->close();

	if (oldCursor)
		oldCursor->connection()->deleteCursor(oldCursor);

	//TODO: maybe allow writing and inserting for single-table relations?
	tableView()->setReadOnly( true );
	tableView()->setInsertingEnabled( false );
	return true;
}

tristate
KexiQueryView::afterSwitchFrom(int mode)
{
	if (mode==Kexi::NoViewMode) {
		KexiDB::QuerySchema *querySchema = static_cast<KexiDB::QuerySchema *>(parentDialog()->schemaData());
		if (!executeQuery(querySchema)) {
			return false;
		}
	}
	else if (mode==Kexi::DesignViewMode || Kexi::TextViewMode) {
		KexiQueryPart::TempData * temp = static_cast<KexiQueryPart::TempData*>(parentDialog()->tempData());
		if (!executeQuery(temp->query())) {
			return false;
		}
	}

#if 0
	if (m_doc && m_doc->schema())
	{
		KexiDB::Cursor *rec = mainWin()->project()->dbConnection()->executeQuery(*m_doc->schema());
		QString statement = mainWin()->project()->dbConnection()->selectStatement(*m_doc->schema());
		if(!rec && !statement.isEmpty())
		{
			KexiDB::Parser *parser = new KexiDB::Parser(mainWin()->project()->dbConnection());
			parser->parse(statement);
			m_doc->setSchema(parser->select());

			if(parser->operation() == KexiDB::Parser::OP_Error)
				m_doc->addHistoryItem(statement, parser->error().error());
//			else
				//m_doc->addHistoryItem(statement, mainWin()->project()->dbConnection()->serverErrorMsg());
				//m_doc->addHistoryItem(statement, "The user is stupid");

//			delete parser;
			return true;
		}
		setData(rec);

		m_doc->addHistoryItem(statement, "");
	}
#endif
	return true;
}


#include "kexiqueryview.moc"

