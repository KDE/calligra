/*
  $Id$

  Actually this code is a modification of the KFontDialog class, written
  by Bernd Johannes Wuebben  (wuebben@math.cornell.edu).
  
  Copyright (C) 1996 Bernd Johannes Wuebben   
  wuebben@math.cornell.edu

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

#ifndef FontSelector_h_
#define FontSelector_h_

#include <qmsgbox.h>
#include <qpixmap.h>
#include <qapp.h>
#include <qframe.h> 
#include <qbttngrp.h>
#include <qchkbox.h>
#include <qcombo.h>
#include <qframe.h>
#include <qgrpbox.h>
#include <qlabel.h>
#include <qlined.h>
#include <qlistbox.h>
#include <qpushbt.h>
#include <qradiobt.h>
#include <qscrbar.h>
#include <qtooltip.h>

#include <qstring.h>
#include <qfont.h>

/* Unfortunally does MOC create a string out of the classdefinition
 * so #define foo String& doesn't work
 */
#if QT_VERSION >= 199
#include "FontSelector2.h"
#else
#include "FontSelector1.h"
#endif

// If we remove the support of KDE 1 we can copy the contents from
// FontSelector2.h

#endif
