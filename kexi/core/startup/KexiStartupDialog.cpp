/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>
   
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

#include "KexiStartupDialog.h"

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
#include <qobjectlist.h>
#include <qvgroupbox.h>
#include <qapplication.h>
#include <qtooltip.h>
#include <qwidgetstack.h>

#include <klocale.h>
#include <kdeversion.h>
#include <kinstance.h>
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

//! we'll remove this later...
#define NO_DB_TEMPLATES



class TemplateItem : public KIconViewItem
{
	public:
		TemplateItem(KIconView* parent, const QString& name, const QPixmap& icon)
		: KIconViewItem(parent,name,icon)
		{}
		~TemplateItem() {}
		QString key, name, description;
};

TemplatesPage::TemplatesPage( Orientation o, QWidget * parent, const char * name )
	: QSplitter(o, parent, name)
{
	templates = new KIconView(this, "templates");
	templates->setItemsMovable(false);
	templates->setShowToolTips(false);
	info = new KTextBrowser(this,"info");
	setResizeMode(templates,KeepSize);
	setResizeMode(info,KeepSize);

	connect(templates,SIGNAL(selectionChanged(QIconViewItem*)),this,SLOT(itemClicked(QIconViewItem*)));
}

TemplatesPage::~TemplatesPage() {}

void TemplatesPage::addItem(const QString& key, const QString& name, 
	const QString& description, const QPixmap& icon) 
{
	TemplateItem *item = new TemplateItem(templates, name, icon);
	item->key=key;
	item->name=name;
	item->description=description;
}

void TemplatesPage::itemClicked(QIconViewItem *item) {
	if (!item) {
		info->setText("");
		return;
	}
	QString t = QString("<h2>%1</h2><p>%2</p>")
		.arg(static_cast<TemplateItem*>(item)->name)
		.arg(static_cast<TemplateItem*>(item)->description);
#ifdef NO_DB_TEMPLATES
	t += QString("<p>") + i18n("We are sorry, templates are not yet available.") +"</p>";
#endif

	info->setText( t );
}

/*================================================================*/

class KexiStartupDialogPrivate {
public:
	KexiStartupDialogPrivate()
		: pageTemplates(0), pageOpenExisting(0), pageOpenRecent(0)
		, pageTemplatesID(-1), pageOpenExistingID(-1), pageOpenRecentID(-1)
	{
		result = 0;
		QString none, iconname;
		iconname = KMimeType::mimeType("application/x-kexiproject-sqlite")->icon(none,0);
		kexi_sqlite_icon = KGlobal::iconLoader()->loadIcon( iconname, KIcon::Desktop );
		iconname = KMimeType::mimeType("application/x-kexiproject-shortcut")->icon(none,0);
		kexi_shortcut_icon = KGlobal::iconLoader()->loadIcon( iconname, KIcon::Desktop );
		prj_selector = 0;
		chkDoNotShow = 0;
		openExistingConnWidget = 0;
	}
	~KexiStartupDialogPrivate()
	{}

	int dialogType, dialogOptions;

	QFrame *pageTemplates, *pageOpenExisting, *pageOpenRecent;
	int pageTemplatesID, pageOpenExistingID, pageOpenRecentID;

	QCheckBox *chkDoNotShow;
	
	//widgets for template tab:
	KJanusWidget* templatesWidget;
	QObject *templatesWidget_IconListBox;//helper
	
	QWidgetStack *viewBlankTempl;
	TemplatesPage *viewPersonalTempl;
	TemplatesPage *viewBusinessTempl;
		
	int result;
	
	QPixmap kexi_sqlite_icon, kexi_shortcut_icon;
	
