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

#include "KexiConnSelector.h"

#include <kexidb/drivermanager.h>
#include <kexidb/connectiondata.h>

#include "KexiConnSelectorBase.h"
#include "KexiNewFileDBWidget.h"

#include <kapplication.h>
#include <kiconloader.h>
#include <kmimetype.h>
#include <klocale.h>
#include <kdebug.h>
#include <kconfig.h>

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qcheckbox.h>

//! helper class
class ConnectionDataLVItem : public QListViewItem
{
public:
	ConnectionDataLVItem(KexiDB::ConnectionData *d, 
		const KexiDB::Driver::Info& info, QListView *list)
		: QListViewItem(list)
		, data(d)
	{
		setText(0, data->name+"  ");
		const QString &sfile = i18n("File");
		QString drvname = info.caption.isEmpty() ? data->driverName : info.caption;
		if (info.fileBased)
			setText(1, sfile + " ("+drvname+")  " );
		else
			setText(1, drvname+"  " );
		setText(2, (info.fileBased ? (QString("<")+sfile.lower()+">") : data->serverInfoString())+"  " );
	}
	~ConnectionDataLVItem() {}
	
	KexiDB::ConnectionData *data;
};

/*================================================================*/

class KexiConnSelectorWidgetPrivate
{
public:
	KexiConnSelectorWidgetPrivate()
	: conn_sel_shown(false)
	{
	}
	
	bool conn_sel_shown;//! helper
};

/*================================================================*/

/*!
 *  Constructs a KexiConnSelector which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
KexiConnSelectorWidget::KexiConnSelectorWidget( const KexiDBConnectionSet& conn_set, QWidget* parent,  const char* name )
    : QWidgetStack( parent, name )
	,m_conn_set(&conn_set)
	,d(new KexiConnSelectorWidgetPrivate())
{
	QString none, iconname = KMimeType::mimeType("application/x-kexiproject-sqlite")->icon(none,0);
	const QPixmap &icon = KGlobal::iconLoader()->loadIcon( iconname, KIcon::Desktop );
	setIcon( icon );
	
	m_file_sel = new KexiNewFileDBWidget( this, "file_sel" );
	m_file_sel->icon->setPixmap( DesktopIcon("FileNew") );
	m_file_sel->btn_advanced->setIconSet( SmallIconSet("1downarrow") );
	addWidget(m_file_sel);
	raiseWidget( m_file_sel );
	if (m_file_sel->layout())
		m_file_sel->layout()->setMargin(0);
	connect(m_file_sel->btn_advanced,SIGNAL(clicked()),this,SLOT(showAdvancedConn()));
	
	m_conn_sel = new KexiConnSelectorBase(this,"conn_sel");
	m_conn_sel->icon->setPixmap( DesktopIcon("socket") );
	m_conn_sel->btn_back->setIconSet( SmallIconSet("1uparrow") );
	addWidget(m_conn_sel);
	if (m_conn_sel->layout())
		m_conn_sel->layout()->setMargin(0);
	connect(m_conn_sel->btn_back,SIGNAL(clicked()),this,SLOT(showSimpleConn()));
	connect(m_conn_sel->list,SIGNAL(doubleClicked(QListViewItem*)),
		this,SIGNAL(connectionItemDBLClicked(QListViewItem*)));
}

/*!  
 *  Destroys the object and frees any allocated resources
 */
KexiConnSelectorWidget::~KexiConnSelectorWidget()
{
	delete d;
}

void KexiConnSelectorWidget::disconnectShowSimpleConnButton()
{
	m_conn_sel->btn_back->disconnect(this,SLOT(showSimpleConn()));
}

void KexiConnSelectorWidget::showAdvancedConn()
{
	if (!d->conn_sel_shown) {
		d->conn_sel_shown=true;
		//show connections (on demand):
		KexiDB::DriverManager manager;
		KexiDB::ConnectionData::List list = m_conn_set->list();
		KexiDB::ConnectionData *data = list.first();
		while (data) {
			KexiDB::Driver::Info info = manager.driverInfo(data->driverName);
			if (!info.name.isEmpty()) {
				new ConnectionDataLVItem(data, info, m_conn_sel->list);
			}
			else {
				kdWarning() << "KexiConnSelector::KexiConnSelector(): no driver found for '" << data->driverName << "'!" << endl;
			}
			data=list.next();
		}
		if (m_conn_sel->list->firstChild()) {
			m_conn_sel->list->setSelected(m_conn_sel->list->firstChild(),true);
		}
	}
	m_conn_sel->list->setFocus();
	raiseWidget(m_conn_sel);
}

