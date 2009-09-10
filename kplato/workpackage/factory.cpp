/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
   Copyright (C) 2009 Dag Andersen <danders@get2net.dk>

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

#include "factory.h"
#include "part.h"
#include "aboutdata.h"

#include <kcomponentdata.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kdebug.h>
#include <kstandarddirs.h>


namespace KPlatoWork
{

KComponentData* Factory::s_global = 0L;
KAboutData* Factory::s_aboutData = 0L;

Factory::Factory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
    global();
}

Factory::~Factory()
{
    delete s_aboutData;
    s_aboutData = 0L;
    delete s_global;
    s_global = 0L;
}

KParts::Part *Factory::createPartObject(QWidget *parentWidget,
                                        QObject* parent,
                                        const char* classname,
                                        const QStringList &)
{
    Part *part = new Part( parentWidget, parent );
    return part;
}

KAboutData* Factory::aboutData()
{
    if ( !s_aboutData )
        s_aboutData = newAboutData();
    return s_aboutData;
}

const KComponentData &Factory::global()
{
    if ( !s_global )
    {
        s_global = new KComponentData( aboutData() );

        // Add any application-specific resource directories here
        s_global->dirs()->addResourceType("kplatowork_template", "data", "kplatowork/templates/");
        s_global->dirs()->addResourceType("expression", "data", "kplatowork/expression/");
        s_global->dirs()->addResourceType("projects", "data", "kplatowork/projects/");
        
//        s_global->dirs()->addResourceType("toolbar", "data", "koffice/toolbar/");

        // Tell the iconloader about share/apps/koffice/icons
        KIconLoader::global()->addAppDir("koffice");
        
    }
    return *s_global;
}

} // KPlatoWork namespace

#include "factory.moc"
