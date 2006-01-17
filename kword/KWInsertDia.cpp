/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C)  2005 Thomas Zander <zander@kde.org>

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

#include "KWInsertDia.h"
#include "KWTableFrameSet.h"
#include "KWView.h"

#include <klocale.h>

#include <qlabel.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qlayout.h>

KWInsertDia::KWInsertDia( KWView *parent, KWTableFrameSet *table, InsertType type, int insertHint)
    : KDialogBase( Plain, (type==insertRow?i18n("Insert Row") : i18n("Insert Column")), Ok | Cancel, Ok, parent, "Insert Table items dialog", true )
{
    m_type = type;
    m_table = table;
    m_view = parent;

    setupTab1(insertHint);
}

void KWInsertDia::setupTab1(int insertHint)
{
    QWidget *tab1 = plainPage();
    QGridLayout *grid1 = new QGridLayout( tab1, 2, 2, 0, KDialog::spacingHint() );

    QButtonGroup *grp = new QButtonGroup( m_type == insertRow ? i18n( "Insert New Row" ) : i18n( "Insert New Column" ), tab1 );

    QGridLayout *grid2 = new QGridLayout( grp, 3, 1, KDialog::marginHint(), KDialog::spacingHint() );

    m_rBefore = new QRadioButton( i18n( "Before" ), grp, "before_radio_button" );
    grp->insert( m_rBefore );
    grid2->addWidget( m_rBefore, 1, 0 );

    QRadioButton *rAfter = new QRadioButton( i18n( "After" ), grp,  "after_radio_button");
    grp->insert( rAfter );
    grid2->addWidget( rAfter, 2, 0 );
    rAfter->setChecked( true );

    grid2->addRowSpacing( 0, 7 );

    grid1->addMultiCellWidget ( grp, 0, 0, 0, 1 );

    QLabel *rc = new QLabel( m_type == insertRow ? i18n( "Row:" ) : i18n( "Column:" ), tab1 );
    grid1->addWidget( rc, 1, 0 );

    m_value = new QSpinBox( 1, m_type == insertRow ? m_table->getRows() : m_table->getColumns(),
        1, tab1, "row_col_spinbox" );
    m_value->setValue( insertHint + 1 ); // +1 due to the fact that humans count starting at 1

    grid1->addWidget( m_value, 1, 1 );
}

void KWInsertDia::slotOk()
{
    unsigned int insert = m_value->value() - ( m_rBefore->isChecked() ? 1 : 0 );
    if ( m_type == insertRow )
        m_view->tableInsertRow(insert, m_table);
    else
        m_view->tableInsertCol(insert, m_table);
      KDialogBase::slotOk();
}

#include "KWInsertDia.moc"
