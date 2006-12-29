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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KexiNewProjectWizard.h"

#include "KexiConnSelector.h"
#include "KexiConnSelectorBase.h"
#include "KexiNewPrjTypeSelector.h"
#include "KexiOpenExistingFile.h"
#include "KexiDBTitlePage.h"
#include "KexiServerDBNamePage.h"
#include "KexiProjectSelector.h"
#include "kexi.h"

#include <kexiutils/identifier.h>
#include <kexiutils/utils.h>
#include <kexiguimsghandler.h>

#include <kapplication.h>
#include <kiconloader.h>
#include <kmimetype.h>
#include <klocale.h>
#include <kdebug.h>
#include <kconfig.h>
#include <klistview.h>
#include <kurlcombobox.h>
#include <kmessagebox.h>
#include <klineedit.h>

#include <qobjectlist.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qheader.h>

//! @internal
class KexiNewProjectWizardPrivate
{
	public:
	KexiNewProjectWizardPrivate()
	{
		le_dbname_txtchanged_disable = false;
		le_dbname_autofill = true;
//		conndata_to_show = 0;
//		project_set_to_show = 0;
	}
	~KexiNewProjectWizardPrivate()
	{
//		delete conndata_to_show;
//		delete project_set_to_show;
		delete msgHandler;
	}
//	KListView *lv_types;
	KListViewItem *lvi_file, *lvi_server;
	QString chk_file_txt, chk_server_txt; //!< helper

	QString server_db_name_dblist_lbl_txt; //!< helper

	//for displaying db list of the selected conn.
	QGuardedPtr<KexiDB::ConnectionData> conndata_to_show;
	KexiProjectSet *project_set_to_show;

	KexiGUIMessageHandler* msgHandler;

	bool le_dbname_txtchanged_disable : 1;
	bool le_dbname_autofill : 1;
};

KexiNewProjectWizard::KexiNewProjectWizard(KexiDBConnectionSet& conn_set,
	QWidget *parent, const char *name, bool modal, WFlags f)
