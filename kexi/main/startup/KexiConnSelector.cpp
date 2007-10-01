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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KexiConnSelector.h"
#include "KexiStartupFileWidget.h"

#include <kexidb/drivermanager.h>
#include <kexidb/connectiondata.h>

#include <kexi.h>
#include <widget/kexiprjtypeselector.h>
#include <widget/kexidbconnectionwidget.h>

#include <kapplication.h>
#include <kiconloader.h>
#include <kmimetype.h>
#include <klocale.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kurlcombobox.h>
#include <ktoolbar.h>
#include <kmenu.h>

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qtooltip.h>
#include <q3textedit.h>
#include <q3groupbox.h>
#include <q3widgetstack.h>
#include <q3buttongroup.h>
#include <QVBoxLayout>
#include <QPixmap>
#include <QFrame>
#include <QStackedWidget>

KexiConnSelectorBase::KexiConnSelectorBase(QWidget* parent)
 : QWidget(parent)
{
	setupUi(this);
}

KexiConnSelectorBase::~KexiConnSelectorBase()
{
}

/*================================================================*/

ConnectionDataLVItem::ConnectionDataLVItem(KexiDB::ConnectionData *data, 
	const KexiDB::Driver::Info& info, Q3ListView *list)
	: Q3ListViewItem(list)
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
	setText(2, (info.fileBased ? (QString("<")+sfile.toLower()+">") : m_data->serverInfoString(true))+"  " );
}

/*================================================================*/

//! @internal
class KexiConnSelectorWidget::Private
{
public:
	Private()
	: conn_sel_shown(false)
	, file_sel_shown(false)
	, confirmOverwrites(true)
	{
	}
	
	QWidget* openExistingWidget;
	KexiPrjTypeSelector* prjTypeSelector;
	QString startDirOrVariable;
	KAbstractFileWidget::OperationMode fileAccessType;
	QStackedWidget *stack;
	QPointer<KexiDBConnectionSet> conn_set;
	KexiDB::DriverManager manager;
	bool conn_sel_shown : 1;//! helper
	bool file_sel_shown : 1;
	bool confirmOverwrites : 1;
};

/*================================================================*/

