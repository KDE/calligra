/* This file is part of the KDE project
   Copyright (C) 2003,2005 Jaroslaw Staniek <js@iidea.pl>

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

#include <kexi.h>
#include "KexiConnSelectorBase.h"
#include "KexiOpenExistingFile.h"
#include <widget/kexidbconnectionwidget.h>

#include <kapplication.h>
#include <kiconloader.h>
#include <kmimetype.h>
#include <klocale.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kurlcombobox.h>
#include <ktoolbar.h>
#include <kpopupmenu.h>
#include <ktoolbarbutton.h>
#include <kactionclasses.h>

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qtooltip.h>
#include <qtextedit.h>
#include <qgroupbox.h>

ConnectionDataLVItem::ConnectionDataLVItem(KexiDB::ConnectionData *data, 
	const KexiDB::Driver::Info& info, QListView *list)
	: QListViewItem(list)
	, m_data(data)
{
	update(info);
}

ConnectionDataLVItem::~ConnectionDataLVItem() 
{
}

void ConnectionDataLVItem::update(const KexiDB::Driver::Info& info)
{
	setText(0, m_data->caption+"  ");
	const QString &sfile = i18n("File");
	QString drvname = info.caption.isEmpty() ? m_data->driverName : info.caption;
	if (info.fileBased)
		setText(1, sfile + " ("+drvname+")  " );
	else
		setText(1, drvname+"  " );
	setText(2, (info.fileBased ? (QString("<")+sfile.lower()+">") : m_data->serverInfoString(true))+"  " );
}

/*================================================================*/

//! @internal
class KexiConnSelectorWidgetPrivate
{
public:
	KexiConnSelectorWidgetPrivate()
	: conn_sel_shown(false)
	, file_sel_shown(false)
	, confirmOverwrites(true)
	{
	}
	
	QGuardedPtr<KexiDBConnectionSet> conn_set;
	KexiDB::DriverManager manager;
	bool conn_sel_shown;//! helper
	bool file_sel_shown;
	bool confirmOverwrites;
};

/*================================================================*/

KexiConnSelectorWidget::KexiConnSelectorWidget( KexiDBConnectionSet& conn_set, 
	QWidget* parent,  const char* name )
	: QWidgetStack( parent, name )
	,d(new KexiConnSelectorWidgetPrivate())
{
	d->conn_set = &conn_set;
	QString none, iconname = KMimeType::mimeType( KexiDB::Driver::defaultFileBasedDriverMimeType() )->icon(none,0);
	const QPixmap &icon = KGlobal::iconLoader()->loadIcon( iconname, KIcon::Desktop );
	setIcon( icon );
	
	m_file = new KexiOpenExistingFile( this, "KexiOpenExistingFile");
	m_file->btn_advanced->setIconSet( SmallIconSet("1downarrow") );
	m_fileDlg = 0;
		
	addWidget(m_file);
	connect(m_file->btn_advanced,SIGNAL(clicked()),this,SLOT(showAdvancedConn()));
	m_remote = new KexiConnSelectorBase(this,"conn_sel");
	m_remote->icon->setPixmap( DesktopIcon("socket") );
	m_remote->btn_back->setIconSet( SmallIconSet("1uparrow") );
	connect(m_remote->btn_add, SIGNAL(clicked()), this, SLOT(slotRemoteAddBtnClicked()));
	connect(m_remote->btn_edit, SIGNAL(clicked()), this, SLOT(slotRemoteEditBtnClicked()));
	connect(m_remote->btn_remove, SIGNAL(clicked()), this, SLOT(slotRemoteRemoveBtnClicked()));
	QToolTip::add(m_remote->btn_add, i18n("Add a new database connection"));
	QToolTip::add(m_remote->btn_edit, i18n("Edit selected database connection"));
	QToolTip::add(m_remote->btn_remove, i18n("Remove selected database connections"));
	addWidget(m_remote);
	if (m_remote->layout())
		m_remote->layout()->setMargin(0);
	connect(m_remote->btn_back,SIGNAL(clicked()),this,SLOT(showSimpleConn()));
	connect(m_remote->list,SIGNAL(doubleClicked(QListViewItem*)),
		this,SLOT(slotConnectionItemExecuted(QListViewItem*)));
	connect(m_remote->list,SIGNAL(returnPressed(QListViewItem*)),
		this,SLOT(slotConnectionItemExecuted(QListViewItem*)));
	connect(m_remote->list,SIGNAL(selectionChanged()),
		this,SLOT(slotConnectionSelectionChanged()));
}

KexiConnSelectorWidget::~KexiConnSelectorWidget()
{
	delete d;
}

void KexiConnSelectorWidget::disconnectShowSimpleConnButton()
{
	m_remote->btn_back->disconnect(this,SLOT(showSimpleConn()));
}

