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
#include "tkfloatspinboxaction.h"

#include <ktoolbar.h>

TKFloatSpinBoxAction::TKFloatSpinBoxAction( const QString& text, const QString& icon, int accel, QObject* parent, const char* name)
: TKAction(parent,name)
{
  setText(text);
  setIcon(icon);
  setAccel(accel);

  m_decimal = 3;
  m_value = 0.0;
  m_minvalue = 0.0;
  m_maxvalue = 100.0;
  m_linestep = 0.1;
  m_buttonSymbol = TKFloatSpinBox::UpDownArrows;
  m_wrapping = false;
  m_prefix = QString::null;
  m_suffix = QString::null;
  m_minimum = false;
}

TKFloatSpinBoxAction::TKFloatSpinBoxAction( const QString& text, int accel, QObject* parent, const char* name)
: TKAction(parent,name)
{
  setText(text);
  setAccel(accel);

  m_decimal = 3;
  m_value = 0.0;
  m_minvalue = 0.0;
  m_maxvalue = 100.0;
  m_linestep = 0.1;
  m_buttonSymbol = TKFloatSpinBox::UpDownArrows;
  m_wrapping = false;
  m_prefix = QString::null;
  m_suffix = QString::null;
  m_minimum = false;
}

TKFloatSpinBoxAction::~TKFloatSpinBoxAction()
{
}

int TKFloatSpinBoxAction::plug( QWidget* widget, int index )
{
  int id_ = KAction::getToolButtonID();

  TKFloatSpinBox* fsb = new TKFloatSpinBox();
  connect(fsb,SIGNAL(valueChanged(float)),SLOT(slotActivated(float)));

  QWidget* base = createLayout(widget,fsb);

  if ( widget->inherits("KToolBar") ) {
    KToolBar* bar = static_cast<KToolBar*>( widget );
    bar->insertWidget( id_, 0, base, index );
  }

  addContainer( widget, id_ );
  connect( widget, SIGNAL(destroyed()), SLOT(slotDestroyed()) );

  fsb->setButtonSymbols(m_buttonSymbol);
  fsb->setDecimals(m_decimal);
  fsb->setWrapping(m_wrapping);
  fsb->setMinValue(m_minvalue);
  fsb->setMaxValue(m_maxvalue);
  fsb->setLineStep(m_linestep);
  fsb->setPrefix(m_prefix);
  fsb->setSuffix(m_suffix);
  fsb->setValue(m_value);
  fsb->setMinimumStyle(m_minimum);

  return containerCount() - 1;
}

void TKFloatSpinBoxAction::slotActivated(float value)
{
  if ( m_value == value )
    return;

  m_value = value;
  setValue(value);

  emit activated(value);
  emit TKAction::activated();
}

#define SETVALUE(variable,method) \
  variable = value;                                                             \
  int len = containerCount();                                                   \
  for( int id = 0; id < len; ++id ) {                                           \
    QWidget* w = container( id );                                               \
    if ( w->inherits( "KToolBar" ) ) {                                          \
      QWidget* r = static_cast<KToolBar*>( w )->getWidget( itemId( id ) );      \
      if (r) {                                                                  \
        TKFloatSpinBox* fsb = (TKFloatSpinBox*)r->child("widget");                  \
        if (fsb) {                                                              \
          fsb->method(value);                                                 \
        }                                                                       \
      }                                                                         \
    } else {                                                                    \
      QWidget* r = static_cast<QWidget*>(w->child("KTToolBarLayout"));          \
      if (r) {                                                                  \
        TKFloatSpinBox* fsb = (TKFloatSpinBox*)r->child("widget");                  \
        if (fsb) {                                                              \
          fsb->method(value);                                                 \
        }                                                                       \
      }                                                                         \
    }                                                                           \
  }

void TKFloatSpinBoxAction::setValue(float value)
{
  SETVALUE(m_value,setValue)
}

void TKFloatSpinBoxAction::setButtonSymbols( TKFloatSpinBox::ButtonSymbols value )
{
  SETVALUE(m_buttonSymbol,setButtonSymbols)
}

void TKFloatSpinBoxAction::setDecimals( int value )
{
  SETVALUE(m_decimal,setDecimals)
  (void) updateLayout();
}

void TKFloatSpinBoxAction::setWrapping( bool value )
{
  SETVALUE(m_wrapping,setWrapping)
}

void TKFloatSpinBoxAction::setMinValue( float value )
{
  SETVALUE(m_minvalue,setMinValue)
  (void) updateLayout();
}

void TKFloatSpinBoxAction::setMaxValue( float value )
{
  SETVALUE(m_maxvalue,setMaxValue)
  (void) updateLayout();
}

void TKFloatSpinBoxAction::setLineStep( float value )
{
  SETVALUE(m_linestep,setLineStep)
}

void TKFloatSpinBoxAction::setPrefix( const QString& value )
{
  SETVALUE(m_prefix,setPrefix)
  (void) updateLayout();
}

void TKFloatSpinBoxAction::setSuffix( const QString& value )
{
  SETVALUE(m_suffix,setSuffix)
  (void) updateLayout();
}

void TKFloatSpinBoxAction::setMinimumStyle(bool value )
{
  SETVALUE(m_minimum,setMinimumStyle)
  (void) updateLayout();
}


TKUFloatSpinBoxAction::TKUFloatSpinBoxAction( const QString& text, const QString& icon, int accel, QObject* parent, const char* name)
: TKFloatSpinBoxAction(text,icon,accel,parent,name)
{
  setUnit((int)UnitPoint);
}

TKUFloatSpinBoxAction::TKUFloatSpinBoxAction( const QString& text, int accel, QObject* parent, const char* name)
: TKFloatSpinBoxAction(text,accel,parent,name)
{
  setUnit((int)UnitPoint);
}

TKUFloatSpinBoxAction::~TKUFloatSpinBoxAction()
{
}

void TKUFloatSpinBoxAction::setUnit(int unit)
{
  blockSignals(true);
  setSuffix(unitToString(unit));
  float v = cvtPtToUnit(unit,cvtUnitToPt(m_unit,m_value));
  setMinValue( cvtPtToUnit(unit,cvtUnitToPt(m_unit,m_minvalue)) );
  setMaxValue( cvtPtToUnit(unit,cvtUnitToPt(m_unit,m_maxvalue)) );
  TKFloatSpinBoxAction::setValue( v );

  m_unit = unit;
  blockSignals(false);
}

float TKUFloatSpinBoxAction::value(int unit)
{
  return cvtPtToUnit(unit,cvtUnitToPt(m_unit,m_value));
}

void TKUFloatSpinBoxAction::setValue(float value,int unit)
{
  TKFloatSpinBoxAction::setValue( cvtPtToUnit(m_unit,cvtUnitToPt(unit,value)) );
}

#include "tkfloatspinboxaction.moc"
