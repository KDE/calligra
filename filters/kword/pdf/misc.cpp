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
#include <qregexp.h>
#include <qfontmetrics.h>
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

bool DRect::isInside(const DRect &r, double percent) const
{
    return ( more(r.top, top, percent) && less(r.bottom, bottom, percent)
             && more(r.left, left, percent) && less(r.right, right, percent) );
}

DRect DRect::getUnion(const DRect &r) const
{
    DRect rect;
    rect.left = kMin(left, r.left);
    rect.right = kMax(right, r.right);
    rect.top = kMin(top, r.top);
    rect.bottom = kMax(bottom, r.bottom);
    return rect;
}

QString DRect::toString() const
{
    return QString("left=%1 right=%2 top=%3 bottom=%4").arg(left).arg(right)
        .arg(top).arg(bottom);
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


//-----------------------------------------------------------------------------
QDict<Font::Data> *Font::_dict = 0;
const char *Font::FAMILY_DATA[Nb_Family] = {
    "Times", "Helvetica", "Courier", "Symbol"
};

void Font::init()
{
    Q_ASSERT( _dict==0 );
    _dict = new QDict<Data>(100, false); // case insensitive
    _dict->setAutoDelete(true);
}

void Font::cleanup()
{
    delete _dict;
    _dict = 0;
}

Font::Font(const QString &name, uint size, const QColor &color)
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
    // standard XPDF fonts (the order is important for finding !)
    { "times-roman",           Times,       Regular,    false },
    { "times-bolditalic",      Times,       BoldItalic, false },
    { "times-bold",            Times,       Bold,       false },
    { "times-italic",          Times,       Italic,     false },
    { "helvetica-bolditalic",  Helvetica,   BoldItalic, false },
    { "helvetica-bold",        Helvetica,   Bold,       false },
    { "helvetica-italic",      Times,       Italic,     false },
    { "helvetica",             Helvetica,   Regular,    false },
    { "courier-bolditalic",    Courier,     BoldItalic, false },
    { "courier-bold",          Courier,     Bold,       false },
    { "courier-italic",        Courier,     Italic,     false },
    { "courier",               Courier,     Regular,    false },
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

void Font::init(const QString &n)
{
    // check if font already parsed
    _data = _dict->find(n);
    if ( _data==0 ) {
        kdDebug(30516) << "font " << n << endl;
        // replace "Oblique" by "Italic"
        QString name = n.lower();
        name.replace(QRegExp("oblique"), "italic"); // QRegExp(...) for Qt 3.0

        // check if known font
        _data = new Data;
        kdDebug(30516) << "data " << _data << endl;
        uint i = 0;
        while ( KNOWN_DATA[i].name!=0 ) {
            if ( name.find(KNOWN_DATA[i].name)!=-1 ) {
                kdDebug(30516) << "found " << KNOWN_DATA[i].name
                               << " " << isBold(KNOWN_DATA[i].style) << endl;
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

        if ( name.isEmpty() ) name = "##unknown"; // dummy name
        _dict->insert(name, _data);
    }

    // check if QFont already created
    if ( !_data->height.contains(_pointSize) ) {
        kdDebug(30516) << "font " << _data
                       << "bold=" << isBold(_data->style) << endl;
        QFont font(_data->family, _pointSize,
                   (isBold(_data->style) ? QFont::Bold : QFont::Normal),
                   isItalic(_data->style));
        QFontMetrics fm(font);
        _data->height.insert(_pointSize, fm.height());
    }
}

bool Font::operator ==(const Font &font) const
{
    if ( _pointSize!=font._pointSize ) return false;
    if ( _data->family!=font._data->family ) return false;
    if ( _data->style!=font._data->style ) return false;
//    if ( _underline!=font._underline ) return false;
//    if ( _strikeOut!=font._strikeOut ) return false;
    if ( _color!=font._color ) return false;
    return true;
}

bool Font::format(QDomDocument &doc, QDomElement &f,
                  uint pos, uint len, bool all) const
{
    f.setAttribute("id", 1);
    if (!all) f.setAttribute("pos", pos);
    if (!all) f.setAttribute("len", len);

    QDomElement element;
    Font def;

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

void Font::setFamily(FontFamily f)
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
Link::Link(const DRect &rect, LinkAction &action, Catalog &catalog)
    : _rect(rect)
{
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

        _href = QString("bkm://") + pageLinkName(page);
//        kdDebug(30516) << "link to page " << page << endl;
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
        if ( luri.getURI() ) _href = luri.getURI()->getCString();

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

void Link::format(QDomDocument &doc, QDomElement &f, uint pos,
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

QString Link::pageLinkName(uint i)
{
    return QString("page") + QString::number(i);
}

}; // namespace
