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

#include "DefinePrintRangeCommand.h"

using namespace KSpread;

// ----- DefinePrintRangeCommand -----


DefinePrintRangeCommand::DefinePrintRangeCommand( Sheet *s )
{
  doc = s->doc();
  sheetName = s->sheetName();
  printRange = s->print()->printRange();
}

void DefinePrintRangeCommand::redo()
{
    Sheet* sheet = doc->map()->findSheet( sheetName );
    if( !sheet ) return;
    sheet->print()->setPrintRange( printRangeRedo );

}

void DefinePrintRangeCommand::undo()
{
    Sheet* sheet = doc->map()->findSheet( sheetName );
    if( !sheet ) return;
    printRangeRedo = sheet->print()->printRange();
    sheet->print()->setPrintRange( printRange );
}

QString DefinePrintRangeCommand::name() const
{
    return i18n("Set Page Layout");
}
