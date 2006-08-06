/* This file is part of the KDE project
   Copyright (C) 2005 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_MANIPULATOR_FORMAT
#define KSPREAD_MANIPULATOR_FORMAT

#include "Manipulator.h"
#include "Format.h"
#include "Style.h"

namespace KSpread
{

  // struct layoutCell {
//   int row;
//   int col;
//   Format *l;
// };
  //
// struct layoutColumn {
//   int col;
//   ColumnFormat *l;
// };
  //
// struct layoutRow {
//   int row;
//   RowFormat *l;
// };


/**
   * \class FormatManipulator
   * \brief Manipulates the formatting of a cell region.
 */
class FormatManipulator : public Manipulator
{
  public:
    FormatManipulator();
    virtual ~FormatManipulator();

    void setProperty(Style::FlagsSet property) { m_properties |= property; }
    bool isEmpty() { return m_properties == 0; }

// SetSelectionFontWorker
// SetSelectionSizeWorker
    void setFontFamily(const QString& font) { m_properties |= Style::SFont; m_font = font; }
    void setFontSize(int size) { m_properties |= Style::SFont; m_size = size; }
    void setFontBold(uint bold) { m_properties |= Style::SFont; m_bold = bold; }
    void setFontItalic(uint italic) { m_properties |= Style::SFont; m_italic = italic; }
    void setFontStrike(uint strike) { m_properties |= Style::SFont; m_strike = strike; }
    void setFontUnderline(uint underline) { m_properties |= Style::SFont; m_underline = underline; }
// SetSelectionAngleWorker
    void setAngle(int angle) { m_properties |= Style::SAngle; m_angle = angle; }
// SetSelectionTextColorWorker
    void setTextColor(const QColor& textColor) { m_properties |= Style::STextPen; m_textColor = textColor; }
// SetSelectionBgColorWorker
    void setBackgroundColor(const QColor& bgColor) { m_properties |= Style::SBackgroundColor; m_backgroundColor = bgColor; }
// SetSelectionBorderAllWorker
    void setTopBorderPen(const QPen& pen) { m_properties |= Style::STopBorder; m_topBorderPen = pen; }
    void setBottomBorderPen(const QPen& pen) { m_properties |= Style::SBottomBorder; m_bottomBorderPen = pen; }
    void setLeftBorderPen(const QPen& pen) { m_properties |= Style::SLeftBorder; m_leftBorderPen = pen; }
    void setRightBorderPen(const QPen& pen) { m_properties |= Style::SRightBorder; m_rightBorderPen = pen; }
    void setHorizontalPen(const QPen& pen) { m_properties |= Style::STopBorder | Style::SBottomBorder; m_horizontalPen = pen; }
    void setVerticalPen(const QPen& pen) { m_properties |= Style::SLeftBorder | Style::SRightBorder; m_verticalPen = pen; }
    void setFallDiagonalPen(const QPen& pen) { m_properties |= Style::SFallDiagonal; m_fallDiagonalPen = pen; }
    void setGoUpDiagonalPen(const QPen& pen) { m_properties |= Style::SGoUpDiagonal; m_goUpDiagonalPen = pen; }
// SetSelectionAlignWorker
    void setHorizontalAlignment(Style::HAlign align) { m_properties |= Style::SHAlign; m_horAlign = align; }
// SetSelectionAlignWorker
    void setVerticalAlignment(Style::VAlign align) { m_properties |= Style::SVAlign; m_verAlign = align; }

