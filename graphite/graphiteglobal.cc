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

// This file has to be included, or kimageeffect will fail to compile!?!
#include <qimage.h>

#include <qdom.h>

#include <kdebug.h>
#include <kglobal.h>
#include <kstaticdeleter.h>

#include <gobject.h>
#include <graphiteglobal.h>


bool operator==(const Gradient &lhs, const Gradient &rhs) {

    return lhs.ca==rhs.ca && lhs.cb==rhs.cb && lhs.type==rhs.type &&
           lhs.xfactor==rhs.xfactor && lhs.yfactor==rhs.yfactor;
}

bool operator!=(const Gradient &lhs, const Gradient &rhs) {
    return !operator==(lhs, rhs);
}


// some useful "macros" and helper functions
namespace Graphite {

double mm2inch(const double &mm) { return mm*0.039370147; }
double mm2pt(const double &mm) { return mm*2.83465058; }
double inch2mm(const double &inch) { return inch*25.399956; }
double inch2pt(const double &inch) { return inch*72.0; }
double pt2mm(const double &pt) { return pt*0.352777167; }
double pt2inch(const double &pt) { return pt*0.01388888888889; }

const int double2Int(const double &value) {

    if( static_cast<double>((value-static_cast<int>(value)))>=0.5 )
        return static_cast<int>(value)+1;
    else if( static_cast<double>((value-static_cast<int>(value)))<=-0.5 )
        return static_cast<int>(value)-1;
    else
        return static_cast<int>(value);
}

const double rad2deg(const double &rad) {
    return rad*180.0*M_1_PI;   // M_1_PI = 1/M_PI :)
}

const double deg2rad(const double &deg) {
    return deg*M_PI/180.0;
}

const double normalizeRad(const double &rad) {

    double nRad=rad;
    while(nRad>2*M_PI)
        nRad-=2*M_PI;
    while(nRad<0)
        nRad+=2*M_PI;
    return nRad;
}

const double normalizeDeg(const double &deg) {

    double nDeg=deg;
    while(nDeg>360)
        nDeg-=360;
    while(nDeg<0)
        nDeg+=360;
    return nDeg;
}

void rotatePoint(int &x, int &y, const double &angle, const QPoint &center) {

    double dx=static_cast<double>(x-center.x());
    double dy=static_cast<double>(center.y()-y); // Attention: Qt coordinate system!
    double sinalpha=std::sin(angle);
    double cosalpha=std::cos(angle);
    x=center.x()+Graphite::double2Int(dx*cosalpha-dy*sinalpha);
    y=center.y()-Graphite::double2Int(dx*sinalpha+dy*cosalpha); // here too
}

void rotatePoint(unsigned int &x, unsigned int &y, const double &angle, const QPoint &center) {

    // This awkward stuff with the tmp variables is a workaround for
    // "old" compilers (egcs-1.1.2 :)
    int _x=static_cast<int>(x);
    int _y=static_cast<int>(y);
    rotatePoint(_x, _y, angle, center);
    x=static_cast<unsigned int>(_x);
    y=static_cast<unsigned int>(_y);
}

void rotatePoint(double &x, double &y, const double &angle, const QPoint &center) {

    double dx=x-static_cast<double>(center.x());
    double dy=static_cast<double>(center.y())-y; // Attention: Qt coordinate system!
    double sinalpha=std::sin(angle);
    double cosalpha=std::cos(angle);
    x=static_cast<double>(center.x())+(dx*cosalpha-dy*sinalpha);
    y=static_cast<double>(center.y())-(dx*sinalpha+dy*cosalpha); // here too
}

void rotatePoint(QPoint &p, const double &angle, const QPoint &center) {
    rotatePoint(p.rx(), p.ry(), angle, center);
}

void scalePoint(int &x, int &y, const double &xfactor, const double &yfactor,
                const QPoint &center) {
    if(xfactor<=0 || yfactor<=0)
        return;
    x=Graphite::double2Int( static_cast<double>(center.x()) + static_cast<double>(x-center.x())*xfactor );
    y=Graphite::double2Int( static_cast<double>(center.y()) + static_cast<double>(y-center.y())*yfactor );
}

void scalePoint(unsigned int &x, unsigned int &y, const double &xfactor,
                const double &yfactor, const QPoint &center) {
    // This awkward stuff with the tmp variables is a workaround for
    // "old" compilers (egcs-1.1.2 :)
    int _x=static_cast<int>(x);
    int _y=static_cast<int>(y);
    scalePoint(_x, _y, xfactor, yfactor, center);
    x=static_cast<unsigned int>(_x);
    y=static_cast<unsigned int>(_y);
}

void scalePoint(double &x, double &y, const double &xfactor, const double &yfactor,
                const QPoint &center) {
    if(xfactor<=0 || yfactor<=0)
        return;
    x=static_cast<double>(center.x()) + static_cast<double>(x-center.x())*xfactor;
    y=static_cast<double>(center.y()) + static_cast<double>(y-center.y())*yfactor;
}

void scalePoint(QPoint &p, const double &xfactor, const double &yfactor,
                const QPoint &center) {
    scalePoint(p.rx(), p.ry(), xfactor, yfactor, center);
}

}; //namespace Graphite


