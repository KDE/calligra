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

#include "FilterPage.h"

#include <math.h>

#include <kglobal.h>
#include <kdebug.h>

#include "GfxState.h"
#include "data.h"
#include "transform.h"
#include "dialog.h"


namespace PDFImport
{

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
int Paragraph::findTab(double xMin, double epsilon, bool firstLine) const
{
    double dx = xMin - (firstLine ? firstIndent : leftIndent);
    if ( fabs(dx)<epsilon ) return -2;
    for (uint i=0; i<tabs.size(); i++)
        if ( fabs(xMin-tabs[i])<epsilon ) return i;
    return -1;
}

uint Paragraph::findNbTabs(uint i, double prevXMax) const
{
    uint k = 0;
    for (; k<tabs.size(); k++)
        if ( tabs[k]>prevXMax ) break;
    if ( k>i ) return 0;
    return i-k+1;
}

int Paragraph::charFromEnd(uint dec, uint &bi)
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
    GfxRGB rgb;
    state->getFillRGB(&rgb);
    GfxFont *font = state->getFont();
    GString *gname = (font ? font->getName() : 0);
    QString name = (gname ? gname->getCString() : 0);
    if ( fontSize<1 ) kdDebug(30516) << "very small font size=" << fontSize
                                     << endl;
    _font = Font(name, qRound(fontSize), toColor(rgb));
}

DRect String::rect() const
{
    DRect r;
    r.top = yMin;
    r.bottom = yMax;
    r.left = xMin;
    r.right = xMax;
    return r;
}

