/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#include "KexiProjectSelector.h"

#include <kexidb/drivermanager.h>
#include <kexidb/connectiondata.h>

#include <kapplication.h>
#include <kiconloader.h>
#include <kmimetype.h>
#include <klocale.h>
#include <kdebug.h>

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlistview.h>

//! helper class
class ProjectDataLVItem : public QListViewItem
{
public:
	ProjectDataLVItem(const KexiProjectData *d, 
		const KexiDB::Driver::Info& info, QListView *list)
		: QListViewItem(list)
		, data(d)
	{
		setText(0, data->caption()+"  ");
		const KexiDB::ConnectionData *cdata = data->connectionData();
		
		setText(1, data->databaseName()+"  ");
		
		QString drvname = info.caption.isEmpty() ? cdata->driverName : info.caption;
		if (info.fileBased) {
			setText(2, i18n("File") + " ("+drvname+")  " );
		} else {
			setText(2, drvname+"  " );
		}
		
		QString conn;
		if (!cdata->name.isEmpty())
			conn = cdata->name + ": ";
		conn += cdata->serverInfoString();
		setText(3, conn + "  ");
	}
	~ProjectDataLVItem() {}
	
	const KexiProjectData *data;
};

/*================================================================*/

class KexiProjectSelectorWidgetPrivate
{
public:
	KexiProjectSelectorWidgetPrivate()
	{
	}
	
};

/*================================================================*/

/*!
 *  Constructs a KexiProjectSelector which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
KexiProjectSelectorWidget::KexiProjectSelectorWidget( const KexiProjectSet& prj_set, QWidget* parent,  const char* name )
    : KexiProjectSelectorBase( parent, name )
	,m_prj_set(&prj_set)
	,d(new KexiProjectSelectorWidgetPrivate())
{
	QString none, iconname = KMimeType::mimeType("application/x-kexiproject-sqlite")->icon(none,0);
	const QPixmap &myicon = KGlobal::iconLoader()->loadIcon( iconname, KIcon::Desktop );
	setIcon( myicon );
	icon->setPixmap( myicon );
	const QPixmap &dbicon = SmallIcon("db",16);
//	list->setHScrollBarMode( QScrollView::AlwaysOn );

	//show projects
	KexiDB::DriverManager manager;
	KexiProjectData::ConstList prjlist = m_prj_set->list();
	const KexiProjectData *data = prjlist.first();
	while (data) {
		KexiDB::Driver::Info info = manager.driverInfo(data->connectionData()->driverName);
		if (!info.name.isEmpty()) {
			ProjectDataLVItem *item = new ProjectDataLVItem(data, info, list);
			if (info.fileBased)
				item->setPixmap( 0, myicon );
			else
				item->setPixmap( 0, dbicon );
		}
		else {
			kdWarning() << "KexiProjectSelector::KexiProjectSelector(): no driver found for '" << data->connectionData()->driverName << "'!" << endl;
		}
		data=prjlist.next();
	}
	if (list->firstChild()) {
		list->setSelected(list->firstChild(),true);
	}
	connect(list,SIGNAL(doubleClicked(QListViewItem*)),this,SLOT(itemDoubleClicked(QListViewItem*)));
}

/*!  
 *  Destroys the object and frees any allocated resources
 */
KexiProjectSelectorWidget::~KexiProjectSelectorWidget()
{
	delete d;
}

const KexiProjectData* KexiProjectSelectorWidget::selectedProjectData() const
{
	ProjectDataLVItem *item = static_cast<ProjectDataLVItem*>(list->selectedItem());
	if (item)
		return item->data;
	return 0;
}

void KexiProjectSelectorWidget::itemDoubleClicked(QListViewItem *item)
{
	ProjectDataLVItem *ditem = static_cast<ProjectDataLVItem*>(item);
	if (ditem)
		emit projectDoubleClicked( ditem->data );
}

/*================================================================*/
#if 0
KexiProjectSelectorDialog::KexiProjectSelectorDialog( const KexiDBConnectionSet& conn_set, QWidget *parent, const char *name )
 : KDialogBase( Plain, i18n("Creating a new project"), Help | Ok | Cancel, Ok, parent, name )
// , d(new KexiStartupDialogPrivate())
{
	setSizeGripEnabled(true);
	
	QVBoxLayout *lyr = new QVBoxLayout(plainPage(), 0, KDialogBase::spacingHint(), "lyr");
	m_sel = new KexiProjectSelectorWidget(conn_set, plainPage(), "sel");
	lyr->addWidget(m_sel);
	setIcon(*m_sel->icon());
	
	connect(m_sel->m_conn_sel->list,SIGNAL(selectionChanged()),this,SLOT(connectionItemSelected()));
	connect(m_sel->m_conn_sel->list,SIGNAL(doubleClicked(QListViewItem*)),this,SLOT(connectionItemDBLClicked(QListViewItem*)));

	m_sel->setMinimumWidth(500);

//	adjustSize();
}

KexiProjectSelectorDialog::~KexiProjectSelectorDialog()
{
}

int KexiProjectSelectorDialog::selectedConnectionType() const
{
	return m_sel->selectedConnectionType();
}

void KexiProjectSelectorDialog::connectionItemSelected()
{
	updateDialogState();
}

void KexiProjectSelectorDialog::connectionItemDBLClicked(QListViewItem *item)
{
	m_sel->m_conn_sel->list->setSelected(item,true);
	updateDialogState();
	accept();
}

void KexiProjectSelectorDialog::updateDialogState()
{
	if (selectedConnectionType()==KexiProjectSelectorWidget::FileBased) {
		enableButtonOK(true);
	}
	else {
		enableButtonOK(m_sel->selectedConnectionData());
	}
}

const KexiDB::ConnectionData* KexiProjectSelectorDialog::selectedConnectionData() const
{
	return m_sel->selectedConnectionData();
}

#endif

#include "KexiProjectSelector.moc"

