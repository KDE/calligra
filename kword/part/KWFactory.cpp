/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#include "KWFactory.h"
#include "KWAboutData.h"
#include "KWDocument.h"
#include <kinstance.h>
#include <kstandarddirs.h>

#include <kiconloader.h>

KInstance* KWFactory::s_instance = 0;
KAboutData* KWFactory::s_aboutData = 0;

KWFactory::KWFactory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
  // Create our instance, so that it becomes KGlobal::instance if the
  // main app is KWord.
  (void) instance();
}

KWFactory::~KWFactory()
{
    delete s_aboutData;
    s_aboutData=0;
    delete s_instance;
    s_instance=0L;
}

KParts::Part* KWFactory::createPartObject( QWidget *parentWidget, QObject* parent, const char* classname, const QStringList & )
{
    bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

    KWDocument *doc = new KWDocument( parentWidget, parent, !bWantKoDocument );

    if ( !bWantKoDocument )
      doc->setReadWrite( false );

    return doc;
}

KAboutData* KWFactory::aboutData()
{
    if(!s_aboutData) {
        s_aboutData = newKWordAboutData();
    }
    return s_aboutData;
}

KInstance* KWFactory::instance()
{
    if ( !s_instance )
    {
      s_instance = new KInstance( aboutData() );

      s_instance->dirs()->addResourceType( "kword_template",
              KStandardDirs::kde_default("data") + "kword/templates/");

      s_instance->iconLoader()->addAppDir("koffice");
    }
    return s_instance;
}

#include "KWFactory.moc"
