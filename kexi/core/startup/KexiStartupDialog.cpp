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

//! we'll remove this later...
#define NO_DB_TEMPLATES

KexiStartupFileDialog::KexiStartupFileDialog(
		const QString& startDir, QString& filter,
		QWidget *parent, const char *name)
	:  KFileDialog(startDir, filter, parent, name, 0) 
{
	toggleSpeedbar(false);
	setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Minimum);
}

KURL KexiStartupFileDialog::currentURL()
{
	setResult( QDialog::Accepted ); // selectedURL tests for it
	return KFileDialog::selectedURL();
}

bool KexiStartupFileDialog::checkURL() {
	bool ok = true;
	KURL url = currentURL();
	if ( url.isLocalFile() ) {
		ok = QFile::exists( url.path() );
		if ( !ok ) {
			// Maybe offer to create a new document with that name? (see alos KoDocument::openFile)
			KMessageBox::error( this, i18n( "The file %1 doesn't exist." ).arg( url.path() ) );
		}
	}
	return ok;
}

void KexiStartupFileDialog::accept() {
	if ( checkURL() )
		KFileDialog::accept();
}

void KexiStartupFileDialog::reject() {
	kdDebug() << "mydialog: reject!" << endl;
	emit cancelClicked();
//	KFileDialog::reject();
}

/*================================================================*/

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
	{
		result = 0;
		QString none, iconname;
		iconname = KMimeType::mimeType("application/x-kexiproject-sqlite")->icon(none,0);
		kexi_sqlite_icon = KGlobal::iconLoader()->loadIcon( iconname, KIcon::Desktop );
		iconname = KMimeType::mimeType("application/x-kexiproject-shortcut")->icon(none,0);
		kexi_shortcut_icon = KGlobal::iconLoader()->loadIcon( iconname, KIcon::Desktop );
		prj_selector = 0;
		chkDoNotShow = 0;
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
//	QVGroupBox* gbTemplDescription;
//	KexiNewFileDBWidget *newfiledb_widget;
//	KexiConnSelector *newserverdb_widget;
	
	QWidgetStack *viewBlankTempl;
	TemplatesPage *viewPersonalTempl;
	TemplatesPage *viewBusinessTempl;
		
	int result;
	
	QPixmap kexi_sqlite_icon, kexi_shortcut_icon;
	
	//! Key string of selected database template. \sa selectedTemplateKey()
	QString selectedTemplateKey;

	//! used for "open existing"
	const KexiDBConnectionSet *connSet;
	QWidgetStack *openExistingWidgetStack;
	KexiStartupFileDialog *openExistingFileDlg; //! embedded file dialog
	KexiConnSelectorWidget *openExistingConnWidget; //! connection selector for opening existing projects
	KexiOpenExistingFile *openExistingFileLabel; //! label on top of "open existing file"
	QString existingFileToOpen; //! helper for returning a file name to open
	const KexiDB::ConnectionData* selectedExistingConnection; //! helper for returning selected connection
		
	//! used for "open recent"
	const KexiProjectSet *recentProjects;
	KexiProjectSelectorWidget* prj_selector;
};
	
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
	const KexiDBConnectionSet& connSet, const KexiProjectSet& recentProjects,
	QWidget *parent, const char *name )
 : KDialogBase( Tabbed, i18n("Choose a project"), Help | Ok | Cancel, Ok, parent, name )
 , d(new KexiStartupDialogPrivate())
{
	d->recentProjects = &recentProjects;
	d->connSet = &connSet;
	d->dialogType = dialogType;
	d->dialogOptions = dialogOptions;
//    QPushButton* ok = actionButton( KDialogBase::Ok );
//    QPushButton* cancel = actionButton( KDialogBase::Cancel );
//    cancel->setAutoDefault(false);
//    ok->setDefault(true);
	KGlobal::iconLoader()->addAppDir("kexi");

//	setIcon(DesktopIcon("filenew"));
	setIcon(d->kexi_sqlite_icon);

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
	}
	if (d->dialogType & OpenRecent) {
		setupPageOpenRecent();
		d->pageOpenRecentID = id++;
	}
	showPage(0);
	adjustSize();
