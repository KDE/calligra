/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "tkunits.h"

#include <kdebug.h>
#include <klocale.h>

#include <qapplication.h>

const char* unitNames[] = {
  I18N_NOOP("pt"),
  I18N_NOOP("mm"),
  "\"",
  I18N_NOOP("pi"),
  I18N_NOOP("cm"),
  I18N_NOOP("dd"),
  I18N_NOOP("cc")
};

const char* unitLongNames[] = {
  I18N_NOOP("point"),
  I18N_NOOP("millimeter"),
  I18N_NOOP("inch"),
  I18N_NOOP("pica"),
  I18N_NOOP("centimeter"),
  I18N_NOOP("didot"),
  I18N_NOOP("cicero")
};
/**********************************************************************************************************/
TKUnitsLabel::TKUnitsLabel(QWidget* parent, const char* name)
: QLabel(parent,name)
{
  isLong = true;

  QFontMetrics fm(font());
  QStringList list;
  QStringList::Iterator it;

  list = unitsLongNamesList();
  maxlongw = 0;
  for (it = list.begin(); it != list.end(); ++it)
    maxlongw = QMAX(maxlongw, fm.width(*it));

  list = unitsNamesList();
  maxw = 0;
  for (it = list.begin(); it != list.end(); ++it)
    maxw = QMAX(maxw, fm.width(*it));
}

TKUnitsLabel::~TKUnitsLabel()
{
}

void TKUnitsLabel::setUnit(int u)
{
  m_unit = u;
  setText(isLong ? unitToLongString(m_unit) : unitToString(m_unit));
}

void TKUnitsLabel::useLongNames(bool b)
{
  isLong = b;
  setUnit(m_unit);
  updateGeometry();
}

QSize TKUnitsLabel::sizeHint() const
{
  constPolish();
  QFontMetrics fm(font());
  int w = 2*margin() + indent() + (isLong ? maxlongw:maxw);
  int h = 2*margin() + fm.height();

	return QSize(w, h).expandedTo(QApplication::globalStrut());
}
/**********************************************************************************************************/
TKUnitsBox::TKUnitsBox(QWidget* parent, const char* name)
: TKComboBox(parent, name)
{
  useLongNames(true);
}

TKUnitsBox::~TKUnitsBox()
{
}

void TKUnitsBox::useLongNames(bool b)
{
  // for store current item
  int k = unit();

  isLong = b;
  clear();
  insertStringList(isLong ? unitsLongNamesList() : unitsNamesList());

  setUnit(k);
  updateGeometry();
}

void TKUnitsBox::setUnit(int u)
{
  setCurrentItem(u);
}
/**********************************************************************************************************/

/*
 * Functions for converting
 * Point <-> Inch <-> Millimeter<-> Pica <-> Centimetre  <-> Didot <-> Cicero
 * 1 Inch = 72 pt = 6 pica = 25.4 mm = 67.54151050 dd =  5.628459208 cc
 */

float cvtPtToMm(float value)
{
  return 25.4 * value / 72.0;
}

float cvtPtToCm(float value)
{
  return 2.54 * value / 72.0;
}

float cvtPtToPica(float value)
{
  return value / 12.0;
}

float cvtPicaToPt(float value)
{
  return value * 12.0;
}

float cvtPtToInch(float value)
{
  return value / 72.0;
}

float cvtPtToDidot(float value)
{
  return value * 1238.0 / 1157.0; // 1157 dd = 1238 pt
}

float cvtPtToCicero(float value)
{
  return value * 1238.0 / (1157.0 * 12.0); // 1 cc = 12 dd
}

float cvtInchToPt(float value)
{
  return value * 72.0;
}

float cvtMmToPt(float value)
{
  return value / 25.4 * 72.0;
}

float cvtCmToPt(float value)
{
  return value / 2.54 * 72.0;
}

float cvtDidotToPt(float value)
{
  return value * 1157.0 / 1238.0;
}

float cvtCiceroToPt(float value)
{
  return value * (1157.0 * 12.0) / 1238.0;
}

float cvtPtToUnit(int unit, float value)
{
  switch (unit) {
  case UnitMillimeter:
    return cvtPtToMm (value);
    break;
  case UnitPica:
    return cvtPtToPica (value);
    break;
  case UnitInch:
    return cvtPtToInch (value);
    break;
  case UnitCentimeter:
    return cvtPtToCm (value);
    break;
  case UnitDidot:
    return cvtPtToDidot (value);
    break;
  case UnitCicero:
    return cvtPtToCicero (value);
    break;
  default:
    return value;
  }
}

