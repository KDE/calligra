/* -*- C++ -*-
   
  $Id$

  Most of this code is stolen from KLyx, 
  Copyright (C) 1998 Matthias Kalle Dalheimer (kalle@kde.org).

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

#ifndef StartupScreen_h_
#define StartupScreen_h_

#include <qpixmap.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qpixmap.h>

class StartupScreen : public QWidget {
  Q_OBJECT
public:
  StartupScreen (const char* pname, int seconds);

public slots:
  void destroy ();

protected:
 ~StartupScreen ();

  QPixmap pixmap;
  QLabel* label;
  QTimer* timer;
};

#endif
