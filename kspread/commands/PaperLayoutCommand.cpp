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

// Local
#include "PaperLayoutCommand.h"

#include "Doc.h"
#include "Localization.h"
#include "Map.h"
#include "Sheet.h"
#include "SheetPrint.h"

using namespace KSpread;

// ----- PaperLayoutCommand -----


PaperLayoutCommand::PaperLayoutCommand( Sheet *s )
{
  doc = s->doc();
  sheetName = s->sheetName();
  pl = s->print()->paperLayout();
  hf = s->print()->headFootLine();
  unit = doc->unit();
  printGrid = s->print()->settings()->printGrid();
  printCommentIndicator = s->print()->settings()->printCommentIndicator();
  printFormulaIndicator = s->print()->settings()->printFormulaIndicator();
  printRange = s->print()->printRange();
  printRepeatColumns = s->print()->printRepeatColumns();
  printRepeatRows = s->print()->printRepeatRows();
  zoom = s->print()->zoom();
  pageLimitX = s->print()->pageLimitX();
  pageLimitY = s->print()->pageLimitY();
  setText(i18n("Set Page Layout"));
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

    print->settings()->setPrintGrid( printGridRedo );
    print->settings()->setPrintCommentIndicator( printCommentIndicatorRedo );
    print->settings()->setPrintFormulaIndicator( printFormulaIndicatorRedo );

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

    printGridRedo = print->settings()->printGrid();
    print->settings()->setPrintGrid( printGrid );

    printCommentIndicatorRedo = print->settings()->printCommentIndicator();
    print->settings()->setPrintCommentIndicator( printCommentIndicator );

    printFormulaIndicatorRedo = print->settings()->printFormulaIndicator();
    print->settings()->setPrintFormulaIndicator( printFormulaIndicator );

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
