/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
                 2000 Werner Trobin <wtrobin@mandrakesoft.com>

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

// Description: Template Choose Dialog

/******************************************************************/

#include <qfile.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qtabwidget.h>
#include <qradiobutton.h>

#include <klocale.h>
#include <kbuttonbox.h>
#include <kurl.h>
#include <kfiledialog.h>
#include <kinstance.h>
#include <kstddirs.h>

#include "koTemplateChooseDia.h"


class KoTemplateChooseDiaPrivate {
public:
    KoTemplateChooseDiaPrivate(const QString& templateType, KInstance* global,
			       const QString &importFilter, 
			       const KoTemplateChooseDia::DialogType &dialogType) :
	m_templateType(templateType), m_global(global), m_strImportFilter(importFilter),
	m_dialogType(dialogType), m_firstTime(true) {
    }
    ~KoTemplateChooseDiaPrivate() {}

    struct Group {
	QFileInfo m_dir;
	QString m_name;
	QWidget *m_tab;
	MyIconCanvas *m_loadWid;
	QLabel *m_label;
    };

    QString m_templateType;
    KInstance* m_global;
    QString m_strImportFilter;
    KoTemplateChooseDia::DialogType m_dialogType;
    bool m_firstTime;

    QString m_templateName;
    QString m_fullTemplateName;
    QList<Group> m_groupList;
    Group *m_grpPtr;
    QRadioButton *m_rbTemplates;
    QRadioButton *m_rbFile;
    QRadioButton *m_rbEmpty;
    QLabel *m_lFile;
    QPushButton *m_bFile;
    QPushButton *m_ok;
    QTabWidget *m_tabs;
    KoTemplateChooseDia::ReturnType m_returnType;
    QGridLayout *m_grid;
};

/******************************************************************/
/* Class: KoTemplateChooseDia					  */
/******************************************************************/

/*================================================================*/
KoTemplateChooseDia::KoTemplateChooseDia(QWidget *parent, const char *name, KInstance* global,
					 const QString &importFilter, const DialogType &dialogType,
					 const QString& templateType, bool hasCancel) :
    KDialog(parent, name, true) {

    d=new KoTemplateChooseDiaPrivate(templateType, global, importFilter, dialogType);

    d->m_groupList.setAutoDelete(true);
    if(!templateType.isNull() && !templateType.isEmpty() && dialogType!=NoTemplates)
	getGroups();

    KButtonBox *bb=new KButtonBox(this);
    bb->addStretch();
    d->m_ok=bb->addButton(i18n("OK"));
    connect( d->m_ok, SIGNAL( clicked() ), this, SLOT( chosen() ) );
    d->m_ok->setDefault( true );
    d->m_ok->setEnabled( false );
    if ( hasCancel )
	connect( bb->addButton( i18n( "Cancel" ) ), SIGNAL( clicked() ), this, SLOT( reject() ) );
    bb->layout();
    bb->setMaximumHeight( bb->sizeHint().height() );
    setupTabs();
    d->m_grid->addWidget( bb, 9, 0 );
    d->m_grid->setRowStretch( 2, 1 );

    d->m_templateName = "";
    d->m_fullTemplateName = "";
    d->m_returnType = Cancel;
}

KoTemplateChooseDia::~KoTemplateChooseDia() {
    delete d;
    d=0L;
}

/*================================================================*/
KoTemplateChooseDia::ReturnType KoTemplateChooseDia::choose(KInstance* global, QString &file,
							    const QString &importFilter,
							    const KoTemplateChooseDia::DialogType &dialogType,
							    const QString& templateType, bool hasCancel) {
    bool res = false;
    KoTemplateChooseDia *dlg = new KoTemplateChooseDia( 0, "Choose", global, importFilter, 
							dialogType, templateType, hasCancel);
    dlg->resize( 500, 400 );
    dlg->setCaption( i18n( "Choose" ) );

    if ( dlg->exec() == QDialog::Accepted ) {
	res = true;
	file = dlg->getFullTemplate();
    }

    KoTemplateChooseDia::ReturnType rt = dlg->getReturnType();
    delete dlg;

    return res ? rt : KoTemplateChooseDia::Cancel;
}

QString KoTemplateChooseDia::getTemplate() {
    return d->m_templateName;
}

QString KoTemplateChooseDia::getFullTemplate() {
    return d->m_fullTemplateName;
}

KoTemplateChooseDia::ReturnType KoTemplateChooseDia::getReturnType() {
    return d->m_returnType;
}

KoTemplateChooseDia::DialogType KoTemplateChooseDia::getDialogType() {
    return d->m_dialogType;
}

