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


class FilterFont;
class FilterLink;

//-----------------------------------------------------------------------------
class FilterBlock
{
 public:
    FilterBlock() : link(0) {}

    FilterFont *font;
    FilterLink *link;
    uint pos;
    QString text, linkText;
};

//-----------------------------------------------------------------------------
class FilterParagraph
{
 public:
    uint nbLines, frameIndex;
    double firstIndent, leftIndent, offset;
    QValueVector<double> tabs;
    QValueVector<FilterBlock> blocks;

    int findTab(double xMin, double epsilon, bool firstLine) const;
    uint findNbTabs(uint i, double prevXMax) const;
};

//-----------------------------------------------------------------------------
class FilterPage;

class FilterString : public TextString
{
 public:
    FilterString(GfxState *state, double x0, double y0,
                 double fontSize, uint frameIndex);
    ~FilterString();

 private:
    FilterFont *_font;
    FilterLink *_link;
    uint        _frameIndex;

    void addChar(GfxState *state, double x, double y,
                 double dx, double dy, Unicode u);
    bool checkCombination(TextString *s);

    friend class FilterPage;
};

//-----------------------------------------------------------------------------
class FilterData;

class FilterPage : public TextPage
{
 public:
    FilterPage(FilterData &data);

    void beginString(GfxState *, double x0, double y0);
    void addString(TextString *);
    void endString();
    void dump();
    void addLink(FilterLink *link) { _links.push_back(link); }
    void clear();

 private:
    FilterData &_data;
    bool        _empty;
    QValueVector<FilterParagraph> _pars;
    QValueVector<FilterLink *>    _links;
    FilterString *_lastStr;

    void prepare();
};

#endif
