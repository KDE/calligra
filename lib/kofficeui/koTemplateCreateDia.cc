/*
   This file is part of the KDE project
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

#include "koTemplateCreateDia.h"

#include <klocale.h>
#include <kiconloaderdialog.h>
#include <klineeditdlg.h>
#include <kapp.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qdir.h>
#include <qheader.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qfile.h>
#include <qtextstream.h>

#include <stdlib.h>

/****************************************************************************
 *
 * Class: koTemplateCreateDia
 *
 ****************************************************************************/

KoTemplateCreateDia::KoTemplateCreateDia( QWidget *parent, const QString &file_, const QPixmap &pix,
		     const QStringList &templateRoots_, const QString extension_ )
    : KDialogBase( parent, "", TRUE, i18n( "Create a Template" ),
		   KDialogBase::Ok | KDialogBase::Cancel, KDialogBase::Ok ),
      extension( extension_ ), file( file_ )
{
    connect( this, SIGNAL( okClicked() ),
	     this, SLOT( ok() ) );
    connect( this, SIGNAL( cancelClicked() ),
	     this, SLOT( reject() ) );

    QWidget *mainview = new QWidget( this );

    QHBoxLayout *layout = new QHBoxLayout( mainview );
    layout->setSpacing( 5 );

    QVBoxLayout *left = new QVBoxLayout( layout );
    left->setSpacing( 5 );

    QLabel *label = new QLabel( i18n( "Choose the Folder for the Template\n"
				      "(with a rightclick into the list you can create new folders)" ),
				mainview );
    left->addWidget( label );

    folderList = new QListView( mainview );
    left->addWidget( folderList );

    folderList->addColumn( "" );
    folderList->header()->hide();
    folderList->setRootIsDecorated( TRUE );

    QStringList::ConstIterator it = templateRoots_.begin();
    QString privateData = getenv( "HOME" );
    privateData += "/.kde/share/apps/" + kapp->instanceName() + "/templates/";
    bool hadPrivateData = FALSE;
    for ( ; it != templateRoots_.end(); ++it ) {
	if ( *it == privateData )
	    hadPrivateData = TRUE;
	QDir dir( *it );
	if ( QFileInfo( dir, "." ).isWritable() ) {
	    QListViewItem *root = new QListViewItem( folderList );
	    root->setText( 0, *it );
	    root->setOpen( TRUE );
	    QFile file( *it + ".templates" );
	    if ( file.exists() && file.open( IO_ReadOnly ) ) {
		QTextStream ts( &file );
		while ( !ts.atEnd() ) {
		    QString s = ts.readLine();
		    s = s.simplifyWhiteSpace();
		    if ( !s.isEmpty() ) {
			QDir dir( *it + s );
			QListViewItem *folder = new QListViewItem( root, s );
			folder->setOpen( TRUE );
			const QFileInfoList *lst = dir.entryInfoList( "*." + extension );
			QFileInfoListIterator it( *lst );
			QFileInfo *fi;
			while ( ( fi = it.current() ) != 0 ) {
			    ++it;
			    (void)new QListViewItem( folder, fi->fileName() );
			}
		    }
		}
	    }
	    file.close();
	}
    }

    if ( !hadPrivateData ) {
	(void)new QListViewItem( folderList, privateData );
	system( QString( "mkdir -p \"%1\"" ).arg( privateData ) );
    }

    connect( folderList, SIGNAL( rightButtonPressed( QListViewItem *, const QPoint &, int ) ),
	     this, SLOT( createFolder( QListViewItem *, const QPoint &, int ) ) );
    connect( folderList, SIGNAL( doubleClicked( QListViewItem * ) ),
	     this, SLOT( doubleClicked( QListViewItem * ) ) );
    connect( folderList, SIGNAL( selectionChanged( QListViewItem * ) ),
	     this, SLOT( selectionChanged( QListViewItem * ) ) );

    QVBoxLayout *right = new QVBoxLayout( layout );

    QHBoxLayout *tmpLayout = new QHBoxLayout( right );

    label = new QLabel( i18n( "Template Name:" ), mainview );
    tmpLayout->addWidget( label );

    lined = new QLineEdit( mainview );
    tmpLayout->addWidget( lined );
    connect( lined, SIGNAL( textChanged( const QString & ) ),
	     this, SLOT( nameChanged( const QString & ) ) );

    tmpLayout = new QHBoxLayout( right );

    QButtonGroup *grp = new QButtonGroup( mainview );
    grp->hide();

    QRadioButton *rb = new QRadioButton( i18n( "&Use generated pixmap" ), mainview );
    rb->setChecked( TRUE );
    tmpLayout->addWidget( rb );
    grp->insert( rb );

    label = new QLabel( mainview );
    label->setPixmap( pix );
    tmpLayout->addWidget( label );

    tmpLayout = new QHBoxLayout( right );

    currPixmap = pix;

    rb = new QRadioButton( i18n( "&Choose pixmap" ), mainview );
    rb->setChecked( FALSE );
    tmpLayout->addWidget( rb );
    grp->insert( rb );
    rb->setEnabled( FALSE );

