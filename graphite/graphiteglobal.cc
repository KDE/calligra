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

// Note: This file has to be included, or kimageeffect will fail to compile!?!
#include <qimage.h>

#include <qdom.h>
#include <qsize.h>

#include <kdebug.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kstaticdeleter.h>

#include <gobject.h>
#include <graphiteglobal.h>

#include <float.h>
#include <math.h>

bool operator==(const Gradient &lhs, const Gradient &rhs) {

    return lhs.ca==rhs.ca && lhs.cb==rhs.cb && lhs.type==rhs.type &&
           lhs.xfactor==rhs.xfactor && lhs.yfactor==rhs.yfactor;
}

bool operator!=(const Gradient &lhs, const Gradient &rhs) {
    return !operator==(lhs, rhs);
}

Gradient &Gradient::operator=(const Gradient &rhs) {

    ca=rhs.ca;
    cb=rhs.cb;
    type=rhs.type;
    xfactor=rhs.xfactor;
    yfactor=rhs.yfactor;
    return *this;
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
    double twoPi=2*M_PI;
    return rad-static_cast<double>(std::floor(rad/twoPi))*twoPi;
}

const double normalizeDeg(const double &deg) {
    return deg-static_cast<double>(std::floor(deg/360.0))*360.0;
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

void rotatePoint(double &x, double &y, const double &angle, const double &cx, const double &cy) {

    double dx=x-cx;
    double dy=cy-y; // Attention: Qt coordinate system!
    double sinalpha=std::sin(angle);
    double cosalpha=std::cos(angle);
    x=cx+(dx*cosalpha-dy*sinalpha);
    y=cy-(dx*sinalpha+dy*cosalpha); // here too
}

void rotatePoint(QPoint &p, const double &angle, const QPoint &center) {
    rotatePoint(p.rx(), p.ry(), angle, center);
}

void rotatePoint(FxPoint &p, const double &angle, const FxPoint &center) {

    double px=p.x();
    double py=p.y();
    rotatePoint(px, py, angle, center.x(), center.y());
    p.setX(px);
    p.setY(py);
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
                const double &cx, const double &cy) {
    if(xfactor<=0 || yfactor<=0)
        return;
    x=cx + (x-cx)*xfactor;
    y=cy + (y-cy)*yfactor;
}

void scalePoint(QPoint &p, const double &xfactor, const double &yfactor,
                const QPoint &center) {
    scalePoint(p.rx(), p.ry(), xfactor, yfactor, center);
}

void scalePoint(FxPoint &p, const double &xfactor, const double &yfactor, const FxPoint &center) {

    double px=p.x();
    double py=p.y();
    scalePoint(px, py, xfactor, yfactor, center.x(), center.y());
    p.setX(px);
    p.setY(py);
}

PageBorders &PageBorders::operator=(const PageBorders &rhs) {
    left=rhs.left;
    top=rhs.top;
    bottom=rhs.bottom;
    right=rhs.right;
    return *this;
}

bool operator==(const PageBorders &lhs, const PageBorders &rhs) {
    return lhs.top==rhs.top && lhs.left==rhs.left && lhs.right==rhs.right && lhs.bottom==rhs.bottom;
}

bool operator!=(const PageBorders &lhs, const PageBorders &rhs) {
    return !(lhs==rhs);
}

PageLayout &PageLayout::operator=(const PageLayout &rhs) {
    orientation=rhs.orientation;
    layout=rhs.layout;
    size=rhs.size;
    customWidth=rhs.customWidth;
    customHeight=rhs.customHeight;
    borders=rhs.borders;
    return *this;
}

void PageLayout::setWidth(const double &width) {
    if(orientation==KPrinter::Portrait)
        customWidth=width;
    else
        customHeight=width;
}

double PageLayout::width() const {

    if(layout==Graphite::PageLayout::Norm) {
        if(orientation==KPrinter::Portrait)
            return Graphite::pageWidth[size];
        else
            return Graphite::pageHeight[size];
    }
    else {
        if(orientation==KPrinter::Portrait)
            return customWidth;
        else
            return customHeight;
    }
}

void PageLayout::setHeight(const double &height) {
    if(orientation==KPrinter::Portrait)
        customHeight=height;
    else
        customWidth=height;
}

double PageLayout::height() const {

    if(layout==Graphite::PageLayout::Norm) {
        if(orientation==KPrinter::Portrait)
            return Graphite::pageHeight[size];
        else
            return Graphite::pageWidth[size];
    }
    else {
        if(orientation==KPrinter::Portrait)
            return customHeight;
        else
            return customWidth;
    }
}

FxRect PageLayout::fxRect() const {
    return FxRect(borders.left, borders.top,
                  width()-borders.right-borders.left, height()-borders.bottom-borders.top);
}

void PageLayout::saveDefaults() {

    KConfig *config=KGlobal::config();
    config->setGroup(QString::fromLatin1("PageLayout"));
    config->writeEntry(QString::fromLatin1("Orientation"), static_cast<int>(orientation));
    config->writeEntry(QString::fromLatin1("Layout"), static_cast<int>(layout));
    config->writeEntry(QString::fromLatin1("Size"), static_cast<int>(size));
    config->writeEntry(QString::fromLatin1("CustomWidth"), customWidth);
    config->writeEntry(QString::fromLatin1("CustomHeight"), customHeight);
    config->writeEntry(QString::fromLatin1("TopBorder"), borders.top);
    config->writeEntry(QString::fromLatin1("LeftBorder"), borders.left);
    config->writeEntry(QString::fromLatin1("RightBorder"), borders.right);
    config->writeEntry(QString::fromLatin1("BottomBorder"), borders.bottom);
    config->sync();
}

void PageLayout::loadDefaults() {

    KConfig *config=KGlobal::config();
    config->setGroup(QString::fromLatin1("PageLayout"));
    orientation=static_cast<KPrinter::Orientation>(config->readNumEntry(QString::fromLatin1("Orientation"), 0));
    if(config->readNumEntry(QString::fromLatin1("Layout"), 0)==0)
        layout=Graphite::PageLayout::Norm;
    else
        layout=Graphite::PageLayout::Custom;
    size=static_cast<KPrinter::PageSize>(config->readNumEntry(QString::fromLatin1("Size"), 0));
    customWidth=config->readDoubleNumEntry(QString::fromLatin1("CustomWidth"), 210.0);
    customHeight=config->readDoubleNumEntry(QString::fromLatin1("CustomHeight"), 297.0);
    borders.top=config->readDoubleNumEntry(QString::fromLatin1("TopBorder"), 10.0);
    borders.left=config->readDoubleNumEntry(QString::fromLatin1("LeftBorder"), 10.0);
    borders.right=config->readDoubleNumEntry(QString::fromLatin1("RightBorder"), 10.0);
    borders.bottom=config->readDoubleNumEntry(QString::fromLatin1("BottomBorder"), 10.0);
}

bool operator==(const PageLayout &lhs, const PageLayout &rhs) {

    if(lhs.orientation!=rhs.orientation)
        return false;
    if(lhs.layout!=rhs.layout)
        return false;
    if(lhs.size!=rhs.size)
        return false;
    if(lhs.borders!=rhs.borders)
        return false;
    if(lhs.width()!=rhs.width())
        return false;
    if(lhs.height()!=rhs.height())
        return false;
    return true;
}

bool operator!=(const PageLayout &lhs, const PageLayout &rhs) {
    return !(lhs==rhs);
}

QValueList<FxRect> diff(const PageBorders &oldBorders, const PageBorders &newBorders,
             const double &width, const double &height) {

    // lo = left-outside, ti = top-inside and so on
    double lo=min(oldBorders.left, newBorders.left);
    double ro=min(oldBorders.right, newBorders.right);
    double to=min(oldBorders.top, newBorders.top);
    double bo=min(oldBorders.bottom, newBorders.bottom);
    double li=max(oldBorders.left, newBorders.left);
    double ri=max(oldBorders.right, newBorders.right);
    double ti=max(oldBorders.top, newBorders.top);
    double bi=max(oldBorders.bottom, newBorders.bottom);

    QValueList<FxRect> list;
    if(lo!=li)
        list.append(FxRect(lo, to, abs(lo-li), abs(height-bo-to)));
    if(to!=ti)
        list.append(FxRect(lo, to, abs(width-ro-lo), abs(to-ti)));
    if(bo!=bi)
        list.append(FxRect(lo, height-bi, abs(width-ro-lo), abs(bo-bi)));
    if(ro!=ri)
        list.append(FxRect(width-ri, to, abs(ro-ri), abs(height-bo-to)));
    if(list.isEmpty()) {
        // should never happen, but well... we add a very small repaint area here
        // to avoid flicker :)
        list.append(FxRect(0, 0, 1, 1));
        kdWarning(37001) << "++++++++++ reg.isNull(): ++++++++++" << endl;
    }
    return list;
}

QValueList<FxRect> diff(const PageLayout &oldLayout, const PageLayout &newLayout) {

    if(oldLayout.orientation!=newLayout.orientation)
        return QValueList<FxRect>();
    else if(oldLayout.layout!=newLayout.layout)
        return QValueList<FxRect>();
    else if(oldLayout.size!=newLayout.size)
        return QValueList<FxRect>();
    else if(oldLayout.customHeight!=newLayout.customHeight)
        return QValueList<FxRect>();
    else if(oldLayout.customWidth!=newLayout.customWidth)
        return QValueList<FxRect>();
    return diff(oldLayout.borders, newLayout.borders, newLayout.width(), newLayout.height());
}

};  // namespace Graphite


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

