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

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qstrlist.h>
#include <qwidget.h>
#include <qframe.h>
#include <qcheckbox.h>
#include <qpalette.h>

#include <kbuttonbox.h>
#include <kapp.h>
#include <kcolorbtn.h>
#include <kcharselect.h>

#include <stdio.h>

/******************************************************************/
/* class KCharSelectDia                                           */
/******************************************************************/

/*================================================================*/
KCharSelectDia::KCharSelectDia( QWidget *parent, const char *name, QList<QFont> *_font, QList<QColor>* _color, QList<QChar>* _chr )
    : QDialog( parent, name, true )
{
    unsigned int i = 0;

    vFont = _font;
    vColor = _color;
    vChr = _chr;

    _depth = 0;

    setCaption( "Select a character" );

    grid = new QGridLayout( this, 3, 1, 15, 7 );

    charSelect = new KCharSelect( this, "", vFont->at( 0 )->family(), *vChr->at( 0 ) );
    charSelect->resize( charSelect->sizeHint() );
    grid->addWidget( charSelect, 0, 0 );
    connect( charSelect, SIGNAL( activated( const QChar & ) ), this, SLOT( charChanged( const QChar & ) ) );
    connect( charSelect, SIGNAL( fontChanged( const QString & ) ), this, SLOT( fontSelected( const QString & ) ) );

    wid = new QWidget( this );

    grid2 = new QGridLayout( wid, 2, 7, 0, 7 );

    lSize = new QLabel( "Size:", wid );
    lSize->resize( lSize->sizeHint() );
    grid2->addWidget( lSize, 0, 0 );

    lColor = new QLabel( "Color:", wid );
    lColor->resize( lColor->sizeHint() );
    grid2->addWidget( lColor, 0, 1 );

    sizeCombo = new QComboBox( false, wid );
    char chr[ 5 ];
    for ( i = 4; i <= 100; i++ )
    {
        sprintf( chr, "%d", i );
        sizeCombo->insertItem( chr, -1 );
    }
    sizeCombo->resize( sizeCombo->sizeHint() );
    grid2->addWidget( sizeCombo, 1, 0 );
    sizeCombo->setCurrentItem( vFont->at( 0 )->pointSize() - 4 );
    connect( sizeCombo, SIGNAL( activated( int ) ), this, SLOT( sizeSelected( int ) ) );

    colorButton = new KColorButton( *vColor->at( 0 ), wid );
    colorButton->resize( colorButton->sizeHint() );
    grid2->addWidget( colorButton, 1, 1 );
    connect( colorButton, SIGNAL( changed( const QColor& ) ), this, SLOT( colorChanged( const QColor& ) ) );

    lAttrib = new QLabel( "Attributes:", wid );
    lAttrib->resize( lAttrib->sizeHint() );
    grid2->addMultiCellWidget( lAttrib, 0, 0, 2, 4 );

    bold = new QCheckBox( "Bold", wid );
    bold->resize( bold->sizeHint() );
    grid2->addWidget( bold, 1, 2 );
    bold->setChecked( vFont->at( 0 )->bold() );
    connect( bold, SIGNAL( clicked() ), this, SLOT( boldChanged() ) );

    italic= new QCheckBox( "Italic", wid );
    italic->resize( italic->sizeHint() );
    grid2->addWidget( italic, 1, 3 );
    italic->setChecked( vFont->at( 0 )->italic() );
    connect( italic, SIGNAL( clicked() ), this, SLOT( italicChanged() ) );

    underl = new QCheckBox( "Underlined", wid );
    underl->resize( underl->sizeHint() );
    grid2->addWidget( underl, 1, 4 );
    underl->setChecked( vFont->at( 0 )->underline() );
    connect( underl, SIGNAL( clicked() ), this, SLOT( underlChanged() ) );

    lDepth = new QLabel( "Depth", wid );
    lDepth->resize( lDepth->sizeHint() );
    grid2->addWidget( lDepth, 0, 5 );

    depthCombo = new QComboBox( false, wid );
    for ( i = 1; i <= 16; i++ )
    {
        sprintf( chr, "%d", i );
        depthCombo->insertItem( chr, -1 );
    }
    depthCombo->resize( depthCombo->sizeHint() );
    grid2->addWidget( depthCombo, 1, 5 );
    connect( depthCombo, SIGNAL( activated( int ) ), this, SLOT( depthSelected( int ) ) );

    lPreview = new QLabel( wid );
    lPreview->setBackgroundColor( white );
    lPreview->resize( 7+colorButton->height()+lColor->height(),
                      7+colorButton->height()+lColor->height() );
    lPreview->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    lPreview->setText( *vChr->at( 0 ) );
    lPreview->setAlignment( AlignHCenter | AlignVCenter );
    lPreview->setFont( *vFont->at( 0 ) );
    colorChanged( *vColor->at( 0 ) );
    grid2->addMultiCellWidget( lPreview, 0, 1, 6, 6 );

    grid->addWidget( wid, 1, 0 );

    bbox = new KButtonBox( this, Horizontal, 7 );
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

    grid->setRowStretch( 1, 1 );

    charSelect->setFocus();
}

