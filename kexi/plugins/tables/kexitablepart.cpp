/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexitablepart.h"

#include <kdebug.h>
#include <kgenericfactory.h>
#include <kmessagebox.h>
#include <ktabwidget.h>
#include <kiconloader.h>

#include "keximainwindow.h"
#include "kexiproject.h"
#include "kexipartinfo.h"
#include "widget/kexidatatable.h"
#include "widget/tableview/kexidatatableview.h"
#include "kexitabledesignerview.h"
#include "kexitabledesigner_dataview.h"
#include "kexilookupcolumnpage.h"

#include <kexidb/connection.h>
#include <kexidb/cursor.h>
#include <kexidialogbase.h>

//! @internal
class KexiTablePart::Private
{
	public:
		Private()
		{
		}
		~Private()
		{
			delete static_cast<KexiLookupColumnPage*>(lookupColumnPage);
		}
		QGuardedPtr<KexiLookupColumnPage> lookupColumnPage;
};

KexiTablePart::KexiTablePart(QObject *parent, const char *name, const QStringList &l)
 : KexiPart::Part(parent, name, l)
 , d(new Private())
{
	// REGISTERED ID:
	m_registeredPartID = (int)KexiPart::TableObjectType;

	kDebug() << "KexiTablePart::KexiTablePart()" << endl;
	m_names["instanceName"] 
		= i18n("Translate this word using only lowercase alphanumeric characters (a..z, 0..9). "
		"Use '_' character instead of spaces. First character should be a..z character. "
		"If you cannot use latin characters in your language, use english word.", 
		"table");
	m_names["instanceCaption"] = i18n("Table");
	m_supportedViewModes = Kexi::DataViewMode | Kexi::DesignViewMode;
//js TODO: also add Kexi::TextViewMode when we'll have SQL ALTER TABLE EDITOR!!!
}

KexiTablePart::~KexiTablePart()
{
	delete d;
}

void KexiTablePart::initPartActions()
{
}

void KexiTablePart::initInstanceActions()
{
//moved to main window	createSharedAction(Kexi::DataViewMode, i18n("Filter"), "search-filter", 0, "tablepart_filter");

	KAction *a = createSharedToggleAction(
		Kexi::DesignViewMode, i18n("Primary Key"), "key", 0, "tablepart_toggle_pkey");
//		Kexi::DesignViewMode, i18n("Toggle Primary Key"), "key", 0, "tablepart_toggle_pkey");
	a->setWhatsThis(i18n("Sets or removes primary key for currently selected field."));
}

KexiDialogTempData* KexiTablePart::createTempData(KexiDialogBase* dialog)
{
	return new KexiTablePart::TempData(dialog);
}

KexiViewBase* KexiTablePart::createView(QWidget *parent, KexiDialogBase* dialog, 
	KexiPart::Item &item, int viewMode, QMap<QString,QString>*)
{
	KexiMainWindow *win = dialog->mainWin();
	if (!win || !win->project() || !win->project()->dbConnection())
		return 0;


	KexiTablePart::TempData *temp = static_cast<KexiTablePart::TempData*>(dialog->tempData());
	if (!temp->table) {
		temp->table = win->project()->dbConnection()->tableSchema(item.name());
		kDebug() << "KexiTablePart::execute(): schema is " << temp->table << endl;
	}

	if (viewMode == Kexi::DesignViewMode) {
		KexiTableDesignerView *t = new KexiTableDesignerView(win, parent);
		return t;
	}
	else if (viewMode == Kexi::DataViewMode) {
		if(!temp->table)
			return 0; //todo: message
		//we're not setting table schema here -it will be forced to set 
		// in KexiTableDesigner_DataView::afterSwitchFrom()
		KexiTableDesigner_DataView *t = new KexiTableDesigner_DataView(win, parent);
		return t;
	}
	return 0;
}

bool KexiTablePart::remove(KexiMainWindow *win, KexiPart::Item &item)
{
	if (!win || !win->project() || !win->project()->dbConnection())
		return false;

	KexiDB::Connection *conn = win->project()->dbConnection();
	KexiDB::TableSchema *sch = conn->tableSchema(item.identifier());

	if (sch) {
		tristate res = KexiTablePart::askForClosingObjectsUsingTableSchema(
			win, *conn, *sch, 
			i18n("You are about to remove table \"%1\" but following objects using this table are opened:")
			.arg(sch->name()));
		return true == conn->dropTable( sch );
	}
	//last chance: just remove item
	return conn->removeObject( item.identifier() );
}

tristate KexiTablePart::rename(KexiMainWindow *win, KexiPart::Item & item, 
	const QString& newName)
{
//TODO: what about objects (queries/forms) that use old name?
	KexiDB::Connection *conn = win->project()->dbConnection();
	KexiDB::TableSchema *sch = conn->tableSchema(item.identifier());
	if (!sch)
		return false;
	return conn->alterTableName(*sch, newName);
}

KexiDB::SchemaData*
KexiTablePart::loadSchemaData(KexiDialogBase *dlg, const KexiDB::SchemaData& sdata, int viewMode)
{
	Q_UNUSED( viewMode );

	return dlg->mainWin()->project()->dbConnection()->tableSchema( sdata.name() );
}

#if 0
KexiPart::DataSource *
KexiTablePart::dataSource()
{
	return new KexiTableDataSource(this);
}
#endif

