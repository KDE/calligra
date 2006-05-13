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

#include "misc.h"

#include <math.h>
#include <QFontMetrics>
#include <QFontDatabase>
#include <kglobal.h>
#include <kdebug.h>

#include "Link.h"
#include "Catalog.h"
#include "GfxState.h"
#include "GfxFont.h"


namespace PDFImport
{


QColor toColor(GfxRGB &rgb)
{
    return QColor(qRound(rgb.r*255), qRound(rgb.g*255), qRound(rgb.b*255));
}

//-----------------------------------------------------------------------------
bool DRect::operator ==(const DRect &r) const
{
    return ( equal(_top, r._top) && equal(_bottom, r._bottom)
             && equal(_left, r._left) && equal(_right, r._right) );
}

bool DRect::isInside(const DRect &r, double percent) const
{
    return ( more(r._top, _top, percent) && less(r._bottom, _bottom, percent)
             && more(r._left, _left, percent)
             && less(r._right, _right, percent) );
}

void DRect::unite(const DRect &r)
{
    if ( !r.isValid() ) return;
    if ( !isValid() ) {
        *this = r;
        return;
    }
    _left = qMin(_left, r._left);
    _right = qMax(_right, r._right);
    _top = qMin(_top, r._top);
    _bottom = qMax(_bottom, r._bottom);
}

QString DRect::toString() const
{
    if ( !isValid() ) return "invalid rect";
    return QString("left=%1 right=%2 top=%3 bottom=%4").arg(_left).arg(_right)
        .arg(_top).arg(_bottom);
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
    if ( size()==0 ) return DRect();
    DRect r(at(0).x, at(0).x, at(0).y, at(0).y);
    for (uint i=1; i<size(); i++) {
        r.setTop( qMin(r.top(), at(i).y) );
        r.setBottom( qMax(r.bottom(), at(i).y) );
        r.setLeft( qMin(r.left(), at(i).x) );
        r.setRight( qMax(r.right(), at(i).x) );
    }
    return r;
}


//-----------------------------------------------------------------------------
Q3Dict<Font::Data> *Font::_dict = 0;
const char *Font::FAMILY_DATA[Nb_Family] = {
    "Times", "Helvetica", "Courier", "Symbol"
};

void Font::init()
{
    Q_ASSERT( _dict==0 );
    _dict = new Q3Dict<Data>(100, false); // case insensitive
    _dict->setAutoDelete(true);
}

void Font::cleanup()
{
    delete _dict;
    _dict = 0;
}

Font::Font()
    : _pointSize(12), _color(Qt::black)
{
    init("times-roman");
}

Font::Font(const GfxState *state, double size)
{
    if ( size<1 ) kDebug(30516) << "very small font size=" << size << endl;
    _pointSize = qRound(size);

    GfxRGB rgb;
    state->getFillRGB(&rgb);
    _color = toColor(rgb);

    GfxFont *font = state->getFont();
    GString *gname = (font ? font->getName() : 0);
    QString name = (gname ? gname->getCString() : 0);
//    kDebug(30516) << "font: " << name << endl;
    name = name.section('+', 1, 1).lower();
    if ( name.isEmpty() ) name = "##dummy"; // dummy name
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
    { "cmr",                   Times,       Regular,    true  },
    { "cmbx",                  Times,       Bold,       true  },
    { "cmcsc",                 Times,       Regular,    true  }, // small caps
    { "cmmi",                  Times,       Italic,     true  },
    { "cmtt",                  Courier,     Regular,    true  },
    { "cmsy",                  Symbol,      Regular,    true  },
    { "msbm",                  Times,       Regular,    true  }, // math caps

    { 0,                       Nb_Family,   Regular,    false }
};

void Font::init(const QString &n)
{
    // check if font already parsed
    _data = _dict->find(n);
    if ( _data==0 ) {
//        kDebug(30516) << "font " << n << endl;
        QString name = n;
        name.replace("oblique", "italic");

        // check if known font
        _data = new Data;
        uint i = 0;
        while ( KNOWN_DATA[i].name!=0 ) {
            if ( name.find(KNOWN_DATA[i].name)!=-1 ) {
//                kDebug(30516) << "found " << KNOWN_DATA[i].name
//                               << " " << isBold(KNOWN_DATA[i].style) << endl;
                _data->family = FAMILY_DATA[KNOWN_DATA[i].family];
                _data->style = KNOWN_DATA[i].style;
                _data->latex = KNOWN_DATA[i].latex;
                break;
            }
            i++;
        }

        if ( _data->family.isEmpty() ) { // let's try harder
            // simple heuristic
            kDebug(30516) << "unknown font : " << n << endl;
            if ( name.find("times")!=-1 )
            _data->family = FAMILY_DATA[Times];
            else if ( name.find("helvetica")!=-1 )
                _data->family = FAMILY_DATA[Helvetica];
            else if ( name.find("courier")!=-1 )
                _data->family = FAMILY_DATA[Courier];
            else if ( name.find("symbol")!=-1 )
                _data->family = FAMILY_DATA[Symbol];
            else { // with Qt
                QFontDatabase fdb;
                QStringList list = fdb.families();
                list = list.grep(name, false);
                if ( !list.isEmpty() ) {
                    _data->family = list[0];
                    kDebug(30516) << "in Qt database as " << list[0] << endl;
                }
                else {
                    kDebug(30516) << "really unknown font !" << endl;
                    _data->family = name;
                }
            }

            bool italic = ( name.find("italic")!=-1 );
            bool bold = ( name.find("bold")!=-1 );
            _data->style = toStyle(bold, italic);
            _data->latex = false;
        }

        _dict->insert(name, _data);
    }

    // check if QFont already created
    if ( !_data->height.contains(_pointSize) ) {
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
//        kDebug(30516) << "link to page " << page << endl;
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

        kDebug(30516) << "link to filename \"" << _href << "\" (page "
                       << page << ")" <<endl;
        break;
    }

    case actionLaunch: {
        LinkLaunch &llaunch = static_cast<LinkLaunch &>(action);
        _href = "file://";
        if ( llaunch.getFileName() )
            _href += llaunch.getFileName()->getCString();

        kDebug(30516) << "link to launch/open \"" << _href << "\"" << endl;
        break;
    }

    case actionURI: {
        LinkURI &luri = static_cast<LinkURI &>(action);
        if ( luri.getURI() ) _href = luri.getURI()->getCString();

        kDebug(30516) << "link to URI \"" << _href << "\"" << endl;
        break;
    }

    case actionMovie:
    case actionNamed:
    case actionUnknown:
        kDebug(30516) << "unsupported link=" << action.getKind() << endl;
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

} // namespace
