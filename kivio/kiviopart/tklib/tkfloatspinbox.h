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
#ifndef TKFLOATSPINBOX_H
#define TKFLOATSPINBOX_H

#include <qframe.h>

#include <tkunits.h>

class TKToolBarButton;
class QLineEdit;
class QValidator;

class TKFloatRangeControl
{
public:
  TKFloatRangeControl();
  TKFloatRangeControl( double minValue, double maxValue, double lineStep, double pageStep, double value );

  double value() const;
  void setValue( double );
  void addPage();
  void subtractPage();
  void addLine();
  void subtractLine();

  double minValue() const;
  double maxValue() const;
  void setRange( double minValue, double maxValue );

  double lineStep() const;
  double pageStep() const;
  void setSteps( double line, double page );

  double bound( double ) const;

protected:
  void directSetValue( double val );
  double prevValue() const;

  virtual void valueChange();
  virtual void rangeChange();
  virtual void stepChange();

private:
  double minVal, maxVal;
  double line, page;
  double val, prevVal;
};

inline double TKFloatRangeControl::value() const
{ return val; }

inline double TKFloatRangeControl::prevValue() const
{ return prevVal; }

inline double TKFloatRangeControl::minValue() const
{ return minVal; }

inline double TKFloatRangeControl::maxValue() const
{ return maxVal; }

inline double TKFloatRangeControl::lineStep() const
{ return line; }

inline double TKFloatRangeControl::pageStep() const
{ return page; }

/************************************************************************/

class TKFloatSpinBox: public QFrame, public TKFloatRangeControl
{ Q_OBJECT
public:
  TKFloatSpinBox( QWidget* parent = 0, const char *name = 0 );
  TKFloatSpinBox( double minValue, double maxValue, double step = 1.0, int decimal = 2, QWidget* parent = 0, const char* name = 0 );
  ~TKFloatSpinBox();

  enum ButtonSymbols { UpDownArrows, PlusMinus };

  QString text() const;
  QString cleanText() const;

  QString prefix() const;
  QString suffix() const;

  bool wrapping() const;

  void setButtonSymbols( ButtonSymbols );
  ButtonSymbols buttonSymbols() const;

  int decimals() { return m_decimal; }
  double minValue() const;
  double maxValue() const;
  double lineStep() const;
  double value();

  void setValidator( const QValidator* );
  const QValidator* validator() const;

  QSize sizeHint() const;

public slots:
  void setDecimals( int );
  void setWrapping( bool );
  void setValue( double );
  void setMinValue( double );
  void setMaxValue( double );
  void setLineStep( double );
  void setPrefix( const QString &text );
  void setSuffix( const QString &text );
  void stepUp();
  void stepDown();
  void setEnabled( bool );

  void setMinimumStyle(bool);

signals:
  void valueChanged( double );

protected:
  QString mapValueToText( double );
  double mapTextToValue( bool* ok );
  QString currentValueText();

  void updateDisplay();
  void interpretText();

  TKToolBarButton* upButton() const;
  TKToolBarButton* downButton() const;
  QLineEdit* editor() const;

  void valueChange();
  void rangeChange();

  bool eventFilter( QObject* obj, QEvent* ev );
  void resizeEvent( QResizeEvent* ev );
  void wheelEvent( QWheelEvent * );
  void leaveEvent( QEvent* );

  void styleChange( QStyle& );

protected slots:
  void textChanged();

private:
  void initSpinBox();
  void arrangeWidgets();
  void updateButtonSymbols();

  ButtonSymbols buttonSymbol;
  int m_decimal;
  TKToolBarButton* up;
  TKToolBarButton* down;
  QLineEdit* vi;
  QValidator* validate;
  QString pfix;
  QString sfix;
  bool wrap;
  bool edited;
  bool m_minimum;
};
/***************************************************************************************/
class TKUFloatSpinBox: public TKFloatSpinBox
{ Q_OBJECT
public:
  TKUFloatSpinBox( QWidget* parent = 0, const char *name = 0 );
  TKUFloatSpinBox( double minValue, double maxValue, double step = 1.0, int decimal = 2, QWidget* parent = 0, const char* name = 0 );
  ~TKUFloatSpinBox();

  int unit() { return (int)m_unit; }

  double value(int unit = (int)UnitPoint);
  void setValue(double,int unit = (int)UnitPoint);

public slots:
  void setUnit(int);

private:
  MeasurementUnit m_unit;

};

#endif