tristate KexiTablePart::askForClosingObjectsUsingTableSchema(QWidget *parent, KexiDB::Connection& conn, 
	KexiDB::TableSchema& table, const QString& msg)
{
	Q3PtrList<KexiDB::Connection::TableSchemaChangeListenerInterface>* listeners
		= conn.tableSchemaChangeListeners(table);
	if (!listeners || listeners->isEmpty())
		return true;
	
	QString openedObjectsStr = "<ul>";
	for (Q3PtrListIterator<KexiDB::Connection::TableSchemaChangeListenerInterface> it(*listeners);
		it.current(); ++it)	{
			openedObjectsStr += QString("<li>%1</li>").arg(it.current()->listenerInfoString);
	}
	openedObjectsStr += "</ul>";
	int r = KMessageBox::questionYesNo(parent, 
		"<p>"+msg+"</p><p>"+openedObjectsStr+"</p><p>"
		+i18n("Do you want to close all windows for these objects?"), 
		QString::null, KGuiItem(i18n("Close windows"),"window-close"), KStdGuiItem::cancel());
	tristate res;
	if (r == KMessageBox::Yes) {
		//try to close every window
		res = conn.closeAllTableSchemaChangeListeners(table);
		if (res!=true) //do not expose closing errors twice; just cancel
			res = cancelled;
	}
	else
		res = cancelled;

	return res;
}

QString
KexiTablePart::i18nMessage(const QCString& englishMessage, KexiDialogBase* dlg) const
{
	if (englishMessage=="Design of object \"%1\" has been modified.")
		return i18n("Design of table \"%1\" has been modified.");

	if (englishMessage=="Object \"%1\" already exists.")
		return i18n("Table \"%1\" already exists.");

	if (dlg->currentViewMode()==Kexi::DesignViewMode && !dlg->neverSaved()
		&& englishMessage==":additional message before saving design")
		return i18n("Warning! Any data in this table will be removed upon design's saving!");

	return englishMessage;
}

void KexiTablePart::setupCustomPropertyPanelTabs(KTabWidget *tab, KexiMainWindow* mainWin)
{
	if (!d->lookupColumnPage) {
		d->lookupColumnPage = new KexiLookupColumnPage(0);
		connect(d->lookupColumnPage, SIGNAL(jumpToObjectRequested(const QCString&, const QCString&)),
			mainWin, SLOT(highlightObject(const QCString&, const QCString&)));

//! @todo add "Table" tab

	/*
		connect(d->dataSourcePage, SIGNAL(formDataSourceChanged(const QCString&, const QCString&)),
			KFormDesigner::FormManager::self(), SLOT(setFormDataSource(const QCString&, const QCString&)));
		connect(d->dataSourcePage, SIGNAL(dataSourceFieldOrExpressionChanged(const QString&, const QString&, KexiDB::Field::Type)),
			KFormDesigner::FormManager::self(), SLOT(setDataSourceFieldOrExpression(const QString&, const QString&, KexiDB::Field::Type)));
		connect(d->dataSourcePage, SIGNAL(insertAutoFields(const QString&, const QString&, const QStringList&)),
			KFormDesigner::FormManager::self(), SLOT(insertAutoFields(const QString&, const QString&, const QStringList&)));*/
	}

	KexiProject *prj = mainWin->project();
	d->lookupColumnPage->setProject(prj);

//! @todo add lookup field icon
	tab->addTab( d->lookupColumnPage, KIcon("combo"), "");
	tab->setTabToolTip( d->lookupColumnPage, i18n("Lookup column"));
}

KexiLookupColumnPage* KexiTablePart::lookupColumnPage() const
{
	return d->lookupColumnPage;
}

//----------------

#if 0
KexiTableDataSource::KexiTableDataSource(KexiPart::Part *part)
 : KexiPart::DataSource(part)
{
}

KexiTableDataSource::~KexiTableDataSource()
{
}

KexiDB::FieldList *
KexiTableDataSource::fields(KexiProject *project, const KexiPart::Item &it)
{
	kDebug() << "KexiTableDataSource::fields(): " << it.name() << endl;
	return project->dbConnection()->tableSchema(it.name());
}

KexiDB::Cursor *
KexiTableDataSource::cursor(KexiProject * /*project*/, 
	const KexiPart::Item &/*it*/, bool /*buffer*/)
{
	return 0;
}
#endif

//----------------

KexiTablePart::TempData::TempData(QObject* parent)
 : KexiDialogTempData(parent)
 , table(0)
 , tableSchemaChangedInPreviousView(true /*to force reloading on startup*/ )
{
}

//----------------

/**
TODO
*/
/*
AboutData( const char *programName,
	const char *version,
	const char *i18nShortDescription = 0,
	int licenseType = License_Unknown,
	const char *i18nCopyrightStatement = 0,
	const char *i18nText = 0,
	const char *homePageAddress = 0,
	const char *bugsEmailAddress = "submit@bugs.kde.org"
);

#define KEXIPART_EXPORT_FACTORY( libname, partClass, aboutData ) \
	static KexiPart::AboutData * libname ## updateAD(KexiPart::AboutData *ad) \
	{ ad->setAppName( #libname ); return ad; } \
	K_EXPORT_COMPONENT_FACTORY( libname, KGenericFactory<partClass>(libname ## updateAD(#libname)) )
*/

K_EXPORT_COMPONENT_FACTORY( kexihandler_table, KGenericFactory<KexiTablePart>("kexihandler_table") )

#include "kexitablepart.moc"

