/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef GRAPHITE_GLOBAL_H
#define GRAPHITE_GLOBAL_H

#include <qvaluelist.h>
#include <koprinter.h>  // KPrinter enums
#include <kimageeffect.h>   // KImageEffect enum

#include <math.h>

class QPoint;
class QPen;
class QDomDocument;
class QDomElement;
class FxPoint;
class FxRect;

// This struct holds all the necessary information needed to represent
// a KDE gradient
struct Gradient {
    Gradient() : type(KImageEffect::VerticalGradient), xfactor(1), yfactor(1) {}
    QColor ca;
    QColor cb;
    KImageEffect::GradientType type;
    short xfactor;
    short yfactor;

    Gradient &operator=(const Gradient &rhs);
};

bool operator==(const Gradient &lhs, const Gradient &rhs);
bool operator!=(const Gradient &lhs, const Gradient &rhs);


// some useful "macros" and helper functions
namespace Graphite {
template<class T> inline T max(const T &a, const T &b) { return a > b ? a : b; }
template<class T> inline T min(const T &a, const T &b) { return a < b ? a : b; }
template<class T> inline T abs(const T &a) { return a < 0 ? -a : a; }

inline double mm2inch(const double &mm) { return mm*0.039370147; }
inline double mm2pt(const double &mm) { return mm*2.83465058; }
inline double inch2mm(const double &inch) { return inch*25.399956; }
inline double inch2pt(const double &inch) { return inch*72.0; }
inline double pt2mm(const double &pt) { return pt*0.352777167; }
inline double pt2inch(const double &pt) { return pt*0.01388888888889; }

// I didn't need that up to now, but as I already coded it (and choose a different approach
// afterwards)... maybe I need it later
// code to use in the method:
// Look up the correct conversion function using the LUT in
// graphiteglobal.h -- Check against 0!!!
//Graphite::CONVERT convert=Graphite::conversionTable[static_cast<int>(m_unit)][static_cast<int>(unit)];
//if(convert==0)
//    return;
// convert(123.4);
// LUT:
//typedef double (*const CONVERT) (const double &);
//const CONVERT conversionTable[3][3] = { { 0, &mm2pt, &mm2inch },
//                                  { &pt2mm, 0, &pt2inch },
//                                  { &inch2mm, &inch2pt, 0 } };

inline double rad2deg(const double &rad) {
    return rad*180.0*M_1_PI;   // M_1_PI = 1/M_PI :)
}

inline double deg2rad(const double &deg) {
    return deg*M_PI/180.0;
}

inline double normalizeRad(const double &rad) {
    static double twoPi=2*M_PI;
    return rad-static_cast<double>(std::floor(rad/twoPi))*twoPi;
}

inline double normalizeDeg(const double &deg) {
    return deg-static_cast<double>(std::floor(deg/360.0))*360.0;
}

void rotatePoint(int &x, int &y, const double &angle, const QPoint &center);
void rotatePoint(unsigned int &x, unsigned int &y, const double &angle, const QPoint &center);
void rotatePoint(double &x, double &y, const double &angle, const double &cx, const double &cy);
void rotatePoint(QPoint &p, const double &angle, const QPoint &center);
void rotatePoint(FxPoint &p, const double &angle, const FxPoint &center);

void scalePoint(int &x, int &y, const double &xfactor, const double &yfactor, const QPoint &center);
void scalePoint(unsigned int &x, unsigned int &y, const double &xfactor, const double &yfactor, const QPoint &center);
void scalePoint(double &x, double &y, const double &xfactor, const double &yfactor, const double &cx, const double &cy);
void scalePoint(QPoint &p, const double &xfactor, const double &yfactor, const QPoint &center);
void scalePoint(FxPoint &p, const double &xfactor, const double &yfactor, const FxPoint &center);

enum Unit { MM, Pt, Inch };

// paper size (portrait, mm)
// This is the Qt-enum. Use that one as index for the LUT
//    enum PageSize    { A4, B5, Letter, Legal, Executive,
//                       A0, A1, A2, A3, A5, A6, A7, A8, A9, B0, B1,
//                       B10, B2, B3, B4, B6, B7, B8, B9, C5E, Comm10E,
//                       DLE, Folio, Ledger, Tabloid, NPageSize };
const short pageWidth[]={ 210, 182, 216, 216, 191, 841, 594, 420,
                              297, 148, 105, 74, 52, 37, 1030, 728,
                              32, 515, 364, 257, 128, 91, 64, 45,
                              163, 105, 110, 210, 432, 279, -1 };

const short pageHeight[]={ 297, 257, 279, 356, 254, 1189, 841, 594,
                               420, 210, 148, 105, 74, 52, 1456, 1030,
                               45, 728, 515, 364, 182, 128, 91, 64,
                               229, 241, 220, 330, 279, 432, -1 };

// page borders in mm
struct PageBorders {
    PageBorders() : left(10.0), top(10.0), right(10.0), bottom(10.0) {}
    PageBorders &operator=(const PageBorders &rhs);
    double left;
    double top;
    double right;
    double bottom;
};

bool operator==(const PageBorders &lhs, const PageBorders &rhs);
bool operator!=(const PageBorders &lhs, const PageBorders &rhs);

struct PageLayout {
    PageLayout() : orientation(KPrinter::Portrait), layout(Norm),
        size(KPrinter::A4), customWidth(-1.0), customHeight(-1.0) {}
    PageLayout &operator=(const PageLayout &rhs);
    void setWidth(const double &width);
    double width() const;
    void setHeight(const double &height);
    double height() const;

