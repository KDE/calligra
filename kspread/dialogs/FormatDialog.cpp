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
   Boston, MA 02110-1301, USA.
*/

#include <QComboBox>
#include <QFile>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
//Added by qt3to4:
#include <QVBoxLayout>
#include <QPixmap>


#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kconfig.h>

#include "Cell.h"
#include "CellStorage.h"
#include "Doc.h"
#include "Localization.h"
#include "Sheet.h"
#include "Style.h"
#include "StyleManager.h"
#include "Undo.h"
#include "View.h"
#include "Selection.h"

#include "FormatDialog.h"

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


    QStringList lst = Factory::global().dirs()->findAllResources( "sheet-styles", "*.ksts", KStandardDirs::Recursive );

    int index = 0;
    QStringList::Iterator it = lst.begin();
    for( ; it != lst.end(); ++it )
    {
        const KConfigGroup sheetStyleGroup = KConfig( *it, KConfig::OnlyLocal ).group( "Sheet-Style" );

        Entry e;
        e.config = *it;
        e.xml = sheetStyleGroup.readEntry( "XML" );
        e.image = sheetStyleGroup.readEntry( "Image" );
        e.name = sheetStyleGroup.readEntry( "Name" );

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

    QString img = Factory::global().dirs()->findResource( "sheet-styles", m_entries[ index ].image );
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

    QString xml = Factory::global().dirs()->findResource( "sheet-styles", m_entries[ m_combo->currentIndex() ].xml );
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

#ifdef __GNUC__
#warning FIXME Stefan: port to new style storage
#endif
#if 0
    if ( !m_view->doc()->undoLocked() )
    {
        QString title = i18n( "Change Format" );
        UndoCellFormat* undo = new UndoCellFormat( m_view->doc(), m_view->activeSheet(), *m_view->selection(), title );
        m_view->doc()->addCommand( undo );
    }
#endif
    //
    // Set colors, borders etc.
    //
    Sheet* const sheet = m_view->activeSheet();
    Region::ConstIterator end(m_view->selection()->constEnd());
    for ( Region::ConstIterator it(m_view->selection()->constBegin()); it != end; ++it )
    {
        const QRect rect = (*it)->rect();
        // Top left corner
        if ( m_styles[0] && !m_styles[0]->isDefault() )
            sheet->cellStorage()->setStyle( Region(rect.topLeft()), *m_styles[0] );
        // Top column
        for( int col = rect.left() + 1; col <= rect.right(); ++col )
        {
            int pos = 1 + ( ( col - rect.left() - 1 ) % 2 );
            Cell cell( sheet, col, rect.top() );
            if ( !cell.isPartOfMerged() )
            {
                if ( m_styles[pos] && !m_styles[pos]->isDefault() )
                    sheet->cellStorage()->setStyle( Region(col, rect.top()), *m_styles[pos] );

                Style* style = ( col == rect.right() ) ? m_styles[2] : m_styles[1];
                if ( style )
                {
                    Style tmpStyle;
                    tmpStyle.setTopBorderPen( style->topBorderPen() );
                    sheet->cellStorage()->setStyle( Region(col, rect.top()), tmpStyle );
                }

                style = ( col == rect.left() + 1 ) ? m_styles[1] : m_styles[2];
                if ( style )
                {
                    Style tmpStyle;
                    tmpStyle.setLeftBorderPen( style->leftBorderPen() );
                    sheet->cellStorage()->setStyle( Region(col, rect.top()), tmpStyle );
                }
            }
        }

        if ( m_styles[3] )
        {
            Style tmpStyle;
            tmpStyle.setRightBorderPen( m_styles[3]->leftBorderPen() );
            sheet->cellStorage()->setStyle( Region(rect.topRight()), tmpStyle );
        }

        // Left row
        for ( int row = rect.top() + 1; row <= rect.bottom(); ++row )
        {
            int pos = 4 + ( ( row - rect.top() - 1 ) % 2 ) * 4;
            Cell cell( sheet, rect.left(), row );
            if ( !cell.isPartOfMerged() )
            {
                if ( m_styles[pos] && !m_styles[pos]->isDefault() )
                    sheet->cellStorage()->setStyle( Region(rect.left(), row), *m_styles[pos] );

                Style* style = ( row == rect.bottom() ) ? m_styles[8] : m_styles[4];
                if ( style )
                {
                    Style tmpStyle;
                    tmpStyle.setLeftBorderPen( style->leftBorderPen() );
                    sheet->cellStorage()->setStyle( Region(rect.left(), row), tmpStyle );
                }

                style = ( row == rect.top() + 1 ) ? m_styles[4] : m_styles[8];
                if ( style )
                {
                    Style tmpStyle;
                    tmpStyle.setTopBorderPen( style->topBorderPen() );
                    sheet->cellStorage()->setStyle( Region(rect.left(), row), tmpStyle );
                }
            }
        }

        // Body
        for ( int col = rect.left() + 1; col <= rect.right(); ++col )
        {
            for ( int row = rect.top() + 1; row <= rect.bottom(); ++row )
            {
                int pos = 5 + ( ( row - rect.top() - 1 ) % 2 ) * 4 + ( ( col - rect.left() - 1 ) % 2 );
                Cell cell( sheet, col, row );
                if ( !cell.isPartOfMerged() )
                {
                    if ( m_styles[pos] && !m_styles[pos]->isDefault() )
                        sheet->cellStorage()->setStyle( Region(col, row), *m_styles[pos] );

                    Style* style;
                    if ( col == rect.left() + 1 )
                        style = m_styles[ 5 + ( ( row - rect.top() - 1 ) % 2 ) * 4 ];
                    else
                        style = m_styles[ 6 + ( ( row - rect.top() - 1 ) % 2 ) * 4 ];

                    if ( style )
                    {
                        Style tmpStyle;
                        tmpStyle.setLeftBorderPen( style->leftBorderPen() );
                        sheet->cellStorage()->setStyle( Region(col, row), tmpStyle );
                    }

                    if ( row == rect.top() + 1 )
                        style = m_styles[ 5 + ( ( col - rect.left() - 1 ) % 2 ) ];
                    else
                        style = m_styles[ 9 + ( ( col - rect.left() - 1 ) % 2 ) ];

                    if ( style )
                    {
                        Style tmpStyle;
                        tmpStyle.setTopBorderPen( style->topBorderPen() );
                        sheet->cellStorage()->setStyle( Region(col, row), tmpStyle );
                    }
                }
            }
        }

        // Outer right border
        for ( int row = rect.top(); row <= rect.bottom(); ++row )
        {
            Cell cell( sheet, rect.right(), row );
            if ( !cell.isPartOfMerged() )
            {
                if ( row == rect.top() )
                {
                    if ( m_styles[3] )
                    {
                        Style tmpStyle;
                        tmpStyle.setRightBorderPen( m_styles[3]->leftBorderPen() );
                        sheet->cellStorage()->setStyle( Region(rect.right(), row), tmpStyle );
                    }
                }
                else if ( row == rect.right() )
                {
                    if ( m_styles[11] )
                    {
                        Style tmpStyle;
                        tmpStyle.setRightBorderPen( m_styles[11]->leftBorderPen() );
                        sheet->cellStorage()->setStyle( Region(rect.right(), row), tmpStyle );
                    }
                }
                else
                {
                    if ( m_styles[7] )
                    {
                        Style tmpStyle;
                        tmpStyle.setRightBorderPen( m_styles[7]->leftBorderPen() );
                        sheet->cellStorage()->setStyle( Region(rect.right(), row), tmpStyle );
                    }
                }
            }
        }

        // Outer bottom border
        for ( int col = rect.left(); col <= rect.right(); ++col )
        {
            Cell cell( sheet, col, rect.bottom() );
            if(!cell.isPartOfMerged())
            {
                if ( col == rect.left() )
                {
                    if ( m_styles[12] )
                    {
                        Style tmpStyle;
                        tmpStyle.setBottomBorderPen( m_styles[12]->topBorderPen() );
                        sheet->cellStorage()->setStyle( Region(col, rect.bottom()), tmpStyle );
                    }
                }
                else if ( col == rect.right() )
                {
                    if ( m_styles[14] )
                    {
                        Style tmpStyle;
                        tmpStyle.setBottomBorderPen( m_styles[14]->topBorderPen() );
                        sheet->cellStorage()->setStyle( Region(col, rect.bottom()), tmpStyle );
                    }
                }
                else
                {
                    if ( m_styles[13] )
                    {
                        Style tmpStyle;
                        tmpStyle.setBottomBorderPen( m_styles[13]->topBorderPen() );
                        sheet->cellStorage()->setStyle( Region(col, rect.bottom()), tmpStyle );
                    }
                }
            }
        }
    }
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
