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

#include "ToolPalette.h"
#include <qpushbt.h>
#include <qlayout.h>

#include "ToolPalette.moc"

ToolPalette::ToolPalette (int rows, int cols, QWidget* parent,
			  const char* name) : QWidget (parent, name) {
  nrows = rows; ncols = cols;
  buttons.resize (rows);
  QGridLayout* layout = new QGridLayout (this, rows, cols);
  for (int i = 0; i < nrows; i++) {
    buttons.insert (i, new QVector<ToolButton> (ncols));
    for (int k = 0; k < ncols; k++) {
      buttons[i]->insert (k, new ToolButton (this));
      buttons[i]->at (k)->setFixedSize (22, 22);
      buttons[i]->at (k)->setToggleButton (true);
      connect (buttons[i]->at(k), SIGNAL(clicked ()), this,
	       SLOT(buttonPressed ()));
      connect (buttons[i]->at(k), SIGNAL(rightButtonPressed ()), this,
	       SLOT(rightMouseButtonPressed ()));
      layout->addWidget (buttons[i]->at (k), i, k);
    }
  }
  layout->activate ();
  buttons[0]->at (0)->setOn (true);
  current_id = 0;
}

void ToolPalette::setPixmap (int idx, const QPixmap& p) {
  int row, col;

  row = idx / ncols;
  col = idx % ncols;
  buttons[row]->at (col)->setPixmap (p);
}

void ToolPalette::rightMouseButtonPressed () {
  int id = 0;
  ToolButton* button = (ToolButton *) sender ();

  for (int i = 0; i < nrows; i++) {
    for (int k = 0; k < ncols; k++) {
      if (buttons[i]->at (k) == button) {
	id = i * ncols + k;
	emit toolConfigActivated (id);
      }
    }
  }
}

void ToolPalette::buttonPressed () {
  int id = 0;
  ToolButton* button = (ToolButton *) sender ();

  for (int i = 0; i < nrows; i++) {
    for (int k = 0; k < ncols; k++) {
      if (buttons[i]->at (k) == button) {
	id = i * ncols + k;
	if (id == current_id)
	  button->setOn (true);
      }
      else {
	ToolButton *bt = buttons[i]->at (k);
	if (bt->isOn ())
	  bt->setOn (false);
      }
    }
  }
  if (id != current_id) 
    emit toolSelected (id);
  current_id = id;
}
