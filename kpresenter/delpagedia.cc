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

#include "kpresenter_doc.h"
#include "delpagedia.h"
#include "delpagedia.moc"

#include <qlabel.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qlayout.h>

#include <kapp.h>
#include <klocale.h>

/******************************************************************/
/* class DelPageDia                                               */
/******************************************************************/

/*==================== constructor ===============================*/
DelPageDia::DelPageDia( QWidget* parent, const char* name, KPresenterDoc *_doc, int currPageNum )
    : QDialog( parent, name, true )
{
    doc = _doc;

    grid = new QGridLayout( this, 7, 5, 15, 7 );

    label = new QLabel( i18n( "Delete Page Number: " ), this );
    label->resize( label->sizeHint() );
    grid->addWidget( label, 0, 0 );

    spinBox = new QSpinBox( 1, doc->getPageNums(), 1, this );
    spinBox->setValue( currPageNum );
    spinBox->resize( spinBox->sizeHint() );
    grid->addWidget( spinBox, 0, 1 );

    leave = new QRadioButton( i18n( "&Leave all objects untouched." ), this );
    leave->resize( leave->sizeHint() );
    connect( leave, SIGNAL( clicked() ), this, SLOT( leaveClicked() ) );
    grid->addMultiCellWidget( leave, 1, 1, 0, 4 );

    _move = new QRadioButton( i18n( "&Move the objects which are behind the deleted page \n"
                                    "one page backwards, so that they stay on their current page, \n"
                                    "and don't touch the objects, which are on the deleted page." ), this );
    _move->resize( _move->sizeHint() );
    connect( _move, SIGNAL( clicked() ), this, SLOT( moveClicked() ) );
    grid->addMultiCellWidget( _move, 2, 2, 0, 4 );

    del = new QRadioButton( i18n( "&Delete the objects which are on the deleted page and \n"
                                  "leave the other objects untouched." ), this );
    del->resize( del->sizeHint() );
    connect( del, SIGNAL( clicked() ), this, SLOT( delClicked() ) );
    grid->addMultiCellWidget( del, 3, 3, 0, 4 );

    move_del = new QRadioButton( i18n( "M&ove the objects which are behind the deleted page \n"
                                       "one page backwards, so that they stay on their current page, \n"
                                       "and delete the objects which are on the deleted page." ), this );
    move_del->resize( move_del->sizeHint() );
    connect( move_del, SIGNAL( clicked() ), this, SLOT( moveDelClicked() ) );
    grid->addMultiCellWidget( move_del, 4, 4, 0, 4 );

    cancel = new QPushButton( this, "BCancel" );
    cancel->setText( i18n( "Cancel" ) );
    cancel->resize( cancel->sizeHint() );
    connect( cancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    grid->addWidget( cancel, 6, 4 );

    ok = new QPushButton( this, "BOK" );
    ok->setText( i18n( "OK" ) );
    ok->setAutoRepeat( false );
    ok->setAutoResize( false );
    ok->setAutoDefault( true );
    ok->setDefault( true );
    ok->resize( cancel->size() );
    connect( ok, SIGNAL( clicked() ), this, SLOT( okClicked() ) );
    connect( ok, SIGNAL( clicked() ), this, SLOT( accept() ) );
    grid->addWidget( ok, 6, 3 );

//   grid->addRowSpacing( 0, spinBox->height() );
//   grid->addRowSpacing( 1, leave->height() );
//   grid->addRowSpacing( 2, _move->height() );
//   grid->addRowSpacing( 3, del->height() );
//   grid->addRowSpacing( 4, move_del->height() );
//   grid->addRowSpacing( 5, 0 );
//   grid->addRowSpacing( 6, ok->height() );
//   grid->setRowStretch( 5, 1 );

//   grid->addColSpacing( 0, label->width() );
//   grid->addColSpacing( 1, spinBox->width() );
//   grid->addColSpacing(2,max(max(leave->width(),_move->width()),max(del->width(),move_del->width())) -
//            ( label->width() + spinBox->width() + ok->width() + cancel->width() + 30 ) );
//   grid->addColSpacing( 3, ok->width() );
//   grid->addColSpacing( 4, cancel->width() );
//   grid->setColStretch( 2, 1 );

//   grid->activate();

    uncheckAll();
    move_del->setChecked( true );
}

/*================================================================*/
void DelPageDia::uncheckAll()
{
    leave->setChecked( false );
    _move->setChecked( false );
    del->setChecked( false );
    move_del->setChecked( false );
}

/*================================================================*/
void DelPageDia::leaveClicked()
{
    uncheckAll();
    leave->setChecked( true );
}

/*================================================================*/
void DelPageDia::moveClicked()
{
    uncheckAll();
    _move->setChecked( true );
}

/*================================================================*/
void DelPageDia::delClicked()
{
    uncheckAll();
    del->setChecked( true );
}

/*================================================================*/
void DelPageDia::moveDelClicked()
{
    uncheckAll();
    move_del->setChecked( true );
}

/*================================================================*/
void DelPageDia::okClicked()
{
    DelPageMode dpl = DPM_LET_OBJS;

    if ( leave->isChecked() )
        dpl = DPM_LET_OBJS;
    else if ( _move->isChecked() )
        dpl = DPM_MOVE_OBJS;
    else if ( del->isChecked() )
        dpl = DPM_DEL_OBJS;
    else if ( move_del->isChecked() )
        dpl = DPM_DEL_MOVE_OBJS;

    emit deletePage( spinBox->value() - 1, dpl );
}

