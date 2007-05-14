/* This file is part of the KDE project
   Copyright (C) 2003-2007 Jaroslaw Staniek <js@iidea.pl>

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

#include "KexiStartupDialog.h"
#include "KexiStartupDialogTemplatesPage.h"
#include "kexi.h"
#include "KexiProjectSelector.h"
#include "KexiOpenExistingFile.h"
#include "KexiConnSelector.h"
#include "KexiConnSelectorBase.h"

#include <qlayout.h>
#include <qtabwidget.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qpoint.h>
#include <qobject.h>
#include <qvgroupbox.h>
#include <qapplication.h>
#include <qtooltip.h>
#include <q3widgetstack.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <QPixmap>
#include <QLabel>
#include <Q3Frame>
#include <QKeyEvent>
#include <QEvent>

#include <klocale.h>
#include <kdeversion.h>
#include <kcomponentdata.h>
#include <kdebug.h>
#include <kpushbutton.h>
#include <kjanuswidget.h>
#include <kglobalsettings.h>
#include <ktextedit.h>
#include <kfileiconview.h>
#include <kfileitem.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kmimetype.h>
#include <ktextbrowser.h>
#include <kconfig.h>

#ifdef KEXI_SHOW_UNIMPLEMENTED
#define KEXI_STARTUP_SHOW_TEMPLATES
#define KEXI_STARTUP_SHOW_RECENT
#endif

//! @internal
class KexiStartupDialogPrivate {
public:
	KexiStartupDialogPrivate()
		: pageTemplates(0), pageOpenExisting(0), pageOpenRecent(0)
		, pageTemplatesID(-1)
		, pageOpenExistingID(-1)
		, pageOpenRecentID(-1)
	{
		result = 0;
		QString none, iconname;
		iconname = KMimeType::mimeType( KexiDB::Driver::defaultFileBasedDriverMimeType() )->icon(none,0);
		kexi_sqlite_icon = KGlobal::iconLoader()->loadIcon( iconname, K3Icon::Desktop );
		iconname = KMimeType::mimeType("application/x-kexiproject-shortcut")->icon(none,0);
		kexi_shortcut_icon = KGlobal::iconLoader()->loadIcon( iconname, K3Icon::Desktop );
		prj_selector = 0;
		chkDoNotShow = 0;
		openExistingConnWidget = 0;
		templatesWidget = 0;
		templatesWidget_IconListBox = 0;
	}
	~KexiStartupDialogPrivate()
	{}

	int dialogType, dialogOptions;

	Q3Frame *pageTemplates, *pageOpenExisting, *pageOpenRecent;
	int pageTemplatesID;
	int pageOpenExistingID, pageOpenRecentID;
	int templatesSectionID_blank, templatesSectionID_import;
#ifdef DB_TEMPLATES
	int templatesSectionID_templates; //, templatesSectionID_custom2;
#endif
	QCheckBox *chkDoNotShow;

	//widgets for template tab:
	KJanusWidget* templatesWidget;
	QObject *templatesWidget_IconListBox;//helper

	Q3WidgetStack *viewBlankTempl;
	KexiStartupDialogTemplatesPage *viewTemplates;
	//TemplatesPage *viewBusinessTempl;

	int result;

	QPixmap kexi_sqlite_icon, kexi_shortcut_icon;

//	//! Key string of selected database template. \sa selectedTemplateKey()
//	QString selectedTemplateKey;

	//! used for "open existing"
	KexiDBConnectionSet *connSet;
	KexiStartupFileDialog *openExistingFileDlg; //! embedded file dialog
	KexiConnSelectorWidget *openExistingConnWidget;
	QString existingFileToOpen; //! helper for returning a file name to open
	KexiDB::ConnectionData* selectedExistingConnection; //! helper for returning selected connection

	//! used for "open recent"
	KexiProjectSet *recentProjects;
	KexiProjectSelectorWidget* prj_selector;

	//! true if the dialog contain single page, not tabs
	bool singlePage : 1;
};

static QString captionForDialogType(int type)
{
	if (type==KexiStartupDialog::Templates)
		return i18n("Create Project");
	else if (type==KexiStartupDialog::OpenExisting)
		return i18n("Open Existing Project");
	else if (type==KexiStartupDialog::OpenRecent)
		return i18n("Open Recent Project");

	return i18n("Choose Project");
}

/*================================================================*/

