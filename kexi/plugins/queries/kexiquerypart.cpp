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

KexiDialogTempData* 
KexiQueryPart::createTempData(KexiDialogBase* dialog)
{
	return new KexiQueryPart::TempData(dialog);
}

KexiViewBase*
KexiQueryPart::createView(QWidget *parent, KexiDialogBase* dialog, KexiPart::Item &item, int viewMode)
{
	kdDebug() << "KexiQueryPart::createView()" << endl;

	if (viewMode == Kexi::DataViewMode) {
		return new KexiQueryView(dialog->mainWin(), parent, "dataview");
	}
	else if (viewMode == Kexi::DesignViewMode) {
		return new KexiQueryDesignerGuiEditor(dialog->mainWin(), parent, "guieditor");
	}
	else if (viewMode == Kexi::TextViewMode) {
		return new KexiQueryDesignerSQLView(dialog->mainWin(), parent, "sqldesigner");
	}

	return 0;
}

bool
KexiQueryPart::remove(KexiMainWindow *win, KexiPart::Item &item)
{
	if (!win || !win->project() || !win->project()->dbConnection())
		return false;
	KexiDB::Connection *conn = win->project()->dbConnection();
	KexiDB::QuerySchema *sch = conn->querySchema(item.identifier());
	if (sch)
		return conn->dropQuery( sch );
	//last chance: just remove item
	return conn->removeObject( item.identifier() );
}

KexiPart::DataSource *
KexiQueryPart::dataSource()
{
	return new KexiQueryDataSource(this);
}

#if 0
void KexiQueryPart::initPartActions( KActionCollection *col )
{
}

void KexiQueryPart::initInstanceActions( int mode, KActionCollection *col )
{
	if (mode==Kexi::DataViewMode) {
	}
	else if (mode==Kexi::DesignViewMode) {
	}
	else if (mode==Kexi::TextViewMode) {
//		new KAction(i18n("Check Query"), "test_it", 0, this, SLOT(slotCheckQuery()), col, "querypart_check_query");

//TODO		new KAction(i18n("Execute Query"), "?????", 0, this, SLOT(checkQuery()), col, "querypart_execute_query");
	}
}
#endif

void KexiQueryPart::initPartActions()
{
}

void KexiQueryPart::initInstanceActions()
{
//	new KAction(i18n("Check query"), "test_it", 0, this, SLOT(slotCheckQuery()), 
//		m_instanceGuiClients[Kexi::DesignViewMode]->actionCollection(), "querypart_check_query");

	KAction *a = createSharedAction(Kexi::TextViewMode, i18n("Check Query"), "test_it", 
		CTRL|Key_Return, "querypart_check_query");
	a->setToolTip(i18n("Check Query"));
	a->setWhatsThis(i18n("Checks query for validity."));

	a = createSharedToggleAction(
		Kexi::TextViewMode, i18n("Show SQL History"), "view_top_bottom"/*TODO other icon*/,
		0, "querypart_view_toggle_history");
	a->setWhatsThis(i18n("Shows or hides SQL editor's history."));

//	setActionAvailable("querypart_check_query", true);
}

KexiDB::SchemaData*
KexiQueryPart::loadSchemaData(KexiDialogBase *dlg, const KexiDB::SchemaData& sdata, int viewMode)
{
	KexiQueryPart::TempData * temp = static_cast<KexiQueryPart::TempData*>(dlg->tempData());
	QString sqlText;
	if (!loadDataBlock( dlg, sqlText, "sql" )) {
		return 0;
	}
	KexiDB::Parser *parser = dlg->mainWin()->project()->sqlParser();
	parser->parse( sqlText );
	KexiDB::QuerySchema *query = parser->query();
	//error?
	if (!query) {
		if (viewMode==Kexi::TextViewMode) {
			//for SQL view, no parsing is initially needed:
			//-just make a copy:
			return KexiPart::Part::loadSchemaData(dlg, sdata, viewMode);
		}
		/* set this to true on data loading loadSchemaData() to indicate that TextView mode could be used
		   instead of DataView or DesignView, because there are problems with opening object. */
		temp->proposeOpeningInTextViewModeBecauseOfProblems = true;
		//todo
		return 0;
	}
	(KexiDB::SchemaData&)*query = sdata; //copy main attributes

	query->debug();
	return query;
}

//----------------

KexiQueryPart::TempData::TempData(QObject* parent)
 : KexiDialogTempData(parent)
 , query(0)
 , queryChangedInPreviousView(false)
{
}

//----------------

KexiQueryDataSource::KexiQueryDataSource(KexiPart::Part *part)
 : KexiPart::DataSource(part)
{
}

KexiQueryDataSource::~KexiQueryDataSource()
{
}

KexiDB::FieldList *
KexiQueryDataSource::fields(KexiProject *, const KexiPart::Item &)
{
	return 0;
}

KexiDB::Cursor *
KexiQueryDataSource::cursor(KexiProject *, const KexiPart::Item &, bool)
{
	return 0;
}

//----------------

K_EXPORT_COMPONENT_FACTORY( kexihandler_query, KGenericFactory<KexiQueryPart>("kexihandler_query") )

#include "kexiquerypart.moc"

