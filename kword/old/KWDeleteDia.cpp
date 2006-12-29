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

#include "KWTableFrameSet.h"
#include "KWDeleteDia.h"
#include "KWView.h"

#include <klocale.h>
#include <QLabel>
#include <QLayout>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3ValueList>

KWDeleteDia::KWDeleteDia( KWView *parent, KWTableFrameSet *table, DeleteType type, Q3ValueList<uint> remove)
    : KDialog( parent)
{
    setCaption( (type==deleteRow?i18n("Delete Row") : i18n("Delete Column")) );
    setButtons(  Ok | Cancel );
    setDefaultButton( Ok );
    Q_ASSERT(type == deleteRow || type == deleteColumn);
    m_type = type;
    m_table = table;
    m_toRemove = remove;
    m_view = parent;

    setupTab1();
    setButtonGuiItem( Ok, KGuiItem(
        i18n("&Delete"), "editdelete", type == deleteRow ?
        i18n("Delete the row from the table.") :
        i18n("Delete the column from the table.")) );
}

void KWDeleteDia::setupTab1() {
#if 0
    QWidget *tab1 = plainPage();
    Q3GridLayout *grid1 = new Q3GridLayout( tab1, 4, 1, 0, spacingHint() );
    unsigned int count = m_toRemove.count();
    Q_ASSERT(count > 0);

    QString message;
    if ( count == ( (m_type == deleteRow) ? m_table->getRows() : m_table->getColumns() ) )
        // all the columns are selected and the user asked to remove columns or the same with rows
        // => we want to delete the whole table
        message = i18n("Delete the whole table?");
    else if ( count > 10 )
    // do not display hugely long dialogs if many rows/cells are selected
        message = m_type == deleteRow ? i18n("Delete all selected rows?") : i18n("Delete all selected cells?");
    else if ( count == 1 ) {
        message = m_type == deleteRow ? i18n( "Delete row number %1?" ) : i18n( "Delete column number %1?" );
        message = message.arg( m_toRemove.first() + 1 ); // +1 because humans count from 1
    }
    else {
        message = m_type == deleteRow ? i18n( "Delete rows: %1 ?" ) : i18n( "Delete columns: %1 ?" );

        Q3ValueListIterator<uint> items = m_toRemove.begin();
        QString rows;
        for(;items != m_toRemove.end(); ++items) {
            if(! rows.isEmpty())
                rows += ", ";
            rows += QString().setNum((*items) +1);
        }
        message = message.arg( rows );
    }

    QLabel *rc = new QLabel( message , tab1 );
    rc->resize( rc->sizeHint() );
    rc->setAlignment( Qt::AlignLeft | Qt::AlignBottom );
    grid1->addWidget( rc, 1, 0 );
#endif
}

void KWDeleteDia::slotOk() {
    if(m_type == deleteRow)
        m_view->tableDeleteRow(m_toRemove);
    else
        m_view->tableDeleteCol(m_toRemove);
    KDialog::accept();
}

#include "KWDeleteDia.moc"
