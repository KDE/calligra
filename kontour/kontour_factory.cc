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

#include "kontour_factory.h"

#include <kinstance.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <kaboutdata.h>
#include <klocale.h>

#include "kontour_doc.h"
#include "kontour_about.h"

extern "C"
{
  void *init_libkontourpart()
  {
    return new KontourFactory;
  }
};

KInstance *KontourFactory::s_global = 0;
KAboutData *KontourFactory::s_aboutData = 0;

KontourFactory::KontourFactory(QObject *parent, const char *name)
:KoFactory(parent, name)
{
  global();
}

KontourFactory::~KontourFactory()
{
  delete s_aboutData;
  s_aboutData = 0L;
  delete s_global;
  s_global = 0L;
}

KAboutData *KontourFactory::aboutData()
{
  if(!s_aboutData)
    s_aboutData = newKontourAboutData();
  return s_aboutData;
}

KParts::Part *KontourFactory::createPartObject(QWidget *parentWidget, const char *widgetName, QObject* parent, const char *name, const char *classname, const QStringList &)
{
  bool bWantKoDocument = (strcmp(classname, "KoDocument") == 0);

  KontourDocument *doc = new KontourDocument(parentWidget, widgetName, parent, name, !bWantKoDocument);

  if(!bWantKoDocument)
    doc->setReadWrite(false);

  return doc;
}

KInstance* KontourFactory::global()
{
  if(!s_global)
    s_global = new KInstance(aboutData());
  s_global->dirs()->addResourceType("kontour_template", KStandardDirs::kde_default("data") + "kontour/templates/");

  // Tell the iconloader about share/apps/koffice/icons
  s_global->iconLoader()->addAppDir("koffice");
  return s_global;
}

#include "kontour_factory.moc"
