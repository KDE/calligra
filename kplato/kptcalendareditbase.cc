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

#include "kptcalendareditbase.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <kptcalendarpanel.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qbuttongroup.h>
#include <qheader.h>
#include <qlistview.h>
#include <qdatetimeedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

namespace KPlato
{

/*
 *  Constructs a CalendarEditBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
CalendarEditBase::CalendarEditBase( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "CalendarEditBase" );
    CalendarEditBaseLayout = new QHBoxLayout( this, 0, 0, "CalendarEditBaseLayout");

    groupBox2 = new QGroupBox( this, "groupBox2" );
    groupBox2->setColumnLayout(0, Qt::Vertical );
    groupBox2->layout()->setSpacing( 6 );
    groupBox2->layout()->setMargin( 6 );
    groupBox2Layout = new QGridLayout( groupBox2->layout() );
    groupBox2Layout->setAlignment( Qt::AlignTop );

    calendarPanel = new CalendarPanel( groupBox2, "calendarPanel" );

    groupBox2Layout->addWidget( calendarPanel, 1, 0 );

    day = new QButtonGroup( groupBox2, "day" );
    day->setColumnLayout(0, Qt::Vertical );
    day->layout()->setSpacing( 6 );
    day->layout()->setMargin( 6 );
    dayLayout = new QVBoxLayout( day->layout() );
    dayLayout->setAlignment( Qt::AlignTop );

    layout8 = new QHBoxLayout( 0, 0, 6, "layout8");

    state = new QComboBox( FALSE, day, "state" );
    layout8->addWidget( state );

    bApply = new QPushButton( day, "bApply" );
    layout8->addWidget( bApply );
    dayLayout->addLayout( layout8 );

    groupBox4 = new QGroupBox( day, "groupBox4" );
    groupBox4->setColumnLayout(0, Qt::Vertical );
    groupBox4->layout()->setSpacing( 6 );
    groupBox4->layout()->setMargin( 6 );
    groupBox4Layout = new QVBoxLayout( groupBox4->layout() );
    groupBox4Layout->setAlignment( Qt::AlignTop );

    intervalList = new QListView( groupBox4, "intervalList" );
    intervalList->addColumn( tr2i18n( "Work Interval" ) );
    groupBox4Layout->addWidget( intervalList );

    layout6 = new QHBoxLayout( 0, 0, 6, "layout6");

    startTime = new QTimeEdit( groupBox4, "startTime" );
    layout6->addWidget( startTime );

    endTime = new QTimeEdit( groupBox4, "endTime" );
    layout6->addWidget( endTime );
    groupBox4Layout->addLayout( layout6 );

    layout5 = new QHBoxLayout( 0, 0, 6, "layout5");

    bClear = new QPushButton( groupBox4, "bClear" );
    layout5->addWidget( bClear );

    bAddInterval = new QPushButton( groupBox4, "bAddInterval" );
    layout5->addWidget( bAddInterval );
    groupBox4Layout->addLayout( layout5 );
    dayLayout->addWidget( groupBox4 );

    groupBox2Layout->addWidget( day, 1, 1 );
    CalendarEditBaseLayout->addWidget( groupBox2 );
    languageChange();
    resize( QSize(540, 340).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
CalendarEditBase::~CalendarEditBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void CalendarEditBase::languageChange()
{
    setCaption( tr2i18n( "CalendarEditBase" ) );
    groupBox2->setTitle( QString::null );
    day->setTitle( QString::null );
    bApply->setText( tr2i18n( "Apply" ) );
    groupBox4->setTitle( QString::null );
    intervalList->header()->setLabel( 0, tr2i18n( "Work Interval" ) );
    bClear->setText( tr2i18n( "Clear" ) );
    bAddInterval->setText( tr2i18n( "Add Interval" ) );
}

}  //KPlato namespace

#include "kptcalendareditbase.moc"
