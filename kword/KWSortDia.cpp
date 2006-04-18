/* This file is part of the KDE project
   Copyright (C) 2002 Montel Laurent <lmontel@mandrakesoft.com>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/



#include "KWSortDia.h"
#include <qlayout.h>
#include <klocale.h>
#include <qradiobutton.h>
#include <q3groupbox.h>
#include <q3buttongroup.h>
#include <kvbox.h>

KWSortDia::KWSortDia( QWidget *parent, const char* name )
    : KDialogBase( parent, name, true, i18n("Sort Text"), Ok|Cancel )
{
    KVBox *page = makeVBoxMainWidget();

    Q3ButtonGroup *grp = new Q3ButtonGroup( 1, Qt::Horizontal, i18n( "Sort" ),page );
    grp->setRadioButtonExclusive( true );
    grp->layout();
    m_increase = new QRadioButton( i18n("Increase"), grp );
    m_decrease = new QRadioButton( i18n("Decrease"), grp );

    grp->setRadioButtonExclusive( true );
    m_increase->setChecked( true );
    resize( 200, 150 );
    setFocus();
}

SortType KWSortDia::getSortType() const
{
    return m_increase->isChecked() ? KW_SORTINCREASE : KW_SORTDECREASE;
}


#include "KWSortDia.moc"
