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

#include "ksearchdialogs.h"
#include "ksearchdialogs.moc"

#include <qpushbt.h>
#include <qchkbox.h>
#include <qlined.h>
#include <qgrpbox.h>
#include <qlabel.h>

#include <kapp.h>
#include <klocale.h>

/******************************************************************/
/* Class: KSearchDialog                                           */
/******************************************************************/

KSearchDialog::KSearchDialog( QWidget *parent, const char *name )
    : QDialog( parent, name, true )
{
    setCaption( i18n( "Find" ) );

    frame = new QGroupBox( i18n( "Find" ), this, "frame" );

    value = new QLineEdit( frame, "value" );
    value->setFocus();
    connect( value, SIGNAL( returnPressed() ), this, SLOT( search() ) );

    sensitive = new QCheckBox( i18n( "Case Sensitive" ), frame, "case" );
    direction = new QCheckBox( i18n( "Find Backwards" ), frame, "direction" );

    ok = new QPushButton( i18n( "Find" ), this, "find" );
    connect( ok, SIGNAL( clicked() ), this, SLOT( search() ) );

    cancel = new QPushButton( i18n( "Done" ), this, "cancel" );
    connect( cancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

    value->resize( 400, value->sizeHint().height() );
    value->move( 15, 20 );
    frame->move( 10, 10 );
    sensitive->move( value->x(), value->y() + value->height() + 10 );
    sensitive->resize( sensitive->sizeHint() );
    direction->resize( direction->sizeHint() );
    direction->move( value->x() + value->width() - direction->width(), sensitive->y() );
    frame->resize( direction->x() + direction->width() + 15, direction->y() + direction->height() + 15 );
    cancel->resize( cancel->sizeHint() );
    ok->resize( ok->sizeHint() );
    cancel->move( frame->x() + frame->width() - cancel->width(), frame->y() + frame->height() + 15 );
    ok->move( cancel->x() - cancel->width() - 5, cancel->y() );

    resize( cancel->x() + cancel->width() + 10, cancel->y() + cancel->height() + 10 );
}

/******************************************************************/
/* Class: KSearchReplaceDialog                                    */
/******************************************************************/

KSearchReplaceDialog::KSearchReplaceDialog( QWidget *parent, const char *name )
    : QDialog( parent, name, true )
{
    setCaption( i18n( "Find & Replace" ) );

    frame = new QGroupBox( i18n( "Find" ), this, "frame" );

    search = new QLineEdit( frame, "search" );
    search->setFocus();
    connect( search, SIGNAL( returnPressed() ), this, SLOT( gotoReplace() ) );

    replace = new QLineEdit( frame, "replace" );
    connect( replace, SIGNAL( returnPressed() ), this, SLOT( searchReplace() ) );

    label = new QLabel( "Replace:", frame );

    sensitive = new QCheckBox( i18n( "Case Sensitive" ), frame, "case" );
    direction = new QCheckBox( i18n( "Find Backwards" ), frame, "direction" );

    replace_all = new QPushButton( i18n( "Replace all" ), this, "find" );
    connect( replace_all, SIGNAL( clicked() ), this, SLOT( searchReplaceAll() ) );

    ok = new QPushButton( i18n( "Replace" ), this, "find" );
    connect( ok, SIGNAL( clicked() ), this, SLOT( searchReplace() ) );

    cancel = new QPushButton( i18n( "Done" ), this, "cancel" );
    connect( cancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

    search->resize( 400, search->sizeHint().height() );
    search->move( 15, 20 );
    label->resize( label->sizeHint() );
    label->move( search->x(), search->y() + search->height() + 10 );
    replace->resize( 400, replace->sizeHint().height() );
    replace->move( search->x(), label->y() + label->height() + 5 );
    frame->move( 10, 10 );
    sensitive->move( replace->x(), replace->y() + replace->height() + 10 );
    sensitive->resize( sensitive->sizeHint() );
    direction->resize( direction->sizeHint() );
    direction->move( replace->x() + replace->width() - direction->width(), sensitive->y() );
    frame->resize( direction->x() + direction->width() + 15, direction->y() + direction->height() + 15 );
    cancel->resize( cancel->sizeHint() );
    ok->resize( ok->sizeHint() );
    replace_all->resize( ok->sizeHint() );
    cancel->move( frame->x() + frame->width() - cancel->width(), frame->y() + frame->height() + 15 );
    ok->move( cancel->x() - ok->width() - 5, cancel->y() );
    replace_all->move( ok->x() - replace_all->width() - 5, cancel->y() );

    resize( cancel->x() + cancel->width() + 10, cancel->y() + cancel->height() + 10 );
}

