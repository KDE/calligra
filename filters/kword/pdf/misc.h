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

#ifndef MISC_H
#define MISC_H

#include <math.h>

#include <qcolor.h>
#include <qfont.h>
#include <qdom.h>
#include <qsize.h>
#include <qvaluevector.h>
#include <qdict.h>
#include <qintdict.h>

#include <koGlobal.h>

class GfxRGB;

namespace PDFImport
{
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
    //-------------------------------------------------------------------------
    inline double toPoint(double mm) { return mm * 72 / 25.4; }
    inline bool equal(double d1, double d2, double delta = 0.1) {
        return ( fabs(d1 - d2)<delta );
    }
    inline bool more(double d1, double d2, double delta = 0.1) {
        return ( d1>(d2+delta) );
    }
    inline bool less(double d1, double d2, double delta = 0.1) {
        return ( d1<(d2+delta) );
    }
    QColor toColor(GfxRGB &);

    //-------------------------------------------------------------------------
    class DRect {
    public:
        double width() const { return right - left; }
        double height() const { return bottom - top; }

        bool operator ==(const DRect &) const;

        double top, bottom, right, left;
    };

    struct DPoint {
        double x, y;
    };

    class DPath : public QValueVector<DPoint>
    {
    public:
        DPath() {}

        bool isSegment() const { return size()==2; }
        bool isRectangle() const;
        DRect boundingRect() const;
    };

    typedef QValueVector<DPath> DPathVector;
};

//-----------------------------------------------------------------------------
class KoFilterChain;

class FilterData
{
 public:
    FilterData(KoFilterChain *, const PDFImport::DRect &pageRect, KoPageLayout,
               uint nbPages);

    QDomElement createElement(const QString &name)
        { return _document.createElement(name); }
    QDomElement createParagraph(const QString &text,
                                const QValueVector<QDomElement> &layouts,
                                const QValueVector<QDomElement> &formats);

    KoFilterChain *chain() const { return _chain; }
    QDomDocument document() const { return _document; }
    uint imageIndex() const { return _imageIndex; }
    uint textIndex() const { return _textIndex; }
    QDomElement bookmarks() const { return _bookmarks; }
    QDomElement pictures() const { return _pictures; }

    void checkTextFrameset();
    QDomElement pictureFrameset(const PDFImport::DRect &);

    void startPage();
    void endPage();

 private:
    KoFilterChain   *_chain;
    QDomDocument     _document;
    uint             _pageIndex, _imageIndex, _textIndex;
    bool             _needNewTextFrameset;
    QDomElement      _mainElement, _framesets, _pictures, _bookmarks;
    QDomElement      _textFrameset, _mainTextFrameset, _lastMainLayout;
    PDFImport::DRect _pageRect;
    typedef QValueList<QDomElement> FramesetList;
    FramesetList     _framesetList;

    enum FramesetType { Text, Picture };
    QDomElement createFrameset(FramesetType);
    QDomElement createFrame(FramesetType, const PDFImport::DRect &,
                            bool forceMainFrameset);
};

//-----------------------------------------------------------------------------
class FilterFont
{
 public:
    FilterFont(const QString &name = "Times-Roman",
               uint size = 12, const QColor &color = Qt::black);

    bool operator ==(const FilterFont &) const;
    bool format(QDomDocument &, QDomElement &format, uint pos, uint len,
                bool all = false) const;
    const QFont &font() const { return *_data->fonts[_pointSize]; }
    const QColor &color() const { return _color; }
    bool isLatex() const { return _data->latex; }

    void setFamily(PDFImport::FontFamily);

    static void init();
    static void cleanup();

 private:
    uint   _pointSize;
    QColor _color;

    class Data {
    public:
        Data() { fonts.setAutoDelete(true); }

        QString family;
        PDFImport::FontStyle   style;
        bool    latex;
        QIntDict<QFont> fonts;
    };
    Data *_data;

    static QDict<Data> *_dict;
    static const char *FAMILY_DATA[PDFImport::Nb_Family];

 private:
    void init(const QString &name);
};

//-----------------------------------------------------------------------------
class LinkAction;
class Catalog;

class FilterLink
{
 public:
    FilterLink() {}
    FilterLink(double x1, double x2, double y1, double y2,
               LinkAction &, Catalog &);

    bool operator ==(const FilterLink &) const;

    bool isNull() const { return _href.isNull(); }
    bool inside(double xMin, double xMax, double yMin, double yMax) const;
    void format(QDomDocument &, QDomElement &format,
                uint pos, const QString &text) const;

 public:
    QString text;

 private:
    double  _xMin, _yMin, _xMax, _yMax;
    QString _href;
};

#endif