: KWizard(parent, name, modal, f)
, d(new KexiNewProjectWizardPrivate() )
{
	d->msgHandler = new KexiGUIMessageHandler(this);
	setIcon( DesktopIcon("filenew") );
	setCaption( i18n("Creating New Project") );
	finishButton()->setText(i18n("Create"));

	//page: type selector
	m_prjtype_sel = new KexiNewPrjTypeSelector(this, "KexiNewPrjTypeSelector");
//	lv_types = new KListView(m_prjtype_sel, "types listview");
//	m_prjtype_sel->lv_types->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum, 0, 2));
#if KDE_IS_VERSION(3,3,9)
	m_prjtype_sel->lv_types->setShadeSortColumn(false);
#endif
	m_prjtype_sel->lv_types->header()->hide();
	m_prjtype_sel->lv_types->setSorting(-1);
	m_prjtype_sel->lv_types->setAlternateBackground(QColor()); //disable altering
	m_prjtype_sel->lv_types->setItemMargin( KDialogBase::marginHint() );
	QString none;
	d->lvi_file = new KListViewItem( m_prjtype_sel->lv_types, i18n("New Project Stored in File") );
	d->lvi_file->setPixmap(0, 
		KGlobal::iconLoader()->loadIcon( KMimeType::mimeType( 
			KexiDB::Driver::defaultFileBasedDriverMimeType() )->icon(none,0), KIcon::Desktop 
		)
	);
	d->lvi_file->setMultiLinesEnabled( true );
	d->lvi_server = new KListViewItem( m_prjtype_sel->lv_types, d->lvi_file, 
		i18n("New Project Stored on Database Server") );
	d->lvi_server->setPixmap(0, DesktopIcon("network") );
	d->lvi_server->setMultiLinesEnabled( true );
//	m_prjtype_sel->lv_types->resize(d->m_prjtype_sel->lv_types->width(), d->lvi_file->height()*3);
	m_prjtype_sel->lv_types->setFocus();
//	QString txt_dns = i18n("Don't show me this question again.");
//	d->chk_file_txt = m_prjtype_sel->chk_always->text() +"\n"+txt_dns;
//	d->chk_server_txt = i18n("Always &use database server for creating new projects.")
//		+"\n"+txt_dns;

	connect(m_prjtype_sel->lv_types,SIGNAL(executed(QListViewItem*)),this,SLOT(slotLvTypesExecuted(QListViewItem*)));
	connect(m_prjtype_sel->lv_types,SIGNAL(returnPressed(QListViewItem*)),this,SLOT(slotLvTypesExecuted(QListViewItem*)));
	connect(m_prjtype_sel->lv_types,SIGNAL(selectionChanged( QListViewItem*)),this,SLOT(slotLvTypesSelected(QListViewItem*)));

//	static_cast<QVBoxLayout*>(m_prjtype_sel->layout())->insertWidget(1,d->m_prjtype_sel->lv_types);
//	static_cast<QVBoxLayout*>(m_prjtype_sel->layout())->insertStretch(3,1);
//	updateGeometry();

	addPage(m_prjtype_sel, i18n("Select Storage Method"));
//	d->m_prjtype_sel->lv_types->setMinimumHeight(QMAX(d->lvi_file->height(),d->lvi_server->height())+25);

	//page: db title
	m_db_title = new KexiDBTitlePage(QString::null, this, "KexiDBTitlePage");
	addPage(m_db_title, i18n("Select Project's Caption"));

	//page: connection selector
	m_conn_sel_widget = new QWidget(this);
	QVBoxLayout* conn_sel_lyr = new QVBoxLayout(m_conn_sel_widget);
	QLabel *conn_sel_label = new QLabel(i18n("Enter a new Kexi project's file name:"), m_conn_sel_widget);
	conn_sel_label->setAlignment(Qt::AlignAuto|Qt::AlignTop|Qt::WordBreak);
	conn_sel_lyr->addWidget( conn_sel_label );
	conn_sel_lyr->addSpacing(KDialogBase::spacingHint());

	m_conn_sel = new KexiConnSelectorWidget(conn_set, ":OpenExistingOrCreateNewProject", 
		m_conn_sel_widget, "KexiConnSelectorWidget");
	conn_sel_lyr->addWidget( m_conn_sel );

	//"Select database server connection"
//	m_conn_sel->m_file->btn_advanced->hide();
//	m_conn_sel->m_file->label->hide();
//TODO	m_conn_sel->m_file->lbl->setText( i18n("Enter a new Kexi project's file name:") );
	m_conn_sel->hideHelpers();

	m_conn_sel->m_remote->label->setText(
	 i18n("Select database server's connection you wish to use to create a new Kexi project. "
	 "<p>Here you may also add, edit or remove connections from the list."));
//	m_conn_sel->m_remote->label_back->hide();
//	m_conn_sel->m_remote->btn_back->hide();

	m_conn_sel->showSimpleConn();
	//anyway, db files will be _saved_
	m_conn_sel->m_fileDlg->setMode( KexiStartupFileDialog::SavingFileBasedDB );
//	m_conn_sel->m_fileDlg->setMode( KFile::LocalOnly | KFile::File );
//	m_conn_sel->m_fileDlg->setOperationMode( KFileDialog::Saving );
////js	connect(m_conn_sel->m_fileDlg,SIGNAL(rejected()),this,SLOT(reject()));
//	connect(m_conn_sel->m_fileDlg,SIGNAL(fileHighlighted(const QString&)),this,SLOT(slotFileHighlighted(const QString&)));
	connect(m_conn_sel->m_fileDlg,SIGNAL(accepted()),this,SLOT(accept()));
	m_conn_sel->showAdvancedConn();
	connect(m_conn_sel,SIGNAL(connectionItemExecuted(ConnectionDataLVItem*)),
		this,SLOT(next()));

	addPage(m_conn_sel_widget, i18n("Select Project's Location"));

	//page: server db name
	m_server_db_name = new KexiServerDBNamePage(this, "KexiServerDBNamePage");
	d->server_db_name_dblist_lbl_txt = i18n("Existing project databases on <b>%1</b> database server:");
	connect(m_server_db_name->le_caption, SIGNAL(textChanged(const QString&)),
		this,SLOT(slotServerDBCaptionTxtChanged(const QString&)));
	connect(m_server_db_name->le_dbname, SIGNAL(textChanged(const QString&)),
		this,SLOT(slotServerDBNameTxtChanged(const QString&)));
	connect(m_server_db_name->le_caption, SIGNAL(returnPressed()),
		this,SLOT(accept()));
	connect(m_server_db_name->le_dbname, SIGNAL(returnPressed()),
		this,SLOT(accept()));
	m_server_db_name->le_caption->setText(i18n("New database"));
	m_server_db_name->le_dbname->setValidator(new KexiUtils::IdentifierValidator(this, "id_val"));
	m_project_selector = new KexiProjectSelectorWidget(
		m_server_db_name->frm_dblist, "KexiProjectSelectorWidget", 0, false, false );
	GLUE_WIDGET(m_project_selector, m_server_db_name->frm_dblist);
	m_project_selector->setFocusPolicy(NoFocus);
	m_project_selector->setSelectable(false);

	addPage(m_server_db_name, i18n("Select Project's Caption & Database Name"));

	setFinishEnabled(m_prjtype_sel,false);
	setFinishEnabled(m_db_title,false);
	setFinishEnabled(m_server_db_name,true);

	//finish:
	updateGeometry();
	m_prjtype_sel->lv_types->setSelected(d->lvi_file, true);
}

