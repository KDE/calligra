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

#ifndef ToolPalette_h_
#define ToolPalette_h_

#include <qwidget.h>
#include <qpushbt.h>
#include <qvector.h>

#include "ToolButton.h"

class ToolPalette : public QWidget {
  Q_OBJECT

public:
  ToolPalette (int width, int height, QWidget *parent = 0,
	       const char* name = 0);
  ~ToolPalette () {}
  
  void setPixmap (int idx, const QPixmap& p);

signals:
  void toolSelected (int id);
  void toolConfigActivated (int id);

private slots:
  void buttonPressed ();
  void rightMouseButtonPressed ();

private:
  int nrows, ncols;
  QVector<QVector<ToolButton> > buttons;
  int current_id;
};

#endif
