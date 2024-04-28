/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1999-2006 The KSpread Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef CALLIGRA_SHEETS_CELLEDITORBASE_H
#define CALLIGRA_SHEETS_CELLEDITORBASE_H

#include "sheets_ui_export.h"

class QFont;
class KoViewConverter;
class QString;
class QWidget;

namespace Calligra
{
namespace Sheets
{

class CALLIGRA_SHEETS_UI_EXPORT CellEditorBase
{
public:
    CellEditorBase();
    virtual ~CellEditorBase();
    virtual QWidget *widget() = 0;
    virtual void selectionChanged() = 0;
    virtual void setEditorFont(QFont const &font, bool updateSize, const KoViewConverter *viewConverter) = 0;
    virtual void setText(const QString &text, int cursorPos = -1) = 0;
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