    void setBackgroundBrush(const QBrush& brush) { m_properties |= Style::SBackgroundBrush; m_backgroundBrush = brush; }
    void setIndent(double indent) { m_properties |= Style::SIndent; m_indent = indent; }
    void setMultiRow(bool multiRow) { m_properties |= Style::SMultiRow; m_multiRow = multiRow; }
    void setVerticalText(bool verticalText) { m_properties |= Style::SVerticalText; m_verticalText = verticalText; }
    void setDontPrintText(bool dontPrintText) { m_properties |= Style::SDontPrintText; m_dontPrintText = dontPrintText; }
    void setNotProtected(bool notProtected) { m_properties |= Style::SNotProtected; m_notProtected = notProtected; }
    void setHideAll(bool hideAll) { m_properties |= Style::SHideAll; m_hideAll = hideAll; }
    void setHideFormula(bool hideFormula) { m_properties |= Style::SHideFormula; m_hideFormula = hideFormula; }
    void setComment(const QString& comment) { m_properties |= Style::SComment; m_comment = comment; }
    void setPrefix(const QString& prefix) { m_properties |= Style::SPrefix; m_prefix = prefix; }
    void setPostfix(const QString& postfix) { m_properties |= Style::SPostfix; m_postfix = postfix; }
    void setPrecision(int precision) { m_properties |= Style::SPrecision; m_precision = precision; }
    void setFloatFormat(Style::FloatFormat floatFormat) { m_properties |= Style::SFloatFormat; m_floatFormat = floatFormat; }
    void setFloatColor(Style::FloatColor floatColor) { m_properties |= Style::SFloatColor; m_floatColor = floatColor; }
    void setFormatType(FormatType formatType) { m_properties |= Style::SFormatType; m_formatType = formatType; }
    void setCurrency(int type, const QString& symbol) { m_currencyType = type; m_currencySymbol = symbol; }

  protected:
    virtual QString name() const { return i18n("Format Change"); }

    virtual bool preProcessing();
    virtual bool process(Element*);

    void copyFormat(QLinkedList<layoutCell> &list,
                    QLinkedList<layoutColumn> &listCol,
                    QLinkedList<layoutRow> &listRow);
    bool testCondition(RowFormat*);
    void doWork(Format*, bool isTop, bool isBottom, bool isLeft, bool isRight);
    void prepareCell(Cell*);

  private:
    quint32 m_properties;

// TODO Stefan: find a more elegant way to store the format
    QLinkedList<layoutCell> m_lstFormats;
    QLinkedList<layoutCell> m_lstRedoFormats;
    QLinkedList<layoutColumn> m_lstColFormats;
    QLinkedList<layoutColumn> m_lstRedoColFormats;
    QLinkedList<layoutRow> m_lstRowFormats;
    QLinkedList<layoutRow> m_lstRedoRowFormats;

// SetSelectionFontWorker
// SetSelectionSizeWorker
    QString m_font;
    int m_size;
    signed char m_bold;
    signed char m_italic;
    signed char m_strike;
    signed char m_underline;
// SetSelectionAngleWorker
    int m_angle;
    int m_precision;
    int m_currencyType;
    double m_indent;
    bool m_multiRow;
    bool m_verticalText;
    bool m_dontPrintText;
    bool m_notProtected;
    bool m_hideAll;
    bool m_hideFormula;

// SetSelectionTextColorWorker
    QColor m_textColor;
// SetSelectionBgColorWorker
    QColor m_backgroundColor;
// SetSelectionBorderAllWorker
    QPen m_topBorderPen;
    QPen m_bottomBorderPen;
    QPen m_leftBorderPen;
    QPen m_rightBorderPen;
    QPen m_horizontalPen;
    QPen m_verticalPen;
    QPen m_fallDiagonalPen;
    QPen m_goUpDiagonalPen;

    QBrush m_backgroundBrush;
    QString m_comment;
    QString m_prefix;
    QString m_postfix;
    QString m_currencySymbol;

// SetSelectionAlignWorker
    Style::HAlign m_horAlign;
// SetSelectionAlignWorker
    Style::VAlign m_verAlign;
    Style::FloatFormat m_floatFormat;
    Style::FloatColor m_floatColor;
    FormatType m_formatType;
};

}  // namespace KSpread

#endif // KSPREAD_MANIPULATOR_FORMAT