void KexiConnSelectorWidget::showAdvancedConn()
{
	if (!d->conn_sel_shown) {
		d->conn_sel_shown=true;
		//setup
//TODO 
//		new KToolBarButton("change", 1, m_remote->frm_change, "change", i18n("Change"));
/*		KPopupMenu *pm = new KPopupMenu(0);
		pm->insertItem("aaa");
		new KDropDownButtton(m_remote->frm_change, pm, 
	m_remote->frm_change->setFixedWidth(100);
//		m_remote->frm_change->setBackgroundColor(black);
	//	KToolBar *tbar = new KToolBar(m_remote->frm_change);
//		tbar->insertButton("change", 1, pm, true, i18n("Change"));
		tbar->insertButton("change", 1, 
		KActionMenu * act_change = new KActionMenu(i18n2005-04-07("Change"), this, "change");
		act_change->insert( new KAction("Add connection", KShortcut(), 0, 0, 0) );
		act_change->plug( tbar );*/
//TODO
		
		//show connections (on demand):
		for (KexiDB::ConnectionData::ListIterator it(d->conn_set->list()); it.current(); ++it) {
			addConnectionData( it.current() );
//			else {
//this error should be more verbose:
//				kdWarning() << "KexiConnSelector::KexiConnSelector(): no driver found for '" << it.current()->driverName << "'!" << endl;
//			}
		}
		if (m_remote->list->firstChild()) {
			m_remote->list->setSelected(m_remote->list->firstChild(),true);
		}
		m_remote->descriptionEdit->setPaletteBackgroundColor(palette().active().background());
		m_remote->descGroupBox->layout()->setMargin(2);
		m_remote->list->setFocus();
		slotConnectionSelectionChanged();
	}
	raiseWidget(m_remote);
}

ConnectionDataLVItem* KexiConnSelectorWidget::addConnectionData( KexiDB::ConnectionData* data )
{
	const KexiDB::Driver::Info info( d->manager.driverInfo(data->driverName) );
//	if (!info.name.isEmpty()) {
	return new ConnectionDataLVItem(data, info, m_remote->list);
//	}
}

void KexiConnSelectorWidget::showSimpleConn()
{
	if (!d->file_sel_shown) {
		d->file_sel_shown=true;
		m_fileDlg = new KexiStartupFileDialog( "", KexiStartupFileDialog::Opening,
			m_file, "openExistingFileDlg");
		m_fileDlg->setConfirmOverwrites( d->confirmOverwrites );
		static_cast<QVBoxLayout*>(m_file->layout())->insertWidget( 2, m_fileDlg );

		for (QWidget *w = parentWidget(true);w;w=w->parentWidget(true)) {
			if (w->isDialog()) {
//#ifndef Q_WS_WIN
				connect(m_fileDlg,SIGNAL(rejected()),static_cast<QDialog*>(w),SLOT(reject()));
//#endif
//				connect(m_fileDlg,SIGNAL(cancelled()),static_cast<QDialog*>(w),SLOT(reject()));
				break;
			}
		}
	}
	raiseWidget(m_file);
#ifndef KEXI_SERVER_SUPPORT
	m_file->spacer->hide();
	m_file->label->hide();
	m_file->btn_advanced->hide();
	m_file->label->parentWidget()->hide();
#endif
}

int KexiConnSelectorWidget::selectedConnectionType() const
{
	return (visibleWidget()==m_file) ? FileBased : ServerBased;
}

/*ConnectionDataLVItem* KexiConnSelectorWidget::selectedConnectionDataItem() const
{
	if (selectedConnectionType()!=KexiConnSelectorWidget::ServerBased)
		return 0;
	ConnectionDataLVItem *item = 0; // = static_cast<ConnectionDataLVItem*>(m_remote->list->selectedItem());
	for (QListViewItemIterator it(m_remote->list); it.current(); ++it) {
		if (it.current()->isSelected()) {
			if (item)
				return 0; //multiple
			item = static_cast<ConnectionDataLVItem*>(it.current());
		}
	}
	return item;
}*/

KexiDB::ConnectionData* KexiConnSelectorWidget::selectedConnectionData() const
{
	ConnectionDataLVItem *item = static_cast<ConnectionDataLVItem*>(m_remote->list->selectedItem()); //ConnectionDataItem();
	if (!item)
		return 0;
	return item->data();
}

QString KexiConnSelectorWidget::selectedFileName()
{
	if (selectedConnectionType()!=KexiConnSelectorWidget::FileBased)
		return QString::null;
	return m_fileDlg->currentFileName();
}

void KexiConnSelectorWidget::slotConnectionItemExecuted(QListViewItem *item)
{
	emit connectionItemExecuted(static_cast<ConnectionDataLVItem*>(item));
}

