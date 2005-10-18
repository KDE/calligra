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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KWDocument.h"
#include "KWTableFrameSet.h"
#include "KWDeleteDia.h"
#include "KWDeleteDia.moc"
#include "KWCommand.h"
#include "KWView.h"
#include "KWCanvas.h"

#include <klocale.h>

#include <qlabel.h>
#include <qspinbox.h>

#include <stdlib.h>

/******************************************************************/
/* Class: KWDeleteDia                                             */
/******************************************************************/

KWDeleteDia::KWDeleteDia( QWidget *parent, const char *name, KWTableFrameSet *_table, KWDocument *_doc, DeleteType _type, KWCanvas *_canvas )
    : KDialogBase( Plain, QString::null, Ok | Cancel, Ok, parent, name, true )
{
    type = _type;
    table = _table;
    doc = _doc;
    canvas = _canvas;
    m_toRemove.clear();

    setupTab1();
    setButtonOK( KGuiItem(
        i18n("&Delete"), "editdelete", type == ROW ?
        i18n("Delete the row from the table.") :
        i18n("Delete the column from the table.")) );
}

void KWDeleteDia::setupTab1()
{
    tab1 = plainPage();
    grid1 = new QGridLayout( tab1, 4, 1, 0, spacingHint() );
    QString rowList;
    bool firstSelectedCell = true; // used to know whether to add a ", " to the rowList string.

    uint max = (type == ROW) ? table->getRows() : table->getCols(); // max row/col to loop up to
    for (uint i=0;i<max; i++)
    {
        if ( ( (type == ROW) && table->isRowSelected(i)) ||
                ( (type == COL) && table->isColSelected(i) ) )
        {
            if (!firstSelectedCell)
                rowList += ", "; // i18n??
            rowList += QString::number(i+1);
            m_toRemove.push_back(i);
            firstSelectedCell = false;
       }
    }
    if ( m_toRemove.isEmpty() ) // Nothing selected, so we want to remove the row/col where the cursor is
    {
        int val = type == ROW ? canvas->currentTableRow() : canvas->currentTableCol();
        Q_ASSERT( val != -1 );
        rowList += QString::number(val+1);
        m_toRemove.push_back(val);
    }
    unsigned int count = m_toRemove.count();
    Q_ASSERT(count > 0);

    QString message;
    if ( count == ( (type == ROW) ? table->getRows() : table->getCols() ) )
        // all the columns are selected and the user asked to remove columns or the same with rows
        // => we want to delete the whole table
        message = i18n("Delete the whole table?");
    else if ( count > 10 )
    // do not display hugely long dialogs if many rows/cells are selected
        message = ROW ? i18n("Delete all selected rows?") : i18n("Delete all selected cells?");
    else if ( count == 1 ) {
        message = type == ROW ? i18n( "Delete row number %1?" ) : i18n( "Delete column number %1?" );
        message = message.arg( m_toRemove.first() );
    } else {
        message = type == ROW ? i18n( "Delete rows: %1 ?" ) : i18n( "Delete columns: %1 ?" );
        message = message.arg( rowList );
    }

    rc = new QLabel( message , tab1 );
    rc->resize( rc->sizeHint() );
    rc->setAlignment( AlignLeft | AlignBottom );
    grid1->addWidget( rc, 1, 0 );
}

bool KWDeleteDia::doDelete()
{
    KWView *view = canvas->gui()->getView();
    if(!view) return false; // can't happen

    if(type == ROW)
        view->tableDeleteRow(m_toRemove);
    else
        view->tableDeleteCol(m_toRemove);

    return true;
}

void KWDeleteDia::slotOk()
{
   if (doDelete())
   {
      KDialogBase::slotOk();
   }
}
