/*
 * Copyright (c) 2002-2003 Nicolas HADACEK (hadacek@kde.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef FSTRING_H
#define FSTRING_H

#include "TextOutputDev.h"

#include <q3valuevector.h>
//Added by qt3to4:
#include <Q3ValueList>

#include "misc.h"


namespace PDFImport
{

class Data;

//-----------------------------------------------------------------------------
class Tabulator
{
public:
    enum Alignment { Left = 0, Center, Right, Character };
    enum Filling { Blank = 0, Dots, Line, Dash, DashDot, DashDotDot };

    Tabulator() : alignment(Left) {}

    QDomElement createElement(Data &data) const;

public:
    double    pos;
    Alignment alignment;
    Filling   filling;
    QChar     alignmentChar;
};

inline bool operator <(const Tabulator &t1, const Tabulator &t2)
{ return t1.pos<t2.pos; }

//-----------------------------------------------------------------------------
class Block
{
public:
    Block() : link(0) {}

public:
    Font        font;
    const Link *link;
    QString     text;
};

//-----------------------------------------------------------------------------
enum Align { AlignLeft, AlignRight, AlignCenter, AlignBlock };

class Paragraph
{
public:
    Paragraph();
    Paragraph(TextLine *first, uint nbLines);

    const DRect &rect() const { return _rect; }

    bool hasOneLine() const { return _lines.count()==1; }
    const Q3ValueList<TextLine *> lines() const { return _lines; }
    bool isFirst(const TextLine *line) const { return line==_lines.first();}
    bool isSecond(const TextLine* line) const
        { return (_lines.count()>1 && line==_lines.first()->next); }
    bool isLast(const TextLine *line) const { return line==_lines.last(); }

    int findTab(double xMin, const TextLine *) const;
    uint findNbTabs(uint i, double prevXMax) const;
    int charFromEnd(uint dec, uint &blockIndex) const;

public:
    ParagraphType type;
    uint          frameIndex;
    double        firstIndent, leftIndent, offset;
    Align         align;
    Q3ValueVector<Tabulator> tabs;
    Q3ValueList<Block>       blocks;

private:
    Q3ValueList<TextLine *> _lines;
    DRect                  _rect;
};

//-----------------------------------------------------------------------------
class String : public TextString
{
public:
    String(GfxState *state, double x0, double y0,
           double fontSize, uint frameIndex);

    uint frameIndex() const { return _frameIndex; }
    const Font &font() const { return _font; }
    DRect rect() const { return DRect(xMin, xMax, yMin, yMax); }
    bool checkCombination(TextString *s);

public:
    const Link *link;

private:
    void addChar(GfxState *state, double x, double y,
                 double dx, double dy, Unicode u);

private:
    Font _font;
    uint _frameIndex;
};

} // namespace

#endif
