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
    _mainTextFrameset = createFrameset(Text);
    _pageIndex = 0;

    // standard style
    QDomElement styles = _document.createElement("STYLES");
    _mainElement.appendChild(styles);

    QDomElement style = _document.createElement("STYLE");
    styles.appendChild(style);

    element = _document.createElement("FORMAT");
    FilterFont font;
    font.format(_document, element, 0, 0, true);
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
    QDomElement frameset = createFrameset(Picture);
    QDomElement frame = createFrame(Picture, r, false);
    frameset.appendChild(frame);
    return frameset;
}

QDomElement FilterData::createFrameset(FramesetType type)
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

    if (text) _textFrameset = frameset;
    _needNewTextFrameset = !text;
    kdDebug(30516) << "new frameset " << index << (text ? " text" : " image")
                   << endl;
    index++;
    return frameset;
}

QDomElement FilterData::createFrame(FramesetType type, const DRect &r,
                                    bool forceMainFrameset)
{
    bool text = (type==Text);
    bool mainFrameset =
        (text ? (forceMainFrameset ? true : _textIndex==1) : false);

    QDomElement frame = _document.createElement("FRAME");
    if (text) frame.setAttribute("autoCreateNewFrame", (mainFrameset ? 1 : 0));
    frame.setAttribute("newFrameBehavior", (mainFrameset ? 0 : 1));
    frame.setAttribute("runaround", 0);
    frame.setAttribute("left", r.left);
    frame.setAttribute("right", r.right);
    double offset = (_pageIndex-1) * _pageRect.height();
    frame.setAttribute("top", r.top + offset);
    frame.setAttribute("bottom", r.bottom + offset);
    if ( text && !mainFrameset ) frame.setAttribute("bkStyle", 0);
    return frame;
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
    QDomElement frame = createFrame(Text, _pageRect, true);
    _mainTextFrameset.appendChild(frame);
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
QDict<FilterFont::Data> *FilterFont::_dict = 0;
const char *FilterFont::FAMILY_DATA[Nb_Family] = {
    "Times", "Helvetica", "Courier", "Symbol"
};

void FilterFont::init()
{
    Q_ASSERT( _dict==0 );
    _dict = new QDict<Data>(100, false); // case insensitive
    _dict->setAutoDelete(true);
}

void FilterFont::cleanup()
{
    delete _dict;
    _dict = 0;
}

FilterFont::FilterFont(const QString &name, uint size, const QColor &color)
    : _pointSize(size), _color(color)
{
    init(name);
}

struct KnownData {
    const char *name;
    FontFamily family;
    FontStyle  style;
    bool   latex;
};
static const KnownData KNOWN_DATA[] = {
    // standard XPDF fonts
    { "times-roman",           Times,       Regular,    false },
    { "times-bold",            Times,       Bold,       false },
    { "times-italic",          Times,       Italic,     false },
    { "times-bolditalic",      Times,       BoldItalic, false },
    { "helvetica",             Helvetica,   Regular,    false },
    { "helvetica-bold",        Helvetica,   Bold,       false },
    { "helvetica-italic",      Times,       Italic,     false },
    { "helvetica-bolditalic",  Helvetica,   BoldItalic, false },
    { "courier",               Courier,     Regular,    false },
    { "courier-bold",          Courier,     Bold,       false },
    { "courier-italic",        Courier,     Italic,     false },
    { "courier-bolditalic",    Courier,     BoldItalic, false },
    { "symbol",                Symbol,      Regular,    false },

    // some latex fonts
    { "+cmr",                  Times,       Regular,    true  },
    { "+cmbx",                 Times,       Bold,       true  },
    { "+cmcsc",                Times,       Regular,    true  }, // small caps
    { "+cmmi",                 Times,       Italic,     true  },
    { "+cmtt",                 Courier,     Regular,    true  },
    { "+cmsy",                 Symbol,      Regular,    true  },
    { "+msbm",                 Times,       Regular,    true  }, // math caps

    { 0,                       Nb_Family,   Regular,    false }
};

void FilterFont::init(const QString &n)
{
    // check if font already parsed
    _data = _dict->find(n);
    if ( _data==0 ) {
        // replace "Oblique" by "Italic"
        QString name = n.lower();
        name.replace("oblique", "italic");

        // check if known font
        _data = new Data;
        uint i = 0;
        while ( KNOWN_DATA[i].name!=0 ) {
            if ( name.find(KNOWN_DATA[i].name)!=-1 ) {
                _data->family = FAMILY_DATA[KNOWN_DATA[i].family];
                _data->style = KNOWN_DATA[i].style;
                _data->latex = KNOWN_DATA[i].latex;
                break;
            }
            i++;
        }

        if ( _data->family.isEmpty() ) { // let's try harder
            kdDebug(30516) << "unknown font : " << n << endl;
            if ( name.find("times")!=-1 )
            _data->family = FAMILY_DATA[Times];
            else if ( name.find("helvetica")!=-1 )
                _data->family = FAMILY_DATA[Helvetica];
            else if ( name.find("courier")!=-1 )
                _data->family = FAMILY_DATA[Courier];
            else if ( name.find("symbol")!=-1 )
                _data->family = FAMILY_DATA[Symbol];
            else {
                kdDebug(30516) << "really unknown font !" << endl;
                _data->family = name;
            }

            bool italic = ( name.find("italic")!=-1 );
            bool bold = ( name.find("bold")!=-1 );
            _data->style = toStyle(bold, italic);
            _data->latex = false;
        }

        _dict->insert(name, _data);
    }

    // check if QFont already created
    if ( _data->fonts[_pointSize]==0 ) {
        QFont *font = new QFont(_data->family, _pointSize,
                          (isBold(_data->style) ? QFont::Bold : QFont::Normal),
                          isItalic(_data->style));
        _data->fonts.insert(_pointSize, font);
    }
}

bool FilterFont::operator ==(const FilterFont &font) const
{
    if ( _pointSize!=font._pointSize ) return false;
    if ( _data->family!=font._data->family ) return false;
    if ( _data->style!=font._data->style ) return false;
//    if ( _underline!=font._underline ) return false;
//    if ( _strikeOut!=font._strikeOut ) return false;
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
    FilterFont def;

    if ( all || _data->family!=def._data->family ) {
        element = doc.createElement("FONT");
        element.setAttribute("name", _data->family);
        f.appendChild(element);
    }
    if ( all || _pointSize!=def._pointSize ) {
        element = doc.createElement("SIZE");
        element.setAttribute("value", _pointSize);
        f.appendChild(element);
    }
    if ( all || isItalic(_data->style)!=isItalic(def._data->style) ) {
        element = doc.createElement("ITALIC");
        element.setAttribute("value", (isItalic(_data->style) ? 1 : 0));
        f.appendChild(element);
    }
    if ( all || isBold(_data->style)!=isBold(def._data->style) ) {
        element = doc.createElement("WEIGHT");
        element.setAttribute("value",
                         (isBold(_data->style) ? QFont::Bold : QFont::Normal));
        f.appendChild(element);
    }
//    if ( all || _underline!=def._underline ) {
//        element = doc.createElement("UNDERLINE");
//        element.setAttribute("value", (_underline ? 1 : 0));
//        f.appendChild(element);
//    }
//    if ( all || _strikeOut!=def._strikeOut ) {
//        element = doc.createElement("STRIKEOUT");
//        element.setAttribute("value", (_strikeOut ? 1 : 0));
//        f.appendChild(element);
//    }

    if (all) {
        element = doc.createElement("VERTALIGN");
        element.setAttribute("value", 0);
        f.appendChild(element);
    }

    if ( all || _color!=def._color ) {
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

void FilterFont::setFamily(FontFamily f)
{
    int k = -1;
    uint i=0;
    while ( KNOWN_DATA[i].name!=0 ) {
        if ( KNOWN_DATA[i].family==f ) {
            if ( KNOWN_DATA[i].style==_data->style ) {
                k = i;
                break;
            }
            if ( k==-1 ) k = i;
        }
        i++;
    }
    if ( k==-1 ) k = 0;

    init(KNOWN_DATA[k].name);
}


//-----------------------------------------------------------------------------
FilterLink::FilterLink(double x1, double x2, double y1, double y2,
                       LinkAction &action, Catalog &catalog)
    : _href("") // null string means not a link
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
        kdDebug(30516) << "unsupported link=" << action.getKind() << endl;
        break;
    }
}

bool FilterLink::operator ==(const FilterLink &link) const
{
    if ( _href.isNull() && link._href.isNull() ) return true;
    return ( _href==link._href && _xMin==link._xMin && _xMax==link._xMax
             && _yMin==link._yMin && _yMax==link._yMax );
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
