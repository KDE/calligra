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
#include <qvbox.h>
#include <qtabwidget.h>
#include <qradiobutton.h>

#include <kapp.h>
#include <kdesktopfile.h>
#include <klocale.h>
#include <kbuttonbox.h>
#include <kurl.h>
#include <kfiledialog.h>
#include <kinstance.h>
#include <kstddirs.h>
#include <koFilterManager.h>

#include "koTemplateChooseDia.h"


class KoTemplateChooseDiaPrivate {
public:
    KoTemplateChooseDiaPrivate(const QString& templateType, KInstance* global,
			       const char *format, const QString &nativePattern,
			       const QString &nativeName,
			       const KoTemplateChooseDia::DialogType &dialogType) :
	m_templateType(templateType), m_global(global), m_format(format),
	m_nativePattern(nativePattern), m_nativeName(nativeName),
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
    const char *m_format;
    QString m_nativePattern, m_nativeName;
    KoTemplateChooseDia::DialogType m_dialogType;
    bool m_firstTime;

    QString m_templateName;
    QString m_fullTemplateName;
    QList<Group> m_groupList;
    Group *m_grpPtr;
    QRadioButton *m_rbTemplates;
    QRadioButton *m_rbFile;
    QRadioButton *m_rbEmpty;
    QString m_file;
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
					 const char *format, const QString &nativePattern,
					 const QString &nativeName, const DialogType &dialogType,
					 const QString& templateType, bool hasCancel) :
    KDialog(parent, name, true) {

    d=new KoTemplateChooseDiaPrivate(templateType, global, format, nativePattern,
				     nativeName, dialogType);

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
							    const char *format, const QString &nativePattern,
							    const QString &nativeName,
							    const KoTemplateChooseDia::DialogType &dialogType,
							    const QString& templateType, bool hasCancel) {
    bool res = false;
    KoTemplateChooseDia *dlg = new KoTemplateChooseDia( 0, "Choose", global, format, nativePattern,
							nativeName, dialogType, templateType, hasCancel);
    if(dialogType!=NoTemplates)
	dlg->resize( 500, 400 );
    else
	dlg->resize( 10, 10 );  // geometry is managed!

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
    d->m_grid = new QGridLayout( this, 8, 1, KDialog::marginHint() , KDialog::spacingHint() );

    QFrame *line;

    if ( d->m_dialogType==Everything ) {
	line = new QFrame( this );
	line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
	line->setMaximumHeight( 20 );
	d->m_grid->addWidget( line, 0, 0 );

	d->m_rbTemplates = new QRadioButton( i18n( "Create new document from a &Template" ), this );
	connect( d->m_rbTemplates, SIGNAL( clicked() ), this, SLOT( openTemplate() ) );
	d->m_grid->addWidget( d->m_rbTemplates, 1, 0 );
    }

    if ( !d->m_groupList.isEmpty() && d->m_dialogType!=NoTemplates ) {
	d->m_tabs = new QTabWidget( this );
	
	for ( d->m_grpPtr = d->m_groupList.first();d->m_grpPtr != 0;d->m_grpPtr = d->m_groupList.next() ) {
	    d->m_grpPtr->m_tab = new QVBox( d->m_tabs );
	    d->m_grpPtr->m_loadWid = new MyIconCanvas( d->m_grpPtr->m_tab );
	    d->m_grpPtr->m_loadWid->loadDir( d->m_grpPtr->m_dir.absFilePath() );
	    d->m_grpPtr->m_loadWid->setBackgroundColor( colorGroup().base() );
	    d->m_grpPtr->m_loadWid->setWordWrapIconText( true ); // DF
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
	QHBoxLayout *row = new QHBoxLayout( d->m_grid );
	d->m_rbFile = new QRadioButton( i18n( "&Open an existing document" ), this );
	connect( d->m_rbFile, SIGNAL( clicked() ), this, SLOT( openFile() ) );
	row->addWidget(d->m_rbFile);
	row->addSpacing(30);
	d->m_bFile = new QPushButton( i18n( "Choose..." ), this );
	d->m_bFile->setMaximumSize( d->m_bFile->sizeHint() );
	row->addWidget(d->m_bFile);
	connect( d->m_bFile, SIGNAL( clicked() ), this, SLOT( chooseFile() ) );
			
	line = new QFrame( this );
	line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
	line->setMaximumHeight( 20 );
	d->m_grid->addWidget( line, 5, 0 );
		
	d->m_rbEmpty = new QRadioButton( i18n( "Start with an &empty document" ), this );
	connect( d->m_rbEmpty, SIGNAL( clicked() ), this, SLOT( openEmpty() ) );
	d->m_grid->addWidget( d->m_rbEmpty, 6, 0 );
		
	line = new QFrame( this );
	line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
	line->setMaximumHeight( 20 );
	d->m_grid->addWidget( line, 7, 0 );
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
    if ( d->m_dialogType==OnlyTemplates || d->m_dialogType==Everything && d->m_rbTemplates->isChecked() ) {
	    d->m_returnType = Template;

	    if ( !d->m_groupList.isEmpty() ) {
		for ( d->m_grpPtr = d->m_groupList.first();d->m_grpPtr != 0;d->m_grpPtr = d->m_groupList.next() ) {
		    if ( d->m_grpPtr->m_tab->isVisible() && !d->m_grpPtr->m_loadWid->getCurrent().isEmpty() ) {
			QFileInfo f(d->m_grpPtr->m_loadWid->getCurrent());
			d->m_templateName = QString( d->m_grpPtr->m_name + "/" + f.fileName() );
			emit templateChosen( d->m_templateName );
			d->m_fullTemplateName = d->m_grpPtr->m_loadWid->getCurrent();	
			accept();
		    }
		}
	    }
	
    } else if ( d->m_dialogType!=OnlyTemplates && d->m_rbFile->isChecked() ) {
	d->m_returnType = File;
	d->m_fullTemplateName = d->m_templateName = d->m_file;
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
    if ( d->m_dialogType==Everything ) {
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
    if(d->m_dialogType!=NoTemplates)
	d->m_rbTemplates->setChecked( FALSE );
    d->m_rbFile->setChecked( TRUE );
    d->m_rbEmpty->setChecked( FALSE );

    d->m_ok->setEnabled( QFile::exists( d->m_file ) );
}

/*================================================================*/
void KoTemplateChooseDia::openEmpty()
{
    if(d->m_dialogType!=NoTemplates)
	d->m_rbTemplates->setChecked( FALSE );
    d->m_rbFile->setChecked( FALSE );
    d->m_rbEmpty->setChecked( TRUE );

    d->m_ok->setEnabled( TRUE );
}

/*================================================================*/
void KoTemplateChooseDia::chooseFile()
{
    // Save current state - in case of Cancel
    bool bEmpty = d->m_rbEmpty->isChecked();
    bool bTemplates = (d->m_dialogType!=NoTemplates) && d->m_rbTemplates->isChecked();
    openFile();

    // Use dir from currently selected file
    QString dir = QString::null;
    if ( QFile::exists( d->m_file ) )
	dir = QFileInfo( d->m_file ).absFilePath();

    KFileDialog *dialog=new KFileDialog(dir, QString::null, 0L, "file dialog", true);
    dialog->setCaption( i18n("Open document") );
    KoFilterManager::self()->prepareDialog(dialog, KoFilterManager::Import, d->m_format,
					   d->m_nativePattern, d->m_nativeName, true);
    KURL u;

    if(dialog->exec()==QDialog::Accepted)
    {
	u=dialog->selectedURL();
    } else //revert state
    {
        if (bEmpty) openEmpty();
        if (bTemplates) openTemplate();
    }

    KoFilterManager::self()->cleanUp();
    delete dialog;

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
            d->m_file=filename;
        else
            d->m_file=url;
	chosen();
    }
}

void KoTemplateChooseDia::tabsChanged( const QString & ) {
    if ( !d->m_firstTime ) openTemplate();
    d->m_firstTime = false;
}

void MyIconCanvas::loadDir( const QString &dirname )
{
    m_dirname = dirname;
    QTimer::singleShot( 0, this, SLOT(slotLoadDir()) );
}

void MyIconCanvas::slotLoadDir()
{
    QString dirname = m_dirname;
    setResizeMode(Fixed);
    QApplication::setOverrideCursor(waitCursor);
    QDir d( dirname );

    if( d.exists() ) {
	QStringList files=d.entryList( QDir::Files | QDir::Readable, QDir::Name );
	for(unsigned int i=0; i<files.count(); ++i)
        {
            emit progress(i);
            kapp->processEvents();

	    QString filePath = dirname + QChar('/') + files[i];		
            //kdDebug() << filePath << endl;
            QString icon;
            QString text;
            QString templatePath;
            // If a desktop file, then read the name from it.
            // Otherwise (or if no name in it?) use file name
            if ( KDesktopFile::isDesktopFile( filePath ) ) {
                KSimpleConfig config( filePath, true );
                config.setDesktopGroup();
                if ( config.readEntry( "Type" ) == "Link" )
                {
                    text = config.readEntry("Name");
                    icon = config.readEntry("Icon");
                    if ( icon[0] != '/' ) // allow absolute paths for icons
                        icon = dirname + '/' + icon;
                    templatePath = config.readEntry("URL");
                    kdDebug() << "Link to : " << templatePath << endl;
                    if ( templatePath[0] != '/' )
                    {
                        if ( templatePath.left(6) == "file:/" ) // I doubt this will happen
                            templatePath = templatePath.right( templatePath.length() - 6 );
                        else
                            //kdDebug() << "dirname=" << dirname << endl;
                            templatePath = dirname + '/' + templatePath;
                    }
                } else
                    continue; // Invalid
            }
            else if ( files[i].right(4) != ".png" )
                // Ignore everything that is not a PNG file
                continue;
            else {
                // Found a PNG file - the template must be here in the same dir.
                icon = filePath;
                QFileInfo fi(filePath);
                text = fi.baseName();
                templatePath = filePath; // Note that we store the .png file as the template !
                // That's the way it's always been done. Then the app replaces the extension...
            }

            // We've got our info, now create the item
            // This code is shamelessly borrowed from KIconCanvas::slotLoadFiles
            QImage img;
            kdDebug() << "Icon=" << icon << endl;
            img.load(icon);
            if (img.isNull())
                continue;
            if (img.width() > 60 || img.height() > 60)
            {
                if (img.width() > img.height())
                {
                    int height = (int) ((60.0 / img.width()) * img.height());
                    img = img.smoothScale(60, height);
                } else
                {
                    int width = (int) ((60.0 / img.height()) * img.width());
                    img = img.smoothScale(width, 60);
                }
            }
            QPixmap pm;
            pm.convertFromImage(img);

            QIconViewItem *item = new QIconViewItem(this, text, pm);
            item->setKey(templatePath);
            item->setDragEnabled(false);
            item->setDropEnabled(false);

        }
    }
    QApplication::restoreOverrideCursor();
    setResizeMode(Adjust);
}

#include "koTemplateChooseDia.moc"
