/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres <nandres@web.de>

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
 * Boston, MA 02110-1301, USA.
*/

#include <opencalcstyleexport.h>

#include <KoGlobal.h>

#include <kspread/Cell.h>
#include <kspread/part/Doc.h>
#include <kspread/Map.h>
#include <kspread/Sheet.h>
#include <kspread/Style.h>
#include <kspread/StyleManager.h>

#include <qdom.h>

using namespace KSpread;

OpenCalcStyles::OpenCalcStyles()
{
}

OpenCalcStyles::~OpenCalcStyles()
{
    while (!m_cellStyles.isEmpty()) delete m_cellStyles.takeFirst();
    while (!m_columnStyles.isEmpty()) delete m_columnStyles.takeFirst();
    while (!m_numberStyles.isEmpty()) delete m_numberStyles.takeFirst();
    while (!m_rowStyles.isEmpty()) delete m_rowStyles.takeFirst();
    while (!m_sheetStyles.isEmpty()) delete m_sheetStyles.takeFirst();
    while (!m_fontList.isEmpty()) delete m_fontList.takeFirst();
}

void OpenCalcStyles::writeStyles(QDomDocument & doc, QDomElement & autoStyles)
{
    addColumnStyles(doc, autoStyles);
    addRowStyles(doc, autoStyles);
    addSheetStyles(doc, autoStyles);
    addNumberStyles(doc, autoStyles);
    addCellStyles(doc, autoStyles);
}

void OpenCalcStyles::writeFontDecl(QDomDocument & doc, QDomElement & fontDecls)
{
    foreach(QFont * f, m_fontList) {
        QDomElement fontDecl = doc.createElement("style:font-decl");

        fontDecl.setAttribute("style:name", f->family());
        fontDecl.setAttribute("fo:font-family", f->family());
        fontDecl.setAttribute("style:font-pitch", (f->fixedPitch() ? "fixed" : "variable"));

        // missing:
        // style:font-charset="x-symbol" style:font-family-generic="swiss"
        // style:font-style-name= "Bold/Standard/Regular"

        fontDecls.appendChild(fontDecl);
    }
}

void OpenCalcStyles::addFont(QFont const & font, bool def)
{
    if (def)
        m_defaultFont = font;

    foreach(QFont * f, m_fontList) {
        if (f->family() == font.family())
            return;

    }

    QFont * f = new QFont(font);
    m_fontList.append(f);
}

QString OpenCalcStyles::cellStyle(CellStyle const & cs)
{
    CellStyle * t = 0;
    foreach(t, m_cellStyles) {
        if (CellStyle::isEqual(t, cs))
            return t->name;
    }

    t = new CellStyle();
    t->copyData(cs);

    m_cellStyles.append(t);

    t->name = QString("ce%1").arg(m_cellStyles.count());

    return t->name;
}

QString OpenCalcStyles::columnStyle(ColumnStyle const & cs)
{
    ColumnStyle * t = 0;
    foreach(t, m_columnStyles) {
        if (ColumnStyle::isEqual(t, cs))
            return t->name;
    }

    t = new ColumnStyle();
    t->copyData(cs);

    m_columnStyles.append(t);

    t->name = QString("co%1").arg(m_columnStyles.count());

    return t->name;
}

QString OpenCalcStyles::numberStyle(NumberStyle const &)
{
    return "";
}

QString OpenCalcStyles::rowStyle(RowStyle const & rs)
{
    RowStyle * t = 0;
    foreach(t, m_rowStyles) {
        if (RowStyle::isEqual(t, rs))
            return t->name;
    }

    t = new RowStyle();
    t->copyData(rs);

    m_rowStyles.append(t);

    t->name = QString("ro%1").arg(m_rowStyles.count());

    return t->name;
}

QString OpenCalcStyles::sheetStyle(SheetStyle const & ts)
{
    SheetStyle * t = 0;
    foreach(t, m_sheetStyles) {
        if (SheetStyle::isEqual(t, ts))
            return t->name;
    }

    t = new SheetStyle();
    t->copyData(ts);

    m_sheetStyles.append(t);

    t->name = QString("ta%1").arg(m_sheetStyles.count());

    return t->name;
}

