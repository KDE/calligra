/*
 * Copyright (c) 2002 Nicolas HADACEK (hadacek@kde.org)
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

#ifndef FILTERPAGE_H
#define FILTERPAGE_H

#include "TextOutputDev.h"

#include <qvaluevector.h>
#include <qdom.h>
#include <qdatetime.h>
#include <qptrlist.h>

#include "misc.h"


namespace PDFImport
{

class Page;
class Data;

//-----------------------------------------------------------------------------
class Block
{
public:
    Block() : link(0), pos(0) {}

public:
    Font    font;
    const Link *link;
    uint    pos;
    QString text;
};

//-----------------------------------------------------------------------------
enum Align { AlignLeft, AlignRight, AlignCenter, AlignBlock };

class Paragraph
{
public:
    Paragraph()
        : nbLines(1), firstIndent(0), leftIndent(0), align(AlignLeft) {}

public:
    uint                 nbLines, frameIndex;
    double               firstIndent, leftIndent, offset;
    Align                align;
    QValueVector<double> tabs;
    QValueList<Block>    blocks;

public:
    int findTab(double xMin, double epsilon, bool firstLine) const;
    uint findNbTabs(uint i, double prevXMax) const;
    int charFromEnd(uint dec, uint &blockIndex);
};

//-----------------------------------------------------------------------------
class String : public TextString
{
public:
    String(GfxState *state, double x0, double y0,
           double fontSize, uint frameIndex);

    uint frameIndex() const { return _frameIndex; }
    const Font &font() const { return _font; }
    DRect rect() const;
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

//-----------------------------------------------------------------------------
class Page : public TextPage
{
public:
    Page(Data &data);

    void beginString(GfxState *, double x0, double y0);
    void addString(TextString *);
    void endString();
    void dump();
    void addLink(Link *link) { _links.append(link); }
    void clear();

    const DRect &rect() const { return _rect; }

private:
    FontFamily checkSpecial(Block &, uint index) const;
    static TextBlock *findLastBlock(TextLine *);
    static bool isLastParagraphLine(TextLine *);
    void createParagraphs();
    void prepare();

private:
    Data &_data;
    QValueList<Paragraph> _pars;
    QPtrList<Link> _links;
    String *_lastStr;
    QTime _time; // debug
    DRect _rect;
};

}; // namespace

#endif