void KexiConnSelectorWidget::showSimpleConn()
{
	raiseWidget(m_file_sel);
}

int KexiConnSelectorWidget::selectedConnectionType() const
{
	return (visibleWidget()==m_file_sel) ? FileBased : ServerBased;
}

KexiDB::ConnectionData* KexiConnSelectorWidget::selectedConnectionData() const
{
	if (selectedConnectionType()==KexiConnSelectorWidget::FileBased)
		return 0;
	ConnectionDataLVItem *item = static_cast<ConnectionDataLVItem*>(m_conn_sel->list->selectedItem());
	if (item)
		return item->data;
	return 0;
}

/*================================================================*/

KexiConnSelectorDialog::KexiConnSelectorDialog( const KexiDBConnectionSet& conn_set, QWidget *parent, const char *name )
 : KDialogBase( Plain, i18n("Creating a new project"), Help | Ok | Cancel, Ok, parent, name )
// , d(new KexiStartupDialogPrivate())
{
	setSizeGripEnabled(true);
	
	QVBoxLayout *lyr = new QVBoxLayout(plainPage(), 0, KDialogBase::spacingHint(), "lyr");
	m_sel = new KexiConnSelectorWidget(conn_set, plainPage(), "sel");
	lyr->addWidget(m_sel);
	setIcon(*m_sel->icon());
	
	connect(m_sel->m_conn_sel->list,SIGNAL(selectionChanged()),
		this,SLOT(connectionItemSelected()));
	connect(m_sel->m_conn_sel,SIGNAL(connectionItemDBLClicked(QListViewItem*)),
		this,SLOT(connectionItemDBLClicked(QListViewItem*)));

	m_sel->setMinimumWidth(500);
	move((qApp->desktop()->width()-width())/2,(qApp->desktop()->height()-height())/2);
}

KexiConnSelectorDialog::~KexiConnSelectorDialog()
{
}

int KexiConnSelectorDialog::selectedConnectionType() const
{
	return m_sel->selectedConnectionType();
}

void KexiConnSelectorDialog::connectionItemSelected()
{
	updateDialogState();
}

void KexiConnSelectorDialog::connectionItemDBLClicked(QListViewItem *item)
{
	m_sel->m_conn_sel->list->setSelected(item,true);
	updateDialogState();
	accept();
}

void KexiConnSelectorDialog::updateDialogState()
{
	if (selectedConnectionType()==KexiConnSelectorWidget::FileBased) {
		enableButtonOK(true);
	}
	else {
		enableButtonOK(m_sel->selectedConnectionData());
	}
}

KexiDB::ConnectionData* KexiConnSelectorDialog::selectedConnectionData() const
{
	return m_sel->selectedConnectionData();
}

bool KexiConnSelectorDialog::alwaysUseFilesForNewProjects() {
	KGlobal::config()->setGroup("Startup");
	return KGlobal::config()->readBoolEntry("AlwaysUseFilesForCreatingNewProjects", false);
}

/*KexiDB::ConnectionData* KexiConnSelectorDialog::connectionDataWeAlwaysUseForCreatingNewProjects()
{
	KGlobal::config()->setGroup("Startup");
	int id = KGlobal::config()->readNumEntry("ConnectionDataWeAlwaysUseForCreatingNewProjects", -1);
	if (id==-1)
		return 0;
	LoadConn
	
	
}*/
void KexiConnSelectorDialog::accept()
{
	//save setting
	if (selectedConnectionType()==KexiConnSelectorWidget::FileBased
		&& m_sel->m_file_sel->chk_always->isChecked()) {
		KGlobal::config()->writeEntry("AlwaysUseFilesForCreatingNewProjects", true);
	}
	else if (m_sel->selectedConnectionData() && m_sel->m_conn_sel->chk_always->isChecked()) {
//		m_sel->m_conn_sel->selectedConnectionData();
		KGlobal::config()->writeEntry("ConnectionDataWeAlwaysUseForNewProjects", m_sel->selectedConnectionData()->id);
		//TODO
	}
	KDialogBase::accept();
}


#include "KexiConnSelector.moc"

