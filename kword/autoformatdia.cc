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

#include "kwdoc.h"
#include "kwcanvas.h"
#include "autoformatdia.h"
#include "autoformatdia.moc"
#include "autoformat.h"
#include "kcharselectdia.h"

#include <klocale.h>
#include <kcharselect.h>

#include <stdlib.h>

#include <qlayout.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qfont.h>
#include <qlistview.h>
#include <qmap.h>

/******************************************************************/
/* Class: KWAutoFormatDia                                         */
/******************************************************************/

/*================================================================*/
KWAutoFormatDia::KWAutoFormatDia( QWidget *parent, const char *name, KWDocument *_doc, KWCanvas *_canvas )
    : KDialogBase( Tabbed, i18n("Autocorrection"), Ok | Cancel, Ok, parent, name, true),
      doc( _doc ),
      oBegin( doc->getAutoFormat().getConfigTypographicQuotes().begin ),
      oEnd( doc->getAutoFormat().getConfigTypographicQuotes().end ),
      quotesChanged( false ),
      m_autoFormat(doc->getAutoFormat())
{
    canvas = _canvas;

    setupTab1();
//    setupTab2();

    setInitialSize( QSize(500, 300) );
}

/*================================================================*/
void KWAutoFormatDia::setupTab1()
{
    tab1 = addPage( i18n( "Simple Autocorrection" ) );
    QVBoxLayout *grid = new QVBoxLayout(tab1, 10, 5);
    grid->setAutoAdd( true );

    cbTypographicQuotes = new QCheckBox( tab1 );
    cbTypographicQuotes->setText( i18n( "Replace &Quotes by Typographical Quotes:" ) );
    cbTypographicQuotes->resize( cbTypographicQuotes->sizeHint() );

    cbTypographicQuotes->setChecked( m_autoFormat.getConfigTypographicQuotes().replace );

    QHBox *quotes = new QHBox( tab1 );
    quotes->setSpacing( 5 );
    pbQuote1 = new QPushButton( quotes );

    pbQuote1->setText( oBegin );

    pbQuote1->resize( pbQuote1->sizeHint() );
    pbQuote2 = new QPushButton( quotes );

    pbQuote2->setText(oEnd );

    pbQuote2->resize( pbQuote2->sizeHint() );
    ( void )new QWidget( quotes );
    quotes->setMaximumHeight( pbQuote1->sizeHint().height() );

    connect( pbQuote1, SIGNAL( clicked() ), this, SLOT( chooseQuote1() ) );
    connect( pbQuote2, SIGNAL( clicked() ), this, SLOT( chooseQuote2() ) );

    ( void )new QWidget( tab1 );

    cbUpperCase = new QCheckBox( tab1 );
    cbUpperCase->setText( i18n( "Convert first letter from the first word of a sentence automatically\n"
                                "to &Upper Case ( e.g. \"bla. this is a Test\" to \"bla. This is a Test\" )" ) );
    cbUpperCase->resize( cbUpperCase->sizeHint() );

    cbUpperCase->setChecked( m_autoFormat.getConfigUpperCase() );

    ( void )new QWidget( tab1 );

    cbUpperUpper = new QCheckBox( tab1 );
    cbUpperUpper->setText( i18n( "Convert two Upper &Case letters to one Upper Case and one Lower Case letter.\n"
                                 "( e.g. HEllo to Hello )" ) );
    cbUpperUpper->resize( cbUpperUpper->sizeHint() );
    cbUpperUpper->setChecked( m_autoFormat.getConfigUpperUpper() );
    ( void )new QWidget( tab1 );
}

/*================================================================*/
void KWAutoFormatDia::setupTab2()
{
    tab2 = addPage( i18n( "Advanced Autocorrection" ) );
    QHBoxLayout *grid = new QHBoxLayout(tab2, 10, 5);
    grid->setAutoAdd( true );

    entries = new QListView( tab2 );
    entries->addColumn( i18n( "Find" ) );
    entries->addColumn( i18n( "Replace" ) );

    QMap< QString, KWAutoFormatEntry >::Iterator it = m_autoFormat.firstAutoFormatEntry();
    for ( ; it != m_autoFormat.lastAutoFormatEntry(); ++it )
        ( void )new QListViewItem( entries, it.key(), it.data().getReplace() );

    QVBox *buttons = new QVBox( tab2 );
    buttons->setSpacing( 5 );
    buttons->setMargin( 5 );

    pbAdd = new QPushButton( i18n( "Add..."), buttons  );
    pbRemove = new QPushButton( i18n( "Remove" ), buttons );
    ( void )new QWidget( buttons );
    pbEdit = new QPushButton( i18n( "Edit..." ), buttons );
    ( void )new QWidget( buttons );
}

/*================================================================*/
bool KWAutoFormatDia::applyConfig()
{
    // iiiiiiiiigit - that's a hack!
    if ( quotesChanged )
    {
        KWAutoFormat::TypographicQuotes tq = m_autoFormat.getConfigTypographicQuotes();
        tq.replace = false;
        m_autoFormat.configTypographicQuotes( tq );
        m_autoFormat.setEnabled( true );
        //        doc->recalcWholeText();
        m_autoFormat.setEnabled( false );
    }

    KWAutoFormat::TypographicQuotes tq = m_autoFormat.getConfigTypographicQuotes();
    tq.replace = cbTypographicQuotes->isChecked();
    tq.begin = pbQuote1->text()[ 0 ];
    tq.end = pbQuote2->text()[ 0 ];
    doc->getAutoFormat().configTypographicQuotes( tq );

    doc->getAutoFormat().configUpperCase( cbUpperCase->isChecked() );
    doc->getAutoFormat().configUpperUpper( cbUpperUpper->isChecked() );

    doc->getAutoFormat().setEnabled( true );
    doc->updateAllViews( 0L );
    doc->updateAllCursors();
    doc->getAutoFormat().setEnabled( false );

    return true;
}

void KWAutoFormatDia::slotOk()
{
    if (applyConfig())
    {
       KDialogBase::slotOk();
    }
}

/*================================================================*/
void KWAutoFormatDia::chooseQuote1()
{
    QString f = font().family();
    QChar c = oBegin;
    if ( KCharSelectDia::selectChar( f, c, false ) )
    {
        pbQuote1->setText( c );
        quotesChanged = true;
    }
}

/*================================================================*/
void KWAutoFormatDia::chooseQuote2()
{
    QString f = font().family();
    QChar c = oEnd;
    if ( KCharSelectDia::selectChar( f, c, false ) )
    {
        pbQuote2->setText( c );
        quotesChanged = true;
    }
}
