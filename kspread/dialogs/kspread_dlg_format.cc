/* This file is part of the KDE project
   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 Ariya Hidayat <ariya@kde.org>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2000-2002 Laurent Montel <montel@kde.org>
             (C) 2000 Werner Trobin <trobin@kde.org>
             (C) 1998-2000 Torben Weis <weis@kde.org>
          
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

#include "kspread_dlg_format.h"
#include "kspread_doc.h"
#include "kspread_locale.h"
#include "kspread_selection.h"
#include "kspread_sheet.h"
#include "kspread_style.h"
#include "kspread_style_manager.h"
#include "kspread_undo.h"
#include "kspread_view.h"

#include <kstandarddirs.h>
#include <ksimpleconfig.h>
#include <kbuttonbox.h>
#include <kmessagebox.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qfile.h>
#include <qpushbutton.h>


KSpreadFormatDlg::KSpreadFormatDlg( KSpreadView* view, const char* name )
    : KDialogBase( view, name, TRUE,i18n("Sheet Style"),Ok|Cancel )
{
    for( int i = 0; i < 16; ++i )
	m_cells[ i ] = 0;

    m_view = view;
    QWidget *page = new QWidget( this );
    setMainWidget(page);
    QVBoxLayout *vbox = new QVBoxLayout( page, 0, spacingHint() );

    QLabel *toplabel = new QLabel( i18n("Select the sheet style to apply:"), page );
    m_combo = new QComboBox( page );
    m_label = new QLabel( page );

    vbox->addWidget( toplabel );
    vbox->addWidget( m_combo );
    vbox->addWidget( m_label );


    QStringList lst = KSpreadFactory::global()->dirs()->findAllResources( "table-styles", "*.ksts", TRUE );

    QStringList::Iterator it = lst.begin();
    for( ; it != lst.end(); ++it )
    {
	KSimpleConfig cfg( *it, TRUE );
	cfg.setGroup( "Sheet-Style" );

	Entry e;
	e.config = *it;
	e.xml = cfg.readEntry( "XML" );
	e.image = cfg.readEntry( "Image" );
	e.name = cfg.readEntry( "Name" );

	m_entries.append( e );

	m_combo->insertItem( e.name );
    }

    slotActivated( 0 );

    connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
    connect( m_combo, SIGNAL( activated( int ) ), this, SLOT( slotActivated( int ) ) );
}

KSpreadFormatDlg::~KSpreadFormatDlg()
{
    for( int i = 0; i < 16; ++i )
	delete m_cells[ i ];
}

void KSpreadFormatDlg::slotActivated( int index )
{
    QString img = KSpreadFactory::global()->dirs()->findResource( "table-styles", m_entries[ index ].image );
    if ( img.isEmpty() )
    {
	QString str( i18n( "Could not find image %1" ) );
	str = str.arg( m_entries[ index ].image );
	KMessageBox::error( this, str );
	return;
    }

    QPixmap pix( img );
    if ( pix.isNull() )
    {
	QString str( i18n( "Could not load image %1" ) );
	str = str.arg( img );
	KMessageBox::error( this,str );
	return;
    }

    m_label->setPixmap( pix );
}

void KSpreadFormatDlg::slotOk()
{
    m_view->doc()->emitBeginOperation( false );

    QString xml = KSpreadFactory::global()->dirs()->findResource( "table-styles", m_entries[ m_combo->currentItem() ].xml );
    if ( xml.isEmpty() )
    {
	QString str( i18n( "Could not find table-style XML file '%1'" ) );
	str = str.arg( m_entries[ m_combo->currentItem() ].xml );
	KMessageBox::error( this, str );
	return;
    }

    QFile file( xml );
    file.open( IO_ReadOnly );
    QDomDocument doc;
    doc.setContent( &file );
    file.close();

    if ( !parseXML( doc ) )
    {
	QString str( i18n( "Parsing error in table-style XML file %1" ) );
	str = str.arg( m_entries[ m_combo->currentItem() ].xml );
	KMessageBox::error( this, str );
	return;
    }

    QRect r = m_view->selection();

    if ( !m_view->doc()->undoLocked() )
    {
        QString title=i18n("Change Format");
        KSpreadUndoCellFormat *undo = new KSpreadUndoCellFormat( m_view->doc(), m_view->activeSheet(), r ,title);
        m_view->doc()->addCommand( undo );
    }
    //
    // Set colors, borders etc.
    //

    // Top left corner
    KSpreadCell* cell = m_view->activeSheet()->nonDefaultCell( r.left(), r.top() );
    cell->copy( *m_cells[0] );

    // Top column
    int x, y;
    for( x = r.left() + 1; x <= r.right(); ++x )
    {
	int pos = 1 + ( ( x - r.left() - 1 ) % 2 );
	KSpreadCell* cell = m_view->activeSheet()->nonDefaultCell( x, r.top() );
        if(!cell->isObscuringForced())
        {
        cell->copy( *m_cells[ pos ] );

	KSpreadFormat* c;
	if ( x == r.right() )
	    c = m_cells[2];
	else
	    c = m_cells[1];

	if ( c )
	    cell->setTopBorderPen( c->topBorderPen( 0, 0 ) );

	if ( x == r.left() + 1 )
	    c = m_cells[1];
	else
	    c = m_cells[2];

	if ( c )
	    cell->setLeftBorderPen( c->leftBorderPen( 0, 0 ) );
        }
    }

    cell = m_view->activeSheet()->nonDefaultCell( r.right(), r.top() );
    if ( m_cells[3] )
	cell->setRightBorderPen( m_cells[3]->leftBorderPen( 0, 0 ) );

    // Left row
    for( y = r.top() + 1; y <= r.bottom(); ++y )
    {
	int pos = 4 + ( ( y - r.top() - 1 ) % 2 ) * 4;
	KSpreadCell* cell = m_view->activeSheet()->nonDefaultCell( r.left(), y );
        if(!cell->isObscuringForced())
        {
        cell->copy( *m_cells[ pos ] );

	KSpreadFormat* c;
	if ( y == r.bottom() )
	    c = m_cells[8];
	else
	    c = m_cells[4];

	if ( c )
	    cell->setLeftBorderPen( c->leftBorderPen( 0, 0 ) );

	if ( y == r.top() + 1 )
	    c = m_cells[4];
	else
	    c = m_cells[8];

	if ( c )
	    cell->setTopBorderPen( c->topBorderPen( 0, 0 ) );
        }
    }

    // Body
    for( x = r.left() + 1; x <= r.right(); ++x )
	for( y = r.top() + 1; y <= r.bottom(); ++y )
        {
	    int pos = 5 + ( ( y - r.top() - 1 ) % 2 ) * 4 + ( ( x - r.left() - 1 ) % 2 );
	    KSpreadCell* cell = m_view->activeSheet()->nonDefaultCell( x, y );
            if(!cell->isObscuringForced())
            {
            cell->copy( *m_cells[ pos ] );

	    KSpreadFormat* c;
	    if ( x == r.left() + 1 )
		c = m_cells[ 5 + ( ( y - r.top() - 1 ) % 2 ) * 4 ];
	    else
		c = m_cells[ 6 + ( ( y - r.top() - 1 ) % 2 ) * 4 ];

	    if ( c )
		cell->setLeftBorderPen( c->leftBorderPen( 0, 0 ) );

	    if ( y == r.top() + 1 )
		c = m_cells[ 5 + ( ( x - r.left() - 1 ) % 2 ) ];
	    else
		c = m_cells[ 9 + ( ( x - r.left() - 1 ) % 2 ) ];

	    if ( c )
		cell->setTopBorderPen( c->topBorderPen( 0, 0 ) );
            }
	}

    // Outer right border
    for( y = r.top(); y <= r.bottom(); ++y )
    {
	KSpreadCell* cell = m_view->activeSheet()->nonDefaultCell( r.right(), y );
        if(!cell->isObscuringForced())
        {
	if ( y == r.top() )
        {
	    if ( m_cells[3] )
		cell->setRightBorderPen( m_cells[3]->leftBorderPen( 0, 0 ) );
	}
	else if ( y == r.right() )
        {
	    if ( m_cells[11] )
		cell->setRightBorderPen( m_cells[11]->leftBorderPen( 0, 0 ) );
	}
	else
        {
	    if ( m_cells[7] )
		cell->setRightBorderPen( m_cells[7]->leftBorderPen( 0, 0 ) );
	}
        }
    }

    // Outer bottom border
    for( x = r.left(); x <= r.right(); ++x )
    {
	KSpreadCell* cell = m_view->activeSheet()->nonDefaultCell( x, r.bottom() );
        if(!cell->isObscuringForced())
        {
        if ( x == r.left() )
        {
	    if ( m_cells[12] )
		cell->setBottomBorderPen( m_cells[12]->topBorderPen( 0, 0 ) );
	}
	else if ( x == r.right() )
        {
	    if ( m_cells[14] )
		cell->setBottomBorderPen( m_cells[14]->topBorderPen( 0, 0 ) );
	}
	else
        {
	    if ( m_cells[13] )
		cell->setBottomBorderPen( m_cells[13]->topBorderPen( 0, 0 ) );
	}
        }
    }

    m_view->selectionInfo()->setSelection( r.topLeft(), r.bottomRight(),
                                           m_view->activeSheet() );
    m_view->doc()->setModified( true );
    m_view->slotUpdateView( m_view->activeSheet() );
    accept();
}

bool KSpreadFormatDlg::parseXML( const QDomDocument& doc )
{
    for( int i = 0; i < 16; ++i )
    {
	delete m_cells[ i ];
	m_cells[ i ] = 0;
    }

    QDomElement e = doc.documentElement().firstChild().toElement();
    for( ; !e.isNull(); e = e.nextSibling().toElement() )
    {
	if ( e.tagName() == "cell" )
        {
	    KSpreadSheet* sheet = m_view->activeSheet();
	    KSpreadFormat* cell = new KSpreadFormat( sheet, sheet->doc()->styleManager()->defaultStyle() );

	    if ( !cell->load( e.namedItem("format").toElement(), Normal ) )
		return false;

	    int row = e.attribute("row").toInt();
	    int column = e.attribute("column").toInt();
	    int i = (row-1)*4 + (column-1);
	    if ( i < 0 || i >= 16 )
		return false;

	    m_cells[ i ] = cell;
	}
    }

    return TRUE;
}

#include "kspread_dlg_format.moc"
