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
#ifndef units_h_
#define units_h_

#include "tkcombobox.h"

#include <qdom.h>
#include <qlabel.h>
#include <qstringlist.h>

enum MeasurementUnit {
  UnitPoint, UnitMillimeter, UnitInch, UnitPica, UnitCentimeter,
  UnitDidot, UnitCicero
};

/**********************************************************************************************************/
class TKUnitsLabel : public QLabel
{ Q_OBJECT
public:
  TKUnitsLabel(QWidget* parent=0, const char* name=0);
  ~TKUnitsLabel();

  void useLongNames(bool);
  bool isLongNames() { return isLong; }

  int unit() { return m_unit; }

public slots:
  void setUnit(int);

private:
  bool isLong;
  int m_unit;
};
/**********************************************************************************************************/
class TKUnitsBox : public TKComboBox
{ Q_OBJECT
public:
  TKUnitsBox(QWidget* parent=0, const char* name=0);
  ~TKUnitsBox();

  void useLongNames(bool);
  bool isLongNames() { return isLong; }

  int unit() { return currentItem(); }

public slots:
  void setUnit(int);

private:
  bool isLong;
};
/**********************************************************************************************************/

struct TKSize
{
  double w;
  double h;
  MeasurementUnit unit;

  void set( double _w, double _h, MeasurementUnit );
  void convertToPt( int zoom );
  double widthToPt();
  double heightToPt();

  double widthToUnit( MeasurementUnit );
  double heightToUnit( MeasurementUnit );

  void save(QDomElement&, const QString&);
  void load(QDomElement&, const QString&, TKSize def);
};

struct TKPoint
{
  double x;
  double y;
  MeasurementUnit unit;

  void set( double _x, double _y, MeasurementUnit _u );
  void convertToPt( int zoom );
  double xToPt();
  double yToPt();

  double xToUnit( MeasurementUnit );
  double yToUnit( MeasurementUnit );

  void save(QDomElement&, const QString&);
  void load(QDomElement&, const QString&, TKPoint def);
};

float cvtPtToMm(float value);
float cvtPtToCm(float value);
float cvtPtToInch(float value);
float cvtPtToPica(float value);
float cvtPtToDidot(float value);
float cvtPtToCicero(float value);
float cvtMmToPt(float value);
float cvtCmToPt(float value);
float cvtInchToPt(float value);
float cvtPicaToPt(float value);
float cvtDidotToPt(float value);
float cvtCiceroToPt(float value);

float cvtPtToUnit(MeasurementUnit unit, float value);
float cvtUnitToPt(MeasurementUnit unit, float value);

QString unitToString(MeasurementUnit unit);
QString unitToLongString(MeasurementUnit unit);

QStringList unitsLongNamesList();
QStringList unitsNamesList();

#endif