void String::addChar(GfxState *state, double x, double y,
                     double dx, double dy, Unicode u)
{
    Unicode res1, res2;
    if ( checkLigature(u, res1, res2) ) {
        kdDebug(30516) << "found ligature " << QString(QChar(res1))
                       << "+" << QString(QChar(res2)) << endl;
        TextString::addChar(state, x, y, dx/2, dy, res1);
        TextString::addChar(state, x+dx/2, y, dx/2, dy, res2);
    } else {
        TextString::addChar(state, x, y, dx, dy, u);
        checkCombination(this);
    }
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

    letter.r.left = (letter.i==0 ? str->xMin : str->xRight[letter.i-1]);
    letter.r.right = str->xRight[letter.i];
    letter.r.top = str->yMin;
    letter.r.bottom = str->yMax;
    accent.r.left = (accent.i==0 ? xMin : xRight[accent.i-1]);
    accent.r.right = xRight[accent.i];
    accent.r.top = yMin;
    accent.r.bottom = yMax;
    bool ok = ( more(accent.r.left, letter.r.left)
                && less(accent.r.right, letter.r.right) );
    // #### above/below detection is not right !!!
//    bool above = more(accent.r.bottom, letter.r.top);
//    bool below = more(accent.r.top, letter.r.bottom);
//    kdDebug(30516) << "found combi " << QString(QChar(res))
//                   << " accent: " << accent.r.toString() << endl;
//    kdDebug(30516) << "letter: " << letter.r.toString()
//                   << endl;
    // #### FIXME we should check for above or below accent !!!
    if ( !ok ) return false;
//    if (below) {
        // #### TODO
//        kdDebug(30516) << "accent below !" << endl;
//        return false;
//    } else if ( !above ) {
//        kdDebug(30516) << "accent inside letter ??" << endl;
//        return false;
//    }
//    kdDebug(30516) << "combi ok" << endl;

    // replace accent by accented letter
    text[accent.i] = res;
    xMax = letter.r.right;
    if ( accent.i==0 ) xMin = letter.r.left;
    else xRight[accent.i-1] = letter.r.left;
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
Page::Page(Data &data)
    : TextPage(false), _data(data), _lastStr(0)
{
    _rect.left = data.pageRect().right;
    _rect.right = data.pageRect().left;
    _rect.top = data.pageRect().bottom;
    _rect.bottom = data.pageRect().top;

    _links.setAutoDelete(true);
}

void Page::clear()
{
    TextPage::clear();
    _lastStr = 0;
    _links.clear();
    _pars.clear();
}

void Page::beginString(GfxState *state, double x0, double y0)
{
    // This check is needed because Type 3 characters can contain
    // text-drawing operations.
    if (curStr) {
        ++nest;
        return;
    }

    _data.checkTextFrameset();
    curStr = new String(state, x0, y0, fontSize, _data.textIndex());
//    kdDebug(30516) << "---" << endl;
}

void Page::endString()
{
//    kdDebug(30516) << "endString..." << " len=" << curStr->len
//                   << " " << _lastStr
//                   << " len=" << (_lastStr ? _lastStr->len : -1) << endl;
    TextPage::endString();
//    kdDebug(30516) << "  ...endString done" << endl;
}

void Page::addString(TextString *str)
{
//    kdDebug(30516) << "addString..." << endl;
//    if ( str->len==0 ) kdDebug(30516) << "empty string !" << endl;
    if (_lastStr) _lastStr->checkCombination(str);
    _lastStr = (str->len==0 ? 0 : static_cast<String *>(str));
    if (str->len) {
        _rect.left = kMin(_rect.left, str->xMin);
        _rect.right = kMax(_rect.right, str->xMax);
        _rect.top = kMin(_rect.top, str->yMin);
        _rect.bottom = kMax(_rect.bottom, str->yMax);
    }
//    QString s;
//    for (int i=0; i<str->len; i++) s += QChar(str->text[i]);
//    kdDebug(30516) << "string: " << s << " ("
//                   << (str->len>0 ? s[0].unicode() : 0) << ")" << endl;
    TextPage::addString(str);
//    kdDebug(30516) << " ...addString done" << endl;
}

FontFamily Page::checkSpecial(Block &b, uint i) const
{
    Unicode res = 0;
    switch ( PDFImport::checkSpecial(b.text[i].unicode(), res) ) {
    case Bullet:
        kdDebug(30516) << "found bullet" << endl;
        // #### FIXME : if list, use a COUNTER
        // temporarly replace by symbol
        b.text[i] = res;
        return Symbol;
    case SuperScript:
        kdDebug(30516) << "found superscript" << endl;
        // #### FIXME
        break;
    case LatexSpecial:
        if ( !b.font.isLatex() ) break;
        kdDebug(30516) << "found latex special" << endl;
        return Times;
    case SpecialSymbol:
        kdDebug(30516) << "found symbol=" << b.text[i].unicode() << endl;
        return Times;
        //return Symbol;
    default:
        break;
    }

    return Nb_Family;
}

TextBlock *Page::findLastBlock(TextLine *line)
{
    for (TextBlock *block = line->blocks; block; block = block->next)
        if ( block->next==0 ) return block;
    return 0;
}

bool Page::isLastParagraphLine(TextLine *line)
{
    if ( line->next==0 ) return true;
    double dy = line->next->yMin - line->yMax;
    double ndy = line->next->yMax - line->next->yMin;
    String *str = static_cast<String *>(line->blocks->strings);
    String *nStr = static_cast<String *>(line->next->blocks->strings);
    if ( dy>0.5*ndy || str->frameIndex()!=nStr->frameIndex() ) return true;
    TextBlock *b = findLastBlock(line);
    if ( b==0 || b->len==0 ) return false;
    QChar c = QChar(b->text[b->len-1]);
    if ( c!='.' && c!=':' ) return false;
    b = line->next->blocks;
    if ( b==0 || b->len==0 ) return false;
    c = QChar(b->text[0]);
    return ( c.category()==QChar::Letter_Uppercase );
}

void Page::createParagraphs()
{
    Paragraph par;
    bool rightAligned = true, centered = true, justified = true;
    for (TextLine *line = lines; line; line = line->next) {
        bool newParagraph = isLastParagraphLine(line);

        // compute alignment
        double xMin = line->blocks->xMin;
        QString text;
        for (int i=0; i<kMin(4, line->blocks->len); i++)
            text += QChar(line->blocks->text[i]);
//        kdDebug(30516) << text << "  /// par=" << _pars.size() << " line="
//                       << par.nbLines << endl;
        if ( !newParagraph ) {
            double nxMin = line->next->blocks->xMin;
            double xMax = findLastBlock(line)->xMax;
            double nxMax = findLastBlock(line->next)->xMax;
            double xMean = (xMin + xMax) / 2;
            double nxMean = (nxMin + nxMax) / 2;
//            kdDebug(30516) << " xMax=" << xMax << " nxMax=" << nxMax
//                           << " equal=" << equal(xMax, nxMax)
//                           << " xMin=" << xMin << " nxMin=" << nxMin
//                           << " equal=" << equal(xMin, nxMin)
//                           << " equal=" << equal(xMean, nxMean) << endl;
            if ( centered && !equal(xMean, nxMean) ) centered = false;
            if ( rightAligned && !equal(xMax, nxMax) ) rightAligned = false;
            if ( justified && par.nbLines>1 && !equal(xMin, nxMin) ) {
//                kdDebug(30516) << "  not justified (min)" << endl;
                justified = false;
            }
            if ( justified && !isLastParagraphLine(line->next)
                 && !equal(xMax, nxMax) ) {
//                kdDebug(30516) << "  not justified (max)" <<endl;
                justified = false;
            }
        }

        // compute indents
        xMin -= _data.deltaX();
        if ( par.nbLines==1 ) {
            par.firstIndent = xMin;
            par.leftIndent = xMin;
        } else par.leftIndent = kMin(par.leftIndent, xMin);

        // compute tabulations
        for (TextBlock *blk = line->blocks; blk; blk = blk->next) {
            double epsilon = 0.1 * (line->yMax-line->yMin);
            double xMin = blk->xMin - _data.deltaX();
            int res = par.findTab(xMin, epsilon, par.nbLines==1);
            if ( res==-1 ) par.tabs.push_back(xMin);
        }
        qHeapSort2(par.tabs);

        // new paragraph ?
        if (newParagraph) {
            // finalize alignment
            if ( par.nbLines>1 ) { // only one line: defaults to AlignLeft
                if (justified) par.align = AlignBlock;
                else if (centered) par.align = AlignCenter;
                else if (rightAligned) par.align = AlignRight;
            }
            _pars.push_back(par);
            par = Paragraph();
            rightAligned = true;
            centered = true;
            justified = true;
        } else par.nbLines++;
    }
}

#define TIME_START(str) { \
    kdDebug(30516) << str << endl; \
    _time.restart(); \
}
#define TIME_END { kdDebug(30516) << "elapsed=" << _time.elapsed() << endl; }