Gradient &Gradient::operator=(const Gradient &rhs) {

    ca=rhs.ca;
    cb=rhs.cb;
    type=rhs.type;
    xfactor=rhs.xfactor;
    yfactor=rhs.yfactor;
    return *this;
}

GraphiteGlobal *GraphiteGlobal::m_self=0;

// As this belongs to a part we can't ignore static objects, but we have
// to clean up on unloading properly.
namespace Graphite {
static KStaticDeleter<GraphiteGlobal> gglobal;
};

GraphiteGlobal *GraphiteGlobal::self() {

    if(m_self==0L)
        m_self=Graphite::gglobal.setObject(new GraphiteGlobal());
    return m_self;
}

void GraphiteGlobal::setHandleSize(const int &handleSize) {
    m_handleSize=handleSize;
    m_handleOffset=Graphite::double2Int(static_cast<double>(handleSize)*0.5);
}

void GraphiteGlobal::setRotHandleSize(const int &rotHandleSize) {
    m_rotHandleSize=rotHandleSize;
    m_rotHandleOffset=Graphite::double2Int(static_cast<double>(rotHandleSize)*0.5);
}

void GraphiteGlobal::setUnit(const Unit &unit) {

    m_unit=unit;
    if(unit==MM)
        m_unitString=QString::fromLatin1("mm");
    else if(unit==Inch)
        m_unitString=QString::fromLatin1("inch");
    else
        m_unitString=QString::fromLatin1("pt");
}

void GraphiteGlobal::setZoom(const double &zoom) {
    m_zoom=zoom;
}

void GraphiteGlobal::setResoltuion(const int &resolution) {
    m_resolution=static_cast<double>(resolution)/25.399956;
}

QDomElement GraphiteGlobal::createElement(const QString &tagName, const QPen &pen, QDomDocument &doc) const {

    static const QString &attrPenColor=KGlobal::staticQString("color");
    static const QString &attrPenStyle=KGlobal::staticQString("style");
    static const QString &attrWidth=KGlobal::staticQString("width");
    static const QString &attrPenJoinStyle=KGlobal::staticQString("joinstyle");
    static const QString &attrPenCapStyle=KGlobal::staticQString("capstyle");

    QDomElement e=doc.createElement(tagName);
    e.setAttribute(attrPenColor, pen.color().name());
    e.setAttribute(attrPenStyle, static_cast<int>(pen.style()));
    e.setAttribute(attrWidth, pen.width());
    e.setAttribute(attrPenJoinStyle, pen.joinStyle());
    e.setAttribute(attrPenCapStyle, pen.capStyle());
    return e;
}

