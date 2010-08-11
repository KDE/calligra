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

// KOffice
#include "ui/CellEditorBase.h"
#include "KoColor.h"
#include "ui/CellEditor.h"


FoCellTool::FoCellTool(KoCanvasBase* canvas)
        : KSpread::CellTool(canvas)
{
}

FoCellTool::~FoCellTool()
{
}

void FoCellTool::selectFontSize(int size){
    fontSize(size);
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

KSpread::CellEditorBase* FoCellTool::editor() const
{
    return KSpread::CellTool::editor();;
}

bool FoCellTool::createEditor(bool clear, bool focus)
{
    return KSpread::CellTool::createEditor(clear, focus);
}
