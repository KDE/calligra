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

#include "FilterPage.h"

#include <math.h>

#include <qfontmetrics.h>
#include <kglobal.h>
#include <kdebug.h>

#include "GfxState.h"
#include "misc.h"
#include "transform.h"

using namespace PDFImport;

// FIX for Qt 3.0
// qvaluevector bug - qheapsort uses 'count' but qvaluevector has only 'size'
template <class Container>
Q_INLINE_TEMPLATES void qHeapSort2( Container &c )
{
    if ( c.begin() == c.end() )
        return;

    // The second last parameter is a hack to retrieve the value type
    // Do the real sorting here
    qHeapSortHelper( c.begin(), c.end(), *(c.begin()), (uint)c.size() );
}

//-----------------------------------------------------------------------------
int FilterParagraph::findTab(double xMin, double epsilon,
                             bool firstLine) const {
    double dx = xMin - (firstLine ? firstIndent : leftIndent);
    if ( fabs(dx)<epsilon ) return -2;
    for (uint i=0; i<tabs.size(); i++)
            if ( fabs(xMin-tabs[i])<epsilon ) return i;
    return -1;
}

uint FilterParagraph::findNbTabs(uint i, double prevXMax) const {
    uint k = 0;
    for (; k<tabs.size(); k++)
        if ( tabs[k]>prevXMax ) break;
    if ( k>i ) return 0;
    return i-k+1;
}

//-----------------------------------------------------------------------------
FilterString::FilterString(GfxState *state, double x0, double y0,
                           double fontSize, uint frameIndex)
    : TextString(state, x0, y0, fontSize), _link(0), _frameIndex(frameIndex)
{
    GfxRGB rgb;
    state->getFillRGB(&rgb);
    GfxFont *font = state->getFont();
    GString *gname = (font ? font->getName() : 0);
    QString name = (gname ? gname->getCString() : 0);
    _font = new FilterFont(name, qRound(fontSize), toColor(rgb));
}

FilterString::~FilterString()
{
    delete _font;
}

void FilterString::addChar(GfxState *state, double x, double y,
                           double dx, double dy, Unicode u)
{
    Unicode res1, res2;
    switch ( checkSpecial(u, res1, res2) ) {
    case Ligature:
        kdDebug(30516) << "found ligature " << QString(QChar(res1))
                       << "+" << QString(QChar(res2)) << endl;
        TextString::addChar(state, x, y, dx/2, dy, res1);
        TextString::addChar(state, x+dx/2, y, dx/2, dy, res2);
        break;
    case Bullet: {
        kdDebug(30516) << "found bullet" << endl;
        // #### FIXME : if list, use a COUNTER
        // temporarly replace by symbol
        _font->setFamily(FilterFont::Symbol);
        TextString::addChar(state, x, y, dx, dy, res1);
        break;
    }
    case SuperScript:
        kdDebug(30516) << "found superscript" << endl;
        // #### FIXME
        TextString::addChar(state, x, y, dx, dy, u);
        break;
    case LatexSymbol: {
        if ( _font->isLatex() && _font->isSymbol() ) {
            kdDebug(30516) << "found latex symbol" << endl;
            // #### FIXME : we should separate the character from the rest
            // of the string ....
            _font->setFamily(FilterFont::Times);
        }
        TextString::addChar(state, x, y, dx, dy, u);
        break;
    }
    case Normal:
        TextString::addChar(state, x, y, dx, dy, u);
        checkCombination(this);
        break;
    }
}

