/* This file is part of the KDE project
   Copyright (C)  2006 Peter Simonsson <peter.simonsson@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KivioFactory.h"

#include <kinstance.h>
#include <kstandarddirs.h>
#include <kiconloader.h>

#include "KivioDocument.h"
#include "KivioAboutData.h"

KInstance* KivioFactory::s_instance = 0;
KAboutData* KivioFactory::s_aboutData = 0;

KivioFactory::KivioFactory(QObject* parent)
  : KoFactory(parent)
{
  (void) instance();
}

KivioFactory::~KivioFactory()
{
  delete s_instance;
  s_instance = 0;
  delete s_aboutData;
  s_aboutData = 0;
}

KParts::Part* KivioFactory::createPartObject(QWidget* parentWidget,
                                        QObject* parent, const char* classname,
                                        const QStringList &args)
{
  Q_UNUSED(args);
  bool singleViewMode = (strcmp(classname, "KoDocument") != 0);

  KivioDocument* doc = new KivioDocument(parentWidget, parent, singleViewMode);

  if(singleViewMode)
    doc->setReadWrite(false);

  return doc;
}

KInstance* KivioFactory::instance()
{
  if (!s_instance) {
    s_instance = new KInstance(aboutData());

    s_instance->dirs()->addResourceType("kivio_template", KStandardDirs::kde_default("data")
                                        + "kivio/templates/");
    s_instance->iconLoader()->addAppDir("koffice");
  }

  return s_instance;
}

KAboutData* KivioFactory::aboutData()
{
  if(!s_aboutData) {
    s_aboutData = newKivioAboutData();
  }

  return s_aboutData;
}

#include "KivioFactory.moc"