KexiConnSelectorWidget::KexiConnSelectorWidget( KexiDBConnectionSet& conn_set, 
	const QString& startDirOrVariable, KAbstractFileWidget::OperationMode fileAccessType, QWidget* parent )
	: QWidget(parent)
	,d(new Private())
{
	d->conn_set = &conn_set;
	d->startDirOrVariable = startDirOrVariable;
	d->fileAccessType = fileAccessType;
	QString iconname = KMimeType::mimeType( 
		KexiDB::Driver::defaultFileBasedDriverMimeType() )->iconName();
	//const QPixmap icon = KIconLoader::global()->loadIcon( iconname, KIconLoader::Desktop, 48 );
	setWindowIcon( KIcon(iconname) );

	QBoxLayout* globalLyr = new QVBoxLayout( this );

	//create header with radio buttons
	d->openExistingWidget = new QWidget(this);
	d->openExistingWidget->setObjectName("openExistingWidget");
	QVBoxLayout* openExistingWidgetLyr = new QVBoxLayout( d->openExistingWidget );
	openExistingWidgetLyr->setMargin(0);
//	QLabel* lbl = new QLabel(i18n("<b>Select existing Kexi project to open:</b>"), openExistingWidget);
//	openExistingWidgetLyr->addWidget( lbl );
	d->prjTypeSelector = new KexiPrjTypeSelector( d->openExistingWidget );
	connect(d->prjTypeSelector->buttonGroup,SIGNAL(clicked(int)),
		this,SLOT(slotPrjTypeSelected(int)));
	openExistingWidgetLyr->addWidget( d->prjTypeSelector );
	openExistingWidgetLyr->addSpacing( KDialog::spacingHint() );
	QFrame* line = new QFrame( d->openExistingWidget );
	line->setFrameShape( QFrame::HLine );
	line->setFrameShadow( QFrame::Sunken );
	openExistingWidgetLyr->addWidget( line );
	globalLyr->addWidget(d->openExistingWidget);

	d->stack = new QStackedWidget(this);
	d->stack->setObjectName("stack");
	globalLyr->addWidget(d->stack, 1);

//	m_file = new KexiOpenExistingFile( this, "KexiOpenExistingFile");
//	m_file->btn_advanced->setIconSet( KIcon("arrow-down") );
	fileWidget = 0;
		
//	addWidget(m_file);
//	connect(m_file->btn_advanced,SIGNAL(clicked()),this,SLOT(showAdvancedConn()));

	m_remote = new KexiConnSelectorBase(d->stack);
	m_remote->setObjectName("conn_sel");
	m_remote->lblIcon->setPixmap( DesktopIcon("network-wired") );
	m_remote->lblIcon->setFixedSize( m_remote->lblIcon->pixmap()->size() );
//	m_remote->btn_back->setIconSet( KIcon("arrow-up") );
	connect(m_remote->btn_add, SIGNAL(clicked()), this, SLOT(slotRemoteAddBtnClicked()));
	connect(m_remote->btn_edit, SIGNAL(clicked()), this, SLOT(slotRemoteEditBtnClicked()));
	connect(m_remote->btn_remove, SIGNAL(clicked()), this, SLOT(slotRemoteRemoveBtnClicked()));
	m_remote->btn_add->setToolTip( i18n("Add a new database connection"));
	m_remote->btn_edit->setToolTip( i18n("Edit selected database connection"));
	m_remote->btn_remove->setToolTip( i18n("Remove selected database connections"));
	d->stack->addWidget(m_remote);
	if (m_remote->layout())
		m_remote->layout()->setMargin(0);
//	connect(m_remote->btn_back,SIGNAL(clicked()),this,SLOT(showSimpleConn()));
	connect(m_remote->list,SIGNAL(doubleClicked(Q3ListViewItem*)),
		this,SLOT(slotConnectionItemExecuted(Q3ListViewItem*)));
	connect(m_remote->list,SIGNAL(returnPressed(Q3ListViewItem*)),
		this,SLOT(slotConnectionItemExecuted(Q3ListViewItem*)));
	connect(m_remote->list,SIGNAL(selectionChanged()),
		this,SLOT(slotConnectionSelectionChanged()));
}

KexiConnSelectorWidget::~KexiConnSelectorWidget()
{
	delete d;
}

/*void KexiConnSelectorWidget::disconnectShowSimpleConnButton()
{
	m_remote->btn_back->disconnect(this,SLOT(showSimpleConn()));
}*/

void KexiConnSelectorWidget::showAdvancedConn()
{
	slotPrjTypeSelected(2);
	d->prjTypeSelector->buttonGroup->setButton(2);
}

