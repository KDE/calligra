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

#include "kcharselectdia.h"
#include "kcharselectdia.moc"

#include <qpushbutton.h>
#include <qlayout.h>
#include <qstrlist.h>
#include <qlist.h>

#include <kbuttonbox.h>
#include <kapp.h>
#include <kcharselect.h>

#include <stdio.h>

/******************************************************************/
/* class KCharSelectDia                                           */
/******************************************************************/

/*================================================================*/
KCharSelectDia::KCharSelectDia( QWidget *parent, const char *name, const QChar &_chr, const QString &_font, bool _enableFont )
    : QDialog( parent, name, true )
{
    setCaption( "Select a character" );

    grid = new QGridLayout( this, 3, 1, 15, 7 );

    charSelect = new KCharSelect( this, "", _font, _chr );
    charSelect->resize( charSelect->sizeHint() );
    charSelect->enableFontCombo( _enableFont );
    grid->addWidget( charSelect, 0, 0 );

    grid->addWidget( new QWidget( this ), 1, 0 );

    bbox = new KButtonBox( this, KButtonBox::HORIZONTAL, 7 );
    bbox->addStretch( 20 );
    bOk = bbox->addButton( "OK" );
    bOk->setAutoRepeat( false );
    bOk->setAutoResize( false );
    bOk->setAutoDefault( true );
    bOk->setDefault( true );
    connect( bOk, SIGNAL( clicked() ), SLOT( accept() ) );
    bCancel = bbox->addButton( "Cancel" );
    connect( bCancel, SIGNAL( clicked() ), SLOT( reject() ) );
    bbox->layout();
    grid->addWidget( bbox, 2, 0 );

    grid->addColSpacing( 0, charSelect->width() );

    grid->addRowSpacing( 0, charSelect->height() );
    grid->addRowSpacing( 1, 0 );
    grid->addRowSpacing( 2, bCancel->height() );
    grid->setRowStretch( 0, 0 );
    grid->setRowStretch( 1, 1 );
    grid->setRowStretch( 2, 0 );

    grid->activate();

    charSelect->setFocus();
}

/*================================================================*/
bool KCharSelectDia::selectChar( QString &_font, QChar &_chr, bool _enableFont )
{
    bool res = false;

    KCharSelectDia *dlg = new KCharSelectDia( 0L, "Select Character", _chr, _font, _enableFont );

    if ( dlg->exec() == QDialog::Accepted )
    {
        _font = dlg->font();
        _chr = dlg->chr();
        res = true;
    }

    delete dlg;

    return res;
}

/*================================================================*/
QChar KCharSelectDia::chr()
{
    return charSelect->chr();
}

/*================================================================*/
QString KCharSelectDia::font()
{
    return charSelect->font();
}
