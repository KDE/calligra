/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001 Igor Janssen (rm@linux.ru.net)

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

#ifndef __kontour_factory_h__
#define __kontour_factory_h__

#include <koFactory.h>

class KInstance;
class KAboutData;

class KontourFactory : public KoFactory
{
  Q_OBJECT
public:
  KontourFactory(QObject *parent = 0, const char *name = 0);
  ~KontourFactory();

  virtual KParts::Part *createPartObject(QWidget *parentWidget = 0, const char *widgetName = 0, QObject *parent = 0, const char *name = 0, const char *classname = "KoDocument", const QStringList &args = QStringList());

  static KInstance *global();
  static KAboutData *aboutData();

private:
  static KInstance *s_global;
  static KAboutData *s_aboutData;
};

#endif