QPen GraphiteGlobal::toPen(const QDomElement &element) const {

    QPen pen;

    static const QString &attrPenColor=KGlobal::staticQString("color");
    static const QString &attrPenStyle=KGlobal::staticQString("style");
    static const QString &attrWidth=KGlobal::staticQString("width");
    static const QString &attrPenJoinStyle=KGlobal::staticQString("joinstyle");
    static const QString &attrPenCapStyle=KGlobal::staticQString("capstyle");

    if(element.hasAttribute(attrPenColor))
        pen.setColor(QColor(element.attribute(attrPenColor)));
    if(element.hasAttribute(attrPenStyle))
        pen.setStyle(static_cast<Qt::PenStyle>(element.attribute(attrPenStyle).toInt()));
    if(element.hasAttribute(attrWidth))
        pen.setWidth(element.attribute(attrWidth).toInt());
    if(element.hasAttribute(attrPenJoinStyle))
        pen.setJoinStyle(static_cast<Qt::PenJoinStyle>(element.attribute(attrPenJoinStyle).toInt()));
    if(element.hasAttribute(attrPenCapStyle))
        pen.setCapStyle(static_cast<Qt::PenCapStyle>(element.attribute(attrPenCapStyle).toInt()));
    return pen;
}

QDomElement GraphiteGlobal::createElement(const QString &tagName, const QRect &rect, QDomDocument &doc) const {

    static const QString &attrX=KGlobal::staticQString("x");
    static const QString &attrY=KGlobal::staticQString("y");
    static const QString &attrWidth=KGlobal::staticQString("width");
    static const QString &attrHeight=KGlobal::staticQString("height");

    QDomElement e=doc.createElement(tagName);
    e.setAttribute(attrX, rect.left());
    e.setAttribute(attrY, rect.top());
    e.setAttribute(attrWidth, rect.width());
    e.setAttribute(attrHeight, rect.height());
    return e;
}

QRect GraphiteGlobal::toRect(const QDomElement &element) const {

    QRect rect;

    static const QString &attrX=KGlobal::staticQString("x");
    static const QString &attrY=KGlobal::staticQString("y");
    static const QString &attrWidth=KGlobal::staticQString("width");
    static const QString &attrHeight=KGlobal::staticQString("height");

    if(element.hasAttribute(attrX))
        rect.setTop(element.attribute(attrX).toInt());
    if(element.hasAttribute(attrY))
        rect.setTop(element.attribute(attrY).toInt());
    if(element.hasAttribute(attrWidth))
        rect.setTop(element.attribute(attrWidth).toInt());
    if(element.hasAttribute(attrHeight))
        rect.setTop(element.attribute(attrHeight).toInt());
    return rect;
}

GraphiteGlobal::GraphiteGlobal() : m_fuzzyBorder(3), m_handleSize(4),
                                   m_rotHandleSize(4), m_thirdHandleTrigger(20),
                                   m_handleOffset(2), m_rotHandleOffset(2),
                                   m_unit(MM), m_zoom(1.0), m_resolution(2.8346457) {
    m_unitString=QString::fromLatin1("mm");
}


FxValue &FxValue::operator=(const FxValue &rhs) {

    m_value=rhs.value();
    m_pixel=rhs.pxValue();
    return *this;
}

void FxValue::setValue(const double &value) {
    m_value=value;
    recalculate();
}

void FxValue::setPxValue(const int &pixel) {
    m_value=static_cast<double>(m_pixel)/GraphiteGlobal::self()->zoom();
    m_pixel=pixel;
}

const double FxValue::valueUnit() const {

    if(GraphiteGlobal::self()->unit()==GraphiteGlobal::MM)
        return value();
    else if(GraphiteGlobal::self()->unit()==GraphiteGlobal::Inch)
        return valueInch();
    else
        return valuePt();
}

const double FxValue::valueInch() const {
    return m_value; // *
}

const double FxValue::valuePt() const {
    return m_value; // *
}

void FxValue::recalculate() {

}