QString convertPenToString(QPen const & pen)
{
    QString s(QString("%1cm solid ").arg(pen.width() * 0.035));
    s += pen.color().name();

    return s;
}

void OpenCalcStyles::addCellStyles(QDomDocument & doc, QDomElement & autoStyles)
{

    CellStyle * t = 0;
    foreach(t, m_cellStyles) {
        QDomElement ts = doc.createElement("style:style");
        ts.setAttribute("style:name", t->name);
        ts.setAttribute("style:family", "table-cell");
        ts.setAttribute("style:parent-style-name", "Default");
        if (t->numberStyle.length() > 0)
            ts.setAttribute("style:data-style-name", t->numberStyle);

        QDomElement prop = doc.createElement("style:properties");

        if (t->font.family() != m_defaultFont.family())
            prop.setAttribute("style:font-name", t->font.family());

        if (t->font.bold() != m_defaultFont.bold())
            prop.setAttribute("fo:font-weight", (t->font.bold() ? "bold" : "light"));

        prop.setAttribute("fo:font-size", QString("%1pt").arg(t->font.pointSize()));

        if (t->font.underline() != m_defaultFont.underline()) {
            prop.setAttribute("style:text-underline", (t->font.underline() ? "single" : "none"));
            if (t->font.underline())
                prop.setAttribute("style:text-underline-color", "font-color");
        }

        if (t->font.italic() != m_defaultFont.italic())
            prop.setAttribute("fo:font-style", (t->font.italic() ? "italic" : "none"));

        if (t->font.strikeOut() != m_defaultFont.strikeOut())
            prop.setAttribute("style:text-crossing-out", (t->font.strikeOut() ? "single-line" : "none"));

        if (t->color.name() != "#000000")
            prop.setAttribute("fo:color", t->color.name());

        if (t->bgColor.name() != "#ffffff")
            prop.setAttribute("fo:background-color", t->bgColor.name());

        if (t->alignX != KSpread::Style::HAlignUndefined) {
            QString value;
            if (t->alignX == KSpread::Style::Center)
                value = "center";
            else if (t->alignX == KSpread::Style::Right)
                value = "end";
            else if (t->alignX == KSpread::Style::Left)
                value = "start";
            prop.setAttribute("fo:text-align", value);
        }

        if (t->alignY != KSpread::Style::Bottom)   // default in OpenCalc
            prop.setAttribute("fo:vertical-align", (t->alignY == KSpread::Style::Middle ? "middle" : "top"));

        if (t->indent > 0.0) {
            prop.setAttribute("fo:margin-left", QString("%1pt").arg(t->indent));
            if (t->alignX == KSpread::Style::HAlignUndefined)
                prop.setAttribute("fo:text-align", "start");
        }

        if (t->wrap)
            prop.setAttribute("fo:wrap-option", "wrap");

        if (t->vertical) {
            prop.setAttribute("fo:direction", "ttb");
            prop.setAttribute("style:rotation-angle", "0");
        }

        if (t->angle != 0)
            prop.setAttribute("style:rotation-angle", QString::number(t->angle));

        if (!t->print)
            prop.setAttribute("style:print-content", "false");

        if (t->hideAll)
            prop.setAttribute("style:cell-protect", "hidden-and-protected");
        else if (t->notProtected && !t->hideFormula)
            prop.setAttribute("style:cell-protect", "none");
        else if (t->notProtected && t->hideFormula)
            prop.setAttribute("style:cell-protect", "formula-hidden");
        else if (t->hideFormula)
            prop.setAttribute("style:cell-protect", "protected formula-hidden");
        else if (!t->notProtected)
            prop.setAttribute("style:cell-protect", "protected");


        if ((t->left == t->right) && (t->left == t->top) && (t->left == t->bottom)) {
            if ((t->left.width() != 0) && (t->left.style() != Qt::NoPen))
                prop.setAttribute("fo:border", convertPenToString(t->left));
        } else {
            if ((t->left.width() != 0) && (t->left.style() != Qt::NoPen))
                prop.setAttribute("fo:border-left", convertPenToString(t->left));

            if ((t->right.width() != 0) && (t->right.style() != Qt::NoPen))
                prop.setAttribute("fo:border-right", convertPenToString(t->right));

            if ((t->top.width() != 0) && (t->top.style() != Qt::NoPen))
                prop.setAttribute("fo:border-top", convertPenToString(t->top));

            if ((t->bottom.width() != 0) && (t->bottom.style() != Qt::NoPen))
                prop.setAttribute("fo:border-bottom", convertPenToString(t->bottom));
        }

        ts.appendChild(prop);
        autoStyles.appendChild(ts);
    }
}

