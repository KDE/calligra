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

#ifndef FloatSpinBox_h_
#define FloatSpinBox_h_

#include <qspinbox.h>
#include <qvalidator.h>

class FloatSpinBox : public QSpinBox {
  Q_OBJECT
public:
  FloatSpinBox (QWidget* parent = 0L, const char* name = 0L, 
                int align = AlignLeft);
  ~FloatSpinBox ();
  
  void setFormatString (const char* fmt);
  
  float getValue ();
  void setValue (float value);
  
  void setStep (float step);
  float getStep () const;

  void setRange (float minVal, float maxVal);
  void getRange (float& minVal, float& maxVal);

signals:
  void valueChanged (float v);

public slots:
  void slotIncrease ();
  void slotDecrease ();

protected slots:
  void slotValueChange ();
  void reportChanges ();

protected:
  int mapTextToValue (bool *ok);
  QString mapValueToText (int v);

private:
  float step, minval, maxval;
  QString format;
  QDoubleValidator *val;
};

#endif