KexiNewProjectWizard::~KexiNewProjectWizard()
{
	delete d;
}

void KexiNewProjectWizard::show()
{
	KDialog::centerOnScreen(this);
	KWizard::show();
}

void KexiNewProjectWizard::slotLvTypesExecuted(QListViewItem *)
{
	next();
}

void KexiNewProjectWizard::slotLvTypesSelected(QListViewItem *item)
{
/*	if (item==d->lvi_file) {
		m_prjtype_sel->chk_always->setText(d->chk_file_txt);
	}
	else if (item==d->lvi_server) {
		m_prjtype_sel->chk_always->setText(d->chk_server_txt);
	}*/
	setAppropriate( m_db_title, item==d->lvi_file );
	setAppropriate( m_server_db_name, item==d->lvi_server );
}

void KexiNewProjectWizard::showPage(QWidget *page)
{
	if (page==m_prjtype_sel) {//p 1
		m_prjtype_sel->lv_types->setFocus();
		m_prjtype_sel->lv_types->setCurrentItem(m_prjtype_sel->lv_types->currentItem());
	} else if (page==m_db_title) {//p 2
		if (m_db_title->le_caption->text().stripWhiteSpace().isEmpty())
			m_db_title->le_caption->setText(i18n("New database"));
		m_db_title->le_caption->selectAll();
		m_db_title->le_caption->setFocus();
	} else if (page==m_conn_sel_widget) {//p 3
		if (m_prjtype_sel->lv_types->currentItem()==d->lvi_file) {
			m_conn_sel->showSimpleConn();
			QString fn = KexiUtils::string2FileName( m_db_title->le_caption->text() );
			if (!fn.endsWith(".kexi"))
				fn += ".kexi";
			m_conn_sel->m_fileDlg->setLocationText(fn);
			setFinishEnabled(m_conn_sel_widget,true);
			m_conn_sel->setFocus();
		}
		else {
			m_conn_sel->showAdvancedConn();
			setFinishEnabled(m_conn_sel_widget,false);
			m_conn_sel->setFocus();
			m_server_db_name->le_caption->selectAll();
		}
	} else if (page==m_server_db_name) {
		if (m_conn_sel->selectedConnectionData()
		 && (static_cast<KexiDB::ConnectionData*>(d->conndata_to_show) != m_conn_sel->selectedConnectionData())) {
			m_project_selector->setProjectSet(0);
//			delete d->project_set_to_show;
			d->conndata_to_show = 0;
			d->project_set_to_show = new KexiProjectSet(*m_conn_sel->selectedConnectionData(), d->msgHandler);
			if (d->project_set_to_show->error()) {
				delete d->project_set_to_show;
				d->project_set_to_show = 0;
				return;
			}
			d->conndata_to_show = m_conn_sel->selectedConnectionData();
			//-refresh projects list
			m_project_selector->setProjectSet( d->project_set_to_show );
		}
	}
	KWizard::showPage(page);
}

void KexiNewProjectWizard::next()
{
	//let's check if move to next page is allowed:
	if (currentPage()==m_db_title) { //pg 2
		if (m_db_title->le_caption->text().stripWhiteSpace().isEmpty()) {
			KMessageBox::information(this, i18n("Enter project caption."));
			m_db_title->le_caption->setText("");
			m_db_title->le_caption->setFocus();
			return;
		}
	} else if (currentPage()==m_conn_sel_widget) {//p 3
		if (m_prjtype_sel->lv_types->currentItem()==d->lvi_file) {
			//test for db file selection
		}
		else {
			//test for db conn selection
			if (!m_conn_sel->selectedConnectionData()) {
				KMessageBox::information(this, i18n("Select server connection for a new project."));
				return;
			}
			m_project_selector->label->setText(
				d->server_db_name_dblist_lbl_txt.arg(m_conn_sel->selectedConnectionData()->serverInfoString(false)) );
			m_server_db_name->le_caption->setFocus();

		}
	}
	KWizard::next();
}

