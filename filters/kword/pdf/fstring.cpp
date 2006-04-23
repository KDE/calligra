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

#include "fstring.h"

#include <kglobal.h>
#include <kdebug.h>

#include "data.h"
#include "transform.h"
//Added by qt3to4:
#include <Q3ValueList>


namespace PDFImport {

//-----------------------------------------------------------------------------
QDomElement Tabulator::createElement(Data &data) const
{
    QDomElement element = data.createElement("TABULATOR");
    element.setAttribute("type", alignment);
    element.setAttribute("ptpos", pos);
    element.setAttribute("filling", filling);
    if ( alignment==Character )
        element.setAttribute("alignchar", QString(alignmentChar));
    return element;
}

//-----------------------------------------------------------------------------
Paragraph::Paragraph()
    : type(Body), firstIndent(0), leftIndent(0), align(AlignLeft)
{}

Paragraph::Paragraph(TextLine *first, uint nbLines)
    :  type(Body), firstIndent(0), leftIndent(0), align(AlignLeft)
{
    // fill line list
    TextLine *line = first;
    for (uint i=0; i<nbLines; i++) {
        Q_ASSERT( line!=0 );
        _lines.append(line);
        line = line->next;
    }

    // compute rectangle
    Q3ValueList<TextLine *>::const_iterator it;
    for (it = _lines.begin(); it!=_lines.end(); ++it)
        for (TextBlock *block = (*it)->blocks; block; block = block->next) {
            DRect br(block->xMin, block->xMax, block->yMin, block->yMax);
            _rect.unite(br);
        }
}

int Paragraph::findTab(double xMin, const TextLine *line) const
{
    double epsilon = 0.1 * (line->yMax - line->yMin);
    double dx = xMin - (isFirst(line) ? firstIndent : leftIndent);
    if ( fabs(dx)<epsilon ) return -2;
    for (uint i=0; i<tabs.size(); i++)
        if ( fabs(xMin-tabs[i].pos)<epsilon ) return i;
    return -1;
}

uint Paragraph::findNbTabs(uint i, double prevXMax) const
{
    uint k = 0;
    for (; k<tabs.size(); k++)
        if ( tabs[k].pos>prevXMax ) break;
    if ( k>i ) return 0;
    return i-k+1;
}

int Paragraph::charFromEnd(uint dec, uint &bi) const
{
    uint k = 0;
    for (uint i=blocks.size(); i>0; i--) {
        for (uint j=blocks[i-1].text.length(); j>0; j--) {
            if ( k==dec ) {
                bi = i-1;
                return j-1;
            }
            k++;
        }
    }
    return -1;
}

//-----------------------------------------------------------------------------
String::String(GfxState *state, double x0, double y0,
               double fontSize, uint frameIndex)
    : TextString(state, x0, y0, fontSize), link(0), _frameIndex(frameIndex)
{
    _font = Font(state, fontSize);
}

void String::addChar(GfxState *state, double x, double y,
                     double dx, double dy, Unicode u)
{
    Unicode res[MaxLigatureLength];
    uint nb = checkLigature(u, res);
    QString sdebug;
    if ( nb>1 ) sdebug = "found ligature ";
    double ddx = dx / nb;
    for (uint i=0; i<nb; i++) {
        TextString::addChar(state, x + ddx*i, y, ddx, dy, res[i]);
        if ( nb>1 ) sdebug += QChar(res[i]);
    }
    if ( nb>1 ) kDebug(30516) << sdebug << endl;
    else checkCombination(this);
}

bool String::checkCombination(TextString *str)
{
    if ( len<1 || str->len<1 ) return false;
    if ( str==this && len<2 ) return false;

    struct CharData {
        int i;
        DRect r;
        Unicode u;
    };
    CharData letter, accent;
    letter.i = (str==this ? len-1 : 0);
    letter.u = str->text[letter.i];
    accent.i = (str==this ? len-2 : len-1);
    accent.u = text[accent.i];
    Unicode res = checkCombi(letter.u, accent.u);
    if ( res==0 ) return false;

    letter.r.setLeft(letter.i==0 ? str->xMin : str->xRight[letter.i-1]);
    letter.r.setRight(str->xRight[letter.i]);
    letter.r.setTop(str->yMin);
    letter.r.setBottom(str->yMax);
    accent.r.setLeft(accent.i==0 ? xMin : xRight[accent.i-1]);
    accent.r.setRight(xRight[accent.i]);
    accent.r.setTop(yMin);
    accent.r.setBottom(yMax);
    bool ok = ( more(accent.r.left(), letter.r.left(), 0.005)
                && less(accent.r.right(), letter.r.right(), 0.005) );
    // #### above/below detection is not right !!!
//    bool above = more(accent.r.bottom, letter.r.top);
//    bool below = more(accent.r.top, letter.r.bottom);
//    kDebug(30516) << "found combi " << QString(QChar(res))
//                   << " accent: " << accent.r.toString() << endl;
//    kDebug(30516) << "letter: " << letter.r.toString()
//                   << endl;
    // #### FIXME we should check for above or below accent !!!
    if ( !ok ) return false;
//    if (below) {
        // #### TODO
//        kDebug(30516) << "accent below !" << endl;
//        return false;
//    } else if ( !above ) {
//        kDebug(30516) << "accent inside letter ??" << endl;
//        return false;
//    }
//    kDebug(30516) << "combi ok" << endl;

    // replace accent by accented letter
    text[accent.i] = res;
    xMax = letter.r.right();
    if ( accent.i==0 ) xMin = letter.r.left();
    else xRight[accent.i-1] = letter.r.left();
    yMin = qMin(yMin, str->yMin);
    yMax = qMax(yMax, str->yMax);

    // append remaining string
    if ( str==this ) {
        len--;
        for (int k=letter.i+1; k<str->len; k++) {
            xRight[k-1] = xRight[k];
            text[k-1] = text[k];
        }
    } else {
        for (int k=letter.i+1; k<str->len; k++)
        TextString::addChar(0, str->xRight[k-1], 0,
                            str->xRight[k] - str->xRight[k-1], 0,
                            str->text[k]);
        str->len = 0;
    }

    return true;
}

} // namespace
