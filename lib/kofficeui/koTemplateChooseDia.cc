/******************************************************************/
/* KoTemplateChooseDia - (c) by Reginald Stadlbauer 1997-1998	  */
/* Version: 0.1.0						  */
/* Author: Reginald Stadlbauer					  */
/* E-Mail: reggie@kde.org					  */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs			  */
/* needs c++ library Qt (http://www.troll.no)			  */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)	  */
/* needs OpenParts and Kom (weis@kde.org)			  */
/* written for KDE (http://www.kde.org)				  */
/* License: GNU GPL						  */
/******************************************************************/
/* Module: Template Choose Dialog				  */
/******************************************************************/

#include "koTemplateChooseDia.h"
#include "koTemplateChooseDia.moc"

#include <koFilterManager.h>

#include <klocale.h>
#include <kbuttonbox.h>
#include <kfiledialog.h>
#include <klibglobal.h>

#include <qhbox.h>
#include <qvbox.h>
#include <qlayout.h>
#include <kstddirs.h>

/******************************************************************/
/* Class: KoTemplateChooseDia					  */
/******************************************************************/

/*================================================================*/
KoTemplateChooseDia::KoTemplateChooseDia(QWidget *parent,const char *name, const QString& _template_type,
					 KLibGlobal* _global, bool _hasCancel,bool _onlyTemplates,
					 const QString &importFilter, const QString &mimeType )
    : QDialog(parent,name,true), template_type(_template_type), onlyTemplates(_onlyTemplates),
      m_strImportFilter( importFilter ), m_strMimeType( mimeType )
{
    global = _global;
	
    groupList.setAutoDelete(true);
    getGroups();
    setupTabs();

    KButtonBox *bb = new KButtonBox( this );
    bb->addStretch();
    ok = bb->addButton(i18n("OK"));
    connect(ok,SIGNAL(clicked()),this,SLOT(chosen()));
    ok->setDefault(true);
    if (_hasCancel)
	connect(bb->addButton(i18n("Cancel")),SIGNAL(clicked()),this,SLOT(reject()));
    bb->layout();
    bb->setMaximumHeight(bb->sizeHint().height());
    grid->addWidget( bb, 9, 0 );
    grid->setRowStretch( 2, 1 );
	
    templateName = "";
    fullTemplateName = "";
    returnType = Cancel;
}

/*================================================================*/
KoTemplateChooseDia::ReturnType KoTemplateChooseDia::chooseTemplate(const QString& _template_type, KLibGlobal* global,
								    QString &_template,bool _hasCancel, bool _onlyTemplates,
								    const QString &importFilter, const QString &mimeType )
{
    bool res = false;
    KoTemplateChooseDia *dlg = new KoTemplateChooseDia(0,"Template", _template_type, global, _hasCancel,
						       _onlyTemplates, importFilter, mimeType );

    dlg->resize(500,400);
    dlg->setCaption(i18n("Choose a Template"));

    if (dlg->exec() == QDialog::Accepted)
    {
	res = true;
	_template = dlg->getFullTemplate();
    }

    ReturnType rt = dlg->getReturnType();
    delete dlg;

    return res ? rt : KoTemplateChooseDia::Cancel;
}

/*================================================================*/
void KoTemplateChooseDia::getGroups()
{
    QString str;
    char c[256];
	
    QStringList dirs = global->dirs()->resourceDirs(template_type);
    for (QStringList::ConstIterator it = dirs.begin(); it != dirs.end(); it++) {
	QFile templateInf(*it + ".templates");
	if (templateInf.open(IO_ReadOnly)) {
	    while (!templateInf.atEnd())
	    {
		templateInf.readLine(c,256);
		str = c;
		str = str.stripWhiteSpace();
		if (!str.isEmpty())
		{
		    grpPtr = new Group;
		    grpPtr->dir.setFile(*it + QString(c).stripWhiteSpace() + "/");
		    grpPtr->name = QString(c).stripWhiteSpace();
		    groupList.append(grpPtr);
		}
		*c = 0;
		// strcpy(c,"");
	    }
		
	    templateInf.close();
	}
    }

}

