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

#ifndef version_h_
#define version_h_

#include <qapp.h>

#if QT_VERSION >= 199
#define QT_PRFX Qt
#define NEWKDE
#define QSTR_NULL QString::null
#define I18N(s) i18n(s).ascii()
#else
#define QT_PRFX
#define QSTR_NULL (const char *) 0L
#define I18N(s) i18n(s)
#endif

#define APP_NAME "killustrator"
#define APP_VERSION "0.6.5"

#endif