    KIconLoaderButton *ilb = new KIconLoaderButton( mainview );
    tmpLayout->addWidget( ilb );
    ilb->setEnabled( FALSE );

    setMainWidget( mainview );

    QListViewItemIterator it3( folderList );
    for ( ; it3.current(); ++it3 ) {
	if ( it3.current()->parent() ) {
	    folderList->setSelected( it3.current(), TRUE );
	    break;
	}
    }

    lined->setFocus();

    resize( sizeHint().width(), 300 );
}

void KoTemplateCreateDia::createTemplate( QWidget *parent, const QString &file_, const QPixmap &pix,
					  const QStringList &templateRoots_, const QString extension_ )
{
    KoTemplateCreateDia *dia = new KoTemplateCreateDia( parent, file_, pix, templateRoots_, extension_ );
    dia->exec();
    delete dia;
}

void KoTemplateCreateDia::createFolder( QListViewItem *item, const QPoint &, int )
{
    if ( !item )
	return;

    if ( item->parent() )
	item = item->parent();
    if ( item->parent() )
	item = item->parent();

    QPopupMenu m( this );
    int i = m.insertItem( i18n( "&Add Folder..." ) );
    if ( m.exec( QCursor::pos() ) == i ) {
	QString parent = item->text( 0 );
    	bool ok;
	QString name = KLineEditDlg::getText( i18n( "Add Folder to %1\nFolder Name:" ).arg( parent ),
					     QString::null, &ok, this );
	if ( !name.isEmpty() && ok ) {
	    QDir dir( parent );
	    if ( dir.mkdir( name ) ) {
		QFile f( parent + "/.templates" );
		if ( f.open( IO_WriteOnly | IO_Append ) ) {
		    f.writeBlock( "\n" + name + "\n", name.length() + 2 );
		    f.close();
		    QListViewItem *f = new QListViewItem( item, name );
		    item->setOpen( TRUE );
		    f->setOpen( TRUE );
		}
	    }
	}
    }
}

void KoTemplateCreateDia::selectionChanged( QListViewItem *item )
{
    if ( item && ( !item->parent() ||
	( item->parent() && item->parent()->parent() ) ) ) {
	bool firstDown = !item->parent();
	QListViewItemIterator it( item );
	for ( ; it.current(); firstDown ? ++it : --it ) {
	    if ( it.current()->parent() &&
		 !it.current()->parent()->parent() ) {
		folderList->setSelected( it.current(), TRUE );
		folderList->setCurrentItem( item );
		folderList->setSelected( item, FALSE );
		return;
	    }
	}
	it = QListViewItemIterator( item );
	for ( ; it.current(); firstDown ? --it : ++it ) {
	    if ( it.current()->parent() &&
		 !it.current()->parent()->parent() ) {
		folderList->setSelected( it.current(), TRUE );
		folderList->setCurrentItem( item );
		folderList->setSelected( item, FALSE );
		return;
	    }
	}
	folderList->setSelected( item, FALSE );
    }
}

void KoTemplateCreateDia::doubleClicked( QListViewItem *item )
{
    if ( !item )
	return;
    if ( item->parent() && item->parent()->parent() )
	lined->setText( item->text( 0 ) );
}

void KoTemplateCreateDia::nameChanged( const QString &name )
{
    currName = name;
}

void KoTemplateCreateDia::useGeneratedPixmap()
{
}

void KoTemplateCreateDia::useChosenPixmap()
{
}

void KoTemplateCreateDia::iconChanged( const QString & )
{
}

void KoTemplateCreateDia::ok()
{
    QListViewItem *item = 0;
    QListViewItemIterator it( folderList );
    for ( ; it.current(); ++it ) {
	if ( it.current()->isSelected() ) {
	    item = it.current();
	    break;
	}
    }

    if ( !item ) {
	QMessageBox::critical( this, i18n( "Create Template" ),
			       i18n( "Can't create a Template. Please select a Folder in the Folder List!" ) );
	return;
    }

    if ( currName.isEmpty() ) {
	QMessageBox::critical( this, i18n( "Create Template" ),
			       i18n( "Can't create a Template. Please enter a Name for the Template!" ) );
	return;
    }

    QFileInfo fi( currName );
    if ( fi.extension().isEmpty() || fi.extension() != "kpt" )
	currName += "." + extension;

    if ( QFile( item->parent()->text( 0 ) +
		item->text( 0 ) + "/" + currName ).exists() ) {
	if ( QMessageBox::warning( this, i18n( "Create Template" ),
				   i18n( "A template with the same name (%1) already exists. Do you want "
					 "to override it?" ).arg( currName ), i18n( "&Yes" ), i18n( "&No" ) ) == 1 )
	    return;
    }

    QString cmd = "cp ";
    cmd += file + " ";
    cmd += "\"" + item->parent()->text( 0 ) + item->text( 0 ) +
	"/" + currName + "\"";
    system( cmd.latin1() );
    currName = fi.baseName();
    currPixmap.save( item->parent()->text( 0 ) + item->text( 0 ) +
		     "/" + currName + ".png", "PNG" );
    accept();
}
