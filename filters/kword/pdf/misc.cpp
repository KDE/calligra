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

#include "misc.h"

#include <math.h>
#include <qfontinfo.h>
#include <qregexp.h>
#include <kglobal.h>
#include <kdebug.h>

#include "Link.h"
#include "Catalog.h"
#include "GfxState.h"


namespace PDFImport
{

double toPoint(double mm)
{
    return mm * 72 / 25.4;
}

bool equal(double d1, double d2, double delta)
{
    return ( fabs(d1 - d2)<delta );
}

bool more(double d1, double d2, double delta)
{
    return ( d1>(d2+delta) );
}

bool less(double d1, double d2, double delta)
{
    return ( d1<(d2+delta) );
}

QColor toColor(GfxRGB &rgb)
{
    return QColor(qRound(rgb.r*255), qRound(rgb.g*255), qRound(rgb.b*255));
}

//-----------------------------------------------------------------------------
bool DRect::operator ==(const DRect &r) const
{
    return ( equal(top, r.top) && equal(bottom, r.bottom)
             && equal(left, r.left) && equal(right, r.right) );
}

bool DPath::isRectangle() const
{
    if ( size()!=5 ) return false;
    if ( !equal(at(0).x, at(3).x) || !equal(at(0).x, at(4).x) ) return false;
    if ( !equal(at(0).y, at(1).y) || !equal(at(0).y, at(4).y) ) return false;
    if ( !equal(at(1).x, at(2).x) || !equal(at(2).y, at(3).y) ) return false;
    return true;
}

DRect DPath::boundingRect() const
{
    DRect r;
    if ( size()==0 ) return r;
    r.top = at(0).y;
    r.bottom = at(0).y;
    r.left = at(0).x;
    r.right = at(0).x;
    for (uint i=1; i<size(); i++) {
        r.top = kMin(r.top, at(i).y);
        r.bottom = kMax(r.bottom, at(i).y);
        r.left = kMin(r.left, at(i).x);
        r.right = kMax(r.right, at(i).x);
    }
    return r;
}

}; // namespace

using namespace PDFImport;

//-----------------------------------------------------------------------------
FilterData::FilterData(KoFilterChain *chain, const DRect &pageRect,
                       KoPageLayout page, uint nbPages)
    : _chain(chain), _pageIndex(1), _imageIndex(1), _textIndex(1),
      _needNewTextFrameset(false), _pageRect(pageRect)
{
    _document = QDomDocument("DOC");
    _document.appendChild(
        _document.createProcessingInstruction(
            "xml","version=\"1.0\" encoding=\"UTF-8\""));

    _mainElement = _document.createElement("DOC");
    _mainElement.setAttribute("editor", "KWord's PDF Import Filter");
    _mainElement.setAttribute("mime", "application/x-kword");
    _mainElement.setAttribute("syntaxVersion", 2);
    _document.appendChild(_mainElement);

    QDomElement element = _document.createElement("ATTRIBUTES");
    element.setAttribute("processing", 0);
    element.setAttribute("hasHeader", 0);
    element.setAttribute("hasFooter", 0);
    element.setAttribute("unit", "mm");
    _mainElement.appendChild(element);

    QDomElement paper = _document.createElement("PAPER");
    paper.setAttribute("format", page.format);
    paper.setAttribute("width", pageRect.width());
    paper.setAttribute("height", pageRect.height());
    paper.setAttribute("orientation", page.orientation);
    paper.setAttribute("columns", 1);
    paper.setAttribute("pages", nbPages);
    paper.setAttribute("columnspacing", 2);
    paper.setAttribute("hType", 0);
    paper.setAttribute("fType", 0);
    paper.setAttribute("spHeadBody", 9);
    paper.setAttribute("spFootBody", 9);
    paper.setAttribute("zoom", 100);
    _mainElement.appendChild(paper);

    element = _document.createElement("PAPERBORDERS");
    element.setAttribute("left", 0);
    element.setAttribute("top", 0);
    element.setAttribute("right", 0);
    element.setAttribute("bottom", 0);
    paper.appendChild(element);

    // framesets
    _framesets = _document.createElement("FRAMESETS");
    _mainElement.appendChild(_framesets);

    // main text frameset
    _mainTextFrameset = createFrameset(Text, _pageRect);
    _pageIndex = 0;

    // standard style
    QDomElement styles = _document.createElement("STYLES");
    _mainElement.appendChild(styles);

    QDomElement style = _document.createElement("STYLE");
    styles.appendChild(style);

    element = _document.createElement("FORMAT");
    FilterFont::defaultFont->format(_document, element, 0, 0, true);
    style.appendChild(element);

    element = _document.createElement("NAME");
    element.setAttribute("value","Standard");
    style.appendChild(element);

    element = _document.createElement("FOLLOWING");
    element.setAttribute("name","Standard");
    style.appendChild(element);

    // pictures
    _pictures = _document.createElement("PICTURES");
    _mainElement.appendChild(_pictures);

    // treat pages
    _bookmarks = _document.createElement("BOOKMARKS");
    _mainElement.appendChild(_bookmarks);
}

