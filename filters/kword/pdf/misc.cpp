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

#include "misc.h"

#include <math.h>
#include <kglobal.h>

#include "Link.h"
#include "Catalog.h"


double toPoint(double mm)
{
    return mm * 72 / 25.4;
}

bool equal(double d1, double d2, double delta)
{
    return ( fabs(d1 - d2)<delta );
}

//-----------------------------------------------------------------------------
FilterFont *FilterFont::defaultFont = 0;

FilterFont::FilterFont(const QString &name, uint size, const QColor &color)
    : _color(color)
{
    QString family = name;
    if ( name.contains("times", false) ) family = "Times";
    else if ( name.contains("helvetica", false) ) family = "Helvetica";
    else if ( name.contains("courier", false) ) family = "Courier";
    else if ( name.contains("symbol", false) ) family = "Symbol";

    bool italic = ( name.contains("oblique", false) ||
                    name.contains("italic", false) );
    bool bold = name.contains("bold", false);
    _font = QFont(family, size, (bold ? QFont::Bold : QFont::Normal), italic);
}

bool FilterFont::operator ==(const FilterFont &font) const
{
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
        qDebug("link to page %i", page);
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

        qDebug("link to filename \"%s\" (page %i)", _href.latin1(), page);
        break;
    }

    case actionLaunch: {
        LinkLaunch &llaunch = static_cast<LinkLaunch &>(action);
        _href = "file://";
        if ( llaunch.getFileName() )
            _href += llaunch.getFileName()->getCString();

        qDebug("link to launch/open \"%s\"", _href.latin1());
        break;
    }

    case actionURI: {
        LinkURI &luri = static_cast<LinkURI &>(action);
        if ( luri.getURI() )
            _href = luri.getURI()->getCString();

        qDebug("link to URI \"%s\"", _href.latin1());
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
