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
#ifndef TK2FLOATSPINBOXACTION_H
#define TK2FLOATSPINBOXACTION_H

#include "qwidget.h"
#include "tkaction.h"
#include "tkunits.h"

class TKUFloatSpinBoxAction;
class QVBoxLayout;

/****************************************************************/
class TK2UFloatSpinBoxAction : public TKAction
{ Q_OBJECT
public:
  TK2UFloatSpinBoxAction( QObject* parent, const char* name );
  ~TK2UFloatSpinBoxAction();

  int plug( QWidget* widget, int index = -1 );

  TKUFloatSpinBoxAction* action1() { return m_action1; }
  TKUFloatSpinBoxAction* action2() { return m_action2; }

  int unit();

public slots:  
  void setUnit(int);

protected slots:
  virtual void slotActivated1();
  virtual void slotActivated2();

protected:
  TKUFloatSpinBoxAction* m_action1;
  TKUFloatSpinBoxAction* m_action2;
};
/****************************************************************/
class TK2FloatSpinBoxActionWidget : public QWidget
{ Q_OBJECT
public:
  TK2FloatSpinBoxActionWidget(TKUFloatSpinBoxAction* a1, TKUFloatSpinBoxAction* a2, QWidget* parent=0, const char* name=0);
  ~TK2FloatSpinBoxActionWidget();

protected:
  void resizeEvent(QResizeEvent*);

private:
  QVBoxLayout* m_layout;
};
/****************************************************************/
class TKSizeAction : public TK2UFloatSpinBoxAction
{ Q_OBJECT
public:
  TKSizeAction( QObject* parent, const char* name );
  ~TKSizeAction();

  void size(double&, double&, int unit = (int)UnitPoint);
  void setSize(double, double, int unit = (int)UnitPoint);
};
/****************************************************************/
class TKPositionAction : public TK2UFloatSpinBoxAction
{ Q_OBJECT
public:
  TKPositionAction( QObject* parent, const char* name );
  ~TKPositionAction();

  void pos(double&, double&, int unit = (int)UnitPoint);
  void setPos(double, double, int unit = (int)UnitPoint);
};

#endif

