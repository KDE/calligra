/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998-99 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

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

#ifndef CanvasView_h_
#define CanvasView_h_

#include <qscrollview.h>
#include <qobject.h>

class QWidget;
class QResizeEvent;

class CanvasView : public QScrollView {
  Q_OBJECT
public:
  CanvasView (QWidget *parent=0, const char *name=0, WFlags f=0);
  ~CanvasView ();

signals:
  void viewportResize();
 
protected:
  virtual void viewportResizeEvent(QResizeEvent * ev);
};

#endif