void Page::prepare()
{
    TIME_START("coalesce strings");
    coalesce();
    TIME_END;

    TIME_START("associate links");
    for (Link *link=_links.first(); link; link=_links.next()) {
        const DRect &r = link->rect();
//        kdDebug(30516) << "link " << r.toString() << endl;
        for (TextLine *line = lines; line; line = line->next)
            for (TextBlock *blk = line->blocks; blk; blk = blk->next)
                for (TextString *str = blk->strings; str; str = str->next) {
                    String *fstr = static_cast<String *>(str);
                    DRect sr = fstr->rect();
//                    kdDebug(30516) << "str " << sr.toString() << " "
//                                   << r.isInside(sr) << endl;
                    if ( r.isInside(sr) ) fstr->link = link;
                }
    }
    TIME_END;

    TIME_START("create paragraphs");
    createParagraphs();
    TIME_END;

    TIME_START("fill paragraphs");
    double height = _data.deltaY();
    TextLine *line = lines;
    for (uint i=0; i<_pars.size(); i++) {
        uint pos = 0;

        _pars[i].offset = line->yMin - height;
        if ( _pars[i].offset>0 ) height += _pars[i].offset;

        for (uint l=0; l<_pars[i].nbLines; l++) {
            // end of previous line (inside a paragraph)
            if ( l!=0 ) {
                bool hyphen = false;
                if (_data.options().removeReturns) {
                    // check hyphen
                    uint bi, pbi;
                    int si = _pars[i].charFromEnd(0, bi);
                    Q_ASSERT( si>=0 );
                    QChar c = _pars[i].blocks[bi].text[si];
                    int psi = _pars[i].charFromEnd(1, pbi);
                    QChar prev =
                        (psi<0 ? QChar::null : _pars[i].blocks[pbi].text[psi]);
                    if ( !prev.isNull() && type(c.unicode())==Hyphen )
                        kdDebug(30516) << "hyphen ? " << QString(prev)
                                       << " type=" << type(prev.unicode())
                                       << endl;
                    TextString *next =
                        (line->next ? line->next->blocks->strings : 0);
                    if ( !prev.isNull() && type(c.unicode())==Hyphen
                         && isLetter( type(prev.unicode()) )
                         && next && next->len>0
                         && isLetter( type(next->text[next->len-1]) ) ) {
                        kdDebug(30516) << "found hyphen" << endl;
                        hyphen = true;
                        _pars[i].blocks[bi].text.remove(si, 1);
                        pos--;
                    }
                }
                if ( !hyphen ) {
                    Block b;
                    b.text = (_data.options().removeReturns ? ' ' : '\n');
                    b.pos = pos;
                    b.font =
                        static_cast<String *>(line->blocks->strings)->font();
                    _pars[i].blocks.push_back(b);
                    pos++;
                }
            }

            int lineHeight = 0;
            TextBlock *prevBlk = 0;
            for (TextBlock *blk = line->blocks; blk; blk = blk->next) {
                // tabulations
                double epsilon = 0.1 * (line->yMax-line->yMin);
                double xMin = blk->xMin - _data.deltaX();
                int res = _pars[i].findTab(xMin, epsilon, l==0);
                if ( res>=0 ) {
                    if (prevBlk) {
                        double xMax = prevBlk->xMax - _data.deltaX();
                        res = _pars[i].findNbTabs(res, xMax);
                    } else res++;
                    // no tabs for first block in AlignCenter and AlignRight
                    if ( prevBlk || _pars[i].align==AlignLeft
                         || _pars[i].align==AlignBlock ) {
                        for (uint k=0; k<uint(res); k++) {
                            Block b;
                            b.text = '\t';
                            b.pos = pos;
                            b.font =
                                static_cast<String *>(blk->strings)->font();
                            _pars[i].blocks.push_back(b);
                            pos++;
                        }
                    }
                }

                // text & format
                for (TextString *str = blk->strings; str; str = str->next) {
                    Block b;
                    for (uint k = 0; k<uint(str->len); k++)
                        b.text += QChar(str->text[k]);
                    if (str->spaceAfter) b.text += ' ';
                    b.pos = pos;
                    String *fstr = static_cast<String *>(str);
                    b.font = fstr->font();
                    b.link = fstr->link;
                    _pars[i].blocks.push_back(b);

                    pos += (fstr->link ? 1 : b.text.length());
                    lineHeight = kMax(lineHeight, b.font.height());
                }

                prevBlk = blk;
            }

            height += lineHeight;
            line = line->next;
        }
    }
    TIME_END;

    TIME_START("check for special chars");
    for (uint i=0; i<_pars.size(); i++) {
        uint inc = 0;
        QValueList<Block> blocks;
        for (uint k=0; k<_pars[i].blocks.size(); k++) {
            Block &b = _pars[i].blocks[k];
            b.pos += inc;
            QString res;
            FontFamily family;
            for (uint l=0; l<b.text.length(); l++) {
                if ( checkSpecial(b, l)==Nb_Family ) res += b.text[l];
                else {
                    if ( !res.isEmpty() ) {
                        blocks.push_back(b);
                        blocks.back().text = res;
                        if (b.link) {
                            inc++;
                            b.pos++;
                        } else b.pos += res.length();
                        res = QString::null;
                    }
                    blocks.push_back(b);
                    blocks.back().font.setFamily(family);
                    blocks.back().text = b.text[l];
                    b.pos++;
                }
            }
            if ( !res.isEmpty() ) {
                b.text = res;
                blocks.push_back(b);
            }
        }
        _pars[i].blocks = blocks;
    }
    TIME_END;

    // this is not really required...
    TIME_START("coalesce formats");
    for (uint i=0; i<_pars.size(); i++) {
        uint dec = 0;
        QValueList<Block> blocks;
        blocks.push_back(_pars[i].blocks[0]);
        for (uint k=1; k<_pars[i].blocks.size(); k++) {
            Block &b = _pars[i].blocks[k];
            b.pos -= dec;
            if ( b.link==blocks.back().link && b.font==blocks.back().font ) {
                blocks.back().text += b.text;
                if (b.link) dec++;
            } else blocks.push_back(b);
        }
        _pars[i].blocks = blocks;
    }
    TIME_END;

    // if no paragraph : add an empty one
    if ( _pars.size()==0 ) {
        Block b;
        Paragraph par;
        par.blocks.push_back(b);
        _pars.push_back(par);
    }
}