/*
    if (!templateType.isNull() && !templateType.isEmpty() && dialogType!=NoTemplates)
	d->tree=new KoTemplateTree(templateType, global, true);

    d->m_mainwidget=makeMainWidget();

    d->m_templateName = "";
    d->m_fullTemplateName = "";
    d->m_returnType = Cancel;

    setupDialog();*/

	connect(this, SIGNAL(aboutToShowPage(QWidget*)), this, SLOT(tabShown(QWidget*)));
}

KexiStartupDialog::~KexiStartupDialog()
{
    delete d;
}

void KexiStartupDialog::show()
{
	//just some cleanup
	d->selectedTemplateKey=QString::null;
	d->existingFileToOpen=QString::null;
	
	move((qApp->desktop()->width()-width())/2,(qApp->desktop()->height()-height())/2);
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
	
	if (r==QDialog::Rejected)
		d->result = CancelResult;
	else {
		const int idx = activePageIndex();
		if (idx == d->pageTemplatesID) {
			d->result = TemplateResult;
		}
		else if (idx == d->pageOpenExistingID) {
			d->result = OpenExistingResult;
			// return file or connection:
			if (d->openExistingWidgetStack->visibleWidget()==d->openExistingFileLabel) {
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
		d->chkDoNotShow = new QCheckBox(i18n("Do not show this window in future"), d->pageTemplates, "chkDoNotShow");
    	lyr->addWidget(d->chkDoNotShow);
	}

	//template groups:
	QFrame *templPageFrame;
//	QString str_filedb_prj = i18n("Blank File-based Project");
//	QString str_serverdb_prj = i18n("Blank Project on Database Server");
	
	//- page "blank db"
	templPageFrame = d->templatesWidget->addPage (
		i18n("Blank Databases"), i18n("New Blank Database Project"), DesktopIcon("empty") );
	
	QVBoxLayout *tmplyr = new QVBoxLayout(templPageFrame, 0, KDialogBase::spacingHint());
//	d->viewBlankTempl = new QWidgetStack( templPageFrame, "viewBlankTempl" );
//	d->viewBlankTempl->setMargin(0);
//	tmplyr->addWidget( d->viewBlankTempl );
	
	QLabel *lbl_blank = new QLabel( i18n("Kexi will create a new blank database. Click \"OK\" button to proceed."), templPageFrame );
	lbl_blank->setMargin(0);
	tmplyr->addWidget( lbl_blank );
	tmplyr->addStretch(1);
	
/*	d->newfiledb_widget = new KexiNewFileDBWidget( d->viewBlankTempl, "newfiledb_widget" );
	d->newfiledb_widget->icon->setPixmap( d->kexi_sqlite_icon );
	d->newfiledb_widget->btn_advanced->setIconSet( SmallIconSet("1downarrow") );
	connect(d->newfiledb_widget->btn_advanced,SIGNAL(clicked()),this,SLOT(showAdvancedConn()));
	
	d->viewBlankTempl->addWidget( d->newfiledb_widget );
	d->viewBlankTempl->raiseWidget( d->newfiledb_widget );
	if (d->newfiledb_widget->layout())
		d->newfiledb_widget->layout()->setMargin(0);
	
	d->newserverdb_widget = new KexiConnSelector( d->viewBlankTempl, "newserverdb_widget" );
	d->viewBlankTempl->addWidget( d->newserverdb_widget );
	if (d->newserverdb_widget->layout())
		d->newserverdb_widget->layout()->setMargin(0);
	connect(d->newserverdb_widget->btn_back,SIGNAL(clicked()),this,SLOT(showSimpleConn()));
*/
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

/*	QVBoxLayout *tmplyr = new QVBoxLayout(templGroupFrame, KDialogBase::marginHint(), KDialogBase::spacingHint());
	tmplyr->addWidget(
		new QLabel(QString("<P><B>")+i18n("Kexi will create a new database, which will be stored in a file on this computer.")
		 +"</B><P>&nbsp;<P>"
	 	+i18n("Select \"%1\" template, if you want to create a database on remote server instead a file.")
	 	.arg(str_serverdb_prj), templGroupFrame)
	);
	tmplyr->addStretch(1);
		
	templGroupFrame = d->templatesWidget->addPage (
		str_serverdb_prj, str_serverdb_prj, d->kexi_shortcut_icon );
	*/	
/*    // config
    KConfigGroup grp( d->m_global->config(), "TemplateChooserDialog" );
    int templateNum = grp.readEntry( "TemplateTab" ).toInt();
    QString templateName = grp.readEntry( "TemplateName" );

    // item which will be selected initially
    QIconViewItem * itemtoselect = 0;

    // count the templates inserted
    int entriesnumber = 0;

    for ( KoTemplateGroup *group = d->tree->first(); group!=0L; group=d->tree->next() )
    {
	if (group->isHidden())
	    continue;

	QFrame * frame = d->m_jwidget->addPage (
		group->name(),
		group->name(),
		group->first()->loadPicture());

	QGridLayout * layout = new QGridLayout(frame);
	KoTCDIconCanvas *canvas = new KoTCDIconCanvas( frame );
	layout->addWidget(canvas,0,0);

	canvas->setBackgroundColor( colorGroup().base() );
	canvas->setResizeMode(QIconView::Adjust);
	canvas->setWordWrapIconText( true );
	canvas->show();

	QIconViewItem * tempitem = canvas->load(group, templateName);
	if (tempitem)
	    itemtoselect = tempitem;

	canvas->sort();
	canvas->setSelectionMode(QIconView::Single);

	connect( canvas, SIGNAL( clicked ( QIconViewItem * ) ),
		this, SLOT( currentChanged( QIconViewItem * ) ) );

	connect( canvas, SIGNAL( doubleClicked( QIconViewItem * ) ),
		this, SLOT( chosen(QIconViewItem *) ) );

	entriesnumber++;
    }

    d->boxdescription->setInsideMargin ( 3 );
    d->boxdescription->setInsideSpacing ( 3 );

    d->textedit = new KTextEdit( d->boxdescription );
    d->textedit->setReadOnly(1);
    d->textedit->setText(descriptionText(i18n("Empty Document"), i18n("Creates an empty document")));
    d->textedit->setLineWidth(0);
    d->textedit->setMaximumHeight(50);

    // Hide the widget if there is no template available. This should never happen ;-)
    if (!entriesnumber)
	d->m_jwidget->hide();

    // Set the initially shown page, possibly from the last usage of the dialog
    if (entriesnumber >= templateNum)
	d->m_jwidget->showPage(templateNum);

    // Set the initially selected template, possibly from the last usage of the dialog
    currentChanged(itemtoselect);

    // setup the checkbox
    QString translatedstring = i18n("Always start %1 with the selected template").arg(d->m_nativeName);

    d->m_nodiag = new QCheckBox ( translatedstring , widgetbase);
    layout->addWidget(d->m_nodiag, 2, 0);
    QString  startwithoutdialog = grp.readEntry( "NoStartDlg" );
    d->m_nodiag->setChecked( startwithoutdialog == QString("yes") );*/
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
	d->pageOpenExisting = addPage( i18n("Open &Existing Project") );
	QVBoxLayout *lyr = new QVBoxLayout( d->pageOpenExisting, 0, KDialogBase::spacingHint() );

	lyr->addWidget( d->openExistingWidgetStack = new QWidgetStack(d->pageOpenExisting, "openExistingWidgetStack") );
	
	//main label for "open existing file"
	d->openExistingWidgetStack->addWidget( 
		d->openExistingFileLabel = new KexiOpenExistingFile( d->openExistingWidgetStack, "KexiOpenExistingFile") );
	d->openExistingFileLabel->icon->setPixmap( DesktopIcon("fileopen") );
	d->openExistingFileLabel->btn_advanced->setIconSet( SmallIconSet("1downarrow") );
	
	QString filter;
	KMimeType::Ptr mime = KMimeType::mimeType("application/x-kexiproject-sqlite");
	QStringList allfilters;
	if (mime) {
		filter += QString("\n")+ mime->patterns().join(" ") + "|" + mime->comment() + " ("+mime->patterns().join(" ")+")";
		allfilters += mime->patterns().join(" ");
	}
	mime = KMimeType::mimeType("application/x-kexiproject-shortcut");
	if (mime) {
		filter += QString("\n")+ mime->patterns().join(" ") + "|" + mime->comment() + " ("+mime->patterns().join(" ")+")";
		allfilters += mime->patterns().join(" ");
	}
	mime = KMimeType::mimeType("all/allfiles");
	if (mime) {
		filter += QString("\n")+ mime->patterns().join(" ") + "|" + mime->comment()+ " (*)";
	}
	
	filter = allfilters.join(" ")+"|All Kexi Files ("+allfilters.join(" ")+")" + filter;
	QString startdir; //current dir
	static_cast<QVBoxLayout*>(d->openExistingFileLabel->layout())->insertWidget( 
		1, d->openExistingFileDlg = new KexiStartupFileDialog(
			startdir,
			filter, d->openExistingFileLabel/*d->pageOpenExisting*/, "openExistingFileDlg") 
	);
	QPoint point( 0, 0 );
    d->openExistingFileDlg->reparent( d->openExistingFileLabel/*d->pageOpenExisting*/, point );
	
	if (d->openExistingFileDlg->layout())
		d->openExistingFileDlg->layout()->setMargin(0);
	d->openExistingFileDlg->setMinimumHeight(100);
	d->openExistingFileDlg->setMode( KFile::File );
	d->openExistingFileDlg->setSizeGripEnabled ( FALSE );

	//dirty hack to customize filedialog view:
	QObjectList *l = d->openExistingFileDlg->queryList( "QPushButton" );
	QObjectListIt it( *l );
	QObject *obj;
	while ( (obj = it.current()) != 0 ) {
		++it;
		((QPushButton*)obj)->hide();
	}
	delete l;

	connect(d->openExistingFileDlg, SIGNAL(  okClicked() ),
		this, SLOT(accept()));
	connect(d->openExistingFileDlg, SIGNAL( cancelClicked() ),
		this, SLOT(reject()));
	connect(d->openExistingFileDlg, SIGNAL(fileSelected(const QString&)),
		this, SLOT(existingFileSelected(const QString&)));
	connect(d->openExistingFileLabel->btn_advanced,SIGNAL(clicked()),
		this,SLOT(showAdvancedConnForOpenExisting()));

	//! connection selector for opening existing projects
	d->openExistingWidgetStack->addWidget( 
		d->openExistingConnWidget = new KexiConnSelectorWidget(*d->connSet, d->openExistingWidgetStack, "openExistingConnWidget")
	);
	connect(d->openExistingConnWidget->advancedPage()->btn_back,SIGNAL(clicked()),
		this,SLOT(showSimpleConnForOpenExisting()));
	connect(d->openExistingConnWidget, SIGNAL(connectionItemDBLClicked(QListViewItem*)),
		this,SLOT(connectionItemForOpenExistingDBLClicked(QListViewItem*)));
	d->openExistingConnWidget->showAdvancedConn();
	d->openExistingConnWidget->disconnectShowSimpleConnButton();
	d->openExistingConnWidget->advancedPage()->chk_always->hide();
	d->openExistingConnWidget->advancedPage()->label->setText(
		i18n("<b>Select existing database server's connection from the list below and click \"OK\".</b>"
		"<p>You will see existing Kexi projects available for the selected connection."
		"You may also add, edit or remove connections from the list.</p>"));
	d->openExistingConnWidget->advancedPage()->label_back->setText(
		i18n("Click \"Back\" button if you want to just open existing Kexi project file."));
}

void KexiStartupDialog::connectionItemForOpenExistingDBLClicked(QListViewItem *item)
{
	if (!item)
		return;
	accept();
}

void KexiStartupDialog::showSimpleConnForOpenExisting()
{
	kdDebug() << "simple" << endl;
	d->openExistingWidgetStack->raiseWidget(d->openExistingFileLabel);
}

void KexiStartupDialog::showAdvancedConnForOpenExisting()
{
	kdDebug() << "adv" << endl;
	d->openExistingWidgetStack->raiseWidget(d->openExistingConnWidget);
}

QString KexiStartupDialog::selectedExistingFile() const
{
kdDebug() << activePageIndex() << " " << d->openExistingFileDlg->isVisible() << ": " << d->existingFileToOpen << endl;
	return d->existingFileToOpen;
}

const KexiDB::ConnectionData* KexiStartupDialog::selectedExistingConnection()
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
		*d->recentProjects, d->pageOpenRecent, "prj_selector") );
	connect(d->prj_selector,SIGNAL(projectDoubleClicked(const KexiProjectData*)),
		this,SLOT(recentProjectDoubleClicked(const KexiProjectData*)));
}

