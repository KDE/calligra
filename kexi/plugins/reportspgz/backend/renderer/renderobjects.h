/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */
#ifndef __RENDEROBJECTS_H__
#define __RENDEROBJECTS_H__

#include <QString>
#include <QList>
#include <QPointF>
#include <QSizeF>
#include <QFont>
#include <QImage>
#include <QPen>
#include <QBrush>
#include <parsexmlutils.h>
#include <reportpageoptions.h>
#include <QPicture>
#include <krreportdata.h>

class ORODocument;
class OROPage;
class OROPrimitive;
class OROTextBox;
class OROLine;
class OROImage;
class OROSection;

//
// ORODocument
// This object is a single document containing one or more OROPage elements
//
class ORODocument
{
    friend class OROPage;
    friend class OROSection;
public:
    ORODocument(const QString & = QString());
    virtual ~ORODocument();

    QString title() const {
        return _title;
    };
    void setTitle(const QString &);

    int pages() const {
        return _pages.count();
    };
    OROPage* page(int);
    void addPage(OROPage*);

    int sections() const {
        return _sections.count();
    };
    OROSection* section(int);
    void addSection(OROSection*);

    void setPageOptions(const ReportPageOptions &);
    ReportPageOptions pageOptions() const {
        return _pageOptions;
    };

protected:
    QString _title;
    QList<OROPage*> _pages;
    QList<OROSection*> _sections;
    ReportPageOptions _pageOptions;
};

//
// OROPage
// This object is a single page in a document and may contain zero or more
// OROPrimitive objects all of which represent some form of mark to made on
// a page.
//
class OROPage
{
    friend class ORODocument;
    friend class OROPrimitive;

public:
    OROPage(ORODocument * = 0);
    virtual ~OROPage();

    ORODocument* document() const {
        return _document;
    };
    int page() const; // returns this pages current page number

    int primitives() const {
        return _primitives.count();
    };
    OROPrimitive* primitive(int);
    void addPrimitive(OROPrimitive*, bool = false);

protected:
    ORODocument * _document;
    QList<OROPrimitive*> _primitives;
};
//
// OROSection
// This object is a single row in a document and may contain zero or more
// OROPrimitives
//
class OROSection
{
    friend class ORODocument;
    friend class OROPrimitive;

public:
    enum Sort {
        SortX = 1,
        SortY,
        SortZ
    };

    OROSection(ORODocument * = 0);
    virtual ~OROSection();

    void setHeight(int);
    int height();

    void setBackgroundColor(const QColor&L);
    QColor backgroundColor();

    ORODocument* document() const {
        return _document;
    };
    long row() const; // returns this pages current page number

    void setType(KRSectionData::Section t) {
        _type = t;
    }
    KRSectionData::Section type() {
        return _type;
    }

    int primitives() const {
        return _primitives.count();
    };
    OROPrimitive* primitive(int);
    void addPrimitive(OROPrimitive*);

    void sortPrimatives(Sort);
protected:
    ORODocument * _document;
    QList<OROPrimitive*> _primitives;
    long _row;
    int _height;
    KRSectionData::Section _type;
    QColor _backgroundColor;

private:

    static bool xLessThan(OROPrimitive* s1, OROPrimitive* s2);
};


//
// OROPrimitive
// This object represents the basic primitive with a position and type.
// Other primitives are subclasses with a defined type and any additional
// information they require to define that primitive.
//
class OROPrimitive
{
    friend class OROPage;

public:
    virtual ~OROPrimitive();

    // Returns the type of the primitive which should be
    // set by the subclass
    int type() const {
        return _type;
    };
    OROPage * page() const {
        return _page;
    };

    QPointF position() const {
        return _position;
    };
    void setPosition(const QPointF &);
    virtual OROPrimitive* clone() = 0;
protected:
    OROPrimitive(int);

    OROPage * _page;
    int _type;
    QPointF _position;
};

//
// OROTextBox
// This is a text box primitive it defines a box region and text that will
// be rendered inside that region. It also contains information for font
// and positioning of the text.
//
class OROTextBox : public OROPrimitive
{
public:
    OROTextBox();
    virtual ~OROTextBox();

    QSizeF size() const {
        return _size;
    };
    void setSize(const QSizeF &);

    QString text() const {
        return _text;
    };
    void setText(const QString &);

    ORTextStyleData textStyle() const {
        return _textStyle;
    }
    void setTextStyle(const ORTextStyleData&);

    ORLineStyleData lineStyle() const {
        return _lineStyle;
    }
    void setLineStyle(const ORLineStyleData&);

    void setFont(const QFont &);

    int flags() const {
        return _flags;
    };
    void setFlags(int);

    static const int TextBox;

