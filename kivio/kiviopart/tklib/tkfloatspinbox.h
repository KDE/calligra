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
  TKFloatRangeControl( float minValue, float maxValue, float lineStep, float pageStep, float value );

  float value() const;
  void setValue( float );
  void addPage();
  void subtractPage();
  void addLine();
  void subtractLine();

  float minValue() const;
  float maxValue() const;
  void setRange( float minValue, float maxValue );

  float lineStep() const;
  float pageStep() const;
  void setSteps( float line, float page );

  float bound( float ) const;

protected:
  void directSetValue( float val );
  float prevValue() const;

  virtual void valueChange();
  virtual void rangeChange();
  virtual void stepChange();

private:
  float minVal, maxVal;
  float line, page;
  float val, prevVal;
};

inline float TKFloatRangeControl::value() const
{ return val; }

inline float TKFloatRangeControl::prevValue() const
{ return prevVal; }

inline float TKFloatRangeControl::minValue() const
{ return minVal; }

inline float TKFloatRangeControl::maxValue() const
{ return maxVal; }

inline float TKFloatRangeControl::lineStep() const
{ return line; }

inline float TKFloatRangeControl::pageStep() const
{ return page; }

/************************************************************************/

class TKFloatSpinBox: public QFrame, public TKFloatRangeControl
{ Q_OBJECT
public:
  TKFloatSpinBox( QWidget* parent = 0, const char *name = 0 );
  TKFloatSpinBox( float minValue, float maxValue, float step = 1.0, int decimal = 2, QWidget* parent = 0, const char* name = 0 );
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
  float minValue() const;
  float maxValue() const;
  float lineStep() const;
  float value();

  void setValidator( const QValidator* );
  const QValidator* validator() const;

  QSize sizeHint() const;

public slots:
  void setDecimals( int );
  void setWrapping( bool );
  void setValue( float );
  void setMinValue( float );
  void setMaxValue( float );
  void setLineStep( float );
  void setPrefix( const QString &text );
  void setSuffix( const QString &text );
  void stepUp();
  void stepDown();
  void setEnabled( bool );

  void setMinimumStyle(bool);

signals:
  void valueChanged( float );

protected:
  QString mapValueToText( float );
  float mapTextToValue( bool* ok );
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
  TKUFloatSpinBox( float minValue, float maxValue, float step = 1.0, int decimal = 2, QWidget* parent = 0, const char* name = 0 );
  ~TKUFloatSpinBox();

  int unit() { return m_unit; }

  float value(int unit = UnitPoint);
  void setValue(float,int unit = UnitPoint);

public slots:
  void setUnit(int);

private:
  int m_unit;
};

#endif

