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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kpresenter_factory.h"
#include "kpresenter_doc.h"
#include "preview.h"

#include <kimageio.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kinstance.h>
#include <kaboutdata.h>
#include <qstringlist.h>
#include <klocale.h>
#include <kiconloader.h>


static const char* description=I18N_NOOP("KOffice Presentation Tool");
static const char* version="0.1";

extern "C"
{
    void* init_libkpresenterpart()
    {
        return new KPresenterFactory;
    }
};


KInstance* KPresenterFactory::s_global = 0;

KPresenterFactory::KPresenterFactory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
    global();
}

KPresenterFactory::~KPresenterFactory()
{
    if ( s_global )
    {
      delete s_global->aboutData();
      delete s_global;
      s_global = 0L;
    }
}

KParts::Part* KPresenterFactory::createPart( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, const char* classname, const QStringList & )
{
    bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

    KPresenterDoc *doc = new KPresenterDoc( parentWidget, widgetName, parent, name, !bWantKoDocument );

    if ( !bWantKoDocument )
      doc->setReadWrite( false );

    emit objectCreated(doc);
    return doc;
}

KAboutData* KPresenterFactory::aboutData()
{
      KAboutData *aboutData= new KAboutData( "kpresenter", I18N_NOOP("KPresenter"),
        version, description, KAboutData::License_GPL,
        "(c) 1998-2000, Reginald Stadlbauer");
      aboutData->addAuthor("Reginald Stadlbauer",0, "reggie@kde.org");
      return aboutData;
}

KInstance* KPresenterFactory::global()
{
    if ( !s_global )
    {
      s_global = new KInstance(aboutData());

      s_global->dirs()->addResourceType("kpresenter_template",
                                        KStandardDirs::kde_default("data") + "kpresenter/templates/");
      s_global->dirs()->addResourceType("autoforms",
                                        KStandardDirs::kde_default("data") + "kpresenter/autoforms/");
      s_global->dirs()->addResourceType("slideshow",
                                        KStandardDirs::kde_default("data") + "kpresenter/slideshow/");
      // Tell the iconloader about share/apps/koffice/icons
      s_global->iconLoader()->addAppDir("koffice");
    }
    return s_global;
}

#include "kpresenter_factory.moc"
