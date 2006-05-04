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

#include "FilterPage.h"

#include <math.h>

#include <kglobal.h>
#include <kdebug.h>

#include "data.h"
#include "transform.h"
#include "dialog.h"
//Added by qt3to4:
#include <Q3ValueList>
#include <q3tl.h>

#define TIME_START(str) { \
    kDebug(30516) << str << endl; \
    _time.restart(); \
}
#define TIME_END { kDebug(30516) << "elapsed=" << _time.elapsed() << endl; }


namespace PDFImport
{

//-----------------------------------------------------------------------------
Page::Page(Data &data)
    : TextPage(false), _data(data), _lastStr(0), _rects(Nb_ParagraphTypes)
{
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

//    _data.checkTextFrameset();
    curStr = new String(state, x0, y0, fontSize, _data.textIndex());
//    kDebug(30516) << "---" << endl;
}

void Page::endString()
{
//    kDebug(30516) << "endString..." << " len=" << curStr->len
//                   << " " << _lastStr
//                   << " len=" << (_lastStr ? _lastStr->len : -1) << endl;
    TextPage::endString();
//    kDebug(30516) << "  ...endString done" << endl;
}

void Page::addString(TextString *str)
{
//    kDebug(30516) << "addString..." << endl;
//    if ( str->len==0 ) kDebug(30516) << "empty string !" << endl;
    if (_lastStr) _lastStr->checkCombination(str);
    _lastStr = (str->len==0 ? 0 : static_cast<String *>(str));
//    QString s;
//    for (int i=0; i<str->len; i++) s += QChar(str->text[i]);
//    kDebug(30516) << "string: " << s << " ("
//                   << (str->len>0 ? s[0].unicode() : 0) << ")" << endl;
    TextPage::addString(str);
//    kDebug(30516) << " ...addString done" << endl;
}

TextBlock *Page::block(TextLine *line, int index)
{
    uint k = 0;
    if ( index<0 )
        for (TextBlock *block = line->blocks; block; block = block->next) k++;
    k += index;
    uint i = 0;
    for (TextBlock *block = line->blocks; block; block = block->next) {
        if ( i==k ) return block;
        i++;
    }
    return 0;
}

//-----------------------------------------------------------------------------
bool Page::isLastParagraphLine(TextLine *line, const Paragraph &par)
{
    // last line of page
    if ( line->next==0 ) return true;
    double dy = line->next->yMin - line->yMax;
    double ndy = line->next->yMax - line->next->yMin;
    String *str = static_cast<String *>(line->blocks->strings);
    String *nStr = static_cast<String *>(line->next->blocks->strings);
    // next line far below
    if ( dy>0.5*ndy ) return true;
    // image displayed before next line (?)
    if ( str->frameIndex()!=nStr->frameIndex() ) return true;
    if ( line->blocks==0 ) return false; // should not happen
    // if contains one or more inside tabs
    if (line->blocks->next) return true;
    if ( line->next && line->next->blocks==0 ) return false;//should not happen
    // if next line contains one or more inside tabs
    if ( line->next && line->next->blocks->next ) return true;
    TextBlock *b = block(line, -1);
    if ( b==0 || b->len==0 ) return false; // should not happen
    QChar c = QChar(b->text[b->len-1]);
    // last line char is not '.' or ':'
    if ( c!='.' && c!=':' ) return false;
    // if at line end and block aligned : same paragraph
    return ( !equal(b->xMax, par.rect().right()) );
}

void Page::createParagraphs()
{
    TextLine *first = lines;
    uint nbLines = 0;
    for (TextLine *line = lines; line; line = line->next) {
        nbLines++;
        Paragraph par(first, nbLines);
        if ( isLastParagraphLine(line, par) ) {
            _pars.push_back(par);
            nbLines = 0;
            first = line->next;
        }
    }
}

void Page::checkHeader()
{
    uint s = _pars.size();
    if ( s==0 ) return;
    Paragraph &par = _pars[0];
    if ( par.lines().count()!=1 ) return;
    const TextLine *first = par.lines().first();
    const TextLine *second = (s>1 ? _pars[1].lines().first() : 0);
    double limit = 0.2 * _data.pageRect().height();
    double delta = 2 * qMin(first->yMax - first->yMin, 12.0);
//    kDebug(30516) << "first: " << first->yMax << " (" << limit << ")" << endl;
//    if (second) kDebug(30516) << "second: " << second->yMin << " "
//                               << second->yMin-first->yMax << " (" << delta
//                               << ")" << endl;
    if ( first->yMax>limit ) return;
    if ( second && (second->yMin-first->yMax)<delta ) return;
    par.type = Header;
    _rects[Header] = par.rect();
}

bool Page::hasHeader() const
{
    return (_pars.size()>0 ? _pars[0].type==Header : false);
}

void Page::checkFooter()
{
    uint s = _pars.size();
    if ( s==0 ) return;
    Paragraph &par = _pars[s-1];
    if ( par.lines().count()!=1 ) return;
    const TextLine *last = par.lines().first();
    const TextLine *blast = (s>1 ? _pars[s-2].lines().last() : 0);
    double limit = 0.8 * _data.pageRect().height();
    double delta = 2 * qMin(last->yMax-last->yMin, 12.0);
//    kDebug(30516) << "last: " << last->yMax << " (" << limit << ")" << endl;
//    if (blast) kDebug(30516) << "blast: " << blast->yMin << " "
//                              <<  last->yMin-blast->yMax << " (" << delta
//                              << ")" << endl;
    if ( last->yMin<limit ) return;
    if ( blast && (last->yMin-blast->yMax)<delta ) return;
    par.type = Footer;
    _rects[Footer] = par.rect();
}

bool Page::hasFooter() const
{
    return (_pars.size()>0 ? _pars[_pars.size()-1].type==Footer
            : false);
}

void Page::endPage()
{
    TIME_START("coalesce strings");
    TextPage::coalesce();
    TIME_END;

    createParagraphs();

    // check header and footer
    checkHeader();
//    if ( hasHeader() ) kDebug(30516) << "has header" << endl;
    checkFooter();
//    if ( hasFooter() ) kDebug(30516) << "has footer" << endl;

    // compute body rect
    uint begin = (hasHeader() ? 1 : 0);
    uint end = _pars.size() - (hasFooter() ? 1 : 0);
    for (uint i=begin; i<end; i++)
        _rects[Body].unite(_pars[i].rect());
}


//-----------------------------------------------------------------------------
void Page::initParagraph(Paragraph &par) const
{
    bool rightAligned = true, centered = true, leftAligned = true;
    const double pleft = _rects[par.type].left();
    const double pright = _rects[par.type].right();
    const double pmean = (pleft + pright) / 2;

    Q3ValueList<TextLine *>::const_iterator it;
    for (it = par.lines().begin(); it!=par.lines().end(); ++it) {

        // compute tabulations
        Tabulator tab;
        for (TextBlock *blk = (*it)->blocks; blk; blk = blk->next) {
            // if tabulated text is aligned on right edge: put a tab
            // on right edge and the tab type will be right aligned...
            double tabRightAligned = equal(blk->xMax, pright);
            double dx = (tabRightAligned ? pright : blk->xMin) - pleft;
            // #### if the tab is just at the frame edge:
            // the text is sent to next line ???
            if (tabRightAligned) dx -= 0.1;
            int res = par.findTab(dx, *it);
            if ( res==-1 ) {
                tab.pos = dx;
                if (tabRightAligned) {
                    tab.alignment = Tabulator::Right;
                    kDebug(30516) << "tabulated text right aligned.." << endl;
                } else tab.alignment = Tabulator::Left;
                par.tabs.push_back(tab);
            }
        }
        qSort(par.tabs);

        // compute indents
        double left = (*it)->blocks->xMin - pleft;
        if ( par.isFirst(*it) ) {
            par.firstIndent = left;
            par.leftIndent = left;
        } else if ( par.isSecond(*it) ) par.leftIndent = left;
        else par.leftIndent = qMin(par.leftIndent, left);
    }

    // compute alignment
    for (it = par.lines().begin(); it!=par.lines().end(); ++it) {
        double left = (*it)->blocks->xMin;
        double right = block(*it, -1)->xMax;
        double mean = (left + right) / 2;
//        QString text;
//        for (int i=0; i<kMin(4, (*it)->blocks->len); i++)
//            text += QChar((*it)->blocks->text[i]);
//        kDebug(30516) << text << " left=" << left
//                       << " pleft=" << pleft + par.leftIndent
//                       << " indent=" << par.leftIndent
//                       << " findent=" << par.firstIndent << endl;
        if ( centered && !equal(mean, pmean) ) centered = false;
        if ( leftAligned && (!par.isFirst(*it) || par.hasOneLine())
             && !equal(left, pleft + par.leftIndent, 0.05) ) {
            kDebug(30516) << "not left aligned" << endl;
            leftAligned = false;
        }
        if ( rightAligned && (!par.isLast(*it) || par.hasOneLine())
             && !equal(right, pright, 0.05) ) {
            kDebug(30516) << "not right aligned" << endl;
            rightAligned = false;
        }
    }

    // finalize alignment
    if (rightAligned) par.align = (leftAligned ? AlignBlock : AlignRight);
    else if (centered) par.align = AlignCenter;
}

void Page::fillParagraph(Paragraph &par, double &offset) const
{
    const double pleft = _rects[par.type].left();
    const double pright = _rects[par.type].right();
    par.offset = par.lines().first()->yMin - offset;
//    kDebug(30516) << "offset=" << offset
//                   << " yMin=" << par.lines().first()->yMin
//                   << " paroffset=" << par.offset << endl;
    if ( par.offset>0 ) offset += par.offset;

    Q3ValueList<TextLine *>::const_iterator it;
    for (it = par.lines().begin(); it!=par.lines().end(); ++it) {
        // end of previous line (inside a paragraph)
        if ( !par.isFirst(*it) ) {
            bool hyphen = false;
            if (_data.options().smart) {
                // check hyphen
                uint bi, pbi;
                int si = par.charFromEnd(0, bi);
                Q_ASSERT( si>=0 );
                QChar c = par.blocks[bi].text[si];
                int psi = par.charFromEnd(1, pbi);
                QChar prev = (psi<0 ? QChar() : par.blocks[pbi].text[psi]);
                if ( !prev.isNull() && type(c.unicode())==Hyphen )
                    kDebug(30516) << "hyphen ? " << QString(prev)
                                   << " type=" << type(prev.unicode())
                                   << endl;
                TextString *next =
                    ((*it)->next ? (*it)->next->blocks->strings : 0);
                if ( !prev.isNull() && type(c.unicode())==Hyphen
                     && isLetter( type(prev.unicode()) )
                     && next && next->len>0
                     && isLetter( type(next->text[next->len-1]) ) ) {
                    kDebug(30516) << "found hyphen" << endl;
                    hyphen = true;
                    par.blocks[bi].text.remove(si, 1);
                }
            }
            if ( !hyphen ) {
                Block b;
                bool remove = _data.options().smart;
                if ( remove && par.align!=AlignBlock )
                    remove = ( par.rect().right()>0.9*pright );
                b.text = (remove ? ' ' : '\n');
                b.font = static_cast<String *>((*it)->blocks->strings)->font();
                par.blocks.push_back(b);
            }
        }

        int lineHeight = 0;
        TextBlock *prevBlk = 0;
        for (TextBlock *blk = (*it)->blocks; blk; blk = blk->next) {

            // tabulations
            double tabRightAligned = equal(blk->xMax, pright);
            double dx = (tabRightAligned ? pright : blk->xMin) - pleft;
            int res = par.findTab(dx, *it);
            if ( res>=0 ) {
                if (prevBlk) {
                    double xMax = prevBlk->xMax - pleft;
                    res = par.findNbTabs(res, xMax);
                    if ( res==0 ) continue;
                } else res++;
                // no tabs for first block in AlignCenter and AlignRight
                // if smart mode
                if ( prevBlk || !_data.options().smart
                     || (par.align!=Qt::AlignCenter && par.align!=Qt::AlignRight) ) {
                    Block b;
                    b.font = static_cast<String *>(blk->strings)->font();
                    for (uint k=0; k<(uint)res; k++) b.text += '\t';
                    par.blocks.push_back(b);
                }
            }

            // text & format
            for (TextString *str = blk->strings; str; str = str->next) {
                Block b;
                for (uint k = 0; k<uint(str->len); k++)
                    b.text += QChar(str->text[k]);
                if (str->spaceAfter) b.text += ' ';
                String *fstr = static_cast<String *>(str);
                b.font = fstr->font();
                b.link = fstr->link;
                par.blocks.push_back(b);
                lineHeight = qMax(lineHeight, b.font.height());
            }

            prevBlk = blk;
        }

        offset += lineHeight;
    }
}

FontFamily Page::checkSpecial(QChar &c, const Font &font) const
{
    Unicode res = 0;
    switch ( PDFImport::checkSpecial(c.unicode(), res) ) {
    case Bullet:
        kDebug(30516) << "found bullet" << endl;
        // #### FIXME : if list, use a COUNTER
        // temporarly replace by symbol
        c = res;
        return Symbol;
    case SuperScript:
        kDebug(30516) << "found superscript" << endl;
        // #### FIXME
        break;
    case LatexSpecial:
        if ( !font.isLatex() ) break;
        kDebug(30516) << "found latex special" << endl;
        return Times;
    case SpecialSymbol:
        kDebug(30516) << "found symbol=" << c.unicode() << endl;
        return Times;
        //return Symbol;
    default:
        break;
    }

    return Nb_Family;
}

void Page::checkSpecialChars(Paragraph &par) const
{
    Q3ValueList<Block> blocks;
    for (uint k=0; k<par.blocks.size(); k++) {
        const Block &b = par.blocks[k];
        QString res;
//            kDebug(30516) << "check \"" << b.text << "\"" << endl;
        for (uint l=0; l<b.text.length(); l++) {
            QChar c = b.text[l];
            FontFamily family = checkSpecial(c, b.font);
            if ( family==Nb_Family ) res += c;
            else {
                if ( !res.isEmpty() ) {
                    blocks.push_back(b);
                    blocks.back().text = res;
                    res = QString::null;
                }
                blocks.push_back(b);
                blocks.back().font.setFamily(family);
                blocks.back().text = c;
            }
        }
        if ( !res.isEmpty() ) {
            blocks.push_back(b);
            blocks.back().text = res;
        }
    }
    par.blocks = blocks;
}

void Page::coalesce(Paragraph &par) const
{
    Q3ValueList<Block> blocks;
    blocks.push_back(par.blocks[0]);
    for (uint k=1; k<par.blocks.size(); k++) {
        const Block &b = par.blocks[k];
        if ( b.link==blocks.back().link && b.font==blocks.back().font )
            blocks.back().text += b.text;
        else blocks.push_back(b);
    }
    par.blocks = blocks;
}

void Page::prepare()
{
    TIME_START("associate links");
    for (Link *link=_links.first(); link; link=_links.next()) {
        const DRect &r = link->rect();
//        kDebug(30516) << "link " << r.toString() << endl;
        for (TextLine *line = lines; line; line = line->next)
            for (TextBlock *blk = line->blocks; blk; blk = blk->next)
                for (TextString *str = blk->strings; str; str = str->next) {
                    String *fstr = static_cast<String *>(str);
                    DRect sr = fstr->rect();
//                    kDebug(30516) << "str " << sr.toString() << " "
//                                   << r.isInside(sr) << endl;
                    if ( r.isInside(sr) ) fstr->link = link;
                }
    }
    TIME_END;

    TIME_START("init paragraphs");
    for (uint i=0; i<_pars.size(); i++) {
        initParagraph(_pars[i]);

        // special case for wide and centered one liner without tab
        if ( _pars[i].align==AlignBlock && _pars[i].hasOneLine()
             && _pars[i].tabs.size()==0
             && (_pars.size()==1
                 || (i!=0 && _pars[i-1].align==AlignCenter)
                 || ((i+1)!=_pars.size() && _pars[i+1].align==AlignCenter)) )
            _pars[i].align = AlignCenter;
    }
    TIME_END;

    TIME_START("fill paragraphs");
    uint begin = 0;
    if ( hasHeader() ) {
        double offset = _rects[Header].top();
        fillParagraph(_pars[0], offset);
        begin++;
    }
    uint end = _pars.size();
    if ( hasFooter() ) {
        double offset = _rects[Footer].top();
        end--;
        fillParagraph(_pars[end], offset);
    }
    double offset = _rects[Body].top();
    for (uint i=begin; i<end; i++)
        fillParagraph(_pars[i], offset);
    TIME_END;

    TIME_START("check for special chars");
    for (uint i=0; i<_pars.size(); i++)
        checkSpecialChars(_pars[i]);
    TIME_END;

    // this is not really required...
    TIME_START("coalesce formats");
    for (uint i=0; i<_pars.size(); i++)
        coalesce(_pars[i]);
    TIME_END;

    // if no paragraph : add an empty one
    if ( _pars.size()==0 ) {
        Block b;
        Paragraph par(0, 0);
        par.blocks.push_back(b);
        _pars.push_back(par);
    }
}

void Page::dump(const Paragraph &par)
{
    Q3ValueVector<QDomElement> layouts;
    Q3ValueVector<QDomElement> formats;

    // tabulations
    for (uint k=0; k<par.tabs.size(); k++) {
        QDomElement element = par.tabs[k].createElement(_data);
        layouts.push_back(element);
    }

    // indents
    if ( !_data.options().smart || par.align!=Qt::AlignCenter ) {
        QDomElement element = _data.createElement("INDENTS");
        element.setAttribute("left", par.leftIndent);
        double delta = par.firstIndent - par.leftIndent;
        if ( !equal(delta, 0) ) element.setAttribute("first", delta);
        layouts.push_back(element);
    }

    // offset before
    if ( par.offset>0 ) {
        QDomElement element = _data.createElement("OFFSETS");
        element.setAttribute("before", par.offset);
        layouts.push_back(element);
    }

    // flow
    if (_data.options().smart) {
        QString flow;
//        kDebug(30516) << "flow=" << par.align << endl;
        switch (par.align) {
        case Qt::AlignLeft: break;
        case Qt::AlignRight: flow = "right"; break;
        case Qt::AlignCenter: flow = "center"; break;
        case AlignBlock: flow = "justify"; break;
        }
        if ( !flow.isEmpty() ) {
            QDomElement element = _data.createElement("FLOW");
            element.setAttribute("align", flow);
            layouts.push_back(element);
        }
    }

    // text and formats
    QString text;
    uint pos = 0;
    for (uint k=0; k<par.blocks.size(); k++) {
        const Block &b = par.blocks[k];
        text += (b.link ? "#" : b.text);
        uint len = (b.link ? 1 : b.text.length());
        QDomElement element = _data.createElement("FORMAT");
        QDomDocument document = _data.document();
        bool r = b.font.format(document, element, pos, len);
        if (b.link) b.link->format(document, element, pos, b.text);
        if ( r || b.link ) formats.push_back(element);
        pos += len;
    }

    _data.createParagraph(text, par.type, layouts, formats);
}

void Page::dump()
{
    prepare();

    TIME_START("dump XML");
    for (uint i=0; i<_pars.size(); i++)
        dump(_pars[i]);
    TIME_END;
}

} // namespace
