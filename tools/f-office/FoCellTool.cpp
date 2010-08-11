/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

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
#include "FoCellTool.h"
#include "Cell.h"
#include "Style.h"
#include "KoColor.h"

using KSpread::Cell;
using KSpread::Style;

FoCellTool::FoCellTool(KoCanvasBase* canvas)
        : KSpread::CellTool(canvas)
{
}

FoCellTool::~FoCellTool()
{
}

void FoCellTool::selectFontSize(int size){
    fontSize(size);

    //below function will automatically adjust size of row as per font size we select
    adjustRow();
}

void FoCellTool::selectFontType(const QString& fonttype){
    font(fonttype);
}

void FoCellTool::selectTextColor(const QColor &color) {
    KoColor *textcolor = new KoColor();
    textcolor->fromQColor(color);
    changeTextColor(*textcolor);
}

void FoCellTool::selectTextBackgroundColor(const QColor &color) {
    KoColor *textbackgroundcolor = new KoColor();
    textbackgroundcolor->fromQColor(color);
    changeBackgroundColor(*textbackgroundcolor);
}

int FoCellTool::getFontSize() {
    const Style style = Cell(selection()->activeSheet(), selection()->marker()).style();
    return style.fontSize();
}

QString FoCellTool::getFontType() {
    const Style style = Cell(selection()->activeSheet(), selection()->marker()).style();
    return style.fontFamily();
}

bool FoCellTool::isFontBold() {
    const Style style = Cell(selection()->activeSheet(), selection()->marker()).style();
    return style.bold();
}

bool FoCellTool::isFontItalic() {
    const Style style = Cell(selection()->activeSheet(), selection()->marker()).style();
    return style.italic();
}

bool FoCellTool::isFontUnderline() {
    const Style style = Cell(selection()->activeSheet(), selection()->marker()).style();
    return style.underline();
}

KSpread::CellEditorBase* FoCellTool::editor() const
{
    return KSpread::CellTool::editor();;
}

bool FoCellTool::createEditor(bool clear, bool focus)
{
    return KSpread::CellTool::createEditor(clear, focus);
}
