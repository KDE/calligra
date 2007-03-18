/* This file is part of the KDE project
   Copyright 2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2004 Laurent Montel <montel@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#include "Doc.h"
#include "Localization.h"
#include "Map.h"
#include "Sheet.h"
#include "SheetPrint.h"

#include "PaperLayoutCommand.h"

using namespace KSpread;

// ----- PaperLayoutCommand -----


PaperLayoutCommand::PaperLayoutCommand( Sheet *s )
{
  doc = s->doc();
  sheetName = s->sheetName();
  pl = s->print()->paperLayout();
  hf = s->print()->headFootLine();
  unit = doc->unit();
  printGrid = s->print()->printGrid();
  printCommentIndicator = s->print()->printCommentIndicator();
  printFormulaIndicator = s->print()->printFormulaIndicator();
  printRange = s->print()->printRange();
  printRepeatColumns = s->print()->printRepeatColumns();
  printRepeatRows = s->print()->printRepeatRows();
  zoom = s->print()->zoom();
  pageLimitX = s->print()->pageLimitX();
  pageLimitY = s->print()->pageLimitY();
}

void PaperLayoutCommand::redo()
{
    Sheet* sheet = doc->map()->findSheet( sheetName );
    if( !sheet ) return;
    SheetPrint* print = sheet->print();

    print->setPaperLayout( plRedo.left,  plRedo.top,
                           plRedo.right, plRedo.bottom,
                           plRedo.format, plRedo.orientation );

    print->setHeadFootLine( hfRedo.headLeft, hfRedo.headMid, hfRedo.headRight,
                            hfRedo.footLeft, hfRedo.footMid, hfRedo.footRight );

    doc->setUnit( unitRedo );

    print->setPrintGrid( printGridRedo );
    print->setPrintCommentIndicator( printCommentIndicatorRedo );
    print->setPrintFormulaIndicator( printFormulaIndicatorRedo );

    print->setPrintRange( printRangeRedo );
    print->setPrintRepeatColumns( printRepeatColumnsRedo );
    print->setPrintRepeatRows( printRepeatRowsRedo );

    print->setZoom( zoomRedo );

    print->setPageLimitX( pageLimitX );
    print->setPageLimitY( pageLimitY );
}

void PaperLayoutCommand::undo()
{
    Sheet* sheet = doc->map()->findSheet( sheetName );
    if( !sheet ) return;
    SheetPrint* print = sheet->print();
    plRedo = print->paperLayout();
    print->setPaperLayout( pl.left,  pl.top,
                           pl.right, pl.bottom,
                           pl.format,  pl.orientation );

    hfRedo = print->headFootLine();
    print->setHeadFootLine( hf.headLeft, hf.headMid, hf.headRight,
                            hf.footLeft, hf.footMid, hf.footRight );

    unitRedo = doc->unit();
    doc->setUnit( unit );

    printGridRedo = print->printGrid();
    print->setPrintGrid( printGrid );

    printCommentIndicatorRedo = print->printCommentIndicator();
    print->setPrintCommentIndicator( printCommentIndicator );

    printFormulaIndicatorRedo = print->printFormulaIndicator();
    print->setPrintFormulaIndicator( printFormulaIndicator );

    printRangeRedo = print->printRange();
    print->setPrintRange( printRange );

    printRepeatColumnsRedo = print->printRepeatColumns();
    print->setPrintRepeatColumns( printRepeatColumns );

    printRepeatRowsRedo = print->printRepeatRows();
    print->setPrintRepeatRows( printRepeatRows );

    zoomRedo = print->zoom();
    print->setZoom( zoom );

    pageLimitXRedo = print->pageLimitX();
    print->setPageLimitX( pageLimitX );

    pageLimitYRedo = print->pageLimitY();
    print->setPageLimitY( pageLimitY );

}

QString PaperLayoutCommand::name() const
{
    return i18n("Set Page Layout");
}