	//! Key string of selected database template. \sa selectedTemplateKey()
	QString selectedTemplateKey;

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

QString captionForDialogType(int type)
{
	if (type==KexiStartupDialog::OpenExisting) {
		return I18N_NOOP("Open Existing Project");
	}
	return I18N_NOOP("Choose a project");
}
	
/*================================================================*/
/*KexiStartupDialog::KexiStartupDialog(QWidget *parent, const char *name, KInstance* global,
	const QCString &format, const QString &nativePattern,
	const QString &nativeName, const DialogType &dialogType,
	const QCString& templateType) :
	KDialogBase(parent, name, true, i18n("Open Document"), KDialogBase::Ok | KDialogBase::Cancel,
	KDialogBase::Ok) {
*/
KexiStartupDialog::KexiStartupDialog( 
	int dialogType, int dialogOptions,
	KexiDBConnectionSet& connSet, KexiProjectSet& recentProjects,
	QWidget *parent, const char *name )
 : KDialogBase( 
 	dialogType==OpenExisting ? Plain : Tabbed
	,captionForDialogType(dialogType)
 	,Help | Ok | Cancel, Ok, parent, name )
 , d(new KexiStartupDialogPrivate())
{
	d->recentProjects = &recentProjects;
	d->connSet = &connSet;
	d->dialogType = dialogType;
	d->dialogOptions = dialogOptions;
 	d->singlePage = (dialogType==OpenExisting);
	
	if (dialogType==OpenExisting) {//this dialog has "open" tab only!
		setIcon(DesktopIcon("fileopen"));
	} else {
		setIcon(d->kexi_sqlite_icon);
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
	if (d->dialogType & OpenRecent) {
		setupPageOpenRecent();
		d->pageOpenRecentID = id++;
	}
	showPage(0);
	adjustSize();

	if (!d->singlePage) {
		connect(this, SIGNAL(aboutToShowPage(QWidget*)), this, SLOT(tabShown(QWidget*)));
	}
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
	d->selectedTemplateKey=QString::null;
	d->existingFileToOpen=QString::null;
	
	CENTER_ME;
	KDialogBase::show();
}

int KexiStartupDialog::result() const
{
	return d->result;
}

void KexiStartupDialog::done(int r)
{
	kdDebug() << "KexiStartupDialog::done(" << r << ")" << endl;
	updateSelectedTemplateKeyInfo();
	
	//save settings
	KGlobal::config()->setGroup("Startup");
	if (d->openExistingConnWidget)
		KGlobal::config()->writeEntry("OpenExistingType", (d->openExistingConnWidget->selectedConnectionType() == KexiConnSelectorWidget::FileBased) ? "File" : "Server");
	if (d->chkDoNotShow)
		KGlobal::config()->writeEntry("ShowStartupDialog",!d->chkDoNotShow->isChecked());
	
	KGlobal::config()->sync();
	
	if (r==QDialog::Rejected) {
		d->result = CancelResult;
	} else {
		const int idx = activePageIndex();
		if (idx == d->pageTemplatesID) {
			d->result = TemplateResult;
		}
		else if (idx == d->pageOpenExistingID) {
			d->result = OpenExistingResult;
			// return file or connection:
			if (d->openExistingConnWidget->selectedConnectionType()==KexiConnSelectorWidget::FileBased) {
				d->existingFileToOpen = d->openExistingFileDlg->selectedFile();
				d->selectedExistingConnection = 0;
			} else {
				d->existingFileToOpen = QString::null;
				d->selectedExistingConnection
					= d->openExistingConnWidget->selectedConnectionData();
			}
		}
		else {
			d->result = OpenRecentResult;
		}
	}
	KDialogBase::done(r);
}

void KexiStartupDialog::reject()
{
 	d->result = CancelResult;
	KDialogBase::reject();
}

void KexiStartupDialog::setupPageTemplates()
{
	d->pageTemplates = d->pageTemplates = addPage( i18n("&Create Project") );
	QVBoxLayout *lyr = new QVBoxLayout( d->pageTemplates, 0, KDialogBase::spacingHint() );
	
    d->templatesWidget = new KJanusWidget( 
		d->pageTemplates, "templatesWidget", KJanusWidget::IconList);
	{//aaa! dirty hack
		d->templatesWidget_IconListBox = d->templatesWidget->child(0,"KListBox");
		if (d->templatesWidget_IconListBox)
			d->templatesWidget_IconListBox->installEventFilter(this);
	}
    lyr->addWidget(d->templatesWidget);
	connect(d->templatesWidget, SIGNAL(aboutToShowPage(QWidget*)), this, SLOT(templatesPageShown(QWidget*)));
	
	if (d->dialogOptions & CheckBoxDoNotShowAgain) {
		d->chkDoNotShow = new QCheckBox(i18n("Don't show me this dialog again"), d->pageTemplates, "chkDoNotShow");
    	lyr->addWidget(d->chkDoNotShow);
	}

	//template groups:
	QFrame *templPageFrame;
	
	//- page "blank db"
	templPageFrame = d->templatesWidget->addPage (
		i18n("Blank Databases"), i18n("New Blank Database Project"), DesktopIcon("empty") );
	
	QVBoxLayout *tmplyr = new QVBoxLayout(templPageFrame, 0, KDialogBase::spacingHint());
	
	QLabel *lbl_blank = new QLabel( i18n("Kexi will create a new blank database. Click \"OK\" button to proceed."), templPageFrame );
	lbl_blank->setMargin(0);
	tmplyr->addWidget( lbl_blank );
	tmplyr->addStretch(1);
	
	//- page "personal db"
	templPageFrame = d->templatesWidget->addPage (
		i18n("Personal Databases"), i18n("New Personal Database Project Templates"), DesktopIcon("folder_home") );
	tmplyr = new QVBoxLayout(templPageFrame, 0, KDialogBase::spacingHint());
	d->viewPersonalTempl = new TemplatesPage( Vertical, templPageFrame, "personal_page" );
	tmplyr->addWidget( d->viewPersonalTempl );
	connect(d->viewPersonalTempl->templates,SIGNAL(doubleClicked(QIconViewItem*)),this,SLOT(templateItemExecuted(QIconViewItem*)));
	connect(d->viewPersonalTempl->templates,SIGNAL(returnPressed(QIconViewItem*)),this,SLOT(templateItemExecuted(QIconViewItem*)));
	connect(d->viewPersonalTempl->templates,SIGNAL(currentChanged(QIconViewItem*)),this,SLOT(templateItemSelected(QIconViewItem*)));
	  
	//- page "business db"
	templPageFrame = d->templatesWidget->addPage (
		i18n("Business Databases"), i18n("New Business Database Project Templates"),
		DesktopIcon( "business_user" ));
	tmplyr = new QVBoxLayout(templPageFrame, 0, KDialogBase::spacingHint());
	d->viewBusinessTempl = new TemplatesPage( Vertical, templPageFrame, "business_page" );
	tmplyr->addWidget( d->viewBusinessTempl );
	connect(d->viewBusinessTempl->templates,SIGNAL(doubleClicked(QIconViewItem*)),this,SLOT(templateItemExecuted(QIconViewItem*)));
	connect(d->viewBusinessTempl->templates,SIGNAL(returnPressed(QIconViewItem*)),this,SLOT(templateItemExecuted(QIconViewItem*)));
	connect(d->viewBusinessTempl->templates,SIGNAL(currentChanged(QIconViewItem*)),this,SLOT(templateItemSelected(QIconViewItem*)));
}

void KexiStartupDialog::templatesPageShown(QWidget *page)
{
	int idx = d->templatesWidget->pageIndex(page);
	KIconView *templ = 0;
	if (idx==0) {//blank
//		kdDebug() << "blank" << endl;
	}
	else if (idx==1) {//personal
		templ = d->viewPersonalTempl->templates;
		if (templ->count()==0) {
			//add items (on demand):
			d->viewPersonalTempl->addItem("cd_catalog", i18n("CD Catalog"), 
				i18n("Easy to use database for storing information about your CDs collection."),
				DesktopIcon("cdrom_unmount"));
			d->viewPersonalTempl->addItem("expenses", i18n("Expenses"), 
				i18n("A database for managing your personal expenses."),
				DesktopIcon("kcalc"));
			d->viewPersonalTempl->addItem("image_gallery", i18n("Image Gallery"), 
				i18n("A database for archiving your image collection in a form of gallery."),
				DesktopIcon("icons"));
		}
	}
	else if (idx==2) {//business
		templ = d->viewBusinessTempl->templates;
		if (templ->count()==0) {
			//add items (on demand):
			d->viewBusinessTempl->addItem("address_book", i18n("Address Book"), 
				i18n("A database that offers you a contact information"),
				DesktopIcon("contents"));
		}
	}
	updateDialogOKButton(d->pageTemplates);
}

void KexiStartupDialog::templateItemSelected(QIconViewItem *)
{
	updateDialogOKButton(d->pageTemplates);
}

void KexiStartupDialog::templateItemExecuted(QIconViewItem *item)
{
	if (!item)
		return;
	updateSelectedTemplateKeyInfo();
#ifndef NO_DB_TEMPLATES
	accept();
#endif
}

void KexiStartupDialog::updateSelectedTemplateKeyInfo()
{
	if (activePageIndex()!=d->pageTemplatesID) {//not a 'new db' tab is selected
		d->selectedTemplateKey=QString::null;
		return;
	}
	QIconViewItem *item;
	if (d->templatesWidget->activePageIndex()==0) {
		d->selectedTemplateKey = "blank";
	}
	else if (d->templatesWidget->activePageIndex()==1) {
		item = d->viewPersonalTempl->templates->currentItem();
		if (!item) {
			d->selectedTemplateKey=QString::null;
			return;
		}
		d->selectedTemplateKey=QString("personal/")+static_cast<TemplateItem*>(item)->key;
	}
	else  if (d->templatesWidget->activePageIndex()==2) {
		item = d->viewBusinessTempl->templates->currentItem();
		if (!item) {
			d->selectedTemplateKey=QString::null;
			return;
		}
		d->selectedTemplateKey=QString("business/")+static_cast<TemplateItem*>(item)->key;
	}
}

void KexiStartupDialog::tabShown(QWidget *w)
{
	updateDialogOKButton(w);
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
#ifdef NO_DB_TEMPLATES
		enable = (t_id==0);
#else
		enable = (t_id==0 || (t_id==1 && d->viewPersonalTempl->templates->currentItem()!=0) || (t_id==2 && d->viewBusinessTempl->templates->currentItem()!=0));
#endif
	}
	else if (w==d->pageOpenExisting) {
		enable = !d->openExistingFileDlg->selectedFile().isEmpty();
	}
	else if (w==d->pageOpenRecent) {
		enable = (d->prj_selector->selectedProjectData()!=0);
	}
	enableButton(Ok,enable);
}

QString KexiStartupDialog::selectedTemplateKey() const
{
	return d->selectedTemplateKey;
}

void KexiStartupDialog::setupPageOpenExisting()
{
	if (d->singlePage)
		d->pageOpenExisting = plainPage();
	else
		d->pageOpenExisting = addPage( i18n("Open &Existing Project") );
	QVBoxLayout *lyr = new QVBoxLayout( d->pageOpenExisting, 0, KDialogBase::spacingHint() );

	d->openExistingConnWidget = new KexiConnSelectorWidget(*d->connSet, d->pageOpenExisting, "KexiConnSelectorWidget");
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
}

void KexiStartupDialog::connectionItemForOpenExistingExecuted(ConnectionDataLVItem *item)
{
	if (!item)
		return;
	accept();
}

void KexiStartupDialog::showSimpleConnForOpenExisting()
{
	kdDebug() << "simple" << endl;
	d->openExistingConnWidget->showSimpleConn();
}

void KexiStartupDialog::showAdvancedConnForOpenExisting()
{
	kdDebug() << "adv" << endl;
	d->openExistingConnWidget->showAdvancedConn();
}

QString KexiStartupDialog::selectedExistingFile() const
{
	kdDebug() << activePageIndex() << " " << d->openExistingFileDlg->isVisible() 
		<< ": " << d->existingFileToOpen << endl;
	return d->existingFileToOpen;
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
	d->pageOpenRecent = addPage( i18n("Open &Recent Project") );
	QVBoxLayout *lyr = new QVBoxLayout( d->pageOpenRecent, 0, KDialogBase::spacingHint() );
	lyr->addWidget( d->prj_selector = new KexiProjectSelectorWidget(
		d->pageOpenRecent, "prj_selector", d->recentProjects ) );
	connect(d->prj_selector,SIGNAL(projectExecuted(KexiProjectData*)),
		this,SLOT(recentProjectItemExecuted(KexiProjectData*)));
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
		if (e->type()==QEvent::KeyPress && (static_cast<QKeyEvent*>(e)->key()==Key_Enter || static_cast<QKeyEvent*>(e)->key()==Key_Return)
			|| e->type()==QEvent::MouseButtonDblClick) {
			if (d->templatesWidget->activePageIndex()==0 ) {
				accept();
			}
		}
	}
	return KDialogBase::eventFilter(o,e);
}

// internal reimplementation
int KexiStartupDialog::activePageIndex() const
{
	kdDebug() << "int KexiStartupDialog::activePageIndex() const" << endl;
	if (!d->singlePage)
		return KDialogBase::activePageIndex();
	return 0; //there is always "plain page" #0 selected
}


#include "KexiStartupDialog.moc"