    FxRect fxRect() const;

    void saveDefaults();
    void loadDefaults();

    KPrinter::Orientation orientation;
    enum { Norm, Custom } layout;
    KPrinter::PageSize size;
    double customWidth;
    double customHeight;
    PageBorders borders;
};

bool operator==(const PageLayout &lhs, const PageLayout &rhs);
bool operator!=(const PageLayout &lhs, const PageLayout &rhs);

// Create a "diff" of the page sizes to allow flicker free repainting in
// some simple cases
QValueList<FxRect> diff(const PageBorders &oldBorders, const PageBorders &newBorders,
             const double &width, const double &height);
QValueList<FxRect> diff(const PageLayout &oldLayout, const PageLayout &newLayout);

// Small struct to keep track of the mouse state
struct MouseState {
    MouseState() : lbPressed(false), haveToErase(false), oldMX(0), oldMY(0),
        startSelectionX(0), startSelectionY(0) {}

    bool lbPressed;
    bool haveToErase;
    int oldMX, oldMY;
    int startSelectionX;
    int startSelectionY;
};

}; //namespace Graphite


// This class is used to access some configurable values.
// We also use this class to save the rc file.
// Note: Follows the singleton pattern
class GraphiteGlobal {

public:
    static GraphiteGlobal *self();

    // size of the "fuzzy" zone for selections
    const int &fuzzyBorder() const { return m_fuzzyBorder; }
    void setFuzzyBorder(const int &fuzzyBorder) { m_fuzzyBorder=fuzzyBorder; }

    // size of the "handles"
    const int &handleSize() const { return m_handleSize; }
    void setHandleSize(const int &handleSize);
    // the size of the handle / 2
    const int &handleOffset() const { return m_handleOffset; }

    // size (diameter) of the "rot-handles"
    const int &rotHandleSize() const { return m_rotHandleSize; }
    void setRotHandleSize(const int &rotHandleSize);
    // the size of the rot-handle / 2
    const int &rotHandleOffset() const { return m_rotHandleOffset; }

    // how much space do we allow between the two "corner" handles before
    // we introduce a thrid one in the middle?
    const int &thirdHandleTrigger() const { return m_thirdHandleTrigger; }
    void setThirdHandleTrigger(const int &thirdHandleTrigger) { m_thirdHandleTrigger=thirdHandleTrigger; }

    // current zoom factor (for the "active" (read: being painted on currently) view), 1.0 == 100%
    // This global zoom factor is needed that we don't have to get zoom factors in every single
    // Fx* Object
    const double &zoom() const { return m_zoom; }
    void setZoom(const double &zoom);

    // note: returns pixel per mm (not per inch!)
    const double &resolution() const { return m_resolution; }
    // note: set the dpi resolution!
    void setResoltuion(const int &resolution);

    // To avoid uncounted zoom*resolution calculations we cache this
    // value here...
    // There's no way to set it, of course, but it will be kept up to date ;)
    const double &zoomedResolution() const { return m_zoomedResolution; }

    QDomElement createElement(const QString &tagName, const QPen &pen, QDomDocument &doc) const;
    QPen toPen(const QDomElement &element) const;

    QDomElement createElement(const QString &tagName, const QRect &rect, QDomDocument &doc) const;
    QRect toQRect(const QDomElement &element) const;

    QDomElement createElement(const QString &tagName, const FxRect &rect, QDomDocument &doc) const;
    FxRect toFxRect(const QDomElement &element) const;