void OpenCalcStyles::addColumnStyles(QDomDocument & doc, QDomElement & autoStyles)
{
    ColumnStyle * t = 0;
    foreach(t, m_columnStyles) {
        QDomElement ts = doc.createElement("style:style");
        ts.setAttribute("style:name", t->name);
        ts.setAttribute("style:family", "table-column");

        QDomElement prop = doc.createElement("style:properties");
        if (t->breakB != ::Style::none)
            prop.setAttribute("fo:break-before", (t->breakB == ::Style::automatic ? "auto" : "page"));
        prop.setAttribute("style:column-width", QString("%1cm").arg(t->size));

        ts.appendChild(prop);
        autoStyles.appendChild(ts);
    }
}

void OpenCalcStyles::addNumberStyles(QDomDocument & /*doc*/, QDomElement & /*autoStyles*/)
{
}

void OpenCalcStyles::addRowStyles(QDomDocument & doc, QDomElement & autoStyles)
{
    RowStyle * t = 0;
    foreach(t, m_rowStyles) {
        QDomElement ts = doc.createElement("style:style");
        ts.setAttribute("style:name", t->name);
        ts.setAttribute("style:family", "table-row");

        QDomElement prop = doc.createElement("style:properties");
        prop.setAttribute("style:row-height", QString("%1cm").arg(t->size));
        if (t->breakB != ::Style::none)
            prop.setAttribute("fo:break-before", (t->breakB == ::Style::automatic ? "auto" : "page"));

        ts.appendChild(prop);
        autoStyles.appendChild(ts);

    }
}

void OpenCalcStyles::addSheetStyles(QDomDocument & doc, QDomElement & autoStyles)
{
    SheetStyle * t = 0;
    foreach(t, m_sheetStyles) {
        QDomElement ts = doc.createElement("style:style");
        ts.setAttribute("style:name", t->name);
        ts.setAttribute("style:family", "table");
        ts.setAttribute("style:master-page-name", "Default");

        QDomElement prop = doc.createElement("style:properties");
        prop.setAttribute("table:display", (t->visible ? "true" : "false"));

        ts.appendChild(prop);
        autoStyles.appendChild(ts);
    }
}

bool SheetStyle::isEqual(SheetStyle const * const t1, SheetStyle const & t2)
{
    if (t1->visible == t2.visible)
        return true;

    return false;
}

CellStyle::CellStyle()
        : color(Qt::black),
        bgColor(Qt::white),
        indent(-1.0),
        wrap(false),
        vertical(false),
        angle(0),
        print(true),
        left(Qt::black, 0, Qt::NoPen),
        right(Qt::black, 0, Qt::NoPen),
        top(Qt::black, 0, Qt::NoPen),
        bottom(Qt::black, 0, Qt::NoPen),
        hideAll(false),
        hideFormula(false),
        notProtected(false),
        alignX(KSpread::Style::HAlignUndefined),
        alignY(KSpread::Style::Middle)
{
}

void CellStyle::copyData(CellStyle const & ts)
{
    font          = ts.font;
    numberStyle   = ts.numberStyle;
    color         = ts.color;
    bgColor       = ts.bgColor;
    indent        = ts.indent;
    wrap          = ts.wrap;
    vertical      = ts.vertical;
    angle         = ts.angle;
    print         = ts.print;
    left          = ts.left;
    right         = ts.right;
    top           = ts.top;
    bottom        = ts.bottom;
    hideAll       = ts.hideAll;
    hideFormula   = ts.hideFormula;
    notProtected  = ts.notProtected;
    alignX        = ts.alignX;
    alignY        = ts.alignY;
}