void GraphiteGlobal::setZoom(const double &zoom) {
    m_zoom=zoom;
    m_zoomedResolution=m_zoom*m_resolution;
}

void GraphiteGlobal::setResoltuion(const int &resolution) {
    m_resolution=1.0/Graphite::inch2mm(1.0/static_cast<double>(resolution));
    m_zoomedResolution=m_zoom*m_resolution;
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

QRect GraphiteGlobal::toQRect(const QDomElement &element) const {

    QRect rect;

    static const QString &attrX=KGlobal::staticQString("x");
    static const QString &attrY=KGlobal::staticQString("y");
    static const QString &attrWidth=KGlobal::staticQString("width");
    static const QString &attrHeight=KGlobal::staticQString("height");

    if(element.hasAttribute(attrX))
        rect.setLeft(element.attribute(attrX).toInt());
    if(element.hasAttribute(attrY))
        rect.setTop(element.attribute(attrY).toInt());
    if(element.hasAttribute(attrWidth))
        rect.setWidth(element.attribute(attrWidth).toInt());
    if(element.hasAttribute(attrHeight))
        rect.setHeight(element.attribute(attrHeight).toInt());
    return rect;
}

QDomElement GraphiteGlobal::createElement(const QString &tagName, const FxRect &rect, QDomDocument &doc) const {

    static const QString &attrX=KGlobal::staticQString("x");
    static const QString &attrY=KGlobal::staticQString("y");
    static const QString &attrWidth=KGlobal::staticQString("width");
    static const QString &attrHeight=KGlobal::staticQString("height");

    QDomElement e=doc.createElement(tagName);
    e.setAttribute(attrX, QString::number(rect.left(), 'g', DBL_MANT_DIG));
    e.setAttribute(attrY, QString::number(rect.top(), 'g', DBL_MANT_DIG));
    e.setAttribute(attrWidth, QString::number(rect.width(), 'g', DBL_MANT_DIG));
    e.setAttribute(attrHeight, QString::number(rect.height(), 'g', DBL_MANT_DIG));
    return e;
}

FxRect GraphiteGlobal::toFxRect(const QDomElement &element) const {

    FxRect rect;

    static const QString &attrX=KGlobal::staticQString("x");
    static const QString &attrY=KGlobal::staticQString("y");
    static const QString &attrWidth=KGlobal::staticQString("width");
    static const QString &attrHeight=KGlobal::staticQString("height");

    if(element.hasAttribute(attrX))
        rect.setLeft(element.attribute(attrX).toDouble());
    if(element.hasAttribute(attrY))
        rect.setTop(element.attribute(attrY).toDouble());
    if(element.hasAttribute(attrWidth))
        rect.setWidth(element.attribute(attrWidth).toDouble());
    if(element.hasAttribute(attrHeight))
        rect.setHeight(element.attribute(attrHeight).toDouble());
    return rect;
}

GraphiteGlobal::GraphiteGlobal() : m_fuzzyBorder(3), m_handleSize(4),
                                   m_rotHandleSize(4), m_thirdHandleTrigger(20),
                                   m_handleOffset(2), m_rotHandleOffset(2),
                                   m_zoom(1.0) {
    m_resolution=1.0/Graphite::inch2mm(1.0/static_cast<double>(QPaintDevice::x11AppDpiY()));  // we use *only* Y, because Qt also does that :)
    m_zoomedResolution=m_zoom*m_resolution;
}


FxValue &FxValue::operator=(const FxValue &rhs) {
    setValue(rhs.value());
    return *this;
}

void FxValue::setValue(const double &value) {
    m_value=value;
    recalculate();
}

void FxValue::setPxValue(const int &pixel) {
    m_value=static_cast<double>(pixel)/GraphiteGlobal::self()->zoomedResolution();
    m_pixel=pixel;
}

void FxValue::recalculate() const {
    m_pixel=Graphite::double2Int(m_value*GraphiteGlobal::self()->zoomedResolution());
}

// compares the current pixel values!
bool operator==(const FxValue &lhs, const FxValue &rhs) { return lhs.pxValue()==rhs.pxValue(); }
bool operator!=(const FxValue &lhs, const FxValue &rhs) { return lhs.pxValue()!=rhs.pxValue(); }


// compares the px values!
bool operator==(const FxPoint &lhs, const FxPoint &rhs) { return lhs.fx()==rhs.fx() && lhs.fy()==rhs.fy(); }
bool operator!=(const FxPoint &lhs, const FxPoint &rhs) { return lhs.fx()!=rhs.fx() || lhs.fy()!=rhs.fy(); }


FxRect::FxRect() : m_tl(0.0, 0.0), m_br(-DBL_MIN, -DBL_MIN) {
}

FxRect::FxRect(const FxPoint &topleft, const QSize &size) : m_tl(topleft) {
    m_br.setPxPoint(m_tl.pxX()+size.width(),
        (m_tl.pxY()+size.height()));
}

FxRect::FxRect(const double &left, const double &top, const double &width, const double &height) :
    m_tl(left, top), m_br(left+width, top+height) {
}

FxRect::FxRect(const QRect &rect) {
    setRect(rect);
}

bool FxRect::isNull() const {
    return m_tl.x()==0.0 && m_tl.y()==0.0 && m_br.x()==-DBL_MIN && m_br.y()==-DBL_MIN;
}

bool FxRect::isEmpty() const {
    return m_tl.x()>m_br.x() || m_tl.y()>m_br.y();
}

FxRect FxRect::normalized() const {

    double x, y, w, h;
    (m_tl.x()<m_br.x()) ? (x=m_tl.x(), w=m_br.x()-m_tl.x()) : (x=m_br.x(), w=m_tl.x()-m_br.x());
    (m_tl.y()<m_br.y()) ? (y=m_tl.y(), h=m_br.y()-m_tl.y()) : (y=m_br.y(), h=m_tl.y()-m_br.y());
    return FxRect(x, y, w, h);
}

void FxRect::normalize() {

    double tmp;
    if(m_tl.x()>m_br.x()) {
        tmp=m_tl.x();
        m_tl.setX(m_br.x());
        m_br.setX(tmp);
    }
    if(m_tl.y()>m_br.y()) {
        tmp=m_tl.y();
        m_tl.setY(m_br.y());
        m_br.setY(tmp);
    }
}

FxPoint FxRect::center() const {
    return FxPoint( Graphite::abs(m_br.x()-m_tl.x())*0.5+Graphite::min(m_br.x(), m_tl.x()),
                    Graphite::abs(m_br.y()-m_tl.y())*0.5+Graphite::min(m_br.y(), m_tl.y()) );
}

void FxRect::moveTopLeft(const FxPoint &topleft) {

    m_br.setX(topleft.x()+(m_br.x()-m_tl.x()));
    m_br.setY(topleft.y()+(m_br.y()-m_tl.y()));
    m_tl=topleft;
}

void FxRect::moveBottomRight(const FxPoint &bottomright) {

    m_tl.setX(bottomright.x()-(m_br.x()-m_tl.x()));
    m_tl.setY(bottomright.y()-(m_br.y()-m_tl.y()));
    m_br=bottomright;
}

void FxRect::moveTopRight(const FxPoint &topright) {

    double w=m_br.x()-m_tl.x();
    double h=m_br.y()-m_tl.y();
    m_tl.setX(topright.x()-w);
    m_tl.setY(topright.y());
    m_br.setX(topright.x());
    m_br.setY(topright.y()+h);
}

void FxRect::moveBottomLeft(const FxPoint &bottomleft) {

    double w=m_br.x()-m_tl.x();
    double h=m_br.y()-m_tl.y();
    m_tl.setX(bottomleft.x());
    m_tl.setY(bottomleft.y()-h);
    m_br.setX(bottomleft.x()+w);
    m_br.setY(bottomleft.y());
}

void FxRect::moveCenter(const FxPoint &center) {

    double dx=(m_br.x()-m_tl.x())*0.5;
    double dy=(m_br.y()-m_tl.y())*0.5;
    m_tl.setX(center.x()-dx);
    m_tl.setY(center.y()-dy);
    m_br.setX(center.x()+dx);
    m_br.setY(center.y()+dy);
}

void FxRect::moveBy(const double &dx, const double &dy) {

    m_tl.setX(m_tl.x()+dx);
    m_tl.setY(m_tl.y()+dy);
    m_br.setX(m_br.x()+dx);
    m_br.setY(m_br.y()+dy);
}

void FxRect::setRect(const double &x, const double &y, const double &width, const double &height) {

    m_tl.setX(x);
    m_tl.setY(y);
    m_br.setX(x+width);
    m_br.setY(y+height);
}

void FxRect::setRect(const QRect &rect) {
    m_tl.setPxPoint(rect.left(), rect.top());
    m_br.setPxPoint(rect.right(), rect.bottom());
}

void FxRect::setCoords(const double &x1, const double &y1, const double &x2, const double &y2) {

    m_tl.setX(x1);
    m_tl.setY(y1);
    m_br.setX(x2);
    m_br.setY(y2);
}

QSize FxRect::size() const {
    return QSize(Graphite::abs(m_br.pxX()-m_tl.pxX()),
                 Graphite::abs(m_br.pxY()-m_tl.pxY()));
}

void FxRect::setSize(const QSize &size) {
    m_br.setPxX(m_tl.pxX()+size.width());
    m_br.setPxY(m_tl.pxY()+size.height());
}

FxRect &FxRect::operator|=(const FxRect &rhs) {

    if(rhs.isEmpty())
        return *this;
    if(m_tl.x() > rhs.left())
        m_tl.setX(rhs.left());
    if(m_tl.y() > rhs.top())
        m_tl.setY(rhs.top());
    if(m_br.x() < rhs.right())
        m_br.setX(rhs.right());
    if(m_br.y() < rhs.bottom())
        m_br.setY(rhs.bottom());
    return *this;
}

FxRect &FxRect::operator&=(const FxRect &rhs) {

    if(m_tl.x() < rhs.left())
        m_tl.setX(rhs.left());
    if(m_tl.y() < rhs.top())
        m_tl.setY(rhs.top());
    if(m_br.x() > rhs.right())
        m_br.setX(rhs.right());
    if(m_br.y() > rhs.bottom())
        m_br.setY(rhs.bottom());
    return *this;
}

bool FxRect::contains(const FxPoint &p, bool proper) const {

    if(proper)
        return (p.x() > m_tl.x() && p.x() < m_br.x() && p.y() > m_tl.y() && p.y() < m_br.y());
    else
        return (p.x() >= m_tl.x() && p.x() <= m_br.x() && p.y() >= m_tl.y() && p.y() <= m_br.y());
}

bool FxRect::contains(const QPoint &p, bool proper) const {

    if(proper)
        return (p.x() > m_tl.pxX() && p.x() < m_br.pxX() && p.y() > m_tl.pxY() && p.y() < m_br.pxY());
    else
        return (p.x() >= m_tl.pxX() && p.x() <= m_br.pxX() && p.y() >= m_tl.pxY() && p.y() <= m_br.pxY());
}

bool FxRect::contains(const double &x, const double &y, bool proper) const {

    if(proper)
        return (x > m_tl.x() && x < m_br.x() && y > m_tl.y() && y < m_br.y());
    else
        return (x >= m_tl.x() && x <= m_br.x() && y >= m_tl.y() && y <= m_br.y());
}

bool FxRect::contains(const FxRect &r, bool proper) const {

    if(proper)
        return (r.left() > m_tl.x() && r.right() < m_br.x() && r.top() > m_tl.y() && r.bottom() < m_br.y());
    else
        return (r.left() >= m_tl.x() && r.right() <= m_br.x() && r.top() >= m_tl.y() && r.bottom() <= m_br.y());
}

bool FxRect::contains(const QRect &r, bool proper) const {

    if(proper)
        return (r.left() > m_tl.pxX() && r.right() < m_br.pxX() && r.top() > m_tl.pxY() && r.bottom() < m_br.pxY());
    else
        return (r.left() >= m_tl.pxX() && r.right() <= m_br.pxX() && r.top() >= m_tl.pxY() && r.bottom() <= m_br.pxY());
}

FxRect FxRect::unite(const FxRect &r) const {
    return *this | r;
}

FxRect FxRect::unite(const QRect &r) const {
    return *this | FxRect(r);
}

FxRect FxRect::intersect(const FxRect &r) const {
    return *this & r;
}

FxRect FxRect::intersect(const QRect &r) const {
    return *this & FxRect(r);
}

bool FxRect::intersects(const FxRect &r) const {
    return ( Graphite::max(m_tl.x(), r.left()) <= Graphite::min(m_br.x(), r.right()) &&
             Graphite::max(m_tl.y(), r.top()) <= Graphite::min(m_br.y(), r.bottom()) );
}

bool FxRect::intersects(const QRect &r) const {
    return ( Graphite::max(m_tl.pxX(), r.left()) <= Graphite::min(m_br.pxX(), r.right()) &&
             Graphite::max(m_tl.pxY(), r.top()) <= Graphite::min(m_br.pxY(), r.bottom()) );
}

FxRect operator|(const FxRect &lhs, const FxRect &rhs) {

    if(lhs.isEmpty())
        return rhs;
    if(rhs.isEmpty())
        return lhs;
    FxRect tmp;
    tmp.setCoords( (lhs.left() < rhs.left() ? lhs.left() : rhs.left()),
                   (lhs.top() < rhs.top() ? lhs.top() : rhs.top()),
                   (lhs.right() > rhs.right() ? lhs.right() : rhs.right()),
                   (lhs.bottom() > rhs.bottom() ? lhs.bottom() : rhs.bottom()) );
    return tmp;
}

FxRect operator&(const FxRect &lhs, const FxRect &rhs) {

    FxRect tmp;
    tmp.setCoords( (lhs.left() > rhs.left() ? lhs.left() : rhs.left()),
                   (lhs.top() > rhs.top() ? lhs.top() : rhs.top()),
                   (lhs.right() < rhs.right() ? lhs.right() : rhs.right()),
                   (lhs.bottom() < rhs.bottom() ? lhs.bottom() : rhs.bottom()) );
    return tmp;
}

bool operator==(const FxRect &lhs, const FxRect &rhs) {
    return ( lhs.topLeft().pxX()==rhs.topLeft().pxX() &&
             lhs.topLeft().pxY()==rhs.topLeft().pxY() &&
             lhs.bottomRight().pxX()==rhs.bottomRight().pxX() &&
             lhs.bottomRight().pxY()==rhs.bottomRight().pxY() );
}

bool operator!=(const FxRect &lhs, const FxRect &rhs) {
    return ( lhs.topLeft().pxX()!=rhs.topLeft().pxX() ||
             lhs.topLeft().pxY()!=rhs.topLeft().pxY() ||
             lhs.bottomRight().pxX()!=rhs.bottomRight().pxX() ||
             lhs.bottomRight().pxY()!=rhs.bottomRight().pxY() );
}