    virtual OROPrimitive* clone();

protected:
    QSizeF _size;
    QString _text;
    ORTextStyleData _textStyle;
    ORLineStyleData _lineStyle;
    Qt::Alignment _align;
    int _flags; // Qt::AlignmentFlag and Qt::TextFlag OR'd
};

//
// OROLine
// This primitive defines a line with a width/weight.
//
class OROLine : public OROPrimitive
{
public:
    OROLine();
    virtual ~OROLine();

    QPointF startPoint() const {
        return position();
    };
    void setStartPoint(const QPointF &);

    QPointF endPoint() const {
        return _endPoint;
    };
    void setEndPoint(const QPointF &);

    ORLineStyleData lineStyle() const {
        return _ls;
    };
    void setLineStyle(const ORLineStyleData&);

    static const int Line;
    virtual OROPrimitive* clone();
protected:
    QPointF _endPoint;
    ORLineStyleData _ls;
};

//
// OROImage
// This primitive defines an image
//
class OROImage: public OROPrimitive
{
public:
    OROImage();
    virtual ~OROImage();

    QImage image() const {
        return _image;
    };
    void setImage(const QImage &);

    QSizeF size() const {
        return _size;
    };
    void setSize(const QSizeF &);

    bool scaled() const {
        return _scaled;
    };
    void setScaled(bool);

    int transformationMode() const {
        return _transformFlags;
    };
    void setTransformationMode(int);

    int aspectRatioMode() const {
        return _aspectFlags;
    };
    void setAspectRatioMode(int);

    static const int Image;
    virtual OROPrimitive* clone();

protected:
    QImage _image;
    QSizeF _size;
    bool _scaled;
    int _transformFlags;
    int _aspectFlags;
};

class OROPicture: public OROPrimitive
{
public:
    OROPicture();
    virtual ~OROPicture();

    void setPicture(const QPicture& p) {
        _picture = p;
    }
    QPicture* picture() {
        return &_picture;
    };

    QSizeF size() const {
        return _size;
    };
    void setSize(const QSizeF &);

    static const int Picture;
    virtual OROPrimitive* clone();
protected:
    QPicture _picture;
    QSizeF _size;

};
//
// ORORect
// This primitive defines a drawn rectangle
//
class ORORect: public OROPrimitive
{
public:
    ORORect();
    virtual ~ORORect();

    QSizeF size() const {
        return _size;
    }
    void setSize(const QSizeF &);

    QRectF rect() const {
        return QRectF(_position, _size);
    };
    void setRect(const QRectF &);

    QPen pen() const {
        return _pen;
    };
    void setPen(const QPen &);

    QBrush brush() const {
        return _brush;
    };
    void setBrush(const QBrush &);

    static const int Rect;
    virtual OROPrimitive* clone();
protected:
    QSizeF _size;
    QPen _pen;
    QBrush _brush;
};

//
// ORORect
// This primitive defines a drawn rectangle
//
class OROEllipse: public OROPrimitive
{
public:
    OROEllipse();
    virtual ~OROEllipse();

    QSizeF size() const {
        return _size;
    }
    void setSize(const QSizeF &);

    QRectF rect() const {
        return QRectF(_position, _size);
    };
    void setRect(const QRectF &);

    QPen pen() const {
        return _pen;
    };
    void setPen(const QPen &);

    QBrush brush() const {
        return _brush;
    };
    void setBrush(const QBrush &);

    static const int Ellipse;
    virtual OROPrimitive* clone();

protected:
    QSizeF _size;
    QPen _pen;
    QBrush _brush;
};

class OROCheck : public OROPrimitive
{
public:
    OROCheck();
    virtual ~OROCheck();
    virtual OROPrimitive* clone();
    static const int Check;

    void setCheckType(const QString& t){
        if (t == "Cross" || t == "Tick" || t == "Dot") {
            _checkType = t;
        }
        else {
            _checkType = "Cross";
        }
    }

    QString checkType(){return _checkType;};

    QSizeF size() const {
        return _size;
    }
    void setSize(const QSizeF &s){
        _size = s;
    }

    void setValue(bool v){
        _value = v;
    }
    bool value(){
        return _value;
    }

    void setLineStyle(const ORLineStyleData& ls){
        _lineStyle = ls;
    }

    ORLineStyleData lineStyle(){
        return _lineStyle;
    }
    void setForegroundColor(const QColor& fg){
        _fgcolor = fg;
    }
    QColor foregroundColor(){
        return _fgcolor;
    }

    void setBackgroundColor(const QColor& bg){
        _bgcolor = bg;
    }
    QColor backgroundColor(){
        return _bgcolor;
    }

    protected:
            QSizeF _size;
            QString _checkType;
            bool _value;
            ORLineStyleData _lineStyle;
            QColor _fgcolor;
            QColor _bgcolor;


};

#endif // __RENDEROBJECTS_H__
