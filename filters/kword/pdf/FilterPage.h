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

#ifndef FILTERPAGE_H
#define FILTERPAGE_H

#include "TextOutputDev.h"

#include <QDateTime>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <Q3ValueList>

#include "fstring.h"


namespace PDFImport
{

class Paragraph;
class Data;

class Page : public TextPage
{
public:
    Page(Data &data);

    Q3ValueVector<DRect> &rects() { return _rects; }
    bool hasHeader() const;
    bool hasFooter() const;

    void clear();

    // first pass
    void beginString(GfxState *, double x0, double y0);
    void addString(TextString *);
    void endString();
    void addLink(Link *link) { _links.append(link); }
    void endPage();

    // second pass
    void dump();

public:
    Q3ValueList<QDomElement> pictures;

private:
    FontFamily checkSpecial(QChar &, const Font &) const;
    static TextBlock *block(TextLine *, int index);
    static bool isLastParagraphLine(TextLine *, const Paragraph &);

    // first pass
    void createParagraphs();
    void checkHeader();
    void checkFooter();

    // second pass
    void initParagraph(Paragraph &) const;
    void fillParagraph(Paragraph &, double &offset) const;
    void checkSpecialChars(Paragraph &) const;
    void coalesce(Paragraph &) const;
    void prepare();
    void dump(const Paragraph &);

private:
    Data &_data;
    Q3ValueList<Paragraph> _pars;
    Q3PtrList<Link> _links;
    String *_lastStr;
    QTime _time; // debug
    Q3ValueVector<DRect> _rects;
};

} // namespace

#endif
