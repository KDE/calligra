/* This file is part of the KDE project
   Copyright (C) 2004 - 2005 Dag Andersen <danders@get2net.dk>

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

#include "kptcalendarlistdialogbase.h"
#include "kptcalendaredit.h"

#include <qvariant.h>
#include <qframe.h>
#include <qgroupbox.h>
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

#include <klocale.h>
#include <kdebug.h>

namespace KPlato
{

CalendarListDialogBase::CalendarListDialogBase( QWidget* parent, const char* name, WFlags fl )
    : CalendarListPanel( parent, name, fl )
{
    if ( !name )
        setName( "CalendarListDialogBase" );
        
    QVBoxLayout *l = new QVBoxLayout(calendarBox);
    calendar = new CalendarEdit(calendarBox);
    l->addWidget(calendar);
}


} //Kplato namespace
#include "kptcalendarlistdialogbase.moc"