/*================================================================*/
void KoTemplateChooseDia::getGroups()
{
    QString str;
    char c[256];

    QStringList dirs = d->m_global->dirs()->resourceDirs(d->m_templateType);
    for(QStringList::ConstIterator it=dirs.begin(); it!=dirs.end(); ++it) {
	QFile templateInf(*it + ".templates");
	if(templateInf.open(IO_ReadOnly)) {
	    while(!templateInf.atEnd()) {
		templateInf.readLine(c, 256);
		str=c;
		str=str.stripWhiteSpace();
		if(!str.isEmpty())
		{
		    d->m_grpPtr=new KoTemplateChooseDiaPrivate::Group;
		    d->m_grpPtr->m_dir.setFile(*it+QString(c).stripWhiteSpace()+"/");
		    d->m_grpPtr->m_name=QString(c).stripWhiteSpace();
		    d->m_groupList.append(d->m_grpPtr);
		}
		*c=0;
	    }
	    templateInf.close();
	}
    }
}

/*================================================================*/
void KoTemplateChooseDia::setupTabs()
{
    d->m_grid = new QGridLayout( this, 10, 1, 7 , 7 );
    d->m_grid->setMargin(KDialog::marginHint());
    d->m_grid->setSpacing(KDialog::spacingHint());

    QFrame *line;

    if ( d->m_dialogType!=OnlyTemplates && d->m_dialogType!=NoTemplates ) {
	line = new QFrame( this );
	line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
	line->setMaximumHeight( 20 );
	d->m_grid->addWidget( line, 0, 0 );

	d->m_rbTemplates = new QRadioButton( i18n( "Create new document from a &Template" ), this );
	d->m_grid->addWidget( d->m_rbTemplates, 1, 0 );
    }

    if ( !d->m_groupList.isEmpty() && d->m_dialogType!=NoTemplates ) {
	d->m_tabs = new QTabWidget( this );

	for ( d->m_grpPtr = d->m_groupList.first();d->m_grpPtr != 0;d->m_grpPtr = d->m_groupList.next() ) {
	    d->m_grpPtr->m_tab = new QVBox( d->m_tabs );
	    d->m_grpPtr->m_loadWid = new MyIconCanvas( d->m_grpPtr->m_tab );
	    d->m_grpPtr->m_loadWid->loadDir( d->m_grpPtr->m_dir.absFilePath(), "*.png" );
	    d->m_grpPtr->m_loadWid->setBackgroundColor( colorGroup().base() );
	    d->m_grpPtr->m_loadWid->show();
	    connect( d->m_grpPtr->m_loadWid, SIGNAL( doubleClicked( QIconViewItem * ) ),
		     this, SLOT( chosen() ) );
	    connect( d->m_grpPtr->m_loadWid, SIGNAL( currentChanged( const QString & ) ),
		    this, SLOT( currentChanged( const QString & ) ) );
	    d->m_grpPtr->m_label = new QLabel( d->m_grpPtr->m_tab );
	    d->m_grpPtr->m_label->setText( " " );
	    d->m_grpPtr->m_label->setMaximumHeight( d->m_grpPtr->m_label->sizeHint().height() );
	    d->m_tabs->addTab( d->m_grpPtr->m_tab, d->m_grpPtr->m_name );
	}
	connect( d->m_tabs, SIGNAL( selected( const QString & ) ), this, SLOT( tabsChanged( const QString & ) ) );
	d->m_grid->addWidget( d->m_tabs, 2, 0 );
    }

    line = new QFrame( this );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMaximumHeight( 20 );
    d->m_grid->addWidget( line, 3, 0 );

    if ( d->m_dialogType!=OnlyTemplates ) {
	d->m_rbFile = new QRadioButton( i18n( "&Open an existing document" ), this );
	connect( d->m_rbFile, SIGNAL( clicked() ), this, SLOT( openFile() ) );
	d->m_grid->addWidget( d->m_rbFile, 4, 0 );
		
	QHBox *row = new QHBox( this );
	row->setMargin(KDialog::marginHint());
	row->setSpacing(KDialog::spacingHint());
	d->m_lFile = new QLabel( i18n( "No File" ), row );
	d->m_lFile->setFrameStyle( QFrame::Panel | QFrame::Sunken );
	d->m_bFile = new QPushButton( i18n( "Choose..." ), row );
	d->m_lFile->setMaximumHeight( d->m_bFile->sizeHint().height() );
	d->m_bFile->setMaximumSize( d->m_bFile->sizeHint() );
	row->setMaximumHeight( d->m_bFile->sizeHint().height() + 10 );
	connect( d->m_bFile, SIGNAL( clicked() ), this, SLOT( chooseFile() ) );
	d->m_grid->addWidget( row, 5, 0 );
		
	line = new QFrame( this );
	line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
	line->setMaximumHeight( 20 );
	d->m_grid->addWidget( line, 6, 0 );
		
	d->m_rbEmpty = new QRadioButton( i18n( "Start with an &empty document" ), this );
	connect( d->m_rbEmpty, SIGNAL( clicked() ), this, SLOT( openEmpty() ) );
	d->m_grid->addWidget( d->m_rbEmpty, 7, 0 );
		
	line = new QFrame( this );
	line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
	line->setMaximumHeight( 20 );
	d->m_grid->addWidget( line, 8, 0 );
		
	connect( d->m_rbTemplates, SIGNAL( clicked() ), this, SLOT( openTemplate() ) );
	openEmpty();
    }
}

