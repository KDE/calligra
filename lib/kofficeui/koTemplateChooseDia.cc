/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "koTemplateChooseDia.h"
#include "koTemplateChooseDia.moc"

#include <koFilterManager.h>

#include <klocale.h>
#include <kbuttonbox.h>
#include <kfiledialog.h>
#include <kinstance.h>

#include <qhbox.h>
#include <qvbox.h>
#include <qlayout.h>
#include <kstddirs.h>

#include <qfiledialog.h>

/******************************************************************/
/* Class: KoTemplateChooseDia					  */
/******************************************************************/

/*================================================================*/
KoTemplateChooseDia::KoTemplateChooseDia( QWidget *parent, const char *name, const QString& _template_type,
					 KInstance* _global, bool _hasCancel, bool _onlyTemplates,
					 const QString &importFilter, const QString &mimeType )
    : QDialog( parent, name, TRUE ), template_type( _template_type ), onlyTemplates( _onlyTemplates ),
      m_strImportFilter( importFilter ), m_strMimeType( mimeType )
{
    firstTime = TRUE;
    global = _global;

    groupList.setAutoDelete( TRUE );
    getGroups();

    KButtonBox *bb = new KButtonBox( this );
    bb->addStretch();
    ok = bb->addButton( i18n( "OK" ) );
    connect( ok, SIGNAL( clicked() ), this, SLOT( chosen() ) );
    ok->setDefault( TRUE );
    ok->setEnabled( FALSE );
    if ( _hasCancel )
	connect( bb->addButton( i18n( "Cancel" ) ), SIGNAL( clicked() ), this, SLOT( reject() ) );
    bb->layout();
    bb->setMaximumHeight( bb->sizeHint().height() );
    setupTabs();
    grid->addWidget( bb, 9, 0 );
    grid->setRowStretch( 2, 1 );

    templateName = "";
    fullTemplateName = "";
    returnType = Cancel;
}