void Page::dump()
{
    prepare();

    TIME_START("dump XML");
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
        double delta = _pars[i].firstIndent - _pars[i].leftIndent;
        if ( !equal(delta, 0) ) element.setAttribute("first", delta);
        layouts.push_back(element);

        // offset before
        if ( _pars[i].offset>0 ) {
            QDomElement element = _data.createElement("OFFSETS");
            element.setAttribute("before", _pars[i].offset);
            layouts.push_back(element);
        }

        // flow
        if (_data.options().removeReturns) {
            QString flow;
            kdDebug(30516) << "par=" << i << " flow=" << _pars[i].align
                           << endl;
            switch (_pars[i].align) {
            case AlignLeft: break;
            case AlignRight: flow = "right"; break;
            case AlignCenter: flow = "center"; break;
            case AlignBlock: flow = "justify"; break;
            }
            if ( !flow.isEmpty() ) {
                QDomElement element = _data.createElement("FLOW");
                element.setAttribute("align", flow.utf8());
                layouts.push_back(element);
            }
        }

        // formats
        QString text;
        for (uint k=0; k<_pars[i].blocks.size(); k++) {
            const Block &b = _pars[i].blocks[k];
            text += (b.link ? "#" : b.text);
            QDomElement element = _data.createElement("FORMAT");
            QDomDocument document = _data.document();
            uint len = (b.link ? 1 : b.text.length());
            bool r = b.font.format(document, element, b.pos, len);
            if (b.link) b.link->format(document, element, b.pos, b.text);
            if ( r || b.link ) formats.push_back(element);
        }

        _data.createParagraph(text, layouts, formats);
    }
    TIME_END;
}

}; // namespace
