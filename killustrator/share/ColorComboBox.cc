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

#include "ColorComboBox.h"
#include "ColorComboBox.moc"

ColorComboBox::ColorComboBox (QWidget *parent, const char* name) :
  KColorCombo (parent, name) {
  connect (this, SIGNAL(activated (const QColor&)), this, 
           SLOT(selectColor (const QColor&)));
}

void ColorComboBox::selectColor (const QColor& c) {
  selectedColor = c;
}

void ColorComboBox::setColor (const QColor& c) {
  if (selectedColor != c) {
    KColorCombo::setColor (c);
    selectedColor = c;
  }
}

QColor ColorComboBox::getColor () const {
  return selectedColor;
}