void KexiNewProjectWizard::accept()
{
	if (m_prjtype_sel->lv_types->currentItem()==d->lvi_file) {//FILE:
		//check if new db file name is ok
		kdDebug() << "********** sender() " << sender()->className() << endl;
		if (sender()==finishButton()) { /*(only if signal does not come from filedialog)*/
			kdDebug() << "********** sender()==finishButton() ********" << endl;
//			if (!m_conn_sel->m_fileDlg->checkURL()) {
			if (!m_conn_sel->m_fileDlg->checkFileName()) {
				return;
			}
		}
	} else {//SERVER:
		//check if we have enough of data
		if (m_server_db_name->le_caption->text().stripWhiteSpace().isEmpty()) {
			KMessageBox::information(this, i18n("Enter project caption."));
			m_server_db_name->le_caption->setText("");
			m_server_db_name->le_caption->setFocus();
			return;
		}
		QString dbname = m_server_db_name->le_dbname->text().stripWhiteSpace();
		if (dbname.isEmpty()) {
			KMessageBox::information(this, i18n("Enter project's database name."));
			m_server_db_name->le_dbname->setText("");
			m_server_db_name->le_dbname->setFocus();
			return;
		}
		//check for duplicated dbname
		if (m_conn_sel->confirmOverwrites() && m_project_selector->projectSet() && m_project_selector->projectSet()
			->findProject( m_server_db_name->le_dbname->text() )) {
			if (KMessageBox::Continue!=KMessageBox::warningContinueCancel( this, "<qt>"
				+i18n("<b>A project with database name \"%1\" already exists</b>"
				"<p>Do you want to delete it and create a new one?")
				.arg( m_server_db_name->le_dbname->text() ), QString::null, KStdGuiItem::del(), 
				QString::null, KMessageBox::Notify|KMessageBox::Dangerous ))
			{
				m_server_db_name->le_dbname->setFocus();
				return;
			}
		}
	}

	KWizard::accept();
}

void KexiNewProjectWizard::done(int r)
{
/*	//save state (always, no matter if dialog is accepted or not)
	KGlobal::config()->setGroup("Startup");
	if (!m_prjtype_sel->chk_always->isChecked())
		KGlobal::config()->deleteEntry("DefaultStorageForNewProjects");
	else if (m_prjtype_sel->lv_types->currentItem()==d->lvi_file)
		KGlobal::config()->writeEntry("DefaultStorageForNewProjects","File");
	else
		KGlobal::config()->writeEntry("DefaultStorageForNewProjects","Server");*/

	KGlobal::config()->sync();
	KWizard::done(r);
}

QString KexiNewProjectWizard::projectDBName() const
{
	if (m_prjtype_sel->lv_types->currentItem()==d->lvi_server)
		return m_server_db_name->le_dbname->text();
	return m_conn_sel->selectedFileName();
}

QString KexiNewProjectWizard::projectCaption() const
{
	if (m_prjtype_sel->lv_types->currentItem()==d->lvi_server) {
		return m_server_db_name->le_caption->text();
	}
	return m_db_title->le_caption->text();
}

KexiDB::ConnectionData* KexiNewProjectWizard::projectConnectionData() const
{
	if (m_prjtype_sel->lv_types->currentItem()==d->lvi_file)
		return 0;
	return m_conn_sel->selectedConnectionData();
}

void KexiNewProjectWizard::slotServerDBCaptionTxtChanged(const QString &capt)
{
	if (m_server_db_name->le_dbname->text().isEmpty())
		d->le_dbname_autofill=true;
	if (d->le_dbname_autofill) {
		d->le_dbname_txtchanged_disable = true;
		QString captionAsId = KexiUtils::string2Identifier(capt);
		m_server_db_name->le_dbname->setText(captionAsId);
		d->le_dbname_txtchanged_disable = false;
	}
}

void KexiNewProjectWizard::slotServerDBNameTxtChanged(const QString &)
{
	if (d->le_dbname_txtchanged_disable)
		return;
	d->le_dbname_autofill = false;
}

/*! If true, user will be asked to accept overwriting existing file. 
 This is true by default. */
void KexiNewProjectWizard::setConfirmOverwrites(bool set)
{
	m_conn_sel->setConfirmOverwrites(set);
}


#include "KexiNewProjectWizard.moc"

