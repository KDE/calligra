/* This file is part of the KDE project
   Copyright (C) 2004   Lucijan Busch <lucijan@kde.org>

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

#include "kexiquerypart.h"

#include <kdebug.h>
#include <kgenericfactory.h>

#include <keximainwindow.h>
#include <kexidialogbase.h>
#include <kexiproject.h>
#include <kexidb/queryschema.h>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>
#include <kexidb/parser/parser.h>

#include "kexiquerydocument.h"
#include "kexiqueryview.h"
#include "kexiquerydesignerguieditor.h"
#include "kexiquerydesignersql.h"

KexiQueryPart::KexiQueryPart(QObject *parent, const char *name, const QStringList &l)
 : KexiPart::Part(parent, name, l)
{
	m_names["instance"] = i18n("Query");
	m_supportedViewModes = Kexi::DataViewMode | Kexi::DesignViewMode | Kexi::TextViewMode;
}

KexiQueryPart::~KexiQueryPart()
{
}

KexiViewBase*
KexiQueryPart::createView(QWidget *parent, KexiDialogBase* dialog,  const KexiPart::Item &item, int viewMode)
{
	kdDebug() << "KexiQueryPart::createView()" << endl;
	if (viewMode == Kexi::DataViewMode) {
		return new KexiQueryView(dialog->mainWin(), parent, data(dialog->mainWin()->project()->dbConnection(), item), "dataview");
	}
	else if (viewMode == Kexi::DesignViewMode) {
		return new KexiQueryDesignerGuiEditor(dialog->mainWin(), parent, data(dialog->mainWin()->project()->dbConnection(), item), "guieditor");
	}
	else if (viewMode == Kexi::TextViewMode) {
		return new KexiQueryDesignerSQL(dialog->mainWin(), parent, data(dialog->mainWin()->project()->dbConnection(), item), "sqldesigner");
	}

	return 0;
}

/*QString
KexiQueryPart::instanceName() const
{
	return i18n("Query");
}*/

bool
KexiQueryPart::remove(KexiMainWindow *, const KexiPart::Item &)
{
	//TODO
	return false;
}

KexiQueryDocument *
KexiQueryPart::data(KexiDB::Connection *conn, const KexiPart::Item &i)
{
	KexiQueryDocument *doc = m_data[i.identifier()];
	if(doc)
		return doc;

//	KexiDB::Cursor *cursor = conn->executeQuery(QString("SELECT q_sql FROM kexi__querydata WHERE q_id = %1").arg(i.identifier()));
	KexiDB::RowData data;
	if (!conn->querySingleRecord( 
		QString("SELECT q_sql FROM kexi__querydata WHERE q_id=%1").arg(i.identifier()), data))
		return 0;

//	if (!cursor->moveFirst())
//		return 0;
//js NOT THIS WAY	if(cursor->eof())
//		return 0;

	KexiDB::Parser *parser = new KexiDB::Parser(conn);
	parser->parse(data.at(0).toString());

	doc = new KexiQueryDocument(parser->select());
	m_data.insert(i.identifier(), doc);

	delete parser;
	return doc;
}

K_EXPORT_COMPONENT_FACTORY( kexihandler_query, KGenericFactory<KexiQueryPart> )


#include "kexiquerypart.moc"

