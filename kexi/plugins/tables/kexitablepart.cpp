/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>

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

#include <kexidb/connection.h>
#include <kexidb/cursor.h>
#include <kexidialogbase.h>


/*class KexiTablePartGUIClient: public KexiPart::GUIClient {
public:
	KexiTablePartGUIClient(KexiMainWindow *win, KexiPart::Part *part)
		: KexiPart::GUIClient(part, i18n("Table"))
	{
//	new KAction(i18n("Table..."), info()->itemIcon(), 0, this, SLOT(create()), actionCollection(), "tablepart_create");
//	new KAction(i18n("Table..."), "createTable", 0, this, SLOT(create()), actionCollection(), "tablepart_create");
		setXMLFile("kexitablepartui.rc");
		win->guiFactory()->addClient(this);
	}
	virtual ~KexiTablePartGUIClient(){}

};*/

KexiTablePart::KexiTablePart(QObject *parent, const char *name, const QStringList &l)
 : KexiPart::Part(parent, name, l)
{
	kdDebug() << "KexiTablePart::KexiTablePart()" << endl;
	m_names["instance"] = i18n("Table");
	m_supportedViewModes = Kexi::DataViewMode | Kexi::DesignViewMode;
//js TODO: also add Kexi::TextViewMode when we'll have SQL ALTER TABLE EDITOR!!!

//	m_names["new"] = i18n("New Table...");
}

KexiTablePart::~KexiTablePart()
{
}

void KexiTablePart::initPartActions(KActionCollection *)
{
//this is automatic? -no
//create child guicilent: guiClient()->setXMLFile("kexidatatableui.rc");

	kdDebug()<<"INIT ACTIONS***********************************************************************"<<endl;
	//TODO
}

void KexiTablePart::initInstanceActions( int mode, KActionCollection *col )
{
	if (mode==Kexi::AllViewModes) {
	}
	else if (mode==Kexi::DataViewMode) {
		new KAction(i18n("Filter"), "filter", 0, this, SLOT(filter()), col, "tablepart_filter");
		//TODO
	}
}

#if 0
void
KexiTablePart::execute(KexiMainWindow *win, KexiPart::Item &item)
{
	KexiDB::TableSchema *sch = win->project()->dbConnection()->tableSchema(item.name());
	kdDebug() << "KexiTablePart::execute(): schema is " << sch << endl;
	if(!sch)
		return;

	KexiDB::Cursor *c = win->project()->dbConnection()->prepareQuery(*sch);

	kdDebug() << "KexiTablePart::execute(): cursor is " << c << endl;
	if(!c)
		return;

	KexiDataTable *t = new KexiDataTable(win, QString("%1 - %2").arg(item.name()).arg(instanceName()), c);
	t->resize(t->sizeHint());
	t->setMinimumSize(t->minimumSizeHint().width(),t->minimumSizeHint().height());

	t->setIcon( DesktopIcon( info()->itemIcon() ) );
	t->setId(item.identifier());
}
#endif

//KexiDialogBase*
//KexiTablePart::createInstance(KexiMainWindow *win, KexiPart::Item &item, int viewMode)
KexiViewBase* KexiTablePart::createView(QWidget *parent, KexiDialogBase* dialog, 
	KexiPart::Item &item, int viewMode)
{
	KexiMainWindow *win = dialog->mainWin();
	if (!win || !win->project() || !win->project()->dbConnection())
		return 0;

	KexiDB::TableSchema *sch = win->project()->dbConnection()->tableSchema(item.name());
	kdDebug() << "KexiTablePart::execute(): schema is " << sch << endl;
/*new schema will be crated later
	if(!sch) {
		//js TODO add error msg
		sch = new KexiDB::TableSchema(item.name());
		sch->setCaption(item.caption());
		win->project()->dbConnection()->executeQuery(QString("DELETE FROM kexi__objects WHERE o_id = %1").arg(item.identifier()));
	}*/

	if (viewMode == Kexi::DesignViewMode) {
		KexiAlterTableDialog *t = new KexiAlterTableDialog(win, parent, sch, "altertable");
		return t;
	}
	else if (viewMode == Kexi::DataViewMode) {
		if(!sch)
			return 0; //todo: message
		KexiDB::Cursor *c = win->project()->dbConnection()->prepareQuery(*sch);

		kdDebug() << "KexiTablePart::execute(): cursor is " << c << endl;
		if(!c) {
			//js TODO add error msg
			return 0;
		}

		KexiDataTable *t = new KexiDataTable(win, parent, c, "datatable"); //QString("%1 - %2").arg(item.name()).arg(instanceName()), c);
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

KexiPart::DataSource *
KexiTablePart::dataSource()
{
	return new KexiTableDataSource(this);
}

K_EXPORT_COMPONENT_FACTORY( kexihandler_table, KGenericFactory<KexiTablePart> )

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

#include "kexitablepart.moc"