void FilterData::checkTextFrameset()
{
// #### only use main text frameset
//    if ( !_needNewTextFrameset ) return;
//    createFrameset(Text, _pageSize);
}

QDomElement FilterData::pictureFrameset(const DRect &r)
{
    return createFrameset(Picture, r);
}

QDomElement FilterData::createFrameset(FramesetType type, const DRect &r)
{
    bool text = (type==Text);
    uint &index = (text ? _textIndex : _imageIndex);

    QDomElement frameset = _document.createElement("FRAMESET");
    frameset.setAttribute("frameType", (text ? 1 : 2));
    QString name = (text ? QString("Text Frameset %1")
                    : QString("Picture %1")).arg(index);
    frameset.setAttribute("name", name);
    frameset.setAttribute("frameInfo", 0);
    _framesetList.append(frameset);

    QDomElement frame = _document.createElement("FRAME");
    if (text) frame.setAttribute("autoCreateNewFrame", (index==1 ? 1 : 0));
    frame.setAttribute("newFrameBehavior", (text && index==1 ? 0 : 1));
    frame.setAttribute("runaround", 0);
    frame.setAttribute("left", r.left);
    frame.setAttribute("right", r.right);
    double offset = (_pageIndex-1) * _pageRect.height();
    frame.setAttribute("top", r.top + offset);
    frame.setAttribute("bottom", r.bottom + offset);
    if ( text && index>1 ) frame.setAttribute("bkStyle", 0);
    frameset.appendChild(frame);

    if (text) _textFrameset = frameset;
    _needNewTextFrameset = !text;
    kdDebug(30516) << "new frameset " << index << (text ? " text" : " image")
                   << endl;
    index++;
    return frameset;
}

void FilterData::startPage()
{
    _pageIndex++;
    _textFrameset = _mainTextFrameset;
    _needNewTextFrameset = false;
    if ( !_lastMainLayout.isNull() ) {
        QDomElement element = _document.createElement("PAGEBREAKING");
        element.setAttribute("hardFrameBreakAfter", "true");
        _lastMainLayout.appendChild(element);
    }
    _lastMainLayout = QDomElement();
}

void FilterData::endPage()
{
    FramesetList::const_iterator it;
    for (it=_framesetList.begin(); it!=_framesetList.end(); ++it)
        _framesets.appendChild(*it);
}

QDomElement FilterData::createParagraph(const QString &text,
                                      const QValueVector<QDomElement> &layouts,
                                      const QValueVector<QDomElement> &formats)
{
    QDomElement paragraph = _document.createElement("PARAGRAPH");
    _textFrameset.appendChild(paragraph);

    QDomElement textElement = _document.createElement("TEXT");
    textElement.appendChild( _document.createTextNode(text) );
    paragraph.appendChild(textElement);

    QDomElement layout = _document.createElement("LAYOUT");
    paragraph.appendChild(layout);
    QDomElement element = _document.createElement("NAME");
    element.setAttribute("value", "Standard");
    layout.appendChild(element);
    for (uint i=0; i<layouts.size(); i++)
        layout.appendChild(layouts[i]);
    if ( _textFrameset==_mainTextFrameset )
        _lastMainLayout = layout;

    if ( formats.size() ) {
        QDomElement format = _document.createElement("FORMATS");
        paragraph.appendChild(format);
        for (uint i=0; i<formats.size(); i++)
            format.appendChild(formats[i]);
    }
    return paragraph;
}

//-----------------------------------------------------------------------------
FilterFont *FilterFont::defaultFont = 0;
const char *FilterFont::FAMILY_DATA[FilterFont::Nb_Family] = {
    "Times", "Helvetica", "Courier", "Symbol"
};

