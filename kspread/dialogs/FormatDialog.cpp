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

#include <QComboBox>
#include <QFile>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
//Added by qt3to4:
#include <QVBoxLayout>
#include <QPixmap>

#include <kbuttonbox.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <ksimpleconfig.h>
#include <kinstance.h>

#include "FormatDialog.h"
#include "Cell.h"
#include "Doc.h"
#include "Localization.h"
#include "Sheet.h"
#include "Style.h"
#include "StyleManager.h"
#include "Undo.h"
#include "View.h"
#include "Selection.h"


using namespace KSpread;

FormatDialog::FormatDialog( View* view, const char* name )
    : KDialog( view )
{
    setCaption( i18n("Sheet Style") );
    setObjectName( name );
    setModal( true );
    setButtons( Ok|Cancel );

    for( int i = 0; i < 16; ++i )
	m_styles[ i ] = 0;

    m_view = view;
    QWidget *page = mainWidget();

    QVBoxLayout *vbox = new QVBoxLayout( page );
    vbox->setMargin(KDialog::marginHint());
    vbox->setSpacing(KDialog::spacingHint());

    QLabel *toplabel = new QLabel( i18n("Select the sheet style to apply:"), page );
    m_combo = new QComboBox( page );
    m_label = new QLabel( page );

    vbox->addWidget( toplabel );
    vbox->addWidget( m_combo );
    vbox->addWidget( m_label );


    QStringList lst = Factory::global()->dirs()->findAllResources( "sheet-styles", "*.ksts", true );

    int index = 0;
    QStringList::Iterator it = lst.begin();
    for( ; it != lst.end(); ++it )
    {
	KSimpleConfig cfg( *it, true );
	cfg.setGroup( "Sheet-Style" );

	Entry e;
	e.config = *it;
	e.xml = cfg.readEntry( "XML" );
	e.image = cfg.readEntry( "Image" );
	e.name = cfg.readEntry( "Name" );

	m_entries.append( e );

	m_combo->insertItem( index++, e.name );
    }

    slotActivated( 0 );

    connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
    connect( m_combo, SIGNAL( activated( int ) ), this, SLOT( slotActivated( int ) ) );
}

FormatDialog::~FormatDialog()
{
    for( int i = 0; i < 16; ++i )
	delete m_styles[ i ];
}

