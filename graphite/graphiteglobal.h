/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <wtrobin@mandrakesoft.com>

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

struct Gradient {
    QColor ca;
    QColor cb;
    KImageEffect::GradientType type;
    short xfactor;
    short yfactor;

    Gradient &operator=(const Gradient &rhs);
};

const bool operator==(const Gradient &lhs, const Gradient &rhs);
const bool operator!=(const Gradient &lhs, const Gradient &rhs);

// This class is used to access some configurable values.
// We also use this class to save the rc file.
// Note: Follows the singleton pattern!
class GraphiteGlobal {

public:
    enum Unit { MM, Inch, Pt };

    static GraphiteGlobal *self();

    // size of the "fuzzy" zone for selections
    const int &fuzzyBorder() const { return m_fuzzyBorder; }
    void setFuzzyBorder(const int &fuzzyBorder) { m_fuzzyBorder=fuzzyBorder; }

    // size of the "handles"
    const int &handleSize() const { return m_handleSize; }
    void setHandleSize(const int &handleSize);

    // size (diameter) of the "rot-handles"
    const int &rotHandleSize() const { return m_rotHandleSize; }
    void setRotHandleSize(const int &rotHandleSize);

    const int &thirdHandleTrigger() const { return m_thirdHandleTrigger; }
    void setThirdHandleTrigger(const int &thirdHandleTrigger) { m_thirdHandleTrigger=thirdHandleTrigger; }

    const int offset() const { return m_offset; }

    const Unit &unit() const { return m_unit; }
    void setUnit(const Unit &unit);
    const QString &unitString() const { return m_unitString; }

    // more to come...
    // maybe I'll add a init(...) method which takes a KConfig file/pointer
    // and initializes all the "global" vars.

private:
    GraphiteGlobal();
    // don't try to copy or assing this object
    GraphiteGlobal(const GraphiteGlobal &rhs);
    GraphiteGlobal &operator=(const GraphiteGlobal &rhs);

    ~GraphiteGlobal() {}

    static GraphiteGlobal *m_self;
    int m_fuzzyBorder;
    int m_handleSize;
    int m_rotHandleSize;
    int m_thirdHandleTrigger;
    int m_offset;
    Unit m_unit;
    QString m_unitString;
};


class FxValue {

public:
    FxValue();
    explicit FxValue(const int &pixel, const double &zoom=1.0,
		     const int &resolution=72);
    FxValue(const FxValue &v);
    ~FxValue() {}

    // compares the current pixel values!
    FxValue &operator=(const FxValue &rhs);
    const bool operator==(const FxValue &rhs);
    const bool operator!=(const FxValue &rhs);

    const double zoom() const { return m_zoom; }
    void setZoom(const double &zoom);

    const int resolution() const { return m_resolution; }
    void setResoltuion(const int &resolution);

    const double &value() const { return m_value; }
    void setValue(const double &value);

    const int &pxValue() const { return m_pixel; }
    void setPxValue(const int &pixel);

    const double valueUnit() const;  // current unit
    const double valueMM() const;
    const double valueInch() const;
    const double valuePt() const;

private:
    void recalc();

    double m_value;    // value in mm
    int m_pixel;       // current pixel value (approximated)
    double m_zoom;     // 1.0 ^= 100%
    int m_resolution;  // in dpi (defaults to 72 -> screens)
};


class FxPoint {

public:
    FxPoint(const FxValue &x, const FxValue &y);
    explicit FxPoint(const QPoint &p, const double zoom=1.0, const int dpi=72);
    FxPoint(const int &x, const int &y, const double zoom=1.0,
	    const int dpi=72);
    ~FxPoint() {}

    // compares the px values!
    FxPoint &operator=(const FxPoint &rhs);
    const bool operator==(const FxPoint &rhs);
    const bool operator!=(const FxPoint &rhs);

    const double zoom() const;
    void setZoom(const double &zoom);

    const int resolution() const;
    void setResoltuion(const int &resolution);

    const FxValue &x() const { return m_x; }
    void setX(const FxValue &x);

    const FxValue &y() const { return m_y; }
    void setY(const FxValue &y);

    const int pxX() const { return m_x.pxValue(); }
    void setPxX(const int &x, double zoom=1.0, int dpi=72);

    const int pxY() const { return m_y.pxValue(); }
    void setPxY(const int &y, double zoom=1.0, int dpi=72);

    void setPoint(const FxValue &x, const FxValue &y);

    const QPoint pxPoint() { return QPoint(pxX(), pxY()); }
    void setPxPoint(const int &x, const int &y, const double zoom=1.0,
		    const int dpi=72);

private:
    FxValue m_x, m_y;
};


class FxPointArray {
};


class FxRect {
};
#endif // GRAPHITE_GLOBAL_H
