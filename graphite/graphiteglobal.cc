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

// This file has to be included, or kimageeffect will fail to compile!?!
#include <qimage.h>
#include <kdebug.h>

#include <gobject.h>
#include <graphiteglobal.h>


const bool operator==(const Gradient &lhs, const Gradient &rhs) {

    return lhs.ca==rhs.ca && lhs.cb==rhs.cb && lhs.type==rhs.type &&
           lhs.xfactor==rhs.xfactor && lhs.yfactor==rhs.yfactor;
}

const bool operator!=(const Gradient &lhs, const Gradient &rhs) {
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

GraphiteGlobal *GraphiteGlobal::m_self=0;

GraphiteGlobal *GraphiteGlobal::self() {

    if(m_self==0L)
	m_self=new GraphiteGlobal();
    return m_self;	
}

void GraphiteGlobal::setHandleSize(const int &handleSize) {
    m_handleSize=handleSize;
    m_offset=Graphite::double2Int(static_cast<double>(handleSize)*0.5);
}

void GraphiteGlobal::setRotHandleSize(const int &rotHandleSize) {
    m_rotHandleSize=rotHandleSize;
    m_offset=Graphite::double2Int(static_cast<double>(rotHandleSize)*0.5);
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

GraphiteGlobal::GraphiteGlobal() : m_fuzzyBorder(3), m_handleSize(4),
				   m_rotHandleSize(4), m_thirdHandleTrigger(20),
				   m_offset(2), m_unit(MM) {
    m_unitString=QString::fromLatin1("mm");
}

FxValue::FxValue() : m_value(0), m_pixel(0), m_zoom(1.0), m_resolution(72) {
}

FxValue::FxValue(const int &pixel, const double &zoom, const int &resolution) :
    m_zoom(zoom), m_resolution(resolution) {
    setPxValue(pixel);
}

FxValue::FxValue(const FxValue &v) : m_value(v.value()), m_pixel(v.pxValue()),
				     m_zoom(v.zoom()), m_resolution(v.resolution()) {
}

FxValue &FxValue::operator=(const FxValue &rhs) {

    m_value=rhs.value();
    m_pixel=rhs.pxValue();
    m_zoom=rhs.zoom();
    m_resolution=rhs.resolution();
    return *this;
}

const bool FxValue::operator==(const FxValue &rhs) {

    if(m_zoom!=rhs.zoom()) {
	kdDebug(37001) << "FxValue::operator==(): The zoom factor differs!" << endl;
	return false;
    }
    if(m_resolution!=rhs.resolution()) {
	kdDebug(37001) << "FxValue::operator==(): The resoltuion differs!" << endl;
	return false;
    }
    return m_pixel==rhs.pxValue();
}

const bool FxValue::operator!=(const FxValue &rhs) {
    return !(*this==rhs);
}

void FxValue::setZoom(const double &zoom) {

    m_zoom=zoom;
    recalc();
}

void FxValue::setResoltuion(const int &resolution) {

    m_resolution=resolution;
    recalc();
}

void FxValue::setValue(const double &value) {

    m_value=value;
    recalc();
}

void FxValue::setPxValue(const int &/*pixel*/) {
}

const double FxValue::valueUnit() const {

    if(GraphiteGlobal::self()->unit()==GraphiteGlobal::MM)
	return valueMM();
    else if(GraphiteGlobal::self()->unit()==GraphiteGlobal::Inch)
	return valueInch();
    else
	return valuePt();
}

const double FxValue::valueMM() const {
    return 0.0;
}

const double FxValue::valueInch() const {
    return 0.0;
}

const double FxValue::valuePt() const {
    return 0.0;
}

void FxValue::recalc() {
}