void FormatDialog::slotActivated( int index )
{
	enableButtonOk(true);

    QString img = Factory::global()->dirs()->findResource( "sheet-styles", m_entries[ index ].image );
    if ( img.isEmpty() )
    {
	QString str( i18n( "Could not find image %1." ) );
	str = str.arg( m_entries[ index ].image );
	KMessageBox::error( this, str );

	enableButtonOk(false);

	return;
    }

    QPixmap pix( img );
    if ( pix.isNull() )
    {
	QString str( i18n( "Could not load image %1." ) );
	str = str.arg( img );
	KMessageBox::error( this,str );

	enableButtonOk(false);

	return;
    }

    m_label->setWindowIcon( pix );
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
    KoXmlDocument doc;
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

#ifndef KSPREAD_NEW_STYLE_STORAGE // copy
    if ( !m_view->doc()->undoLocked() )
    {
        QString title=i18n("Change Format");
        UndoCellFormat *undo = new UndoCellFormat( m_view->doc(), m_view->activeSheet(), Region(r), title);
        m_view->doc()->addCommand( undo );
    }
    //
    // Set colors, borders etc.
    //

    // Top left corner
    Cell* cell = m_view->activeSheet()->nonDefaultCell( r.left(), r.top() );
    cell->format()->copy( *m_styles[0] );

    // Top column
    int x, y;
    for( x = r.left() + 1; x <= r.right(); ++x )
    {
	int pos = 1 + ( ( x - r.left() - 1 ) % 2 );
	Cell* cell = m_view->activeSheet()->nonDefaultCell( x, r.top() );
        if(!cell->isPartOfMerged())
        {
          cell->format()->copy( *m_styles[ pos ] );

	Style* style;
	if ( x == r.right() )
	    style = m_styles[2];
	else
	    style = m_styles[1];

	if ( style )
	    cell->setTopBorderPen( style->topBorderPen( 0, 0 ) );

	if ( x == r.left() + 1 )
	    style = m_styles[1];
	else
	    style = m_styles[2];

	if ( style )
	    cell->setLeftBorderPen( style->leftBorderPen( 0, 0 ) );
        }
    }

    cell = m_view->activeSheet()->nonDefaultCell( r.right(), r.top() );
    if ( m_styles[3] )
	cell->setRightBorderPen( m_styles[3]->leftBorderPen( 0, 0 ) );

    // Left row
    for( y = r.top() + 1; y <= r.bottom(); ++y )
    {
	int pos = 4 + ( ( y - r.top() - 1 ) % 2 ) * 4;
	Cell* cell = m_view->activeSheet()->nonDefaultCell( r.left(), y );
        if(!cell->isPartOfMerged())
        {
          cell->format()->copy( *m_styles[ pos ] );

	Style* style;
	if ( y == r.bottom() )
	    style = m_styles[8];
	else
	    style = m_styles[4];

	if ( style )
	    cell->setLeftBorderPen( style->leftBorderPen( 0, 0 ) );

	if ( y == r.top() + 1 )
	    style = m_styles[4];
	else
	    style = m_styles[8];

	if ( style )
	    cell->setTopBorderPen( style->topBorderPen( 0, 0 ) );
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
              cell->format()->copy( *m_styles[ pos ] );

	    Style* style;
	    if ( x == r.left() + 1 )
		style = m_styles[ 5 + ( ( y - r.top() - 1 ) % 2 ) * 4 ];
	    else
		style = m_styles[ 6 + ( ( y - r.top() - 1 ) % 2 ) * 4 ];

	    if ( style )
		cell->setLeftBorderPen( style->leftBorderPen( 0, 0 ) );

	    if ( y == r.top() + 1 )
		style = m_styles[ 5 + ( ( x - r.left() - 1 ) % 2 ) ];
	    else
		style = m_styles[ 9 + ( ( x - r.left() - 1 ) % 2 ) ];

	    if ( style )
		cell->setTopBorderPen( style->topBorderPen( 0, 0 ) );
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
	    if ( m_styles[3] )
		cell->setRightBorderPen( m_styles[3]->leftBorderPen( 0, 0 ) );
	}
	else if ( y == r.right() )
        {
	    if ( m_styles[11] )
		cell->setRightBorderPen( m_styles[11]->leftBorderPen( 0, 0 ) );
	}
	else
        {
	    if ( m_styles[7] )
		cell->setRightBorderPen( m_styles[7]->leftBorderPen( 0, 0 ) );
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
	    if ( m_styles[12] )
		cell->setBottomBorderPen( m_styles[12]->topBorderPen( 0, 0 ) );
	}
	else if ( x == r.right() )
        {
	    if ( m_styles[14] )
		cell->setBottomBorderPen( m_styles[14]->topBorderPen( 0, 0 ) );
	}
	else
        {
	    if ( m_styles[13] )
		cell->setBottomBorderPen( m_styles[13]->topBorderPen( 0, 0 ) );
	}
        }
    }
#endif

    m_view->selectionInfo()->initialize(r);//,       m_view->activeSheet() );
    m_view->doc()->setModified( true );
    m_view->slotUpdateView( m_view->activeSheet() );
    accept();
}

bool FormatDialog::parseXML( const KoXmlDocument& doc )
{
    for( int i = 0; i < 16; ++i )
    {
	delete m_styles[ i ];
	m_styles[ i ] = 0;
    }

    KoXmlElement e = doc.documentElement().firstChild().toElement();
    for( ; !e.isNull(); e = e.nextSibling().toElement() )
    {
        if ( e.tagName() == "cell" )
        {
            Style* style = new Style();
            style->setDefault();
            KoXmlElement tmpElement( e.namedItem("format").toElement() );
            if ( !style->loadXML( tmpElement ) )
                return false;

            int row = e.attribute("row").toInt();
            int column = e.attribute("column").toInt();
            int i = (row-1)*4 + (column-1);
            if ( i < 0 || i >= 16 )
                return false;

            m_styles[ i ] = style;
        }
    }

    return true;
}

#include "FormatDialog.moc"