    // ### maybe I'll add a init(...) method which takes a KConfig file/pointer
    // and initializes all the "global" vars.

private:
    //friend class gcc_you_freak;  // to avoid an ugly warning
    GraphiteGlobal();
    // don't try to copy or assing this object
    GraphiteGlobal(const GraphiteGlobal &rhs);
    GraphiteGlobal &operator=(const GraphiteGlobal &rhs);

    static GraphiteGlobal *m_self;
    int m_fuzzyBorder;
    int m_handleSize;
    int m_rotHandleSize;
    int m_thirdHandleTrigger;
    int m_handleOffset;
    int m_rotHandleOffset;
    double m_zoom;
    double m_resolution;
    double m_zoomedResolution;
};


// FxValue is the base for all coordinate classes. It "knows" the
// accurate (read: double) value and the pixel value for the
// current zoom factor (cache).
class FxValue {

public:
    FxValue() : m_value(0.0), m_pixel(0) {}
    explicit FxValue(const int &pixel) { setPxValue(pixel); }
    explicit FxValue(const double &value) : m_value(value) { recalculate(); }
    FxValue(const FxValue &v) : m_value(v.value()) { recalculate(); }
    ~FxValue() {}

    inline FxValue &operator=(const FxValue &rhs);

    const double &value() const { return m_value; }
    inline void setValue(const double &value);

    const int &pxValue() const { return m_pixel; }
    inline void setPxValue(const int &pixel);

    // When the zoom factor and/or the resoltuion changes
    // we have to recalculate the pixel value
    // const, because it's more convenient and in fact the *real*
    // value remains the same... not bitwise const, though ;)
    inline void recalculate() const;

private:
    double m_value;    // value in mm
    mutable int m_pixel;       // current pixel value (approximated, zoomed)
};

// compares the current pixel values!
inline bool operator==(const FxValue &lhs, const FxValue &rhs) { return lhs.pxValue()==rhs.pxValue(); }
inline bool operator!=(const FxValue &lhs, const FxValue &rhs) { return lhs.pxValue()!=rhs.pxValue(); }


class FxPoint {

public:
    FxPoint() {}
    FxPoint(const FxValue &x, const FxValue &y) : m_x(x), m_y(y) {}
    explicit FxPoint(const QPoint &p) { setPxPoint(p); }
    FxPoint(const int &x, const int &y) { setPxPoint(x, y); }
    FxPoint(const double &x, const double &y) : m_x(x), m_y(y) {}
    ~FxPoint() {}

    FxPoint &operator=(const FxPoint &rhs) { m_x=rhs.fx(); m_y=rhs.fy(); return *this; }

    const FxValue &fx() const { return m_x; }
    const double &x() const { return m_x.value(); }
    void setX(const FxValue &x) { m_x=x; }
    void setX(const double &x) { m_x.setValue(x); }

    const FxValue &fy() const { return m_y; }
    const double &y() const { return m_y.value(); }
    void setY(const FxValue &y) { m_y=y; }
    void setY(const double &y) { m_y.setValue(y); }

    const int &pxX() const { return m_x.pxValue(); }
    void setPxX(const int &x) { m_x.setPxValue(x); }

    const int &pxY() const { return m_y.pxValue(); }
    void setPxY(const int &y) { m_y.setPxValue(y); }

    void setPoint(const FxValue &x, const FxValue &y) { m_x=x; m_y=y; }
    void setPoint(const double &x, const double &y) { m_x.setValue(x); m_y.setValue(y); }

    const QPoint pxPoint() const { return QPoint(pxX(), pxY()); }
    void setPxPoint(const int &x, const int &y) { m_x.setPxValue(x); m_y.setPxValue(y); }
    void setPxPoint(const QPoint &p) { m_x.setPxValue(p.x()); m_y.setPxValue(p.y()); }

    void recalculate() const { m_x.recalculate(); m_y.recalculate(); }

private:
    FxValue m_x, m_y;
};


// compares the px values!
inline bool operator==(const FxPoint &lhs, const FxPoint &rhs) { return lhs.fx()==rhs.fx() && lhs.fy()==rhs.fy(); }
inline bool operator!=(const FxPoint &lhs, const FxPoint &rhs) { return lhs.fx()!=rhs.fx() || lhs.fy()!=rhs.fy(); }


