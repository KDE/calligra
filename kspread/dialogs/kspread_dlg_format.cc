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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qcombobox.h>
#include <qfile.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <QPixmap>

#include <kbuttonbox.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <ksimpleconfig.h>
#include <kinstance.h>

#include "kspread_dlg_format.h"
#include "kspread_doc.h"
#include "kspread_locale.h"
#include "kspread_sheet.h"
#include "kspread_style.h"
#include "kspread_style_manager.h"
#include "kspread_undo.h"
#include "kspread_view.h"
#include "selection.h"


using namespace KSpread;

FormatDialog::FormatDialog( View* view, const char* name )
    : KDialogBase( view, name, TRUE,i18n("Sheet Style"),Ok|Cancel )
{
    for( int i = 0; i < 16; ++i )
	m_cells[ i ] = 0;

    m_view = view;
    QWidget *page = new QWidget( this );
    setMainWidget(page);
    QVBoxLayout *vbox = new QVBoxLayout( page );
    vbox->setMargin(KDialogBase::marginHint());
    vbox->setSpacing(KDialogBase::spacingHint());

    QLabel *toplabel = new QLabel( i18n("Select the sheet style to apply:"), page );
    m_combo = new QComboBox( page );
    m_label = new QLabel( page );

    vbox->addWidget( toplabel );
    vbox->addWidget( m_combo );
    vbox->addWidget( m_label );


    QStringList lst = Factory::global()->dirs()->findAllResources( "sheet-styles", "*.ksts", TRUE );

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

FormatDialog::~FormatDialog()
{
    for( int i = 0; i < 16; ++i )
	delete m_cells[ i ];
}

void FormatDialog::slotActivated( int index )
{
	enableButtonOK(true);

    QString img = Factory::global()->dirs()->findResource( "sheet-styles", m_entries[ index ].image );
    if ( img.isEmpty() )
    {
	QString str( i18n( "Could not find image %1." ) );
	str = str.arg( m_entries[ index ].image );
	KMessageBox::error( this, str );

	enableButtonOK(false);

	return;
    }

    QPixmap pix( img );
    if ( pix.isNull() )
    {
	QString str( i18n( "Could not load image %1." ) );
	str = str.arg( img );
	KMessageBox::error( this,str );

	enableButtonOK(false);

	return;
    }

    m_label->setPixmap( pix );
}

void FormatDialog::slotOk()
{

    m_view->doc()->emitBeginOperation( false );

    QString xml = Factory::global()->dirs()->findResource( "sheet-styles", m_entries[ m_combo->currentIndex() ].xml );
    if ( xml.isEmpty() )
    {
	QString str( i18n( "Could not find sheet-style XML file '%1'." ) );
	str = str.arg( m_entries[ m_combo->currentIndex() ].xml );
	KMessageBox::error( this, str );
	return;
    }

    QFile file( xml );
    file.open( QIODevice::ReadOnly );
    QDomDocument doc;
    doc.setContent( &file );
    file.close();

    if ( !parseXML( doc ) )
    {
	QString str( i18n( "Parsing error in sheet-style XML file %1." ) );
	str = str.arg( m_entries[ m_combo->currentIndex() ].xml );
	KMessageBox::error( this, str );
	return;
    }

    QRect r = m_view->selectionInfo()->selection();

    if ( !m_view->doc()->undoLocked() )
    {
        QString title=i18n("Change Format");
        UndoCellFormat *undo = new UndoCellFormat( m_view->doc(), m_view->activeSheet(), r, title);
        m_view->doc()->addCommand( undo );
    }
    //
    // Set colors, borders etc.
    //

    // Top left corner
    Cell* cell = m_view->activeSheet()->nonDefaultCell( r.left(), r.top() );
    cell->format()->copy( *m_cells[0] );

    // Top column
    int x, y;
    for( x = r.left() + 1; x <= r.right(); ++x )
    {
	int pos = 1 + ( ( x - r.left() - 1 ) % 2 );
	Cell* cell = m_view->activeSheet()->nonDefaultCell( x, r.top() );
        if(!cell->isPartOfMerged())
        {
          cell->format()->copy( *m_cells[ pos ] );

	Format* c;
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
	Cell* cell = m_view->activeSheet()->nonDefaultCell( r.left(), y );
        if(!cell->isPartOfMerged())
        {
          cell->format()->copy( *m_cells[ pos ] );

	Format* c;
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
	    Cell* cell = m_view->activeSheet()->nonDefaultCell( x, y );
            if(!cell->isPartOfMerged())
            {
              cell->format()->copy( *m_cells[ pos ] );

	    Format* c;
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
	Cell* cell = m_view->activeSheet()->nonDefaultCell( r.right(), y );
        if(!cell->isPartOfMerged())
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
	Cell* cell = m_view->activeSheet()->nonDefaultCell( x, r.bottom() );
        if(!cell->isPartOfMerged())
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

    m_view->selectionInfo()->initialize(r);//,       m_view->activeSheet() );
    m_view->doc()->setModified( true );
    m_view->slotUpdateView( m_view->activeSheet() );
    accept();
}

bool FormatDialog::parseXML( const QDomDocument& doc )
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
	    Sheet* sheet = m_view->activeSheet();
	    Format* cell = new Format( sheet, sheet->doc()->styleManager()->defaultStyle() );

     if ( !cell->load( e.namedItem("format").toElement(), Paste::Normal ) )
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