const KexiProjectData* KexiStartupDialog::selectedProjectData() const
{
	if (activePageIndex()==d->pageOpenRecentID) {
		return d->prj_selector->selectedProjectData();
	}
	return 0;
}

void KexiStartupDialog::recentProjectDoubleClicked(const KexiProjectData *data)
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

/*
// static
KexiStartupDialog::ReturnType KexiStartupDialog::choose(KInstance* global, QString &file,
	const QCString &format, const QString &nativePattern,
	const QString &nativeName,
	const KexiStartupDialog::DialogType &dialogType,
	const QCString& templateType) {

    KexiStartupDialog *dlg = new KexiStartupDialog( 0, "Choose", global, format, nativePattern,
	    nativeName, dialogType, templateType);

    KexiStartupDialog::ReturnType rt = Cancel;

    if (dlg->noStartupDlg())
    {
	// start with the default template
	file = dlg->getFullTemplate();
	rt = dlg->getReturnType();
    }
    else
    {
	dlg->resize( 700, 480 );
	if ( dlg->exec() == QDialog::Accepted )
	{
	    file = dlg->getFullTemplate();
	    rt = dlg->getReturnType();
	}
    }

    delete dlg;
    if ( rt == Cancel && dialogType == Everything )
        // The button says quit, so let's quit
        kapp->quit();

    return rt;
}

bool KexiStartupDialog::noStartupDlg() const {
    return d->m_nostartupdlg;
}


QString KexiStartupDialog::getTemplate() const{
    return d->m_templateName;
}

QString KexiStartupDialog::getFullTemplate() const{
    return d->m_fullTemplateName;
}

KexiStartupDialog::ReturnType KexiStartupDialog::getReturnType() const {
    return d->m_returnType;
}

KexiStartupDialog::DialogType KexiStartupDialog::getDialogType() const {
    return d->m_dialogType;
}

// private
void KexiStartupDialog::setupRecentDialog(QWidget * widgetbase, QGridLayout * layout)
{

        d->m_recent = new KoTCDRecentFilesIconView(widgetbase, "recent files");
        // I prefer the icons to be in "most recent first" order (DF)
        d->m_recent->setSorting( static_cast<QDir::SortSpec>( QDir::Time | QDir::Reversed ) );
        layout->addWidget(d->m_recent,0,0);

        QString oldGroup = d->m_global->config()->group();
        d->m_global->config()->setGroup( "RecentFiles" );

        int i = 0;
        QString value;
        do {
                QString key=QString( "File%1" ).arg( i );
                value=d->m_global->config()->readPathEntry( key );
                if ( !value.isEmpty() ) {
                        KURL url(value);
                        KFileItem *item = new KFileItem( KFileItem::Unknown, KFileItem::Unknown, url );
                        d->m_recent->insertItem(item);
                }
                i++;
        } while ( !value.isEmpty() || i<=10 );

        d->m_global->config()->setGroup( oldGroup );
        d->m_recent->showPreviews();

	connect(d->m_recent, SIGNAL( doubleClicked ( QIconViewItem * ) ),
			this, SLOT( recentSelected( QIconViewItem * ) ) );

}

// private
void KexiStartupDialog::setupFileDialog(QWidget * widgetbase, QGridLayout * layout)
{
    QString dir = QString::null;
    QPoint point( 0, 0 );

    d->m_filedialog=new MyFileDialog(dir,
	    QString::null,
	    widgetbase,
	    "file dialog",
	    false);

    layout->addWidget(d->m_filedialog,0,0);
    d->m_filedialog->reparent( widgetbase , point );
    //d->m_filedialog->setOperationMode( KFileDialog::Opening);

    QObjectList *l = d->m_filedialog->queryList( "QPushButton" );
    QObjectListIt it( *l );
    QObject *obj;
    while ( (obj = it.current()) != 0 ) {
	++it;
	((QPushButton*)obj)->hide();
    }
    delete l;

    d->m_filedialog->setSizeGripEnabled ( FALSE );
    d->m_filedialog->setMimeFilter(
	    KoFilterManager::mimeFilter( d->m_format, KoFilterManager::Import ));

    connect(d->m_filedialog, SIGNAL(  okClicked() ),
	    this, SLOT (  slotOk() ));

    connect(d->m_filedialog, SIGNAL( cancelClicked() ),
	    this, SLOT (  slotCancel() ));

}


// private
void KexiStartupDialog::setupDialog()
{

    QGridLayout *maingrid=new QGridLayout( d->m_mainwidget, 1, 1, 2, 6);
    KConfigGroup grp( d->m_global->config(), "TemplateChooserDialog" );

    if (d->m_dialogType == Everything)
    {

	// the user may want to start with his favorite template
	if (grp.readEntry( "NoStartDlg" ) == QString("yes") )
	{
	    d->m_nostartupdlg = true;
	    d->m_returnType = Empty;

	    // no default template, just start with an empty document
	    if (grp.readEntry("LastReturnType") == QString("Empty") )
		return;

	    // start with the default template
	    d->m_templateName = grp.readEntry( "TemplateName" );
	    d->m_fullTemplateName = grp.readPathEntry( "FullTemplateName" );

	    // be paranoid : invalid template means empty template
	    if (!QFile::exists(d->m_fullTemplateName))
		return;

	    if (d->m_fullTemplateName.length() < 2)
		return;

	    d->m_returnType = Template;
	    return;
	}


	setButtonCancelText(i18n("&Quit"));

	d->tabWidget = new QTabWidget( d->m_mainwidget, "tabWidget" );
	maingrid->addWidget( d->tabWidget, 0, 0 );

	// new document
	d->newTab = new QWidget( d->tabWidget, "newTab" );
	d->tabWidget->insertTab( d->newTab, i18n( "&Create Document" ) );
	QGridLayout * newTabLayout = new QGridLayout( d->newTab, 1, 1, KDialogBase::marginHint(), KDialogBase::spacingHint());

	// existing document
	d->existingTab = new QWidget( d->tabWidget, "existingTab" );
	d->tabWidget->insertTab( d->existingTab, i18n( "Open &Existing Document" ) );
	QGridLayout * existingTabLayout = new QGridLayout( d->existingTab, 1, 1, 0, KDialog::spacingHint());

        // recent document
        d->recentTab = new QWidget( d->tabWidget, "recentTab" );
        d->tabWidget->insertTab( d->recentTab, i18n( "Open &Recent Document" ) );
        QGridLayout * recentTabLayout = new QGridLayout( d->recentTab, 1, 1, KDialogBase::marginHint(), KDialog::spacingHint());

	setupTemplateDialog(d->newTab, newTabLayout);
	setupFileDialog(d->existingTab, existingTabLayout);
	setupRecentDialog(d->recentTab, recentTabLayout);

	QString tabhighlighted = grp.readEntry("LastReturnType");
	if ( tabhighlighted == QString("Template"))
	    d->tabWidget->setCurrentPage(0);
	else
	    d->tabWidget->setCurrentPage(1);
    }
    else
    {
	// open a file
	if (d->m_dialogType == NoTemplates)
	{
	    setupFileDialog(d->m_mainwidget, maingrid);
	}
	// create a new document from a template
	if (d->m_dialogType == OnlyTemplates)
	{
	    setCaption(i18n( "Create Document" ));
	    setupTemplateDialog(d->m_mainwidget, maingrid);
	}
    }
}

// private SLOT
void KexiStartupDialog::currentChanged( QIconViewItem * item)
{
    if (item)
    {
	QIconView* canvas =  item->iconView();

	// set text in the textarea
	d->textedit->setText( descriptionText(
				item->text(),
				((KoTCDIconViewItem *) item)->getDescr()
				));

	// set the icon in the canvas selected
	if (canvas)
	    canvas->setSelected(item,1,0);

	// register the current template
	d->m_templateName = item->text();
	d->m_fullTemplateName = ((KoTCDIconViewItem *) item)->getFName();
    }
}

// private SLOT
void KexiStartupDialog::chosen(QIconViewItem * item)
{
    // the user double clicked on a template
    if (item)
    {
	currentChanged(item);
	slotOk();
    }
}

// private SLOT
void KexiStartupDialog::recentSelected( QIconViewItem * item)
{
	if (item)
	{
		slotOk();
	}
}

// protected SLOT
void KexiStartupDialog::slotOk()
{
    // Collect info from the dialog into d->m_returnType and d->m_templateName etc.
    if (collectInfo())
    {
	// Save it for the next time
	KConfigGroup grp( d->m_global->config(), "TemplateChooserDialog" );
	static const char* const s_returnTypes[] = { 0 , "Template", "File", "Empty" };
	if ( d->m_returnType <= Empty )
	{
	    grp.writeEntry( "LastReturnType", QString::fromLatin1(s_returnTypes[d->m_returnType]) );
	    if (d->m_returnType == Template)
	    {
		grp.writeEntry( "TemplateTab", d->m_jwidget->activePageIndex() );
		grp.writeEntry( "TemplateName", d->m_templateName );
#if KDE_IS_VERSION(3,1,3)
		grp.writePathEntry( "FullTemplateName", d->m_fullTemplateName);
#else
		grp.writeEntry( "FullTemplateName", d->m_fullTemplateName);
#endif
	    }

	    if (d->m_nodiag)
	    {
		if (d->m_nodiag->isChecked())
		    grp.writeEntry( "NoStartDlg", "yes");
		else
		    grp.writeEntry( "NoStartDlg", "no");
	    }
	}
	else
	{
	    kdWarning(30003) << "Unsupported template chooser result: " << d->m_returnType << endl;
	    grp.writeEntry( "LastReturnType", QString::null );
	}
	KDialogBase::slotOk();
    }
}

// private
bool KexiStartupDialog::collectInfo()
{


    // to determine what tab is selected in "Everything" mode
    bool newTabSelected = false;
    if ( d->m_dialogType == Everything)
	if ( d->tabWidget->currentPage() == d->newTab )
	    newTabSelected = true;

    // is it a template or a file ?
    if ( d->m_dialogType==OnlyTemplates || newTabSelected )
    {
	// a template is chosen
	if (d->m_templateName.length() > 0)
	    d->m_returnType = Template;
	else
	    d->m_returnType=Empty;

	return true;
    }
    else if ( d->m_dialogType != OnlyTemplates )
    {
	// a file is chosen
	if (d->m_dialogType == Everything && d->tabWidget->currentPage() == d->recentTab)
	{
		// Recent file
		KFileItem * item = d->m_recent->currentFileItem();
		if (! item)
			return false;
		KURL url = item->url();
		if(url.isLocalFile() && !QFile::exists(url.path()))
		{
			KMessageBox::error( this, i18n( "The file %1 doesn't exist." ).arg( url.path() ) );
			return false;
		}
		d->m_fullTemplateName = url.url();
		d->m_returnType = File;
	}
	else
	{
		// Existing file from file dialog
		KURL url = d->m_filedialog->currentURL();
		d->m_fullTemplateName = url.url();
		d->m_returnType = File;
                return d->m_filedialog->checkURL();
	}
	return true;
    }

    d->m_returnType=Empty;
    return false;
}

//private
QString KexiStartupDialog::descriptionText(const QString &name, const QString &description)
{
	QString descrText(i18n("Name:"));
	descrText += " " + name;
	descrText += "\n";
	descrText += i18n("Description:");
	if (description.isEmpty())
	      descrText += " " + i18n("No description available");
	else
              descrText += " " + description;
	return descrText;
}


QIconViewItem * KoTCDIconCanvas::load( KoTemplateGroup *group, const QString& name)
{
    QIconViewItem * itemtoreturn = 0;

    for (KoTemplate *t=group->first(); t!=0L; t=group->next()) {
	if (t->isHidden())
	    continue;
	QIconViewItem *item = new KoTCDIconViewItem(
		this,
		t->name(),
		t->loadPicture(),
		t->description(),
		t->file());

	if (name == t->name())
	{
	    itemtoreturn = item;
	}

	item->setKey(t->name());
	item->setDragEnabled(false);
	item->setDropEnabled(false);
    }

    return itemtoreturn;
}


KoTCDRecentFilesIconView::~KoTCDRecentFilesIconView()
{
    removeToolTip();
}

void KoTCDRecentFilesIconView::showToolTip( QIconViewItem* item )
{
    removeToolTip();
    if ( !item )
        return;

    // Mostly duplicated from KFileIconView, because it only shows tooltips
    // for truncated icon texts, and we want tooltips on all icons,
    // with the full path...
    // KFileIconView would need a virtual method for deciding if a tooltip should be shown,
    // and another one for deciding what's the text of the tooltip...
    const KFileItem *fi = ( (KFileIconViewItem*)item )->fileInfo();
    QString toolTipText = fi->url().prettyURL( 0, KURL::StripFileProtocol );
    toolTip = new QLabel( QString::fromLatin1(" %1 ").arg(toolTipText), 0,
                          "myToolTip",
                          WStyle_StaysOnTop | WStyle_Customize | WStyle_NoBorder | WStyle_Tool | WX11BypassWM );
    toolTip->setFrameStyle( QFrame::Plain | QFrame::Box );
    toolTip->setLineWidth( 1 );
    toolTip->setAlignment( AlignLeft | AlignTop );
    toolTip->move( QCursor::pos() + QPoint( 14, 14 ) );
    toolTip->adjustSize();
    QRect screen = QApplication::desktop()->screenGeometry(
        QApplication::desktop()->screenNumber(QCursor::pos()));
    if (toolTip->x()+toolTip->width() > screen.right()) {
        toolTip->move(toolTip->x()+screen.right()-toolTip->x()-toolTip->width(), toolTip->y());
    }
    if (toolTip->y()+toolTip->height() > screen.bottom()) {
        toolTip->move(toolTip->x(), screen.bottom()-toolTip->y()-toolTip->height()+toolTip->y());
    }
    toolTip->setFont( QToolTip::font() );
    toolTip->setPalette( QToolTip::palette(), TRUE );
    toolTip->show();
}

void KoTCDRecentFilesIconView::removeToolTip()
{
    delete toolTip;
    toolTip = 0;
}

void KoTCDRecentFilesIconView::hideEvent( QHideEvent *ev )
{
    removeToolTip();
    KFileIconView::hideEvent( ev );
}
*/

#include "KexiStartupDialog.moc"
