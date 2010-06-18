/* This file is part of the KDE project
   Copyright 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef KSPREAD_STYLE_COMMAND
#define KSPREAD_STYLE_COMMAND

#include <QPair>
#include <QPen>

#include "AbstractRegionCommand.h"
#include "Style.h"

namespace KSpread
{
class Style;

/**
 * \class StyleCommand
 * \ingroup Commands
 * \brief Manipulates the style of a cell region.
 */
class StyleCommand : public AbstractRegionCommand
{
public:
    StyleCommand(QUndoCommand* parent = 0);
    virtual ~StyleCommand();

    // SetSelectionFontWorker
    // SetSelectionSizeWorker
    void setFontFamily(const QString& font) {
        m_style->setFontFamily(font);
    }
    void setFontSize(int size) {
        m_style->setFontSize(size);
    }
    void setFontBold(bool bold) {
        m_style->setFontBold(bold);
    }
    void setFontItalic(uint italic) {
        m_style->setFontItalic(italic);
    }
    void setFontStrike(uint strike) {
        m_style->setFontStrikeOut(strike);
    }
    void setFontUnderline(uint underline) {
        m_style->setFontUnderline(underline);
    }
    // SetSelectionAngleWorker
    void setAngle(int angle) {
        m_style->setAngle(angle);
    }
    // SetSelectionTextColorWorker
    void setFontColor(const QColor& textColor) {
        m_style->setFontColor(textColor);
    }
    // SetSelectionBgColorWorker
    void setBackgroundColor(const QColor& bgColor) {
        m_style->setBackgroundColor(bgColor);
    }
    // SetSelectionBorderAllWorker
    void setTopBorderPen(const QPen& pen) {
        m_style->setTopBorderPen(pen);
    }
    void setBottomBorderPen(const QPen& pen) {
        m_style->setBottomBorderPen(pen);
    }
    void setLeftBorderPen(const QPen& pen) {
        m_style->setLeftBorderPen(pen);
    }
    void setRightBorderPen(const QPen& pen) {
        m_style->setRightBorderPen(pen);
    }
    void setHorizontalPen(const QPen& pen) {
        m_horizontalPen = pen; m_horizontalPenChanged = true;
    }
    void setVerticalPen(const QPen& pen) {
        m_verticalPen = pen; m_verticalPenChanged = true;
    }
    void setFallDiagonalPen(const QPen& pen) {
        m_style->setFallDiagonalPen(pen);
    }
    void setGoUpDiagonalPen(const QPen& pen) {
        m_style->setGoUpDiagonalPen(pen);
    }
    // SetSelectionAlignWorker
    void setHorizontalAlignment(Style::HAlign align) {
        m_style->setHAlign(align);
    }
    // SetSelectionAlignWorker
    void setVerticalAlignment(Style::VAlign align) {
        m_style->setVAlign(align);
    }

    void setBackgroundBrush(const QBrush& brush) {
        m_style->setBackgroundBrush(brush);
    }
    void setIndentation(double indent) {
        m_style->setIndentation(indent);
    }
    void setMultiRow(bool multiRow) {
        m_style->setWrapText(multiRow);
    }
    void setVerticalText(bool verticalText) {
        m_style->setVerticalText(verticalText);
    }
    void setShrinkToFit(bool shrink) {
        m_style->setShrinkToFit(shrink);
    }
    void setDontPrintText(bool dontPrintText) {
        m_style->setDontPrintText(dontPrintText);
    }
    void setNotProtected(bool notProtected) {
        m_style->setNotProtected(notProtected);
    }
    void setHideAll(bool hideAll) {
        m_style->setHideAll(hideAll);
    }
    void setHideFormula(bool hideFormula) {
        m_style->setHideFormula(hideFormula);
    }
    void setPrefix(const QString& prefix) {
        m_style->setPrefix(prefix);
    }
    void setPostfix(const QString& postfix) {
        m_style->setPostfix(postfix);
    }
    void setPrecision(int precision) {
        m_style->setPrecision(precision);
    }
    void setFloatFormat(Style::FloatFormat floatFormat) {
        m_style->setFloatFormat(floatFormat);
    }
    void setFloatColor(Style::FloatColor floatColor) {
        m_style->setFloatColor(floatColor);
    }
    void setFormatType(Format::Type formatType) {
        m_style->setFormatType(formatType);
    }
    void setCurrency(const Currency& currency) {
        m_style->setCurrency(currency);
    }
    void setParentName(const QString& name) {
        m_style->setParentName(name);
    }
    void setDefault() {
        m_style->setDefault();
    }

protected:
    virtual bool process(Element*);

    virtual bool preProcessing();
    virtual bool mainProcessing();
    virtual bool postProcessing();

private:
    QPen m_horizontalPen;
    QPen m_verticalPen;
    bool m_horizontalPenChanged;
    bool m_verticalPenChanged;

    Style* m_style;
    QList< QPair<QRectF, SharedSubStyle> > m_undoData;
};

}  // namespace KSpread

#endif // KSPREAD_STYLE_COMMAND