FilterFont::FilterFont(const QString &name, uint size, const QColor &color)
    : _name(name), _color(color), _latex(false)
{
    struct Data {
        const char *regexp;
        Family family;
        bool italic, bold, latex;
    };
    static const Data DATA[] = {
        // standard XPDF fonts
        { "^Times-Roman$",           Times,       false, false, false },
        { "^Times-Bold$",            Times,       false, true,  false },
        { "^Times-Oblique$",         Times,       true,  false, false },
        { "^Times-BoldOblique$",     Times,       true,  true,  false },
        { "^Helvetica$",             Helvetica,   false, false, false },
        { "^Helvetica-Bold$",        Helvetica,   false, true,  false },
        { "^Helvetica-Oblique$",     Helvetica,   true,  false, false },
        { "^Helvetica-BoldOblique$", Helvetica,   true,  true,  false },
        { "^Courier$",               Courier,     false, false, false },
        { "^Courier-Bold$",          Courier,     false, true,  false },
        { "^Courier-Oblique$",       Courier,     true,  false, false },
        { "^Courier-BoldOblique$",   Courier,     true,  true,  false },
        { "^Symbol$",                Symbol,      false, false, false },

        // some latex fonts
        { "\\w*\\+CMR\\d+$",         Times,       false, false, true  },
        { "\\w*\\+CMBX\\d+$",        Times,       false, true,  true  }, // X ?
        { "\\w*\\+CMCSC\\d+$",       Times,       false, false, true  }, // sc
        { "\\w*\\+CMTT\\d+$",        Courier,     false, false, true  },
        { "\\w*\\+CMSY\\d+$",        Symbol,      false, false, true  },

        { 0,                         Nb_Family,   false, false, false }
    };

    QString family;
    bool italic, bold;
    QRegExp re;
    uint i = 0;
    while ( DATA[i].regexp!=0 ) {
        re.setPattern(DATA[i].regexp);
        if ( re.exactMatch(name) ) {
            family = FAMILY_DATA[DATA[i].family];
            _latex = DATA[i].latex;
            italic = DATA[i].italic;
            bold = DATA[i].bold;
            break;
        }
        i++;
    }
    if ( family.isEmpty() ) { // let's try harder
        kdDebug(30516) << "unknown font name=" << name << endl;
        if ( name.contains("times", false) ) family = "Times";
        else if ( name.contains("helvetica", false) ) family = "Helvetica";
        else if ( name.contains("courier", false) ) family = "Courier";
        else if ( name.contains("symbol", false) ) family = "Symbol";
        else family = name;

        italic = ( name.contains("oblique", false) ||
                   name.contains("italic", false) );
        bold = name.contains("bold", false);
    }

//    if (_latex)
//        kdDebug(30516) << "latex font name=" << name << endl;

    _font = QFont(family, size, (bold ? QFont::Bold : QFont::Normal), italic);
}

bool FilterFont::operator ==(const FilterFont &font) const
{
    if ( _name!=font._name ) return false;
    if ( _font.family()!=font._font.family() ) return false;
    if ( _font.pointSize()!=font._font.pointSize() ) return false;
    if ( _font.italic()!=font._font.italic() ) return false;
    if ( _font.weight()!=font._font.weight() ) return false;
    if ( _font.underline()!=font._font.underline() ) return false;
    if ( _font.strikeOut()!=font._font.strikeOut() ) return false;
    if ( _color!=font._color ) return false;
    return true;
}