KexiStartupDialog::KexiStartupDialog(
	int dialogType, int dialogOptions,
	KexiDBConnectionSet& connSet, KexiProjectSet& recentProjects,
	QWidget *parent )
 : KPageDialog( parent )
 , d(new KexiStartupDialogPrivate())
{
	setFaceType(
		(dialogType==KexiStartupDialog::Templates
		|| dialogType==KexiStartupDialog::OpenExisting
		|| dialogType==KexiStartupDialog::OpenRecent)
		? Plain : Tabbed
	);
	setCaption( captionForDialogType(dialogType) );
	setButtons( Help | Ok | Cancel );
	
	d->recentProjects = &recentProjects;
	d->connSet = &connSet;
	d->dialogType = dialogType;
	d->dialogOptions = dialogOptions;
 	d->singlePage = dlgSinglePage(dialogType);

	if (dialogType==OpenExisting) {//this dialog has "open" tab only!
		setWindowIcon(DesktopIcon("document-open"));
	} else {
		setWindowIcon(d->kexi_sqlite_icon);
	}

	setSizeGripEnabled(true);
	int id=0;
	if (d->dialogType & Templates) {
		setupPageTemplates();
		d->pageTemplatesID = id++;
		d->templatesWidget->setFocus();
	}
	if (d->dialogType & OpenExisting) {
		setupPageOpenExisting();
		d->pageOpenExistingID = id++;
		if (d->singlePage)
			d->openExistingConnWidget->setFocus();
	}
#ifdef KEXI_STARTUP_SHOW_RECENT
	if (d->dialogType & OpenRecent) {
		setupPageOpenRecent();
		d->pageOpenRecentID = id++;
		if (d->singlePage)
			d->prj_selector->setFocus();
	}
#endif

	if (!d->singlePage) {
		connect(this, SIGNAL(aboutToShowPage(QWidget*)), this, SLOT(slotPageShown(QWidget*)));
		d->templatesWidget->setFocus();
	}
	showPage(0);
	adjustSize();
}

KexiStartupDialog::~KexiStartupDialog()
{
	delete d;
}

bool KexiStartupDialog::shouldBeShown()
{
	KGlobal::config()->setGroup("Startup");
	return KGlobal::config()->readBoolEntry("ShowStartupDialog",true);
}

void KexiStartupDialog::show()
{
	//just some cleanup
//	d->selectedTemplateKey.clear();
	d->existingFileToOpen.clear();
	d->result=-1;

	KDialog::centerOnScreen(this);
	KDialogBase::show();
}

int KexiStartupDialog::result() const
{
	return d->result;
}

void KexiStartupDialog::done(int r)
{
	if (d->result!=-1) //already done!
		return;

//	kDebug() << "KexiStartupDialog::done(" << r << ")" << endl;
//	updateSelectedTemplateKeyInfo();

	if (r==QDialog::Rejected) {
		d->result = CancelResult;
	} else {
		const int idx = activePageIndex();
		if (idx == d->pageTemplatesID) {
			const int templateIdx = d->templatesWidget->activePageIndex();
			if (templateIdx == d->templatesSectionID_blank)
				d->result = CreateBlankResult;
#ifdef DB_TEMPLATES
			else if (templateIdx == d->templatesSectionID_templates)
				d->result = CreateFromTemplateResult;
#endif
			else if (templateIdx == d->templatesSectionID_import)
				d->result = ImportResult;
		}
		else if (idx == d->pageOpenExistingID) {
			// return file or connection:
			if (d->openExistingConnWidget->selectedConnectionType()==KexiConnSelectorWidget::FileBased) {
				if (!d->openExistingFileDlg->checkFileName())
					return;
				d->existingFileToOpen = d->openExistingFileDlg->currentFileName();
//				d->existingFileToOpen = d->openExistingFileDlg->currentURL().path();
				d->selectedExistingConnection = 0;
			} else {
				d->existingFileToOpen.clear();
				d->selectedExistingConnection
					= d->openExistingConnWidget->selectedConnectionData();
			}
			d->result = OpenExistingResult;
		}
		else {
			d->result = OpenRecentResult;
		}
	}

	//save settings
	KGlobal::config()->setGroup("Startup");
	if (d->openExistingConnWidget)
		KGlobal::config()->writeEntry("OpenExistingType", 
		(d->openExistingConnWidget->selectedConnectionType() == KexiConnSelectorWidget::FileBased) 
			? "File" : "Server");
	if (d->chkDoNotShow)
		KGlobal::config()->writeEntry("ShowStartupDialog",!d->chkDoNotShow->isChecked());

	KGlobal::config()->sync();

	KDialogBase::done(r);
}

