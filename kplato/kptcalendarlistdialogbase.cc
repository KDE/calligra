/* This file is part of the KDE project
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <klocale.h>
#include "kptcalendarlistdialogbase.h"
#include "kptcalendaredit.h"

#include <qvariant.h>
#include <qheader.h>
#include <qlistview.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtable.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qlabel.h>
#include <qcombobox.h>

#include <kdebug.h>

namespace KPlato
{

/*
 *  Constructs a KPTCalendarListDialogBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
KPTCalendarListDialogBase::KPTCalendarListDialogBase( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "KPTCalendarListDialogBase" );
    KPTCalendarListDialogBaseLayout = new QHBoxLayout( this, 11, 6, "KPTCalendarListDialogBaseLayout");

    layout6 = new QGridLayout( 0, 1, 1, 0, 6, "layout6");

    calendarList = new QListView( this, "calendarList" );
    calendarList->setMaximumWidth(150);
    calendarList->addColumn( tr2i18n( "Calendar Name" ) );

    layout6->addMultiCellWidget( calendarList, 0, 0, 0, 1 );

    editName = new QLineEdit( this, "editName" );
    editName->setMaximumWidth(150);

    layout6->addMultiCellWidget( editName, 1, 1, 0, 1 );

    bDelete = new QPushButton( this, "bDelete" );
    bDelete->setMaximumWidth(75);

    layout6->addWidget( bDelete, 2, 1 );

    bAdd = new QPushButton( this, "bAdd" );
    bAdd->setMaximumWidth(75);

    layout6->addWidget( bAdd, 2, 0 );
    KPTCalendarListDialogBaseLayout->addLayout( layout6 );
    
    layout1 = new QVBoxLayout( 0, 0, 6, "layout1"); // 
    layout10 = new QHBoxLayout( 0, 0, 6, "layout10"); // label+basecalendar
    layout1->addLayout(layout10);
    
    textLabel2 = new QLabel( this, "textLabel2" );
    layout10->addWidget( textLabel2 );

    baseCalendar = new QComboBox( FALSE, this, "baseCalendar" );
    layout10->addWidget( baseCalendar );
    
    KPTCalendarListDialogBaseLayout->addLayout( layout1 );

    calendar = new KPTCalendarEdit( this, "calendar" );
    layout1->addWidget( calendar );
    KPTCalendarListDialogBaseLayout->setStretchFactor(layout1, 1);
    KPTCalendarListDialogBaseLayout->setStretchFactor(calendar, 2);
    languageChange();
    resize( QSize(598, 350).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

}

/*
 *  Destroys the object and frees any allocated resources
 */
KPTCalendarListDialogBase::~KPTCalendarListDialogBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void KPTCalendarListDialogBase::languageChange()
{
    setCaption( tr2i18n( "KPTCalendarListDialogBase" ) );
    calendarList->header()->setLabel( 0, tr2i18n( "Calendar name" ) );
    bDelete->setText( tr2i18n( "Remove" ) );
    bAdd->setText( tr2i18n( "Add" ) );
    textLabel2->setText( tr2i18n( "Base calendar:" ) );
}

} //Kplato namespace
#include "kptcalendarlistdialogbase.moc"
