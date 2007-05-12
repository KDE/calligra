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

// Local
#include "FormatDialog.h"

#include <QComboBox>
#include <QFile>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
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

#include "commands/AutoFormatCommand.h"

using namespace KSpread;

FormatDialog::FormatDialog( View* view, const char* name )
    : KDialog( view )
{
    setCaption( i18n("Sheet Style") );
    setObjectName( name );
    setModal( true );
    setButtons( Ok|Cancel );

    for ( int i = 0; i < 16; ++i )
        m_styles.append( 0 );

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
        KConfig config( *it, KConfig::OnlyLocal );
        const KConfigGroup sheetStyleGroup = config.group( "Sheet-Style" );

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

    //
    // Set colors, borders etc.
    //
    AutoFormatCommand* command = new AutoFormatCommand();
    command->setSheet( m_view->activeSheet() );
    command->setStyles( m_styles );
    command->add( *m_view->selection() );
    command->execute();

    accept();
}

bool FormatDialog::parseXML( const KoXmlDocument& doc )
{
    qDeleteAll( m_styles );

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
