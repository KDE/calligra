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

#include "StartupScreen.h"
#include "StartupScreen.moc"
#include <qapp.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qtimer.h>
#include <iostream.h>

StartupScreen::StartupScreen (const char* pname, int seconds) :
  QWidget (0, "Startup Screen", WStyle_NoBorder | WStyle_Customize) {
  if (! pixmap.load (pname)) {
    cerr <<  "Could not find startup pixmap: \"" 
	 << pname << "\" !" << endl;
  }

  resize (pixmap.size ());
  move ((QApplication::desktop()->width ()-pixmap.width ()) / 2,
	(QApplication::desktop()->height ()-pixmap.height ()) / 2);
  label = new QLabel (this);
  label->setPixmap (pixmap);
  label->resize (pixmap.size());

  timer = new QTimer (this);
  connect (timer, SIGNAL (timeout ()), this, SLOT (destroy ()));
  timer->start (seconds * 1000, true);
  setActiveWindow ();
  show ();
  raise ();
}


void StartupScreen::destroy () {
  if (timer->isActive ()) // happens if destroy was called explicitly
    timer->stop ();
  hide ();
  delete this;
}


StartupScreen::~StartupScreen () {
  delete label;
  delete timer;
}
