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
#ifndef TKFLOATSPINBOXACTION_H
#define TKFLOATSPINBOXACTION_H

#include "tkaction.h"
#include "tkfloatspinbox.h"
#include "tkunits.h"

class  TKFloatSpinBoxAction : public TKAction
{ Q_OBJECT
public:
  TKFloatSpinBoxAction( const QString& text, const QString& icon, int accel, QObject* parent, const char* name = 0 );
  TKFloatSpinBoxAction( const QString& text, int accel, QObject* parent, const char* name = 0 );
  ~TKFloatSpinBoxAction();

  double value() { return m_value; }

  QString prefix() const { return m_prefix; }
  QString suffix() const { return m_suffix; }

  bool wrapping() const { return m_wrapping; }

  void setButtonSymbols( TKFloatSpinBox::ButtonSymbols );
  TKFloatSpinBox::ButtonSymbols buttonSymbols() const { return m_buttonSymbol; }

  int decimals() { return m_decimal; }
  double minValue() const { return m_minvalue; }
  double maxValue() const { return m_maxvalue; }
  double lineStep() const { return m_linestep; }

  int plug( QWidget* widget, int index = -1 );

public slots:
  void setDecimals( int );
  void setWrapping( bool );
  void setMinValue( double );
  void setMaxValue( double );
  void setLineStep( double );
  void setPrefix( const QString& );
  void setSuffix( const QString& );
  
  void setValue(double);

  void setMinimumStyle(bool);

signals:
  void activated(double);
  void activated(double,int unit);

protected slots:
  void slotActivated(double);

protected:
  double m_value;
  double m_minvalue;
  double m_maxvalue;
  double m_linestep;
  int m_decimal;
  TKFloatSpinBox::ButtonSymbols m_buttonSymbol;
  bool m_wrapping;
  QString m_prefix;
  QString m_suffix;
  bool m_minimum;
};

class  TKUFloatSpinBoxAction : public TKFloatSpinBoxAction
{ Q_OBJECT
public:
  TKUFloatSpinBoxAction( const QString& text, const QString& icon, int accel, QObject* parent, const char* name = 0 );
  TKUFloatSpinBoxAction( const QString& text, int accel, QObject* parent, const char* name = 0 );
  ~TKUFloatSpinBoxAction();

  int unit() { return (int)m_unit; }
  
  double value(int unit = (int)UnitPoint);
  void setValue(double,int unit = (int)UnitPoint);

public slots:
  void setUnit(int);

private:  
  MeasurementUnit m_unit;
};
#endif
