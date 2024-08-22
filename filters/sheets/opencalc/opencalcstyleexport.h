/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef OPENCALCSTYLEEXPORT_H
#define OPENCALCSTYLEEXPORT_H

#include "sheets/core/Style.h"

#include <QColor>
#include <QFont>
#include <QList>
#include <QPen>
#include <QString>

namespace Calligra
{
namespace Sheets
{
class Cell;
}
}

class QDomDocument;
class QDomElement;

class Style
{
public:
    enum breakBefore { none, automatic, page };

    Style()
        : breakB(none)
        , size(0.0)
    {
    }

    QString name;
    uint breakB;
    double size;
};

class SheetStyle
{
public:
    SheetStyle()
        : visible(true)
    {
    }

    void copyData(SheetStyle const &ts)
    {
        visible = ts.visible;
    }
    static bool isEqual(SheetStyle const *const t1, SheetStyle const &t2);

    QString name;
    bool visible;
};

class NumberStyle
{
public:
    NumberStyle() = default;

    enum NumberType { Boolean, Date, Number, Percentage, Time };

    void copyData(NumberStyle const &ts)
    {
        type = ts.type;
    }
    static bool isEqual(NumberStyle const *const t1, NumberStyle const &t2);

    QString name;

    NumberType type;
    QString pattern;
};

class CellStyle
{
public:
    CellStyle();

    void copyData(CellStyle const &ts);
    static bool isEqual(CellStyle const *const t1, CellStyle const &t2);

    // all except the number style
    static void loadData(CellStyle &cs, const Calligra::Sheets::Cell &cell);

    QString name;

    QFont font;
    QString numberStyle;
    QColor color;
    QColor bgColor;
    double indent;
    bool wrap;
    bool vertical;
    int angle;
    bool print;
    QPen left;
    QPen right;
    QPen top;
    QPen bottom;
    bool hideAll;
    bool hideFormula;
    bool notProtected;

    Calligra::Sheets::Style::HAlign alignX;
    Calligra::Sheets::Style::VAlign alignY;
};

class ColumnStyle : public Style
{
public:
    ColumnStyle()
        : Style()
    {
    }

    void copyData(ColumnStyle const &cs);
    static bool isEqual(ColumnStyle const *const c1, ColumnStyle const &c2);
};

class RowStyle : public Style
{
public:
    RowStyle()
        : Style()
    {
    }

    void copyData(RowStyle const &cs);
    static bool isEqual(RowStyle const *const c1, RowStyle const &c2);
};

class OpenCalcStyles
{
public:
    OpenCalcStyles();
    ~OpenCalcStyles();

    void writeStyles(QDomDocument &doc, QDomElement &autoStyles);
    void writeFontDecl(QDomDocument &doc, QDomElement &content);

    void addFont(QFont const &font, bool def = false);

    QString cellStyle(CellStyle const &cs);
    QString columnStyle(ColumnStyle const &cs);
    QString numberStyle(NumberStyle const &ns);
    QString rowStyle(RowStyle const &rs);
    QString sheetStyle(SheetStyle const &ts);

private:
    QList<CellStyle *> m_cellStyles;
    QList<ColumnStyle *> m_columnStyles;
    QList<NumberStyle *> m_numberStyles;
    QList<RowStyle *> m_rowStyles;
    QList<SheetStyle *> m_sheetStyles;
    QList<QFont *> m_fontList;

    QFont m_defaultFont;

    void addCellStyles(QDomDocument &doc, QDomElement &autoStyles);
    void addColumnStyles(QDomDocument &doc, QDomElement &autoStyles);
    void addNumberStyles(QDomDocument &doc, QDomElement &autoStyles);
    void addRowStyles(QDomDocument &doc, QDomElement &autoStyles);
    void addSheetStyles(QDomDocument &doc, QDomElement &autoStyles);
};

#endif
