/* This file is part of the KDE project
   Copyright 1999-2006 The KSpread Team <calligra-devel@kde.org>
   Copyright 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

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
#ifndef CALLIGRA_SHEETS_CELLEDITORBASE_H
#define CALLIGRA_SHEETS_CELLEDITORBASE_H

#include "sheets_common_export.h"

class QFont;
class KoViewConverter;
class QString;
class QWidget;

namespace Calligra
{
namespace Sheets
{

class CALLIGRA_SHEETS_COMMON_EXPORT CellEditorBase
{
public:
    CellEditorBase();
    virtual ~CellEditorBase();
    virtual QWidget* widget() = 0;
    virtual void selectionChanged() = 0;
    virtual void setEditorFont(QFont const & font, bool updateSize, const KoViewConverter *viewConverter) = 0;
    virtual void setText(const QString& text, int cursorPos = -1) = 0;
    virtual QString toPlainText() const = 0;
    virtual void setCursorPosition(int pos) = 0;
    virtual int cursorPosition() const = 0;
    virtual void cut() = 0;
    virtual void copy() = 0;
    virtual void paste() = 0;
    virtual void setActiveSubRegion(int index) = 0;
    virtual bool captureArrowKeys() const = 0;
    virtual void setCaptureArrowKeys(bool capture) = 0;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CELLEDITORBASE_H
