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

#ifndef FOCELL_TOOL
#define FOCELL_TOOL

#include "part/CellTool.h"

/**
 * The tool to change cell ranges.
 */
class KSPREAD_EXPORT FoCellTool : public KSpread::CellTool
{
    Q_OBJECT

public:
    explicit FoCellTool(KoCanvasBase* canvas);
    ~FoCellTool();

    /*!
     * Function for selecting font size
     */
    void selectFontSize(int size);
    /*!
     * function for selecting font type
     */
    void selectFontType(const QString& font);
    /*!
     * Function for selecting text foreground color
     */
    void selectTextColor(const QColor& color);
    /*!
     * function for selecting text background color
     */
    void selectTextBackgroundColor(const QColor& color);
    /*!
     * function for retreiving font size
     */
    int getFontSize();
    /*!
     * function for retreiving font type
     */
    QString getFontType();
    /*!
     * function for bold check
     */
    bool isFontBold();
    /*!
     * function for italic check
     */
    bool isFontItalic();
    /*!
     * function for underline check
     */
    bool isFontUnderline();


    // custom cell editor
    virtual KSpread::CellEditorBase* editor() const;
    virtual bool createEditor(bool clear, bool focus);
private:
    Q_DISABLE_COPY(FoCellTool)
};


#endif // FOCELL_TOOL
