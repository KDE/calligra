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

#include "spacingdia.h"
#include "spacingdia.moc"

#include <qlabel.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qvalidator.h>
#include <qlineedit.h>

#include <kapp.h>
#include <klocale.h>

#include <stdio.h>
#include <stdlib.h>

/******************************************************************/
/* class SpacingDia                                               */
/******************************************************************/

/*==================== constructor ===============================*/
SpacingDia::SpacingDia( QWidget* parent, int _lineSpacing, int _distBefore, int _distAfter, int _gap )
    : QDialog( parent, "", true )
{
    QString str;

    lLineSpacing = new QLabel( i18n( "Line Spacing ( pt )" ), this );
    lLineSpacing->move( 10, 10 );
    lLineSpacing->resize( lLineSpacing->sizeHint() );

    eLineSpacing = new QLineEdit( this );
    eLineSpacing->setValidator( new QIntValidator( eLineSpacing ) );
    eLineSpacing->move( lLineSpacing->width() + 15, 10 );
    eLineSpacing->resize( eLineSpacing->sizeHint().width() / 2, eLineSpacing->sizeHint().height() );
    str=QString::number( _lineSpacing );
    eLineSpacing->setText( str );

    lDistBefore = new QLabel( i18n( "Distance before the Paragraph ( pt )" ), this );
    lDistBefore->move( 10, eLineSpacing->y() + eLineSpacing->height() + 10 );
    lDistBefore->resize( lDistBefore->sizeHint() );

    eDistBefore = new QLineEdit( this );
    eDistBefore->setValidator( new QIntValidator( eDistBefore ) );
    eDistBefore->move( lDistBefore->width() + 15, lDistBefore->y() );
    eDistBefore->resize( eDistBefore->sizeHint().width() / 2, eDistBefore->sizeHint().height() );
    str=QString::number( _distBefore );
    eDistBefore->setText( str );

    lDistAfter = new QLabel( i18n( "Distance after the Paragraph ( pt )" ), this );
    lDistAfter->move( 10, eDistBefore->y() + eDistBefore->height() + 10 );
    lDistAfter->resize( lDistAfter->sizeHint() );

    eDistAfter = new QLineEdit( this );
    eDistAfter->setValidator( new QIntValidator( eDistAfter ) );
    eDistAfter->move( lDistAfter->width() + 15, lDistAfter->y() );
    eDistAfter->resize( eDistAfter->sizeHint().width() / 2, eDistAfter->sizeHint().height() );
    str=QString::number( _distAfter );
    eDistAfter->setText( str );

    lGap = new QLabel( i18n( "Gap ( pt )" ), this );
    lGap->move( 10, eDistAfter->y() + eDistAfter->height() + 10 );
    lGap->resize( lGap->sizeHint() );

    eGap = new QLineEdit( this );
    eGap->setValidator( new QIntValidator( eGap ) );
    eGap->move( lGap->width() + 15, lGap->y() );
    eGap->resize( eGap->sizeHint().width() / 2, eGap->sizeHint().height() );
    str=QString::number( _gap );
    eGap->setText( str );

    int m = QMAX( QMAX( QMAX( eGap->x(), eDistBefore->x() ), eDistAfter->x() ), eLineSpacing->x() );

    eLineSpacing->move( m, eLineSpacing->y() );
    eDistBefore->move( m, eDistBefore->y() );
    eDistAfter->move( m, eDistAfter->y() );
    eGap->move( m, eGap->y() );

    cancelBut = new QPushButton( this, "BCancel" );
    cancelBut->setText( i18n( "Cancel" ) );

    okBut = new QPushButton( this, "BOK" );
    okBut->setText( i18n( "OK" ) );
    okBut->setAutoRepeat( false );
    okBut->setAutoResize( false );
    okBut->setAutoDefault( true );
    okBut->setDefault( true );

    int butW = QMAX( cancelBut->sizeHint().width() , okBut->sizeHint().width() );
    int butH = cancelBut->sizeHint().height();

    cancelBut->resize( butW, butH );
    okBut->resize( butW, butH );

    cancelBut->move( eGap->x() + eGap->width() - cancelBut->width(), eGap->y() + eGap->height() + 20 );
    okBut->move( cancelBut->x() - okBut->width() - 5, cancelBut->y() );

    connect( okBut, SIGNAL( clicked() ), this, SLOT( slotSpacingDiaOk() ) );
    connect( cancelBut, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( okBut, SIGNAL( clicked() ), this, SLOT( accept() ) );

    resize( cancelBut->x() + cancelBut->width() + 10, okBut->y()+okBut->height()+10 );
}

/*====================== spacing dia ok ==========================*/
void SpacingDia::slotSpacingDiaOk()
{
    int _lineSpacing = atoi( eLineSpacing->text() );
    int _distBefore = atoi( eDistBefore->text() );
    int _distAfter = atoi( eDistAfter->text() );
    int _gap = atoi( eGap->text() );

    emit spacingDiaOk( _lineSpacing, _distBefore, _distAfter, _gap );
}












