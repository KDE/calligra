/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef UnitBox_h_
#define UnitBox_h_

#include <qhbox.h>
#include <units.h>

class QComboBox;
class KDoubleNumInput;

class UnitBox : public QHBox {
  Q_OBJECT
public:
  UnitBox (QWidget* parent = 0L, const char* name = 0L);
  ~UnitBox ();

  void setFormatString (const char* fmt);

  float getValue ();
  void setValue (float value);

  void setStep (float step);
  float getStep () const;

  void setRange (float minVal, float maxVal);
  void getRange (float& minVal, float& maxVal);

  void setEditable (bool);
  void enableUnits (bool flag);

  static void setDefaultMeasurementUnit (MeasurementUnit unit);

signals:
  void valueChanged (float val);

protected slots:
  void unitChanged (int id);
  void slotValueChanged(double f);

private:
  KDoubleNumInput *valueBox;
  bool isUnitEnabled;
  QComboBox *unitCombo;
  MeasurementUnit unit;
  float ptMinVal, ptMaxVal; // the minimal and maximal value in points
  float m_step;

  static MeasurementUnit defaultUnit;
};

#endif
