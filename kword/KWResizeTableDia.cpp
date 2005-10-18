/* This file is part of the KDE project
   Copyright (C)  2002 Montel Laurent <lmontel@mandrakesoft.com>

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
#include "KWCanvas.h"
#include <koRect.h>
#include "KWResizeTableDia.h"
#include "KWResizeTableDia.moc"
#include "KWTableFrameSet.h"
#include "KWCommand.h"

#include <klocale.h>

#include <qlabel.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qvbox.h>
#include <koUnitWidgets.h>
#include <koRect.h>

KWResizeTableDia::KWResizeTableDia( QWidget *parent, const char *name, KWTableFrameSet *_table, KWDocument *_doc, ResizeType _type, KWCanvas *_canvas )
    : KDialogBase( parent, name , true, "", Ok | Cancel | User1 | Apply )
{
    setButtonText( KDialogBase::User1, i18n("Reset") );

    type = _type;
    table = _table;
    doc = _doc;
    canvas = _canvas;

    setupTab1();

}

void KWResizeTableDia::setupTab1()
{
    QVBox *page = makeVBoxMainWidget();
    QLabel *rc = new QLabel( type == ROW ? i18n( "Row:" ) : i18n( "Column:" ), page );
    rc->resize( rc->sizeHint() );
    rc->setAlignment( AlignLeft | AlignBottom );

    value = new QSpinBox( 1, type == ROW ? table->getRows() : table->getCols(), 1, page );
    value->resize( value->sizeHint() );
    unsigned int rowSelected;
    unsigned int colSelected;
    bool ret = table->getFirstSelected(rowSelected, colSelected );
    if ( !ret )
    {
        // Get cursor row
        int val = type == ROW ? canvas->currentTableRow() : canvas->currentTableCol();
        if ( val == -1 )
        {
            // Fallback
            val = ( type == ROW ? table->getRows() : table->getCols() );
        }
        else
            ++val;
        value->setValue( val );
    }
    else
        value->setValue( type == ROW ? (rowSelected+1) : (colSelected+1) );
    rc = new QLabel( type == ROW ? i18n( "Height:" ) : i18n( "Width:" ), page );
    position= new KoUnitDoubleSpinBox( page, 0.01, table->anchorFrameset()->isFloating() ? table->anchorFrameset()->frame(0)->width(): 9999, 1, 0.0, doc->unit(), doc->unit() );
    slotValueChanged( value->value());
    connect( value, SIGNAL( valueChanged ( int )), this, SLOT( slotValueChanged( int )));

}

bool KWResizeTableDia::doResize()
{
    unsigned int resize= value->value() - 1;
    if ( type == ROW )
    {
        KWFrame *frm = table->cell( resize, 0 )->frame(0);
        if (frm)
        {
            FrameIndex index( frm );
            KoRect newRect( frm->normalize() );
            newRect.setHeight( position->value() );
            FrameResizeStruct resizeStruct( frm->normalize(), frm->minFrameHeight(), newRect );
            KWFrameResizeCommand * cmd = new KWFrameResizeCommand( i18n("Resize Column"), index, resizeStruct );
            cmd->execute();
            doc->addCommand( cmd );
        }
    }
    else
    {
        KWFrame *frm = table->cell( 0, resize )->frame(0);
        if (frm)
        {
            FrameIndex index( frm );
            KoRect newRect( frm->normalize() );
            newRect.setWidth( position->value() );
            FrameResizeStruct resizeStruct( frm->normalize(), frm->minFrameHeight(), newRect );
            KWFrameResizeCommand * cmd =new KWFrameResizeCommand( i18n("Resize Column"), index, resizeStruct );
            cmd->execute();
            doc->addCommand( cmd );
        }
    }
    return true;
}

void KWResizeTableDia::slotValueChanged( int pos)
{
    if ( type == ROW )
    {
        KWFrame *frm = table->cell( pos-1, 0 )->frame(0);
        if (frm)
        {
            position->setValue( KoUnit::toUserValue( QMAX(0.00, frm->normalize().height()), doc->unit() ) );
            resetValue = position->value();
        }

    }
    else
    {
        KWFrame *frm = table->cell( 0, pos-1 )->frame(0);
        if (frm)
        {
            position->setValue( KoUnit::toUserValue( QMAX(0.00, frm->normalize().width()), doc->unit() ) );
            resetValue = position->value();
        }
    }
}

void KWResizeTableDia::slotUser1()
{
    position->setValue( KoUnit::toUserValue(resetValue, doc->unit() ) );
    doResize();
}

void KWResizeTableDia::slotApply()
{
    doResize();
}

void KWResizeTableDia::slotOk()
{
   if (doResize())
   {
      KDialogBase::slotOk();
   }
}