void KexiStartupDialog::reject()
{
//	d->result = CancelResult;
	KDialogBase::reject();
}

void KexiStartupDialog::setupPageTemplates()
{
	d->pageTemplates = addPage( i18n("&Create Project") );
	Q3VBoxLayout *lyr = new Q3VBoxLayout( d->pageTemplates, 0, KDialogBase::spacingHint() );

	d->templatesWidget = new KJanusWidget(
		d->pageTemplates, "templatesWidget", KJanusWidget::IconList);
	{//aaa! dirty hack
		d->templatesWidget_IconListBox = d->templatesWidget->child(0,"KListBox");
		if (d->templatesWidget_IconListBox)
			d->templatesWidget_IconListBox->installEventFilter(this);
	}
	lyr->addWidget(d->templatesWidget);
	connect(d->templatesWidget, SIGNAL(aboutToShowPage(QWidget*)), this, SLOT(slotPageShown(QWidget*)));

	if (d->dialogOptions & CheckBoxDoNotShowAgain) {
		d->chkDoNotShow = new QCheckBox(i18n("Do not show me this dialog again"), d->pageTemplates, "chkDoNotShow");
		lyr->addWidget(d->chkDoNotShow);
	}

	//template groups:
	Q3Frame *templPageFrame;
	Q3VBoxLayout *tmplyr;
	int itemID = 0; //used just to set up templatesSectionID_*

	//- page "blank db"
	d->templatesSectionID_blank = itemID++;
	QString clickMsg( "\n\n" + i18n("Click \"OK\" button to proceed.") );
	templPageFrame = d->templatesWidget->addPage(
		i18n("Blank Database"), i18n("New Blank Database Project"), DesktopIcon("empty") );
	tmplyr = new Q3VBoxLayout(templPageFrame, 0, KDialogBase::spacingHint());
	QLabel *lbl_blank = new QLabel( 
		i18n("Kexi will create a new blank database project.")+clickMsg, templPageFrame );
	lbl_blank->setAlignment(Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap);
	lbl_blank->setMargin(0);
	tmplyr->addWidget( lbl_blank );
	tmplyr->addStretch(1);

#ifdef DB_TEMPLATES
	//- page "templates"
	d->templatesSectionID_templates = itemID++;
	QString none;
	QString kexi_sqlite_icon_name 
		= KMimeType::mimeType( KexiDB::Driver::defaultFileBasedDriverMimeType() )->icon(none,0);
	templPageFrame = d->templatesWidget->addPage (
		i18nc("Keep this text narrow: split to multiple rows if needed", "Create From\nTemplate"), 
		i18n("New Database Project From Template"), DesktopIcon(kexi_sqlite_icon_name) );
	tmplyr = new Q3VBoxLayout(templPageFrame, 0, KDialogBase::spacingHint());
	QLabel *lbl_templ = new QLabel( 
		i18n("Kexi will create a new database project using selected template.\n"
		"Select template and click \"OK\" button to proceed."), templPageFrame );
	lbl_templ->setAlignment(Qt::AlignAuto|Qt::AlignTop|Qt::WordBreak);
	lbl_templ->setMargin(0);
	tmplyr->addWidget( lbl_templ );

	d->viewTemplates = new KexiStartupDialogTemplatesPage( templPageFrame );
	tmplyr->addWidget( d->viewTemplates );
	connect(d->viewTemplates,SIGNAL(selected(const QString&)),
		this,SLOT(templateSelected(const QString&)));
/*	connect(d->viewTemplates->templates,SIGNAL(returnPressed(QIconViewItem*)),
		this,SLOT(templateItemExecuted(QIconViewItem*)));
	connect(d->viewTemplates->templates,SIGNAL(currentChanged(QIconViewItem*)),
		this,SLOT(templateItemSelected(QIconViewItem*)));*/
/*later
	templPageFrame = d->templatesWidget->addPage (
		i18n("Personal Databases"), i18n("New Personal Database Project Templates"), DesktopIcon("user-home") );
	tmplyr = new QVBoxLayout(templPageFrame, 0, KDialogBase::spacingHint());
	d->viewPersonalTempl = new TemplatesPage( Vertical, templPageFrame, "personal_page" );
	tmplyr->addWidget( d->viewPersonalTempl );
	connect(d->viewPersonalTempl->templates,SIGNAL(doubleClicked(QIconViewItem*)),this,SLOT(templateItemExecuted(QIconViewItem*)));
	connect(d->viewPersonalTempl->templates,SIGNAL(returnPressed(QIconViewItem*)),this,SLOT(templateItemExecuted(QIconViewItem*)));
	connect(d->viewPersonalTempl->templates,SIGNAL(currentChanged(QIconViewItem*)),this,SLOT(templateItemSelected(QIconViewItem*)));
*/

	//- page "business db"
/*later
	d->templatesSectionID_custom2 = itemID++;
	templPageFrame = d->templatesWidget->addPage (
		i18n("Business Databases"), i18n("New Business Database Project Templates"),
		DesktopIcon( "business_user" ));
	tmplyr = new QVBoxLayout(templPageFrame, 0, KDialogBase::spacingHint());
	d->viewBusinessTempl = new TemplatesPage( Vertical, templPageFrame, "business_page" );
	tmplyr->addWidget( d->viewBusinessTempl );
	connect(d->viewBusinessTempl->templates,SIGNAL(doubleClicked(QIconViewItem*)),this,SLOT(templateItemExecuted(QIconViewItem*)));
	connect(d->viewBusinessTempl->templates,SIGNAL(returnPressed(QIconViewItem*)),this,SLOT(templateItemExecuted(QIconViewItem*)));
	connect(d->viewBusinessTempl->templates,SIGNAL(currentChanged(QIconViewItem*)),this,SLOT(templateItemSelected(QIconViewItem*)));
*/
#endif //DB_TEMPLATES

	//- page "import db"
	d->templatesSectionID_import = itemID++;
	templPageFrame = d->templatesWidget->addPage(
		i18n("Import Existing\nDatabase"), i18n("Import Existing Database as New Database Project"), 
		DesktopIcon("database_import") );
	tmplyr = new Q3VBoxLayout(templPageFrame, 0, KDialogBase::spacingHint());
	QLabel *lbl_import = new QLabel( 
		i18n("Kexi will import the structure and data of an existing database as a new database project.")
		+clickMsg, templPageFrame );
	lbl_import->setAlignment(Qt::AlignAuto|Qt::AlignTop|Qt::WordBreak);
	lbl_import->setMargin(0);
	tmplyr->addWidget( lbl_import );
	tmplyr->addStretch(1);
}

