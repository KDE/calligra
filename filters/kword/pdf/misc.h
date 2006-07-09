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

#ifndef MISC_H
#define MISC_H

#include <math.h>

#include <qcolor.h>
#include <qdom.h>
#include <qsize.h>
#include <qvaluevector.h>
#include <qdict.h>
#include <qmap.h>

class LinkAction;
class Catalog;
class GfxRGB;
class GfxState;

namespace PDFImport
{

// FIX for Qt 3.0
// qvaluevector bug - qheapsort uses 'count' but qvaluevector has only 'size'
template <class Container>
inline void qHeapSort2( Container &c )
{
    if ( c.begin() == c.end() )
        return;

    // The second last parameter is a hack to retrieve the value type
    // Do the real sorting here
    qHeapSortHelper( c.begin(), c.end(), *(c.begin()), (uint)c.size() );
}

enum ParagraphType { Body = 0, Header, Footer, Nb_ParagraphTypes };

//-----------------------------------------------------------------------------
enum FontFamily { Times = 0, Helvetica, Courier, Symbol, Nb_Family };
enum FontStyle { Regular, Bold, Italic, BoldItalic };

inline FontStyle toStyle(bool bold, bool italic) {
    return (bold ? (italic ? BoldItalic : Bold)
            : (italic ? Italic : Regular) );
}
inline bool isItalic(FontStyle style) {
    return (style==Italic || style==BoldItalic);
}
inline bool isBold(FontStyle style) {
    return (style==Bold || style==BoldItalic);
}

//-----------------------------------------------------------------------------
inline double mmToPoint(double mm) { return mm * 72 / 25.4; }

inline bool equal(double d1, double d2, double percent = 0.01) {
    double delta = percent * (fabs(d1)+fabs(d2)) / 2;
    return ( fabs(d1 - d2)<delta );
}
inline bool more(double d1, double d2, double percent = 0.01) {
    double delta = percent * (fabs(d1)+fabs(d2)) / 2;
    return ( (d2-d1)<delta );
}
inline bool less(double d1, double d2, double percent = 0.01) {
    double delta = percent * (fabs(d1)+fabs(d2)) / 2;
    return ( (d1-d2)<delta );
}

QColor toColor(GfxRGB &);

//-----------------------------------------------------------------------------
class DRect {
public:
    DRect() : _left(0), _right(0), _top(0), _bottom(0) {}
    DRect(double left, double right, double top, double bottom)
        : _left(left), _right(right), _top(top), _bottom(bottom) {}

    bool isValid() const { return ( _right>_left && _bottom>_top ); }

    void setTop(double top) { _top = top; }
    void setBottom(double bottom) { _bottom = bottom; }
    void setRight(double right) { _right = right; }
    void setLeft(double left) { _left = left; }

    double top() const { return _top; }
    double bottom() const { return _bottom; }
    double left() const { return _left; }
    double right() const { return _right; }

    double width() const { return _right - _left; }
    double height() const { return _bottom - _top; }

    bool operator ==(const DRect &) const;
    bool isInside(const DRect &, double percent = 0.01) const;
    void unite(const DRect &);
    QString toString() const;

private:
    double _left, _right, _top, _bottom;
};

struct DPoint {
    double x, y;
};

class DPath : public QValueVector<DPoint>
{
public:
    DPath() {}

    bool isSegment() const { return size()==2; }
    bool isHorizontalSegment() const {
        return isSegment() && equal(at(0).y, at(1).y);
    }
    bool isVerticalSegment() const {
        return isSegment() && equal(at(0).x, at(1).y);
    }
    bool isRectangle() const;
    DRect boundingRect() const;
};

typedef QValueVector<DPath> DPathVector;

//-----------------------------------------------------------------------------
class Font
{
public:
    Font();
    Font(const GfxState *, double size);

    bool operator ==(const Font &) const;
    bool format(QDomDocument &, QDomElement &format, uint pos, uint len,
                bool all = false) const;
    int height() const { return _data->height[_pointSize]; }
    const QColor &color() const { return _color; }
    bool isLatex() const { return _data->latex; }

    void setFamily(FontFamily);

    static void init();
    static void cleanup();

private:
    void init(const QString &name);

private:
    uint   _pointSize;
    QColor _color;

    class Data {
    public:
        QString   family;
        FontStyle style;
        bool      latex;
        QMap<int, int> height;
    };
    Data *_data;

    static QDict<Data> *_dict;
    static const char *FAMILY_DATA[PDFImport::Nb_Family];
};

//-----------------------------------------------------------------------------
class Link
{
public:
    Link(const DRect &, LinkAction &, Catalog &);

    const DRect &rect() const { return _rect; }
    void format(QDomDocument &, QDomElement &format,
                uint pos, const QString &text) const;

    static QString pageLinkName(uint i);

private:
    DRect   _rect;
    QString _href;
};

} // namespace

#endif