bool FilterFont::format(QDomDocument &doc, QDomElement &f,
                        uint pos, uint len, bool all) const
{
    f.setAttribute("id", 1);
    if (!all) f.setAttribute("pos", pos);
    if (!all) f.setAttribute("len", len);

    QDomElement element;

    if ( all || _font.family()!=defaultFont->_font.family() ) {
        element = doc.createElement("FONT");
        element.setAttribute("name", _font.family());
        f.appendChild(element);
    }
    if ( all || _font.pointSize()!=defaultFont->_font.pointSize() ) {
        element = doc.createElement("SIZE");
        element.setAttribute("value", _font.pointSize());
        f.appendChild(element);
    }
    if ( all || _font.italic()!=defaultFont->_font.italic() ) {
        element = doc.createElement("ITALIC");
        element.setAttribute("value", (_font.italic() ? 1 : 0));
        f.appendChild(element);
    }
    if ( all || _font.weight()!=defaultFont->_font.weight() ) {
        element = doc.createElement("WEIGHT");
        element.setAttribute("value", _font.weight());
        f.appendChild(element);
    }
    if ( all || _font.underline()!=defaultFont->_font.underline() ) {
        element = doc.createElement("UNDERLINE");
        element.setAttribute("value", (_font.underline() ? 1 : 0));
        f.appendChild(element);
    }
    if ( all || _font.strikeOut()!=defaultFont->_font.strikeOut() ) {
        element = doc.createElement("STRIKEOUT");
        element.setAttribute("value", (_font.strikeOut() ? 1 : 0));
        f.appendChild(element);
    }
    if (all) {
        element = doc.createElement("VERTALIGN");
        element.setAttribute("value", 0);
        f.appendChild(element);
    }


    if ( all || _color!=defaultFont->_color ) {
        element = doc.createElement("COLOR");
        element.setAttribute("red", _color.red());
        element.setAttribute("green", _color.green());
        element.setAttribute("blue", _color.blue());
        f.appendChild(element);
    }

    if (all) { // #### FIXME
        element = doc.createElement("TEXTBACKGROUNDCOLOR");
        element.setAttribute("red",  255);
        element.setAttribute("green",255);
        element.setAttribute("blue", 255);
        f.appendChild(element);
    }

    return f.hasChildNodes();
}


//-----------------------------------------------------------------------------
FilterLink::FilterLink(double x1, double x2, double y1, double y2,
                       LinkAction &action, Catalog &catalog)
{
    _xMin = kMin(x1, x2);
    _xMax = kMax(x1, x2);
    _yMin = kMin(y1, y2);
    _yMax = kMax(y1, y2);

    switch ( action.getKind() ) {
    case actionGoTo: {
        LinkGoTo &lgoto = static_cast<LinkGoTo &>(action);
        LinkDest *dest = (lgoto.getDest() ? lgoto.getDest()->copy()
                          : catalog.findDest( lgoto.getNamedDest() ));
        int page = 1;
        if (dest) {
            if ( dest->isPageRef() ) {
                Ref pageref = dest->getPageRef();
                page = catalog.findPage(pageref.num, pageref.gen);
            } else page = dest->getPageNum();
            delete dest;
        }

        _href = QString("bkm://page%1").arg(page);
        kdDebug(30516) << "link to page " << page << endl;
        break;
    }

    case actionGoToR: {
        LinkGoToR &lgotor = static_cast<LinkGoToR &>(action);
        _href = "file://";
        if ( lgotor.getFileName() )
            _href += lgotor.getFileName()->getCString();
        int page = 1;
        if ( lgotor.getDest() ) {
            LinkDest *dest = lgotor.getDest()->copy();
            if ( !dest->isPageRef() ) page = dest->getPageNum();
            delete dest;
        }

        kdDebug(30516) << "link to filename \"" << _href << "\" (page "
                       << page << ")" <<endl;
        break;
    }

    case actionLaunch: {
        LinkLaunch &llaunch = static_cast<LinkLaunch &>(action);
        _href = "file://";
        if ( llaunch.getFileName() )
            _href += llaunch.getFileName()->getCString();

        kdDebug(30516) << "link to launch/open \"" << _href << "\"" << endl;
        break;
    }

    case actionURI: {
        LinkURI &luri = static_cast<LinkURI &>(action);
        if ( luri.getURI() )
            _href = luri.getURI()->getCString();

        kdDebug(30516) << "link to URI \"" << _href << "\"" << endl;
        break;
    }

    case actionMovie:
    case actionNamed:
    case actionUnknown:
        break;
    }
}

bool FilterLink::inside(double xMin, double xMax,
                        double yMin, double yMax) const
{
    double y = (yMin + yMax)/2;
    return ( y<=_yMax ) && ( y>=_yMin ) && ( xMax<=_xMax ) && ( xMin>=_xMin );
}

void FilterLink::format(QDomDocument &doc, QDomElement &f, uint pos,
                        const QString &text) const
{
    f.setAttribute("id", 4);
    f.setAttribute("pos", pos);
    f.setAttribute("len", 1);

    QDomElement v = doc.createElement("VARIABLE");
    QDomElement element = doc.createElement("TYPE");
    element.setAttribute("type", 9);
    element.setAttribute("key", "STRING");
    element.setAttribute("text", text);
    v.appendChild(element);
    element = doc.createElement("LINK");
    element.setAttribute("linkName", text);
    element.setAttribute("hrefName", _href);
    v.appendChild(element);

    f.appendChild(v);
}