void KexiStartupDialog::slotPageShown(QWidget *page)
{
	int idx = d->templatesWidget->pageIndex(page);
//	K3IconView *templ = 0;
	if (idx==d->templatesSectionID_blank) {//blank
//		kDebug() << "blank" << endl;
	}
	else if (idx==d->templatesSectionID_import) {
	}
#ifdef DB_TEMPLATES
	else if (idx==d->templatesSectionID_templates) {
		d->viewTemplates->populate();
	}
/*later?		KIconView *templ = d->viewTemplates->templates;
		if (templ->count()==0) {
			//add items (on demand):
			d->viewTemplates->addItem("cd_catalog", i18n("CD Catalog"),
				i18n("Easy-to-use database for storing information about your CD collection."),
				DesktopIcon("cdrom_unmount"));
			d->viewTemplates->addItem("expenses", i18n("Expenses"),
				i18n("A database for managing your personal expenses."),
				DesktopIcon("kcalc"));
			d->viewTemplates->addItem("image_gallery", i18n("Image Gallery"),
				i18n("A database for archiving your image collection in a form of gallery."),
				DesktopIcon("icons"));
		}
	}
	else if (idx==d->templatesSectionID_custom2) {//business
		templ = d->viewBusinessTempl->templates;
		if (templ->count()==0) {
			//add items (on demand):
			d->viewBusinessTempl->addItem("address_book", i18n("Address Book"),
				i18n("A database that offers you a contact information"),
				DesktopIcon("help-contents"));
		}
	}*/
#endif
	updateDialogOKButton(d->pageTemplates);
}

#if 0
void KexiStartupDialog::templateItemSelected(Q3IconViewItem *)
{
	updateDialogOKButton(d->pageTemplates);
}

void KexiStartupDialog::templateItemExecuted(Q3IconViewItem *item)
{
	if (!item)
		return;
//	updateSelectedTemplateKeyInfo();
#ifdef DB_TEMPLATES
	accept();
#endif
}