//void KexiConnSelectorWidget::showAdvancedConn()
void KexiConnSelectorWidget::slotPrjTypeSelected(int id)
{
	if (id==1) {//file-based prj type
		showSimpleConn();
	}
	else if (id==2) {//server-based prj type
		if (!d->conn_sel_shown) {
			d->conn_sel_shown=true;
		
			//show connections (on demand):
			for (KexiDB::ConnectionData::ListIterator it(d->conn_set->list()); it.current(); ++it) {
				addConnectionData( it.current() );
	//			else {
	//this error should be more verbose:
	//				kWarning() << "KexiConnSelector::KexiConnSelector(): no driver found for '" << it.current()->driverName << "'!" << endl;
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
		d->stack->setCurrentWidget(m_remote);
	}
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
	d->prjTypeSelector->buttonGroup->setButton(1);
	if (!d->file_sel_shown) {
		d->file_sel_shown = true;
		fileWidget = new KexiStartupFileWidget( 
			KUrl(d->startDirOrVariable), 
				d->fileAccessType==KAbstractFileWidget::Opening
					? KexiStartupFileWidget::Opening : KexiStartupFileWidget::SavingFileBasedDB,
				d->stack );
		fileWidget->setOperationMode( d->fileAccessType );
		fileWidget->setObjectName("openFileWidget");
		fileWidget->setConfirmOverwrites( d->confirmOverwrites );
		d->stack->addWidget(fileWidget);

		for (QWidget *w = parentWidget(); w; w=w->parentWidget()) {
			if (w->windowType()==Qt::Dialog) {
#ifdef __GNUC__
#warning TODO KFileWidget				connect(m_fileDlg, SIGNAL(rejected()), qobject_cast<QDialog*>(w), SLOT(reject()));
#else
#pragma WARNING( TODO KFileWidget				connect(m_fileDlg, SIGNAL(rejected()), qobject_cast<QDialog*>(w), SLOT(reject())); )
#endif
				break;
			}
		}
	}
	d->stack->setCurrentWidget(fileWidget);
}

int KexiConnSelectorWidget::selectedConnectionType() const
{
	return (d->stack->currentWidget()==fileWidget) ? FileBased : ServerBased;
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
		return QString();
	return fileWidget->highlightedFile(); //ok? fileWidget->selectedFile();
}

void KexiConnSelectorWidget::setSelectedFileName(const QString& fileName)
{
	if (selectedConnectionType()!=KexiConnSelectorWidget::FileBased)
		return;
	return fileWidget->setSelection(fileName);
}

void KexiConnSelectorWidget::slotConnectionItemExecuted(Q3ListViewItem *item)
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
	m_remote->descriptionEdit->setText(item ? item->data()->description : QString());
	emit connectionItemHighlighted(item);
}

Q3ListView* KexiConnSelectorWidget::connectionsList() const
{
	return m_remote->list;
}

void KexiConnSelectorWidget::setFocus()
{
	QWidget::setFocus();
	if (d->stack->currentWidget()==fileWidget)
		fileWidget->setFocus(); //m_fileDlg->locationWidget()->setFocus();
	else
		m_remote->list->setFocus();
}

void KexiConnSelectorWidget::hideHelpers()
{
	d->openExistingWidget->hide();

/*	m_file->lbl->hide();
	m_file->line->hide();
	m_file->spacer->hide();
	m_file->label->hide();
	m_remote->label->hide();
	m_remote->label_back->hide();
	m_remote->btn_back->hide();
	m_remote->icon->hide();*/
}

void KexiConnSelectorWidget::setConfirmOverwrites(bool set)
{
	d->confirmOverwrites = set;
	if (fileWidget)
		fileWidget->setConfirmOverwrites( d->confirmOverwrites );
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
	KexiDBConnectionDialog dlg(this, data, QString(),
		KGuiItem(i18n("&Add"), "dialog-ok", i18n("Add database connection")) );
	dlg.setCaption(i18n("Add New Database Connection"));
	if (QDialog::Accepted!=dlg.exec())
		return;

	//store this conn. data
	KexiDB::ConnectionData *newData
		= new KexiDB::ConnectionData(*dlg.currentProjectData().connectionData());
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
	KexiDBConnectionDialog dlg(this, *item->data(), QString(),
		KGuiItem(i18n("&Save"), "document-save", 
			i18n("Save changes made to this database connection")) );
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
	if (KMessageBox::Continue!=KMessageBox::warningContinueCancel(this,
		i18n(
			"Do you want to remove database connection \"%1\" from the list of available connections?",
			item->data()->serverInfoString(true)),
		QString(), //caption
		KStandardGuiItem::del(), KStandardGuiItem::cancel(),
		QString(), //dont'ask name
		KMessageBox::Notify|KMessageBox::Dangerous))
	{
		return;
	}

	Q3ListViewItem* nextItem = item->itemBelow();
	if (!nextItem)
		nextItem = item->itemAbove();
	if (!d->conn_set->removeConnectionData(item->data()))
		return;

	m_remote->list->removeItem(item);
	if (nextItem)
		m_remote->list->setSelected(nextItem, true);
	slotConnectionSelectionChanged();
}

void KexiConnSelectorWidget::hideConnectonIcon()
{
	m_remote->lblIcon->setFixedWidth(0);
	m_remote->lblIcon->setPixmap(QPixmap());
}

#include "KexiConnSelector.moc"