float cvtUnitToPt(int unit, float value)
{
  switch (unit) {
  case UnitMillimeter:
    return cvtMmToPt (value);
    break;
  case UnitInch:
    return cvtInchToPt (value);
    break;
  case UnitCentimeter:
    return cvtCmToPt (value);
    break;
  case UnitDidot:
    return cvtDidotToPt (value);
    break;
  case UnitPica:
    return cvtPicaToPt (value);
    break;
  case UnitCicero:
    return cvtCiceroToPt (value);
    break;
  default:
    return value;
  }
}

QString unitToString(int unit)
{
  return i18n(unitNames[unit]);
}

QString unitToLongString(int unit)
{
  return i18n(unitLongNames[unit]);
}

void TKSize::set(float _w, float _h, int u)
{
  w = _w;
  h = _h;
  unit = u;
}

float TKSize::widthToPt()
{
  return cvtUnitToPt(unit,w);
}

float TKSize::heightToPt()
{
  return cvtUnitToPt(unit,h);
}

void TKSize::convertToPt(float zoom)
{
  w = widthToPt()*zoom;
  h = heightToPt()*zoom;
  unit = UnitPoint;
}

float TKSize::widthToUnit(int u)
{
  return cvtPtToUnit(u,widthToPt());
}

float TKSize::heightToUnit(int u)
{
  return cvtPtToUnit(u,heightToPt());
}

void TKPoint::set(float _x, float _y, int u)
{
  x = _x;
  y = _y;
  unit = u;
}

float TKPoint::xToPt()
{
  return cvtUnitToPt(unit,x);
}

float TKPoint::yToPt()
{
  return cvtUnitToPt(unit,y);
}

float TKPoint::xToUnit(int u)
{
  return cvtPtToUnit(u,xToPt());
}

float TKPoint::yToUnit(int u)
{
  return cvtPtToUnit(u,yToPt());
}

void TKPoint::convertToPt(float zoom)
{
  x = xToPt()*zoom;
  y = yToPt()*zoom;
  unit = UnitPoint;
}

void TKSize::save(QDomElement& element, const QString& name)
{
  element.setAttribute(name+"Unit",(int)unit);
  element.setAttribute(name+"Width",w);
  element.setAttribute(name+"Height",h);
}

void TKSize::load(QDomElement& element, const QString& name, TKSize def)
{
  unit = element.attribute(name+"Unit",QString().setNum(def.unit)).toInt();
  if( element.hasAttribute(name+"Width") ) {
    w = def.w;
  } else {
    w = element.attribute(name+"Width").toFloat();
  }
  if( element.hasAttribute(name+"Height") ) {
    h = def.h;
  } else {
    h = element.attribute(name+"Height").toFloat();
  }

}

void TKPoint::save(QDomElement& element, const QString& name)
{
  element.setAttribute(name+"Unit",(int)unit);
  element.setAttribute(name+"X",x);
  element.setAttribute(name+"Y",y);
}

void TKPoint::load(QDomElement& element, const QString& name, TKPoint def)
{
  unit = element.attribute(name+"Unit",QString().setNum(def.unit)).toInt();
  if( element.hasAttribute(name+"X") ) {
    x = def.x;
  } else {
    x = element.attribute(name+"X").toFloat();
  }
  if( element.hasAttribute(name+"Y") ) {
    y = def.y;
  } else {
    y = element.attribute(name+"Y").toFloat();
  }

}

QStringList unitsLongNamesList()
{
  QStringList items;
  items << unitToLongString(UnitPoint);
  items << unitToLongString(UnitMillimeter);
  items << unitToLongString(UnitInch);
  items << unitToLongString(UnitPica);
  items << unitToLongString(UnitCentimeter);
  items << unitToLongString(UnitDidot);
  items << unitToLongString(UnitCicero);

  return items;
}

QStringList unitsNamesList()
{
  QStringList items;
  items << unitToString(UnitPoint);
  items << unitToString(UnitMillimeter);
  items << unitToString(UnitInch);
  items << unitToString(UnitPica);
  items << unitToString(UnitCentimeter);
  items << unitToString(UnitDidot);
  items << unitToString(UnitCicero);

  return items;
}
#include "tkunits.moc"
