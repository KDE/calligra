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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef FSTRING_H
#define FSTRING_H

#include "TextOutputDev.h"

#include <qvaluevector.h>

#include "misc.h"


namespace PDFImport
{

class Data;

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
    const QValueList<TextLine *> lines() const { return _lines; }
    bool isFirst(const TextLine *line) const { return line==_lines.first();}
    bool isLast(const TextLine *line) const { return line==_lines.last(); }

    int findTab(double xMin, double epsilon, bool firstLine) const;
    uint findNbTabs(uint i, double prevXMax) const;
    int charFromEnd(uint dec, uint &blockIndex);

public:
    ParagraphType        type;
    uint                 frameIndex;
    double               firstIndent, leftIndent, offset;
    Align                align;
    QValueVector<double> tabs;
    QValueList<Block>    blocks;

private:
    QValueList<TextLine *> _lines;
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

}; // namespace

#endif
