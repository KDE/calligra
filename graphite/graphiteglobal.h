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

#include <kimageeffect.h>

class QColor;
class QPoint;
class QPen;
class QDomDocument;
class QDomElement;

// This struct holds all the necessary information needed to represent
// a KDE gradient
struct Gradient {
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

double mm2inch(const double &mm);
double mm2pt(const double &mm);
double inch2mm(const double &inch);
double inch2pt(const double &inch);
double pt2mm(const double &pt);
double pt2inch(const double &pt);

const int double2Int(const double &value);

const double rad2deg(const double &rad);
const double deg2rad(const double &deg);
const double normalizeRad(const double &rad);
const double normalizeDeg(const double &deg);

void rotatePoint(int &x, int &y, const double &angle, const QPoint &center);
void rotatePoint(unsigned int &x, unsigned int &y, const double &angle, const QPoint &center);
void rotatePoint(double &x, double &y, const double &angle, const QPoint &center);
void rotatePoint(QPoint &p, const double &angle, const QPoint &center);

void scalePoint(int &x, int &y, const double &xfactor, const double &yfactor, const QPoint &center);
void scalePoint(unsigned int &x, unsigned int &y, const double &xfactor, const double &yfactor, const QPoint &center);
void scalePoint(double &x, double &y, const double &xfactor, const double &yfactor, const QPoint &center);
void scalePoint(QPoint &p, const double &xfactor, const double &yfactor, const QPoint &center);

}; //namespace Graphite


// This class is used to access some configurable values.
// We also use this class to save the rc file.
// Note: Follows the singleton pattern
class GraphiteGlobal {

public:
    enum Unit { MM, Pt, Inch };

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

    const Unit &unit() const { return m_unit; }
    void setUnit(const Unit &unit);
    QString unitString() const { return m_unitString; }

    // current zoom factor (for the "active" view), 1.0 == 100%
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
    QRect toRect(const QDomElement &element) const;

    // maybe I'll add a init(...) method which takes a KConfig file/pointer
    // and initializes all the "global" vars.

private:
    GraphiteGlobal();
    // please don't try to copy or assing this object
    GraphiteGlobal(const GraphiteGlobal &rhs);
    GraphiteGlobal &operator=(const GraphiteGlobal &rhs);

    static GraphiteGlobal *m_self;
    int m_fuzzyBorder;
    int m_handleSize;
    int m_rotHandleSize;
    int m_thirdHandleTrigger;
    int m_handleOffset;
    int m_rotHandleOffset;
    Unit m_unit;
    QString m_unitString;
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
    FxValue(const FxValue &v) : m_value(v.value()), m_pixel(v.pxValue()) {}
    ~FxValue() {}

    FxValue &operator=(const FxValue &rhs);
    // compares the current pixel values!
    bool operator==(const FxValue &rhs) { return m_pixel==rhs.pxValue(); }
    bool operator!=(const FxValue &rhs) { return m_pixel!=rhs.pxValue(); }

    const double &value() const { return m_value; }
    void setValue(const double &value);

    const int &pxValue() const { return m_pixel; }
    void setPxValue(const int &pixel);

    // value in the current unit (convenience method)
    // Note: value() is always in MM (->missing here ;)
    const double valueUnit() const;
    const double valueInch() const;
    const double valuePt() const;

    // When the zoom factor and/or the resoltuion changes
    // we have to recalculate the pixel value
    void recalculate();

private:
    double m_value;    // value in mm
    int m_pixel;       // current pixel value (approximated, zoomed)
};


class FxPoint {

public:
    FxPoint(const FxValue &x, const FxValue &y);
    explicit FxPoint(const QPoint &p);
    FxPoint(const int &x, const int &y);
    ~FxPoint() {}

    FxPoint &operator=(const FxPoint &rhs);
    // compares the px values!
    bool operator==(const FxPoint &rhs);
    bool operator!=(const FxPoint &rhs);

    const FxValue &x() const { return m_x; }
    void setX(const FxValue &x);

    const FxValue &y() const { return m_y; }
    void setY(const FxValue &y);

    const int &pxX() const { return m_x.pxValue(); }
    void setPxX(const int &x);

    const int &pxY() const { return m_y.pxValue(); }
    void setPxY(const int &y);

    void setPoint(const FxValue &x, const FxValue &y);

    const QPoint pxPoint() { return QPoint(pxX(), pxY()); }
    void setPxPoint(const int &x, const int &y);
    void setPxPoint(const QPoint &p);

    void recalculate();

private:
    FxValue m_x, m_y;
};


class FxPointArray {
};


class FxRect {
};

#endif // GRAPHITE_GLOBAL_H
