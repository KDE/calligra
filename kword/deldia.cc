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

#include "kwdoc.h"
#include "kwframe.h"
#include "kwtableframeset.h"
#include "kwcanvas.h"
#include "deldia.h"
#include "deldia.moc"
#include "kwcommand.h"

#include <klocale.h>

#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qstring.h>
#include <qevent.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qpainter.h>

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

    setupTab1();
    setButtonOKText(i18n("&Delete"), type == ROW ?
    	i18n("Delete the row from the table.") :
    	i18n("Delete the column from the table."));

    setInitialSize( QSize(300, 150) );
}

void KWDeleteDia::setupTab1()
{
    tab1 = plainPage();
    grid1 = new QGridLayout( tab1, 4, 1, 0, spacingHint() );

    rc = new QLabel( type == ROW ? i18n( "Delete Row:" ) : i18n( "Delete Column:" ), tab1 );
    rc->resize( rc->sizeHint() );
    rc->setAlignment( AlignLeft | AlignBottom );
    grid1->addWidget( rc, 1, 0 );

    value = new QSpinBox( 1, type == ROW ? table->getRows() : table->getCols(), 1, tab1 );
    value->resize( value->sizeHint() );
    value->setValue( type == ROW ? table->getRows() : table->getCols() );
    grid1->addWidget( value, 2, 0 );

    grid1->addRowSpacing( 1, rc->height() );
    grid1->addRowSpacing( 2, value->height() );
    grid1->setRowStretch( 0, 1 );
    grid1->setRowStretch( 1, 0 );
    grid1->setRowStretch( 2, 0 );
    grid1->setRowStretch( 3, 1 );

    grid1->addColSpacing( 0, rc->width() );
    grid1->addColSpacing( 0, value->width() );
    grid1->setColStretch( 0, 1 );
}

bool KWDeleteDia::doDelete()
{
    unsigned int remove= value->value() - 1;
    if ( type == ROW )
    {
        KWRemoveRowCommand *cmd = new KWRemoveRowCommand( i18n("Remove row"), table, remove);
         cmd->execute();
         doc->addCommand(cmd);
        //table->deleteRow( value->value() - 1 );
    }
    else
    {
         KWRemoveColumnCommand *cmd = new KWRemoveColumnCommand( i18n("Remove column"), table, remove);
        cmd->execute();
        doc->addCommand(cmd);
        //table->deleteCol( value->value() - 1 );
    }



#if 0
    QPainter p;
    p.begin( canvas );

    if ( type == ROW )
        table->deleteRow( value->value() - 1 );
    else
        table->deleteCol( value->value() - 1 );

    canvas->getCursor()->setFrameSet( doc->getFrameSetNum( table->getFrameSet( 0, 0 ) ) + 1 );
    doc->drawMarker( *canvas->getCursor(), &p, canvas->contentsX(), canvas->contentsY() );
    canvas->getCursor()->init( dynamic_cast<KWTextFrameSet*>( doc->getFrameSet( canvas->getCursor()->getFrameSet() - 1 ) )->getFirstParag(), true );
    canvas->getCursor()->gotoStartOfParag();
    canvas->getCursor()->cursorGotoLineStart();
    p.end();

    canvas->recalcCursor();
#endif
    return true;
}

void KWDeleteDia::slotOk()
{
   if (doDelete())
   {
      KDialogBase::slotOk();
   }
}