/*================================================================*/
KoTemplateChooseDia::ReturnType KoTemplateChooseDia::chooseTemplate( const QString& _template_type,
								     KInstance* global,
								    QString &_template,
								     bool _hasCancel, bool _onlyTemplates,
								    const QString &importFilter,
								     const QString &mimeType )
{
    bool res = FALSE;
    KoTemplateChooseDia *dlg = new KoTemplateChooseDia( 0, "Template", _template_type, global, _hasCancel,
						       _onlyTemplates, importFilter, mimeType );

    dlg->resize( 500, 400 );
    dlg->setCaption( i18n( "Choose a Template" ) );

    if ( dlg->exec() == QDialog::Accepted ) {
	res = TRUE;
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

    QStringList dirs = global->dirs()->resourceDirs( template_type );
    for ( QStringList::ConstIterator it = dirs.begin(); it != dirs.end(); it++ ) {
	QFile templateInf( *it + ".templates" );
	if ( templateInf.open( IO_ReadOnly ) ) {
	    while ( !templateInf.atEnd() ) {
		templateInf.readLine( c, 256 );
		str = c;
		str = str.stripWhiteSpace();
		if ( !str.isEmpty() )
		{
		    grpPtr = new Group;
		    grpPtr->dir.setFile( *it + QString( c ).stripWhiteSpace() + "/" );
		    grpPtr->name = QString( c ).stripWhiteSpace();
		    groupList.append( grpPtr );
		}
		*c = 0;
		// strcpy( c, "" );
	    }

	    templateInf.close();
	}
    }

}

/*================================================================*/
void KoTemplateChooseDia::setupTabs()
{
    grid = new QGridLayout( this, 10, 1, 7 , 7 );

    QFrame *line;

    if ( !onlyTemplates ) {
	line = new QFrame( this );
	line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
	line->setMaximumHeight( 20 );
	grid->addWidget( line, 0, 0 );

	rbTemplates = new QRadioButton( i18n( "Create new document from a &Template" ), this );
	grid->addWidget( rbTemplates, 1, 0 );
    }



    if ( !groupList.isEmpty() ) {
	tabs = new QTabWidget( this );

	for ( grpPtr = groupList.first();grpPtr != 0;grpPtr = groupList.next() ) {
	    grpPtr->tab = new QVBox( tabs );
	    grpPtr->loadWid = new MyIconCanvas( grpPtr->tab );
	    grpPtr->loadWid->loadDir( grpPtr->dir.absFilePath(), "*.png" );
	    grpPtr->loadWid->setBackgroundColor( colorGroup().base() );
	    grpPtr->loadWid->show();
	    connect( grpPtr->loadWid, SIGNAL( doubleClicked( QIconViewItem * ) ),
		     this, SLOT( chosen() ) );
	    connect( grpPtr->loadWid, SIGNAL( nameChanged( const QString & ) ),
		    this, SLOT( nameChanged( const QString & ) ) );
	    connect( grpPtr->loadWid, SIGNAL( currentChanged( const QString & ) ),
		    this, SLOT( currentChanged( const QString & ) ) );
	    grpPtr->label = new QLabel( grpPtr->tab );
	    grpPtr->label->setText( " " );
	    grpPtr->label->setMaximumHeight( grpPtr->label->sizeHint().height() );
	    tabs->addTab( grpPtr->tab, grpPtr->name );
	}
	connect( tabs, SIGNAL( selected( const QString & ) ), this, SLOT( tabsChanged( const QString & ) ) );
	grid->addWidget( tabs, 2, 0 );
    }

    line = new QFrame( this );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setMaximumHeight( 20 );
    grid->addWidget( line, 3, 0 );

    if ( !onlyTemplates ) {
	rbFile = new QRadioButton( i18n( "&Open an existing document" ), this );
	connect( rbFile, SIGNAL( clicked() ), this, SLOT( openFile() ) );
	grid->addWidget( rbFile, 4, 0 );
		
	QHBox *row = new QHBox( this );
	row->setMargin( 5 );
	lFile = new QLabel( i18n( "No File" ), row );
	lFile->setFrameStyle( QFrame::Panel | QFrame::Sunken );
	bFile = new QPushButton( i18n( "Choose..." ), row );
	lFile->setMaximumHeight( bFile->sizeHint().height() );
	bFile->setMaximumSize( bFile->sizeHint() );
	row->setMaximumHeight( bFile->sizeHint().height() + 10 );
	connect( bFile, SIGNAL( clicked() ), this, SLOT( chooseFile() ) );
	grid->addWidget( row, 5, 0 );
		
	line = new QFrame( this );
	line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
	line->setMaximumHeight( 20 );
	grid->addWidget( line, 6, 0 );
		
	rbEmpty = new QRadioButton( i18n( "Start with an &empty document" ), this );
	connect( rbEmpty, SIGNAL( clicked() ), this, SLOT( openEmpty() ) );
	grid->addWidget( rbEmpty, 7, 0 );
		
	line = new QFrame( this );
	line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
	line->setMaximumHeight( 20 );
	grid->addWidget( line, 8, 0 );
		
	connect( rbTemplates, SIGNAL( clicked() ), this, SLOT( openTemplate() ) );
	openEmpty();
    }
}

/*================================================================*/
void KoTemplateChooseDia::nameChanged( const QString &name )
{
    QFileInfo fi( name );

    if ( !groupList.isEmpty() ) {
	for ( grpPtr=groupList.first();grpPtr != 0;grpPtr=groupList.next() ) {
	    grpPtr->label->setText( fi.baseName() );
	    if ( grpPtr->label->text().isEmpty() )
		grpPtr->label->setText( " " );
	}
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
    if ( onlyTemplates || !onlyTemplates && rbTemplates->isChecked() ) {
	returnType = Template;

	if ( !groupList.isEmpty() ) {
	    for ( grpPtr = groupList.first();grpPtr != 0;grpPtr = groupList.next() ) {
		if ( grpPtr->tab->isVisible() && !grpPtr->loadWid->getCurrent().isEmpty() ) {
		    emit templateChosen( QString( grpPtr->name + "/" + grpPtr->loadWid->getCurrent() ) );
		    templateName = QString( grpPtr->name + "/" + grpPtr->loadWid->getCurrent() );
		    fullTemplateName = QString( grpPtr->dir.dirPath( TRUE ) + "/" +
						grpPtr->name + "/" + grpPtr->loadWid->getCurrent() );
		    accept();
		}
	    }
	}
    } else if ( !onlyTemplates && rbFile->isChecked() ) {
	returnType = File;

	QString fileName = lFile->text();

    // I commented that out b/c David reogranised that stuff and now
    // the do-we-need-a-filter check is done later. (Werner)
	//if ( !m_strMimeType.isEmpty() )
    //    {
	    //QString importedFile = KoFilterManager::self()->import( fileName, m_strMimeType );
        //    if ( !importedFile.isEmpty() && importedFile != fileName )
        //        returnType = TempFile; // importedFile points to a temporary file
        //    fileName = importedFile; // open the imported file
    //    }

	fullTemplateName = templateName = fileName;
	accept();
    } else if ( !onlyTemplates && rbEmpty->isChecked() ) {
	returnType = Empty;
	accept();
    } else {
	returnType = Cancel;
	reject();
    }
}

/*================================================================*/
void KoTemplateChooseDia::openTemplate()
{
    if ( !onlyTemplates ) {
	rbTemplates->setChecked( TRUE );
	rbFile->setChecked( FALSE );
	rbEmpty->setChecked( FALSE );
    }

    if ( isVisible() )
	ok->setEnabled( FALSE );

    if ( !groupList.isEmpty() ) {
	for ( grpPtr = groupList.first();grpPtr != 0;grpPtr = groupList.next() ) {
	    if ( grpPtr->tab->isVisible() && grpPtr->loadWid->isCurrentValid() )
		ok->setEnabled( TRUE );
	}
    }
}

/*================================================================*/
void KoTemplateChooseDia::openFile()
{
    rbTemplates->setChecked( FALSE );
    rbFile->setChecked( TRUE );
    rbEmpty->setChecked( FALSE );

    ok->setEnabled( QFile::exists( lFile->text() ) );
}

/*================================================================*/
void KoTemplateChooseDia::openEmpty()
{
    rbTemplates->setChecked( FALSE );
    rbFile->setChecked( FALSE );
    rbEmpty->setChecked( TRUE );

    ok->setEnabled( TRUE );
}

/*================================================================*/
void KoTemplateChooseDia::chooseFile()
{
    openFile();

    QString dir = QString::null;
    if ( QFile::exists( lFile->text() ) )
	dir = QFileInfo( lFile->text() ).absFilePath();

#ifdef USE_QFD
    QString filename;
    filename = QFileDialog::getOpenFileName( dir, m_strImportFilter );
#else
    KURL url;
    url = KFileDialog::getOpenURL( dir, m_strImportFilter );
#endif

#ifdef USE_QFD
    if ( !filename.isEmpty() && QFileInfo( filename ).isFile() ||
	( QFileInfo( filename ).isSymLink() && !QFileInfo( filename ).readLink().isEmpty() &&
	 QFileInfo( QFileInfo( filename ).readLink() ).isFile() ) )
	lFile->setText( filename );
#else
    QString filename = url.url();
    if ( !filename.isEmpty() && QFileInfo( filename ).isFile() ||
	( QFileInfo( filename ).isSymLink() && !QFileInfo( filename ).readLink().isEmpty() &&
	 QFileInfo( QFileInfo( filename ).readLink() ).isFile() ) )
	lFile->setText( filename );
#endif

    openFile();
    if ( !filename.isEmpty() )
	chosen();
}