/*================================================================*/
bool KCharSelectDia::selectChar( QList<QFont> *_font, QList<QColor> *_color, QList<QChar> *_chr )
{
    bool res = false;

    KCharSelectDia *dlg = new KCharSelectDia( 0L, "Select Character", _font, _color, _chr );
    if ( dlg->exec() == QDialog::Accepted )
    {
        _font = dlg->font();
        _color = dlg->color();
        _chr = dlg->chr();
        res = true;
    }

    delete dlg;

    return res;
}

/*================================================================*/
void KCharSelectDia::fontSelected( const QString &_family )
{
    vFont->at( _depth )->setFamily( _family );
    lPreview->setFont( *vFont->at( _depth ) );
}

/*================================================================*/
void KCharSelectDia::sizeSelected( int i )
{
    vFont->at( _depth )->setPointSize( i + 4 );
    lPreview->setFont( *vFont->at( _depth ) );
}

/*================================================================*/
void KCharSelectDia::depthSelected( int d )
{
    _depth = d;

    lPreview->setText( *vChr->at( d ) );
    charSelect->setChar( *vChr->at( d ) );

    charSelect->setFont( vFont->at( d )->family().lower() );

    colorButton->setColor( *vColor->at( d ) );
    colorChanged( *vColor->at( d ) );

    bold->setChecked( vFont->at( d )->bold() );
    italic->setChecked( vFont->at( d )->italic() );
    underl->setChecked( vFont->at( d )->underline() );

    sizeCombo->setCurrentItem( vFont->at( d )->pointSize() - 4 );
}

/*================================================================*/
void KCharSelectDia::colorChanged( const QColor& _color )
{
    *vColor->at( _depth ) = _color;
    QColorGroup cgrp( _color, lPreview->colorGroup().background(),
                      lPreview->colorGroup().light(), lPreview->colorGroup().dark(),
                      lPreview->colorGroup().mid(), _color, lPreview->colorGroup().base() );
    QPalette p( cgrp, cgrp, cgrp );

    lPreview->setPalette( p );
}

/*================================================================*/
void KCharSelectDia::boldChanged()
{
    vFont->at( _depth )->setBold( bold->isChecked() );
    lPreview->setFont( *vFont->at( _depth ) );
}

/*================================================================*/
void KCharSelectDia::italicChanged()
{
    vFont->at( _depth )->setItalic( italic->isChecked() );
    lPreview->setFont( *vFont->at( _depth ) );
}

/*================================================================*/
void KCharSelectDia::underlChanged()
{
    vFont->at( _depth )->setUnderline( underl->isChecked() );
    lPreview->setFont( *vFont->at( _depth ) );
}

/*================================================================*/
void KCharSelectDia::charChanged( const QChar &_chr )
{
    lPreview->setText( _chr );
    *vChr->at( _depth ) = _chr;
}