void KexiStartupDialog::updateSelectedTemplateKeyInfo()
{
	if (activePageIndex()!=d->pageTemplatesID) {//not a 'new db' tab is selected
		d->selectedTemplateKey.clear();
		return;
	}
	Q3IconViewItem *item;
	if (d->templatesWidget->activePageIndex()==d->templatesSectionID_blank) {
		d->selectedTemplateKey = "blank";
	}
	else if (d->templatesWidget->activePageIndex()==d->templatesSectionID_import) {
		d->selectedTemplateKey = "import";
	}
#ifdef DB_TEMPLATES
	else if (d->templatesWidget->activePageIndex()==d->templatesSectionID_templates) {
		item = d->viewTemplates->templates->currentItem();
		if (!item) {
			d->selectedTemplateKey.clear();
			return;
		}
		d->selectedTemplateKey=QString("personal/")+static_cast<TemplateItem*>(item)->key;
	}
/*later?
	else  if (d->templatesWidget->activePageIndex()==d->templatesSectionID_custom2) {
		item = d->viewBusinessTempl->templates->currentItem();
		if (!item) {
			d->selectedTemplateKey.clear();
			return;
		}
		d->selectedTemplateKey=QString("business/")+static_cast<TemplateItem*>(item)->key;
	}*/
#endif
}
#endif // 0

void KexiStartupDialog::tabShown(QWidget *w)
{
//	kDebug() << "KexiStartupDialog::tabShown " << (long)w << " "<< long(d->pageTemplates)<<endl;

	updateDialogOKButton(w);

	if (w==d->pageOpenExisting) {
		d->openExistingConnWidget->setFocus();
	}
}

void KexiStartupDialog::updateDialogOKButton(QWidget *w)
{
	if (!w) {
		int idx = activePageIndex();
		if (idx==d->pageTemplatesID)
			w = d->pageTemplates;
		else if (idx==d->pageOpenExistingID)
			w = d->pageOpenExisting;
		else if (idx==d->pageOpenRecentID)
			w = d->pageOpenRecent;

		if (!w)
			return;
	}
	bool enable = true;
	if (w==d->pageTemplates) {
		int t_id = d->templatesWidget->activePageIndex();
#ifdef DB_TEMPLATES
		enable = (t_id==d->templatesSectionID_blank || d->templatesSectionID_import
			|| (t_id==d->templatesSectionID_templates && !d->viewTemplates->selectedFileName().isEmpty()));
#else
		enable = (t_id==d->templatesSectionID_blank || d->templatesSectionID_import);
#endif
	}
	else if (w==d->pageOpenExisting) {
//		enable = !d->openExistingFileDlg->currentURL().path().isEmpty();
		enable = 
			(d->openExistingConnWidget->selectedConnectionType()==KexiConnSelectorWidget::FileBased)
			? !d->openExistingFileDlg->currentFileName().isEmpty()
			: (bool)d->openExistingConnWidget->selectedConnectionData();
	}
	else if (w==d->pageOpenRecent) {
		enable = (d->prj_selector->selectedProjectData()!=0);
	}
	enableButton(Ok,enable);
}

/*QString KexiStartupDialog::selectedTemplateKey() const
{
	return d->selectedTemplateKey;
}*/

void KexiStartupDialog::setupPageOpenExisting()
{
	if (d->singlePage)
		d->pageOpenExisting = plainPage();
	else
		d->pageOpenExisting = addPage( i18n("Open &Existing Project") );
	Q3VBoxLayout *lyr = new Q3VBoxLayout( d->pageOpenExisting, 0, KDialogBase::spacingHint() );

	d->openExistingConnWidget = new KexiConnSelectorWidget(*d->connSet, 
		":OpenExistingOrCreateNewProject",
		d->pageOpenExisting, "KexiConnSelectorWidget");
	d->openExistingConnWidget->hideConnectonIcon();
	lyr->addWidget( d->openExistingConnWidget );
	if (KGlobal::config()->readEntry("OpenExistingType","File")=="File")
		d->openExistingConnWidget->showSimpleConn();
	else {
		d->openExistingConnWidget->showSimpleConn();
		d->openExistingConnWidget->showAdvancedConn();
	}
	d->openExistingFileDlg = d->openExistingConnWidget->m_fileDlg;
	connect(d->openExistingFileDlg,SIGNAL(accepted()),this,SLOT(accept()));
	connect(d->openExistingConnWidget,SIGNAL(connectionItemExecuted(ConnectionDataLVItem*)),
		this,SLOT(connectionItemForOpenExistingExecuted(ConnectionDataLVItem*)));
	connect(d->openExistingConnWidget,SIGNAL(connectionItemHighlighted(ConnectionDataLVItem*)),
		this,SLOT(connectionItemForOpenExistingHighlighted(ConnectionDataLVItem*)));
}