bool CellStyle::isEqual(CellStyle const * const t1, CellStyle const & t2)
{
    if ((t1->font == t2.font) && (t1->numberStyle == t2.numberStyle)
            && (t1->color == t2.color) && (t1->bgColor == t2.bgColor)
            && (t1->alignX == t2.alignX) && (t1->alignY == t2.alignY)
            && (t1->indent == t2.indent) && (t1->wrap == t2.wrap)
            && (t1->vertical == t2.vertical) && (t1->angle == t2.angle)
            && (t1->print == t2.print) && (t1->left == t2.left)
            && (t1->right == t2.right) && (t1->top == t2.top)
            && (t1->bottom == t2.bottom) && (t1->hideAll == t2.hideAll)
            && (t1->hideFormula == t2.hideFormula) && (t1->notProtected == t2.notProtected)
       )
        return true;

    return false;
}

// all except the number style
void CellStyle::loadData(CellStyle & cs, const Cell& cell)
{
    const KSpread::Style style = cell.style();
    const KSpread::Style* defaultStyle = cell.sheet()->map()->styleManager()->defaultStyle();

    QFont font = style.font();
    if (font != defaultStyle->font())
        cs.font = font;

    QColor color = style.fontColor();
    if (color != defaultStyle->fontColor())
        cs.color   = color;

    QColor bgColor = style.backgroundColor();
    if (bgColor != defaultStyle->backgroundColor())
        cs.bgColor = bgColor;

    if (style.hasAttribute(KSpread::Style::HorizontalAlignment))
        cs.alignX = style.halign();

    if (style.hasAttribute(KSpread::Style::VerticalAlignment))
        cs.alignY = style.valign();

    if (style.hasAttribute(KSpread::Style::Indentation))
        cs.indent = style.indentation();

    if (style.hasAttribute(KSpread::Style::Angle))
        cs.angle  = -style.angle();

    if (style.hasAttribute(KSpread::Style::MultiRow))
        cs.wrap   = style.wrapText();

    if (style.hasAttribute(KSpread::Style::VerticalText))
        cs.vertical = style.verticalText();

    if (style.hasAttribute(KSpread::Style::DontPrintText))
        cs.print = style.printText();

    if (style.hasAttribute(KSpread::Style::LeftPen))
        cs.left  = style.leftBorderPen();

    if (style.hasAttribute(KSpread::Style::RightPen))
        cs.right = style.rightBorderPen();

    if (style.hasAttribute(KSpread::Style::TopPen))
        cs.top  = style.topBorderPen();

    if (style.hasAttribute(KSpread::Style::BottomPen))
        cs.bottom  = style.bottomBorderPen();

    if (style.hasAttribute(KSpread::Style::NotProtected))
        cs.notProtected = style.notProtected();

    if (style.hasAttribute(KSpread::Style::HideAll))
        cs.hideAll = style.hideAll();

    if (style.hasAttribute(KSpread::Style::HideFormula))
        cs.hideFormula = style.hideFormula();
}

bool NumberStyle::isEqual(NumberStyle const * const t1, NumberStyle const & t2)
{
    if ((t1->type == t2.type) && (t1->pattern == t2.pattern))
        return true;

    return false;
}

void ColumnStyle::copyData(ColumnStyle const & cs)
{
    breakB = cs.breakB;
    size   = cs.size;
}

bool ColumnStyle::isEqual(ColumnStyle const * const c1, ColumnStyle const & c2)
{
    if ((c1->breakB == c2.breakB) && (c1->size == c2.size))
        return true;

    return false;
}

void RowStyle::copyData(RowStyle const & cs)
{
    breakB = cs.breakB;
    size   = cs.size;
}

bool RowStyle::isEqual(RowStyle const * const c1, RowStyle const & c2)
{
    if ((c1->breakB == c2.breakB) && (c1->size == c2.size))
        return true;

    return false;
}