class FxRect {

public:
    FxRect() : m_tl(0.0, 0.0), m_br(-DBL_MIN, -DBL_MIN) {}
    FxRect(const FxPoint &topleft, const FxPoint &bottomright) : m_tl(topleft), m_br(bottomright) {}
    FxRect(const FxPoint &topleft, const QSize &size);  // pxSize, current zoom/res
    FxRect(const double &left, const double &top, const double &width, const double &height) :
        m_tl(left, top), m_br(left+width, top+height) {}
    explicit FxRect(const QRect &rect) { setRect(rect); }  // pxSize, current zoom/res
    ~FxRect() {}

    inline bool isNull() const;
    inline bool isEmpty() const;

    FxRect normalized() const;
    void normalize();

    double left() const { return m_tl.x(); }
    double top() const { return m_tl.y(); }
    double right() const { return m_br.x(); }
    double bottom() const { return m_br.y(); }
    void setLeft(const double &left) { m_tl.setX(left); }
    void setTop(const double &top) { m_tl.setY(top); }
    void setRight(const double &right) { m_br.setX(right); }
    void setBottom(const double &bottom) { m_br.setY(bottom); }

    FxPoint topLeft() const { return FxPoint(m_tl); }
    FxPoint bottomRight() const { return FxPoint(m_br); }
    FxPoint topRight() const { return FxPoint(m_br.fx(), m_tl.fy()); }
    FxPoint bottomLeft() const { return FxPoint(m_tl.fx(), m_br.fy()); }
    FxPoint center() const;

    void moveTopLeft(const FxPoint &topleft);
    void moveBottomRight(const FxPoint &bottomright);
    void moveTopRight(const FxPoint &topright);
    void moveBottomLeft(const FxPoint &bottomleft);
    void moveCenter(const FxPoint &center);
    void moveBy(const double &dx, const double &dy);

    void setRect(const double &x, const double &y, const double &width, const double &height);
    void setRect(const QRect &rect);
    void setCoords(const double &x1, const double &y1, const double &x2, const double &y2);

    QSize size() const;
    void setSize(const QSize &size);

    double width() const { return m_br.x()-m_tl.x(); }
    void setWidth(const double &width) { m_br.setX(m_tl.x()+width); }
    void setWidth(const int &width) { m_br.setPxX(m_tl.pxX()+width); }
    double height() const { return m_br.y()-m_tl.y(); }
    void setHeight(const double &height) { m_br.setY(m_tl.y()+height); }
    void setHeight(const int &height) { m_br.setPxY(m_tl.pxY()+height); }

    FxRect &operator|=(const FxRect &rhs);
    FxRect &operator&=(const FxRect &rhs);
    bool contains(const FxPoint &p, bool proper=false) const;
    bool contains(const QPoint &p, bool proper=false) const;
    bool contains(const double &x, const double &y, bool proper=false) const;
    bool contains(const FxRect &r, bool proper=false) const;
    bool contains(const QRect &r, bool proper=false) const;
    FxRect unite(const FxRect &r) const;
    FxRect unite(const QRect &r) const;
    FxRect intersect(const FxRect &r) const;
    FxRect intersect(const QRect &r) const;
    bool intersects(const FxRect &r) const;
    bool intersects(const QRect &r) const;

    void recalculate() const { m_tl.recalculate(); m_br.recalculate(); }
    QRect pxRect() const { return QRect(m_tl.pxX(), m_tl.pxY(), m_br.pxX()-m_tl.pxX()+1,
                                 m_br.pxY()-m_tl.pxY()+1); }
private:
    FxPoint m_tl, m_br;
};

FxRect operator|(const FxRect &lhs, const FxRect &rhs);
FxRect operator&(const FxRect &lhs, const FxRect &rhs);
bool operator==(const FxRect &lhs, const FxRect &rhs);
bool operator!=(const FxRect &lhs, const FxRect &rhs);

// some inline methods...
inline FxValue &FxValue::operator=(const FxValue &rhs) {
    setValue(rhs.value());
    return *this;
}

inline void FxValue::setValue(const double &value) {
    m_value=value;
    recalculate();
}

inline void FxValue::setPxValue(const int &pixel) {
    m_value=static_cast<double>(pixel)/GraphiteGlobal::self()->zoomedResolution();
    m_pixel=pixel;
}

inline void FxValue::recalculate() const {
    m_pixel=qRound(m_value*GraphiteGlobal::self()->zoomedResolution());
}


inline bool FxRect::isNull() const {
    return m_tl.x()==0.0 && m_tl.y()==0.0 && m_br.x()==-DBL_MIN && m_br.y()==-DBL_MIN;
}

inline bool FxRect::isEmpty() const {
    return m_tl.x()>m_br.x() || m_tl.y()>m_br.y();
}

#endif // GRAPHITE_GLOBAL_H