/*================================================================*/
void KoTemplateChooseDia::setupTabs()
{
    grid = new QGridLayout( this, 10, 1, 7 ,7 );
	
    QFrame *line;

    if (!onlyTemplates)
    {
	line = new QFrame(this);
	line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	line->setMaximumHeight(20);
	grid->addWidget( line, 0, 0 );
		
	rbTemplates = new QRadioButton(i18n("Create new document from a &Template"),this);
	grid->addWidget( rbTemplates, 1, 0 );
    }



    if (!groupList.isEmpty())
    {
	tabs = new QTabWidget(this);

	for (grpPtr = groupList.first();grpPtr != 0;grpPtr = groupList.next())
	{
	    grpPtr->tab = new QVBox(tabs);
	    grpPtr->loadWid = new MyIconCanvas(grpPtr->tab);
	    grpPtr->loadWid->loadDir(grpPtr->dir.absFilePath(),"*.png");
	    grpPtr->loadWid->setBackgroundColor(colorGroup().base());
	    grpPtr->loadWid->show();
	    connect(grpPtr->loadWid,SIGNAL(nameChanged(const QString &)),
		    this,SLOT(nameChanged(const QString &)));
	    connect(grpPtr->loadWid,SIGNAL(currentChanged(const QString &)),
		    this,SLOT(currentChanged(const QString &)));
	    grpPtr->label = new QLabel(grpPtr->tab);
	    grpPtr->label->setText(" ");
	    grpPtr->label->setMaximumHeight(grpPtr->label->sizeHint().height());
	    tabs->addTab(grpPtr->tab,grpPtr->name);
	}
	connect(tabs,SIGNAL(selected(const QString &)),this,SLOT(tabsChanged(const QString &)));
	grid->addWidget( tabs, 2, 0 );
    }

    line = new QFrame(this);
    line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
    line->setMaximumHeight(20);
    grid->addWidget( line, 3, 0 );
	
    if (!onlyTemplates)
    {
	rbFile = new QRadioButton(i18n("&Open an existing document"),this);
	connect(rbFile,SIGNAL(clicked()),this,SLOT(openFile()));
	grid->addWidget( rbFile, 4, 0 );
		
	QHBox *row = new QHBox(this);
	row->setMargin(5);
	lFile = new QLabel(i18n("No File"),row);
	lFile->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	bFile = new QPushButton(i18n("Choose..."),row);
	lFile->setMaximumHeight(bFile->sizeHint().height());
	bFile->setMaximumSize(bFile->sizeHint());
	row->setMaximumHeight(bFile->sizeHint().height() + 10);
	connect(bFile,SIGNAL(clicked()),this,SLOT(chooseFile()));
	grid->addWidget( row, 5, 0 );
		
	line = new QFrame(this);
	line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	line->setMaximumHeight(20);
	grid->addWidget( line, 6, 0 );
		
	rbEmpty = new QRadioButton(i18n("Start with an &empty document"),this);
	connect(rbEmpty,SIGNAL(clicked()),this,SLOT(openEmpty()));
	grid->addWidget( rbEmpty, 7, 0 );
		
	line = new QFrame(this);
	line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	line->setMaximumHeight(20);
	grid->addWidget( line, 8, 0);
		
	connect(rbTemplates,SIGNAL(clicked()),this,SLOT(openTemplate()));
	rbTemplates->setChecked(true);
    }
}

/*================================================================*/
void KoTemplateChooseDia::nameChanged(const QString &name)
{
    QFileInfo fi(name);

    if (!groupList.isEmpty())
    {
	for (grpPtr=groupList.first();grpPtr != 0;grpPtr=groupList.next())
	{
	    grpPtr->label->setText(fi.baseName());
	    if (grpPtr->label->text().isEmpty())
		grpPtr->label->setText(" ");
	}
    }
}

/*================================================================*/
void KoTemplateChooseDia::currentChanged(const QString &)
{
    openTemplate();
}

/*================================================================*/
void KoTemplateChooseDia::chosen()
{
    if (onlyTemplates || !onlyTemplates && rbTemplates->isChecked())
    {
	returnType = Template;
		
	if (!groupList.isEmpty())
	{
	    for (grpPtr = groupList.first();grpPtr != 0;grpPtr = groupList.next())
	    {
		if (grpPtr->tab->isVisible() && !grpPtr->loadWid->getCurrent().isEmpty())
		{
		    emit templateChosen(QString(grpPtr->name + "/" + grpPtr->loadWid->getCurrent()));
		    templateName = QString(grpPtr->name + "/" + grpPtr->loadWid->getCurrent());
		    fullTemplateName = QString(grpPtr->dir.dirPath(true) + "/" + grpPtr->name + "/" + grpPtr->loadWid->getCurrent());
		    accept();
		}
	    }
	}
    }
    else if (!onlyTemplates && rbFile->isChecked())
    {
	returnType = File;

	QString fileName = lFile->text();
		
	if ( !m_strMimeType.isEmpty() )
	    fileName = KoFilterManager::self()->import( fileName, m_strMimeType );

	fullTemplateName = templateName = fileName;
	accept();
    }
    else if (!onlyTemplates && rbEmpty->isChecked())
    {
	returnType = Empty;
	accept();
    }
    else
    {
	returnType = Cancel;
	reject();
    }
}

/*================================================================*/
void KoTemplateChooseDia::openTemplate()
{
    if (!onlyTemplates)
    {
	rbTemplates->setChecked(true);
	rbFile->setChecked(false);
	rbEmpty->setChecked(false);
    }

    if (isVisible())
	ok->setEnabled(false);

    if (!groupList.isEmpty())
    {
	for (grpPtr = groupList.first();grpPtr != 0;grpPtr = groupList.next())
	{
	    if (grpPtr->tab->isVisible() && grpPtr->loadWid->isCurrentValid())
		ok->setEnabled(true);
	}
    }
}

/*================================================================*/
void KoTemplateChooseDia::openFile()
{
    rbTemplates->setChecked(false);
    rbFile->setChecked(true);
    rbEmpty->setChecked(false);

    ok->setEnabled(QFile::exists(lFile->text()));
}

/*================================================================*/
void KoTemplateChooseDia::openEmpty()
{
    rbTemplates->setChecked(false);
    rbFile->setChecked(false);
    rbEmpty->setChecked(true);

    ok->setEnabled(true);
}

/*================================================================*/
void KoTemplateChooseDia::chooseFile()
{
    openFile();

    QString dir = QString::null;
    if (QFile::exists(lFile->text()))
	dir = QFileInfo(lFile->text()).absFilePath();

    QString filename = KFileDialog::getOpenFileName( dir, m_strImportFilter );
    if (!filename.isEmpty() && QFileInfo(filename).isFile() ||
	(QFileInfo(filename).isSymLink() && !QFileInfo(filename).readLink().isEmpty() &&
	 QFileInfo(QFileInfo(filename).readLink()).isFile()))
	lFile->setText(filename);

    openFile();
}