/*================================================================*/
void KoTemplateChooseDia::currentChanged( const QString & )
{
    openTemplate();
}

/*================================================================*/
void KoTemplateChooseDia::chosen()
{
    if ( d->m_dialogType==OnlyTemplates || d->m_dialogType!=OnlyTemplates && d->m_rbTemplates->isChecked() ) {
	d->m_returnType = Template;

	if ( !d->m_groupList.isEmpty() ) {
	    for ( d->m_grpPtr = d->m_groupList.first();d->m_grpPtr != 0;d->m_grpPtr = d->m_groupList.next() ) {
		if ( d->m_grpPtr->m_tab->isVisible() && !d->m_grpPtr->m_loadWid->getCurrent().isEmpty() ) {
		    QFileInfo f(d->m_grpPtr->m_loadWid->getCurrent());
		    emit templateChosen( QString( d->m_grpPtr->m_name + "/" + f.fileName() ) );
		    d->m_templateName = QString( d->m_grpPtr->m_name + "/" + f.fileName() );
		    d->m_fullTemplateName = d->m_grpPtr->m_loadWid->getCurrent();	
		    accept();
		}
	    }
	}
    } else if ( d->m_dialogType!=OnlyTemplates && d->m_rbFile->isChecked() ) {
	d->m_returnType = File;

	QString fileName = d->m_lFile->text();
	d->m_fullTemplateName = d->m_templateName = fileName;
	accept();
    } else if ( d->m_dialogType!=OnlyTemplates && d->m_rbEmpty->isChecked() ) {
	d->m_returnType = Empty;
	accept();
    } else {
	d->m_returnType = Cancel;
	reject();
    }
}

/*================================================================*/
void KoTemplateChooseDia::openTemplate()
{
    if ( d->m_dialogType!=OnlyTemplates ) {
	d->m_rbTemplates->setChecked( TRUE );
	d->m_rbFile->setChecked( FALSE );
	d->m_rbEmpty->setChecked( FALSE );
    }

    if ( isVisible() )
	d->m_ok->setEnabled( FALSE );

    if ( !d->m_groupList.isEmpty() ) {
	for ( d->m_grpPtr = d->m_groupList.first();d->m_grpPtr != 0;d->m_grpPtr = d->m_groupList.next() ) {
	    if ( d->m_grpPtr->m_tab->isVisible() && d->m_grpPtr->m_loadWid->isCurrentValid() )
		d->m_ok->setEnabled( TRUE );
	}
    }
}

/*================================================================*/
void KoTemplateChooseDia::openFile()
{
    d->m_rbTemplates->setChecked( FALSE );
    d->m_rbFile->setChecked( TRUE );
    d->m_rbEmpty->setChecked( FALSE );

    d->m_ok->setEnabled( QFile::exists( d->m_lFile->text() ) );
}

/*================================================================*/
void KoTemplateChooseDia::openEmpty()
{
    d->m_rbTemplates->setChecked( FALSE );
    d->m_rbFile->setChecked( FALSE );
    d->m_rbEmpty->setChecked( TRUE );

    d->m_ok->setEnabled( TRUE );
}

/*================================================================*/
void KoTemplateChooseDia::chooseFile()
{
    // Use dir from currently selected file
    QString dir = QString::null;
    if ( QFile::exists( d->m_lFile->text() ) )
	dir = QFileInfo( d->m_lFile->text() ).absFilePath();

    KURL u = KFileDialog::getOpenURL( dir, d->m_strImportFilter );
    QString filename = u.path();
    QString url = u.url();
    bool local = u.isLocalFile();

    bool ok = !url.isEmpty();
    if (local) // additionnal checks for local files
	ok = ok && ( QFileInfo( filename ).isFile() ||
	( QFileInfo( filename ).isSymLink() &&
            !QFileInfo( filename ).readLink().isEmpty() &&
	    QFileInfo( QFileInfo( filename ).readLink() ).isFile() ) );

    if ( ok )
    {
	if (local)
            d->m_lFile->setText( filename );
        else
            d->m_lFile->setText( url );
        openFile();
	chosen();
    }
}

void KoTemplateChooseDia::tabsChanged( const QString & ) {
    if ( !d->m_firstTime ) openTemplate(); d->m_firstTime = false;
}

void MyIconCanvas::loadDir( const QString &dirname, const QString &filter )
{
    QDir d( dirname );
    if( !filter.isEmpty() )
	d.setNameFilter(filter);

    if( d.exists() ) {
	QStringList files=d.entryList( QDir::Files | QDir::Readable, QDir::Name );
	for(unsigned int i=0; i<files.count(); ++i)
	    files[i]=dirname + QChar('/') + files[i];		
	loadFiles(files);
    }
}
#include "koTemplateChooseDia.moc"
