/*
 * Copyright (c) 2002 Nicolas HADACEK (hadacek@kde.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef FILTERPAGE_H
#define FILTERPAGE_H

#include "TextOutputDev.h"

#include <qdom.h>
#include <qvaluevector.h>


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
    uint nbLines;
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
                 double fontSize);
    ~FilterString();

 private:
    FilterFont *_font;
    FilterLink *_link;

    friend class FilterPage;
};

//-----------------------------------------------------------------------------
class FilterPage : public TextPage
{
 public:
    FilterPage(QDomDocument &, QDomElement &mainFrameset);
    ~FilterPage();

    void beginString(GfxState *, double x0, double y0);
    void prepare();
    void dump();
    void addLink(FilterLink *link) { _links.append(link); }

 private:
    QDomDocument _document;
    QDomElement  _mainFrameset;

    QValueVector<FilterParagraph> _pars;
    QValueVector<FilterLink *>    _links;

    void createParagraph(const QString &text,
                         const QValueVector<QDomElement> &layouts,
                         const QValueVector<QDomElement> &formats);
};

#endif