void KexiStartupDialog::connectionItemForOpenExistingExecuted(ConnectionDataLVItem *item)
{
	if (!item)
		return;
	accept();
}

void KexiStartupDialog::connectionItemForOpenExistingHighlighted(ConnectionDataLVItem *item)
{
	actionButton(KDialogBase::Ok)->setEnabled(item);
}

void KexiStartupDialog::slotOk() {
//	kDebug()<<"KexiStartupDialog::slotOk()"<<endl;
	if (activePageIndex()==d->pageOpenExistingID) {
		if (d->openExistingFileDlg) {
			if (d->openExistingFileDlg->okButton())
				d->openExistingFileDlg->okButton()->animateClick();
//			return;
		}
	}
	KDialogBase::slotOk();
}

void KexiStartupDialog::showSimpleConnForOpenExisting()
{
//	kDebug() << "simple" << endl;
	d->openExistingConnWidget->showSimpleConn();
}

void KexiStartupDialog::showAdvancedConnForOpenExisting()
{
//	kDebug() << "adv" << endl;
	d->openExistingConnWidget->showAdvancedConn();
}

QString KexiStartupDialog::selectedFileName() const
{
	if (d->result == OpenExistingResult)
		return d->existingFileToOpen;
	else if (d->result == CreateFromTemplateResult && d->viewTemplates)
		return d->viewTemplates->selectedFileName();
	else
		return QString();
}

KexiDB::ConnectionData* KexiStartupDialog::selectedExistingConnection() const
{
	return d->selectedExistingConnection;
}

void KexiStartupDialog::existingFileSelected(const QString &f)
{
	if (f.isEmpty())
		return;
	d->existingFileToOpen=f;
	updateDialogOKButton(d->openExistingFileDlg);
}

void KexiStartupDialog::setupPageOpenRecent()
{
#ifdef KEXI_STARTUP_SHOW_RECENT
	d->pageOpenRecent = addPage( i18n("Open &Recent Project") );
	Q3VBoxLayout *lyr = new Q3VBoxLayout( d->pageOpenRecent, 0, KDialogBase::spacingHint() );
	lyr->addWidget( d->prj_selector = new KexiProjectSelectorWidget(
		d->pageOpenRecent, "prj_selector", d->recentProjects ) );
	connect(d->prj_selector,SIGNAL(projectExecuted(KexiProjectData*)),
		this,SLOT(recentProjectItemExecuted(KexiProjectData*)));
#endif
}

KexiProjectData* KexiStartupDialog::selectedProjectData() const
{
	if (activePageIndex()==d->pageOpenRecentID) {
		return d->prj_selector->selectedProjectData();
	}
	return 0;
}

void KexiStartupDialog::recentProjectItemExecuted(KexiProjectData *data)
{
	updateDialogOKButton(d->pageOpenRecent);
	if (!data)
		return;
	accept();
}

//! used for accepting templates dialog with just return key press
bool KexiStartupDialog::eventFilter( QObject *o, QEvent *e )
{
	if (o==d->templatesWidget_IconListBox && d->templatesWidget_IconListBox) {
		if (e->type()==QEvent::KeyPress 
			&& (static_cast<QKeyEvent*>(e)->key()==Qt::Key_Enter || static_cast<QKeyEvent*>(e)->key()==Qt::Key_Return)
			|| e->type()==QEvent::MouseButtonDblClick) 
		{
			const int t_id = d->templatesWidget->activePageIndex();
			if (t_id==d->templatesSectionID_blank || t_id==d->templatesSectionID_import) {

				accept();
			}
		}
	}
	return KDialogBase::eventFilter(o,e);
}

// internal reimplementation
int KexiStartupDialog::activePageIndex() const
{
	if (!d->singlePage) {
//		kDebug() << "int KexiStartupDialog::activePageIndex()" << KDialogBase::activePageIndex() << endl;
		return KDialogBase::activePageIndex();
	}
	kDebug() << "int KexiStartupDialog::activePageIndex() == " << 0 << endl;
	return 0; //there is always "plain page" #0 selected
}

void KexiStartupDialog::templateSelected(const QString& fileName)
{
	if (!fileName.isEmpty())
		accept();
}

Q3ValueList<KexiProjectData::ObjectInfo> KexiStartupDialog::autoopenObjects() const
{
	if (d->result != CreateFromTemplateResult || !d->viewTemplates)
		Q3ValueList<KexiProjectData::ObjectInfo>();

	return d->viewTemplates->autoopenObjectsForSelectedTemplate();
}

#include "KexiStartupDialog.moc"