bool FilterString::checkCombination(TextString *str)
{
    if ( len<1 || str->len<1 ) return false;
    if ( str==this && len<2 ) return false;

    struct CharData {
        int i;
        double min, max;
        Unicode u;
    };
    CharData letter, accent;
    letter.i = (str==this ? len-1 : 0);
    letter.u = str->text[letter.i];
    accent.i = (str==this ? len-2 : len-1);
    accent.u = text[accent.i];
    Unicode res;
    if ( !checkAccent(letter.u, accent.u, res) ) return false;

    letter.min = (letter.i==0 ? str->xMin : str->xRight[letter.i-1]);
    letter.max = str->xRight[letter.i];
    accent.min = (accent.i==0 ? xMin : xRight[accent.i-1]);
    accent.max = xRight[accent.i];
//    kdDebug(30516) << "found combi " << QString(QChar(res))
//                   << " accent=[" << accent.min << " , " << accent.max << "] "
//                   << "letter=[" << letter.min << " , " << letter.max << "]"
//                   << " ..." << endl;
    // #### FIXME we should check for above or below accent !!!
    if ( more(letter.min, accent.min) || more(accent.max, letter.max) )
        return false;
//    kdDebug(30516) << "  ...combi ok ! " << endl;

    // replace accent by accented letter
    text[accent.i] = res;
    xMax = letter.max;
    if ( accent.i==0 ) xMin = letter.min;
    else xRight[accent.i-1] = letter.min;
    yMin = kMin(yMin, str->yMin);
    yMax = kMax(yMax, str->yMax);

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

//-----------------------------------------------------------------------------
FilterPage::FilterPage(FilterData &data)
    : TextPage(false), _data(data), _empty(true), _lastStr(0)
{}

void FilterPage::clear()
{
    TextPage::clear();
    _empty = true;
    _lastStr = 0;
    for (uint i=0; i<_links.size(); i++)
        delete _links[i];
    _links.resize(0);
    _pars.resize(0);
}

void FilterPage::beginString(GfxState *state, double x0, double y0)
{
    // This check is needed because Type 3 characters can contain
    // text-drawing operations.
    if (curStr) {
        ++nest;
        return;
    }

    _data.checkTextFrameset();
    curStr = new FilterString(state, x0, y0, fontSize, _data.textIndex());
    _empty = false;
//    kdDebug(30516) << "---" << endl;
}

void FilterPage::endString()
{
//    kdDebug(30516) << "endString..." << " len=" << curStr->len
//                   << " " << _lastStr
//                   << " len=" << (_lastStr ? _lastStr->len : -1) << endl;
    TextPage::endString();
//    kdDebug(30516) << "  ...endString done" << endl;
}

void FilterPage::addString(TextString *str)
{
//    kdDebug(30516) << "addString..." << endl;
    if (_lastStr) _lastStr->checkCombination(str);
//    kdDebug(30516) << "addmid" << endl;
    _lastStr = (str->len==0 ? 0 : static_cast<FilterString *>(str));
//    QString s;
//    for (int i=0; i<str->len; i++) s += QChar(str->text[i]);
//    kdDebug(30516) << "string: " << s <<endl;
    TextPage::addString(str);
//    kdDebug(30516) << " ...addString done" << endl;
}

void FilterPage::prepare()
{
    // associate links to strings
    for (uint i=0; i<_links.size(); i++)
        for (TextLine *line = lines; line; line = line->next)
            for (TextBlock *blk = line->blocks; blk; blk = blk->next)
                for (TextString *str = blk->strings; str; str = str->next)
                    if ( _links[i]->inside(str->xMin, str->xMax, str->yMin,
                                           str->yMax) )
                        static_cast<FilterString *>(str)->_link = _links[i];

    // create paragraphs
    FilterParagraph par;
    par.nbLines = 1;
    for (TextLine *line = lines; line; line = line->next) {
        // compute indents
        double xMin = line->blocks->xMin;
        if ( par.nbLines==1 ) {
            par.firstIndent = xMin;
            par.leftIndent = xMin;
        } else par.leftIndent = kMin(par.leftIndent, xMin);

        // compute tabulations
        for (TextBlock *blk = line->blocks; blk; blk = blk->next) {
            double epsilon = 0.1 * (line->yMax-line->yMin);
            int res = par.findTab(blk->xMin, epsilon, par.nbLines==1);
            if ( res==-1 ) par.tabs.push_back(blk->xMin);
        }
        qHeapSort2(par.tabs);

        // new paragraph ?
        FilterString *nextStr = (line->next ?
            static_cast<FilterString *>(line->next->blocks->strings) : 0);
        FilterString *str = static_cast<FilterString *>(line->blocks->strings);
        if ( line->next==0 || (line->next->yMin - line->yMax) >
             0.5*(line->next->yMax - line->next->yMin) ||
             nextStr->_frameIndex!=str->_frameIndex ) {
            par.frameIndex = str->_frameIndex;
            _pars.push_back(par);
            par.nbLines = 1;
            par.tabs.clear();
        } else par.nbLines++;
    }

    // fill paragraphs
    double height = 0;
    TextLine *line = lines;
    for (uint i=0; i<_pars.size(); i++) {
        uint pos = 0;

        _pars[i].offset = line->yMin - height;
        if ( _pars[i].offset>0 ) height += _pars[i].offset;

        for (uint l=0; l<_pars[i].nbLines; l++) {
            // end of previous line (inside a paragraph)
            if ( l!=0 ) {
                FilterBlock b;
                b.text = '\n';
                b.pos = pos;
                b.font =
                    static_cast<FilterString *>(line->blocks->strings)->_font;
                _pars[i].blocks.push_back(b);
                pos++;
            }

            int lineHeight = 0;
            TextBlock *prevBlk = 0;
            for (TextBlock *blk = line->blocks; blk; blk = blk->next) {
                // tabulations
                double epsilon = 0.1 * (line->yMax-line->yMin);
                int res = _pars[i].findTab(blk->xMin, epsilon, l==0);
                if ( res>=0 ) {
                    if (prevBlk) res = _pars[i].findNbTabs(res, prevBlk->xMax);
                    else res++;
                    for (uint k=0; k<uint(res); k++) {
                        FilterBlock b;
                        b.text = '\t';
                        b.pos = pos;
                        b.font =
                            static_cast<FilterString *>(blk->strings)->_font;
                        _pars[i].blocks.push_back(b);
                        pos++;
                    }
                }

                // text & format
                for (TextString *str = blk->strings; str; str = str->next) {
                    QString tmp;
                    for (uint k = 0; k<uint(str->len); k++)
                        tmp += QChar(str->text[k]);
                    if (str->spaceAfter) tmp += ' ';

                    FilterBlock b;
                    b.pos = pos;
                    FilterString *fstr = static_cast<FilterString *>(str);
                    b.font = fstr->_font;
                    if (fstr->_link) {
                        b.text = '#';
                        b.link = fstr->_link;
                        b.linkText = tmp;
                        pos++;
                    } else {
                        b.text = tmp;
                        pos += tmp.length();
                    }
                    _pars[i].blocks.push_back(b);

                    QFontMetrics fm( b.font->font() );
                    lineHeight = kMax(lineHeight, fm.lineSpacing());
                }

                prevBlk = blk;
            }

            height += lineHeight;
            line = line->next;
        }
    }

    // coalesce formats
    for (uint i=0; i<_pars.size(); i++) {
        uint dec = 0;
        QValueVector<FilterBlock> blocks;
        blocks.push_back(_pars[i].blocks[0]);
        for (uint k=1; k<_pars[i].blocks.size(); k++) {
            FilterBlock &b = _pars[i].blocks[k];
            b.pos -= dec;
            if ( (b.link==blocks.back().link) &&
                 (*b.font)==(*blocks.back().font) ) {
                if (b.link) {
                    blocks.back().linkText += b.linkText;
                    dec++;
                } else blocks.back().text += b.text;
            } else {
                blocks.push_back(b);
            }
        }
        _pars[i].blocks = blocks;
    }

    // if no paragraph : add an empty one
    if ( _pars.size()==0 ) {
        par.firstIndent = 0;
        par.leftIndent = 0;
        FilterBlock b;
        b.font = FilterFont::defaultFont;
        b.pos = 0;
        b.text = "";
        par.blocks.push_back(b);
        _pars.push_back(par);
    }
}

void FilterPage::dump()
{
    if (_empty) return;
    coalesce();
    prepare();

    for (uint i=0; i<_pars.size(); i++) {
        QValueVector<QDomElement> layouts;
        QValueVector<QDomElement> formats;

        // tabulations
        for (uint k=0; k<_pars[i].tabs.size(); k++) {
            QDomElement element = _data.createElement("TABULATOR");
            element.setAttribute("type", 0);
            element.setAttribute("ptpos", _pars[i].tabs[k]);
            element.setAttribute("width", 0);
            element.setAttribute("filling", 0);
            layouts.push_back(element);
        }

        // indents
        QDomElement element = _data.createElement("INDENTS");
        element.setAttribute("left", _pars[i].leftIndent);
        double dx = _pars[i].firstIndent - _pars[i].leftIndent;
        if ( dx!=0 ) element.setAttribute("first", dx);
        layouts.push_back(element);

        // offset before
        if ( _pars[i].offset>0 ) {
            QDomElement element = _data.createElement("OFFSETS");
            element.setAttribute("before", _pars[i].offset);
            layouts.push_back(element);
        }

        // formats
        QString text;
        for (uint k=0; k<_pars[i].blocks.size(); k++) {
            const FilterBlock &b = _pars[i].blocks[k];
            text += b.text;
            QDomElement element = _data.createElement("FORMAT");
            QDomDocument document = _data.document();
            bool r = b.font->format(document, element, b.pos, b.text.length());
            if (b.link) b.link->format(document, element, b.pos, b.linkText);
            if ( r || b.link ) formats.push_back(element);
        }

        _data.createParagraph(text, layouts, formats);
    }
}