void KexiConnSelectorWidget::slotConnectionSelectionChanged()
{
	ConnectionDataLVItem* item = static_cast<ConnectionDataLVItem*>(m_remote->list->selectedItem());
	//update buttons availability
/*	ConnectionDataLVItem *singleItem = 0;
	bool multi = false;
	for (QListViewItemIterator it(m_remote->list); it.current(); ++it) {
		if (it.current()->isSelected()) {
			if (singleItem) {
				singleItem = 0;
				multi = true;
				break;
			}
			else
				singleItem = static_cast<ConnectionDataLVItem*>(it.current());
		}
	}*/
	m_remote->btn_edit->setEnabled(item);
	m_remote->btn_remove->setEnabled(item);
	m_remote->descriptionEdit->setText(item ? item->data()->description : QString::null);
	emit connectionItemHighlighted(item);
}

QListView* KexiConnSelectorWidget::connectionsList() const
{
	return m_remote->list;
}

void KexiConnSelectorWidget::setFocus()
{
	QWidgetStack::setFocus();
	if (visibleWidget()==m_file)
		m_fileDlg->setFocus(); //m_fileDlg->locationWidget()->setFocus();
	else
		m_remote->list->setFocus();
}

void KexiConnSelectorWidget::hideHelpers()
{
	m_file->lbl->hide();
	m_file->line->hide();
	m_file->spacer->hide();
	m_file->label->hide();
	m_remote->label->hide();
	m_remote->label_back->hide();
	m_remote->btn_back->hide();
	m_remote->icon->hide();
}

void KexiConnSelectorWidget::setConfirmOverwrites(bool set)
{
	d->confirmOverwrites = set;
	if (m_fileDlg)
		m_fileDlg->setConfirmOverwrites( d->confirmOverwrites );
}

bool KexiConnSelectorWidget::confirmOverwrites() const
{
	return d->confirmOverwrites;
}

/*static QString msgUnfinished() { 
	return i18n("To define or change a connection, use command line options or click on .kexis file. "
		"You can find example .kexis file at <a href=\"%1\">here</a>.").arg("") //temporary, please do not change for 0.8!
		+ "\nhttp://www.kexi-project.org/resources/testdb.kexis"; */
//		.arg("http://websvn.kde.org/*checkout*/branches/kexi/0.9/koffice/kexi/tests/startup/testdb.kexis");
//}

void KexiConnSelectorWidget::slotRemoteAddBtnClicked()
{
	KexiDB::ConnectionData data;
	KexiDBConnectionDialog dlg(data, QString::null,
		KGuiItem(i18n("&Add"), "button_ok", i18n("Add database connection")) );
	dlg.setCaption(i18n("Add a New Database Connection"));
	if (QDialog::Accepted!=dlg.exec())
		return;

	//store this conn. data
	KexiDB::ConnectionData *newData = new KexiDB::ConnectionData(*dlg.currentProjectData().connectionData());
	if (!d->conn_set->addConnectionData(newData)) {
		//! @todo msg?
		delete newData;
		return;
	}

	ConnectionDataLVItem* item = addConnectionData(newData);
//	m_remote->list->clearSelection();
	m_remote->list->setSelected(item, true);
	slotConnectionSelectionChanged();
}

void KexiConnSelectorWidget::slotRemoteEditBtnClicked()
{
	ConnectionDataLVItem* item = static_cast<ConnectionDataLVItem*>(m_remote->list->selectedItem());
	if (!item)
		return;
	KexiDBConnectionDialog dlg(*item->data(), QString::null,
		KGuiItem(i18n("&Save"), "filesave", i18n("Save changes made to this database connection")) );
	dlg.setCaption(i18n("Edit Database Connection"));
	if (QDialog::Accepted!=dlg.exec())
		return;

	KexiDB::ConnectionData *newData = new KexiDB::ConnectionData( *dlg.currentProjectData().connectionData() );
	if (!d->conn_set->saveConnectionData(item->data(), newData)) {
		//! @todo msg?
		delete newData;
		return;
	}
	const KexiDB::Driver::Info info( d->manager.driverInfo(item->data()->driverName) );
	item->update(info);
	slotConnectionSelectionChanged(); //to update descr. edit
}

void KexiConnSelectorWidget::slotRemoteRemoveBtnClicked()
{
	ConnectionDataLVItem* item = static_cast<ConnectionDataLVItem*>(m_remote->list->selectedItem());
	if (!item)
		return;
	if (KMessageBox::Yes!=KMessageBox::questionYesNo(0, 
		i18n("Do you want to remove database connection \"%1\" from the list of available connections?")
		.arg(item->data()->serverInfoString(true)), 0))
		return;

	QListViewItem* nextItem = item->itemBelow();
	if (!nextItem)
		nextItem = item->itemAbove();
	if (!d->conn_set->removeConnectionData(item->data()))
		return;

	m_remote->list->removeItem(item);
	if (nextItem)
		m_remote->list->setSelected(nextItem, true);
	slotConnectionSelectionChanged();
}

#include "KexiConnSelector.moc"
