/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004,2006 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexiquerypart.h"

#include <kdebug.h>
#include <kgenericfactory.h>

#include <keximainwindow.h>
#include <kexidialogbase.h>
#include <kexiproject.h>
#include <kexipartinfo.h>

#include <kexidb/cursor.h>
#include <kexidb/parser/parser.h>

#include "kexiqueryview.h"
#include "kexiquerydesignerguieditor.h"
#include "kexiquerydesignersql.h"
//Added by qt3to4:
#include <Q3CString>

//------------------------------------------------

KexiQueryPart::KexiQueryPart(QObject *parent, const char *name, const QStringList &l)
 : KexiPart::Part(parent, name, l)
{
	// REGISTERED ID:
	m_registeredPartID = (int)KexiPart::QueryObjectType;

	m_names["instanceName"] 
		= i18n("Translate this word using only lowercase alphanumeric characters (a..z, 0..9). "
		"Use '_' character instead of spaces. First character should be a..z character. "
		"If you cannot use latin characters in your language, use english word.", 
		"query");
	m_names["instanceCaption"] = i18n("Query");
	m_supportedViewModes = Kexi::DataViewMode | Kexi::DesignViewMode | Kexi::TextViewMode;
}

KexiQueryPart::~KexiQueryPart()
{
}

KexiDialogTempData* 
KexiQueryPart::createTempData(KexiDialogBase* dialog)
{
	KexiQueryPart::TempData *data = new KexiQueryPart::TempData(dialog, dialog->mainWin()->project()->dbConnection());
	data->listenerInfoString = dialog->part()->instanceCaption() + " \"" + dialog->partItem()->name() + "\"";
	return data;
}

KexiViewBase*
KexiQueryPart::createView(QWidget *parent, KexiDialogBase* dialog, KexiPart::Item &item, int viewMode, QMap<QString,QString>*)
{
	Q_UNUSED( item );

	kDebug() << "KexiQueryPart::createView()" << endl;

	if (viewMode == Kexi::DataViewMode) {
		return new KexiQueryView(dialog->mainWin(), parent, "dataview");
	}
	else if (viewMode == Kexi::DesignViewMode) {
		KexiQueryDesignerGuiEditor* view = new KexiQueryDesignerGuiEditor(
			dialog->mainWin(), parent, "guieditor");
		//needed for updating tables combo box:
		KexiProject *prj = dialog->mainWin()->project();
		connect(prj, SIGNAL(newItemStored(KexiPart::Item&)),
			view, SLOT(slotNewItemStored(KexiPart::Item&)));
		connect(prj, SIGNAL(itemRemoved(const KexiPart::Item&)),
			view, SLOT(slotItemRemoved(const KexiPart::Item&)));
		connect(prj, SIGNAL(itemRenamed(const KexiPart::Item&, const Q3CString&)),
			view, SLOT(slotItemRenamed(const KexiPart::Item&, const Q3CString&)));

//		connect(dialog->mainWin()->project(), SIGNAL(tableCreated(KexiDB::TableSchema&)),
//			view, SLOT(slotTableCreated(KexiDB::TableSchema&)));
		return view;
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

#if 0
KexiPart::DataSource *
KexiQueryPart::dataSource()
{
	return new KexiQueryDataSource(this);
}

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
//	new KAction(i18n("Check Query"), "test_it", 0, this, SLOT(slotCheckQuery()), 
//		m_instanceGuiClients[Kexi::DesignViewMode]->actionCollection(), "querypart_check_query");

	KAction *a = createSharedAction(Kexi::TextViewMode, i18n("Check Query"), "test_it", 
		Qt::Key_F9, "querypart_check_query");
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
		/* Set this to true on data loading loadSchemaData() to indicate that TextView mode 
		 could be used instead of DataView or DesignView, because there are problems 
		 with opening object. */
		temp->proposeOpeningInTextViewModeBecauseOfProblems = true;
		//todo
		return 0;
	}
	query->debug();
	(KexiDB::SchemaData&)*query = sdata; //copy main attributes

	temp->registerTableSchemaChanges(query);

	query->debug();
	return query;
}

QString KexiQueryPart::i18nMessage(const Q3CString& englishMessage, KexiDialogBase* dlg) const
{
	Q_UNUSED(dlg);
	if (englishMessage=="Design of object \"%1\" has been modified.")
		return i18n("Design of query \"%1\" has been modified.");
	if (englishMessage=="Object \"%1\" already exists.")
		return i18n("Query \"%1\" already exists.");

	return englishMessage;
}

tristate KexiQueryPart::rename(KexiMainWindow *win, KexiPart::Item &item, const QString& newName)
{
	Q_UNUSED(newName);
	if (!win->project()->dbConnection())
		return false;
	win->project()->dbConnection()->setQuerySchemaObsolete( item.name() );
	return true;
}

//----------------

KexiQueryPart::TempData::TempData(KexiDialogBase* parent, KexiDB::Connection *conn)
 : KexiDialogTempData(parent)
 , KexiDB::Connection::TableSchemaChangeListenerInterface()
 , queryChangedInPreviousView(false)
 , m_query(0)
{
	this->conn = conn;
}

KexiQueryPart::TempData::~TempData()
{
	conn->unregisterForTablesSchemaChanges(*this);
}

void KexiQueryPart::TempData::clearQuery()
{
	if (!m_query)
		return;
	unregisterForTablesSchemaChanges();
	m_query->clear();
}

void KexiQueryPart::TempData::unregisterForTablesSchemaChanges()
{
	conn->unregisterForTablesSchemaChanges(*this);
}

void KexiQueryPart::TempData::registerTableSchemaChanges(KexiDB::QuerySchema *q)
{
	if (!q)
		return;
	for (KexiDB::TableSchema::ListIterator it(*q->tables());
		it.current(); ++it)
	{
		conn->registerForTableSchemaChanges(*this, *it.current());
	}
}

tristate KexiQueryPart::TempData::closeListener()
{
	KexiDialogBase* dlg = static_cast<KexiDialogBase*>(parent());
	return dlg->mainWin()->closeDialog(dlg);
}

KexiDB::QuerySchema *KexiQueryPart::TempData::takeQuery()
{
	KexiDB::QuerySchema *query = m_query;
	m_query = 0;
	return query;
}

void KexiQueryPart::TempData::setQuery(KexiDB::QuerySchema *query)
{
	if (m_query && m_query == query)
		return;
	if (m_query
		/* query not owned by dialog */
		&& (static_cast<KexiDialogBase*>(parent())->schemaData() != static_cast<KexiDB::SchemaData*>( m_query )))
	{
		delete m_query;
	}
	m_query = query;
}

//----------------

#if 0
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
#endif

//----------------

K_EXPORT_COMPONENT_FACTORY( kexihandler_query, KGenericFactory<KexiQueryPart>("kexihandler_query") )

#include "kexiquerypart.moc"

