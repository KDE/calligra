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

#include "optiondia.h"
#include "optiondia.moc"

#include <qwidget.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qstring.h>
#include <qgroupbox.h>
#include <qvalidator.h>

#include <kapp.h>
#include <klocale.h>

#include <stdlib.h>

/******************************************************************/
/* class OptionDia                                                */
/******************************************************************/

/*==================== constructor ===============================*/
OptionDia::OptionDia( QWidget *parent, const char *name )
    :QTabDialog( parent, name, true )
{
    int col1 = 20, dummy, w, h;

    /* Tab: General */
    general = new QWidget( this, "general" );

    lRastX = new QLabel( general, "lRastX" );
    lRastX->setText( i18n( "Horizontal Raster: " ) );
    lRastX->move( col1, 20 );
    lRastX->resize( lRastX->sizeHint() );

    lRastY = new QLabel( general, "lRastY" );
    lRastY->setText( i18n( "Vertical Raster: " ) );
    lRastY->move( col1, lRastX->y()+lRastX->height()+15 );
    lRastY->resize( lRastY->sizeHint() );

    dummy = QMAX(lRastX->sizeHint().width(),lRastY->sizeHint().width());
    lRastX->resize( dummy, lRastX->height() );
    lRastY->resize( dummy, lRastY->height() );

    eRastX = new QLineEdit( general );
    eRastX->setValidator( new QIntValidator( eRastX ) );
    eRastX->setText( "20" );
    eRastX->move( lRastX->x()+lRastX->width()+10, lRastX->y() );
    eRastX->resize( eRastX->sizeHint().width()/2, eRastX->sizeHint().height() );
    eRastX->setMaxLength( 2 );

    eRastY = new QLineEdit( general );
    eRastY->setValidator( new QIntValidator( eRastY ) );
    eRastY->setText( "20" );
    eRastY->move( lRastY->x()+lRastY->width()+10, lRastY->y() );
    eRastY->resize( eRastY->sizeHint().width()/2, eRastY->sizeHint().height() );
    eRastY->setMaxLength( 2 );

    lRastX->resize( lRastX->width(), eRastX->height() );
    lRastY->resize( lRastY->width(), eRastY->height() );

    general->resize( 20 + eRastX->x() + eRastX->width(), 20 + eRastY->y() + eRastY->height() );

    /* Tab: Objects */
    objects = new QWidget( this, "objects" );

    txtObj = new QGroupBox( i18n( "Objects in Editing-Mode" ), objects, "txtObjs" );
    txtObj->move( col1, col1 );

    lBackCol = new QLabel( txtObj, "lBackCol" );
    lBackCol->setText( i18n( "Backgroud color:" ) );
    lBackCol->move( 10, 20 );
    lBackCol->resize( lBackCol->sizeHint() );

    bBackCol = new KColorButton( white, txtObj, "bBackCol" );
    bBackCol->setGeometry( lBackCol->x() + lBackCol->width() + 10, lBackCol->y(), 100, 25 );

    txtObj->resize( bBackCol->x() + bBackCol->width() + 20, bBackCol->y() + bBackCol->height() + 10 );

    objects->resize( txtObj->width() + 40, txtObj->height() + 40 );

    w = QMAX(objects->width(),general->width());
    h = QMAX(objects->height(),general->height());


    resize( 300, 200 );

    /* build dialog */
    addTab( general, i18n( "General" ) );
//     addTab( objects, i18n( "Objects" ) );
    objects->hide();
    
    setCancelButton( i18n( "Cancel" ) );
    setApplyButton( i18n( "Apply" ) );
    setOkButton( i18n( "OK" ) );
}

/*===================== destructor ===============================*/
OptionDia::~OptionDia()
{
}
