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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kexitablepart.h"

#include <kdebug.h>
#include <kgenericfactory.h>

#include "keximainwindow.h"
#include "kexiproject.h"
#include "kexipartinfo.h"
#include "widget/kexidatatable.h"
#include "widget/kexidatatableview.h"
#include "kexialtertabledialog.h"
#include "kexialtertable_dataview.h"

#include <kexidb/connection.h>
#include <kexidb/cursor.h>
#include <kexidialogbase.h>


KexiTablePart::KexiTablePart(QObject *parent, const char *name, const QStringList &l)
 : KexiPart::Part(parent, name, l)
{
	kdDebug() << "KexiTablePart::KexiTablePart()" << endl;
	m_names["instance"] = i18n("Table");
	m_supportedViewModes = Kexi::DataViewMode | Kexi::DesignViewMode;
//js TODO: also add Kexi::TextViewMode when we'll have SQL ALTER TABLE EDITOR!!!
}

KexiTablePart::~KexiTablePart()
{
}

void KexiTablePart::initPartActions()
{
}

void KexiTablePart::initInstanceActions()
{
//moved to main window	createSharedAction(Kexi::DataViewMode, i18n("Filter"), "filter", 0, "tablepart_filter");

	KAction *a = createSharedToggleAction(
		Kexi::DesignViewMode, i18n("Toggle Primary Key"), "key", 0, "tablepart_toggle_pkey");
	a->setWhatsThis(i18n("Sets or removes primary key for currently selected field."));
}

KexiViewBase* KexiTablePart::createView(QWidget *parent, KexiDialogBase* dialog, 
	KexiPart::Item &item, int viewMode)
{
	KexiMainWindow *win = dialog->mainWin();
	if (!win || !win->project() || !win->project()->dbConnection())
		return 0;

	KexiDB::TableSchema *sch = win->project()->dbConnection()->tableSchema(item.name());
	kdDebug() << "KexiTablePart::execute(): schema is " << sch << endl;

	if (!dialog->tempData()) {
		dialog->setTempData( new KexiTablePart::TempData(dialog, sch) );
	}

	if (viewMode == Kexi::DesignViewMode) {
		KexiAlterTableDialog *t = new KexiAlterTableDialog(win, parent, sch, "altertable");
		return t;
	}
	else if (viewMode == Kexi::DataViewMode) {
		if(!sch)
			return 0; //todo: message
		//we're not setting table schema here -it will be forced to set 
		// in KexiAlterTable_DataView::afterSwitchFrom()
		KexiAlterTable_DataView *t = new KexiAlterTable_DataView(win, parent, "dataview");
		return t;
	}
	return 0;
}

bool KexiTablePart::remove(KexiMainWindow *win, KexiPart::Item &item)
{
	if (!win || !win->project() || !win->project()->dbConnection())
		return false;

	KexiDB::Connection *conn = win->project()->dbConnection();
	KexiDB::TableSchema *sch = conn->tableSchema(item.name());
	if(!sch) {
		//js TODO add error msg
		return false;
	}

	if (!conn->dropTable( sch )) {
		//js TODO add error msg
		return false;
	}
	return true;
}

KexiDB::SchemaData*
KexiTablePart::loadSchemaData(KexiDialogBase *dlg, const KexiDB::SchemaData& sdata)
{
	return dlg->mainWin()->project()->dbConnection()->tableSchema( sdata.name() );
}

//----------------

KexiPart::DataSource *
KexiTablePart::dataSource()
{
	return new KexiTableDataSource(this);
}

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
	kdDebug() << "KexiTableDataSource::fields(): " << it.name() << endl;
	return project->dbConnection()->tableSchema(it.name());
}

KexiDB::Cursor *
KexiTableDataSource::cursor(KexiProject *project, const KexiPart::Item &it, bool buffer)
{
	return 0;
}

//----------------

KexiTablePart::TempData::TempData(QObject* parent, KexiDB::TableSchema *sch)
 : KexiDialogTempData(parent)
 , table(sch)
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

