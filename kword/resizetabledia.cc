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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kwdoc.h"
#include <koRect.h>
#include "resizetabledia.h"
#include "resizetabledia.moc"
#include "kwtableframeset.h"
#include "kwcommand.h"

#include <klocale.h>

#include <qlabel.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qvbox.h>
#include <knuminput.h>

KWResizeTableDia::KWResizeTableDia( QWidget *parent, const char *name, KWTableFrameSet *_table, KWDocument *_doc, ResizeType _type, KWCanvas *_canvas )
    : KDialogBase( parent, name , true, "", Ok | Cancel )
{
    setCaption( i18n("Change Help Line position") );

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
        value->setValue( type == ROW ? table->getRows() : table->getCols() );
    else
        value->setValue( type == ROW ? (rowSelected+1) : (colSelected+1) );

    rc = new QLabel( type == ROW ? i18n( "Height:(%1)" ).arg(doc->getUnitName()) : i18n( "Width:(%1)" ).arg(doc->getUnitName()), page );
    position= new KDoubleNumInput( page );
}

bool KWResizeTableDia::doResize()
{
    unsigned int resize= value->value() - 1;
    if ( type == ROW )
    {
        KWFrame *frm = table->getCell( resize, 0 )->frame(0);
        if (frm)
        {
            FrameIndex index( frm );
            FrameResizeStruct resize;
            resize.sizeOfBegin= frm->normalize();
            KoRect newRect( frm->normalize() );
            newRect.setHeight( KoUnit::ptFromUnit(  position->value(), doc->getUnit() ));
            resize.sizeOfEnd= newRect;
            KWFrameResizeCommand * cmd =new KWFrameResizeCommand( i18n("Resize Column"), index, resize );
            cmd->execute();
            doc->addCommand( cmd );
        }
    }
    else
    {
        KWFrame *frm = table->getCell( 0, resize )->frame(0);
        if (frm)
        {
            FrameIndex index( frm );
            FrameResizeStruct resize;
            resize.sizeOfBegin= frm->normalize();
            KoRect newRect( frm->normalize() );
            newRect.setWidth( KoUnit::ptFromUnit(  position->value(), doc->getUnit() ));
            resize.sizeOfEnd= newRect;

            KWFrameResizeCommand * cmd =new KWFrameResizeCommand( i18n("Resize Column"), index, resize );
            cmd->execute();
            doc->addCommand( cmd );
        }
    }
    return true;
}

void KWResizeTableDia::slotOk()
{
   if (doResize())
   {
      KDialogBase::slotOk();
   }
}
