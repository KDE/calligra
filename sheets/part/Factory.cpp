/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

// Local
#include "Factory.h"

#include <kdebug.h>
#include <kcomponentdata.h>
#include <kstandarddirs.h>

#include <KoDockRegistry.h>

#include "AboutData.h"
#include "Doc.h"
#include "Part.h"
#include "ui/CellEditorDocker.h"

using namespace Calligra::Sheets;

KComponentData* Factory::s_global = 0;
KAboutData* Factory::s_aboutData = 0;

Factory::Factory(QObject* parent)
        : KPluginFactory(*aboutData(), parent)
{
    //kDebug(36001) <<"Factory::Factory()";
    // Create our instance, so that it becomes KGlobal::instance if the
    // main app is KSpread.
    (void)global();
}

Factory::~Factory()
{
    //kDebug(36001) <<"Factory::~Factory()";
    delete s_aboutData;
    s_aboutData = 0;
    delete s_global;
    s_global = 0;
}

QObject* Factory::create(const char* /*iface*/, QWidget* /*parentWidget*/, QObject *parent, const QVariantList& args, const QString& keyword)
{
    Q_UNUSED(args);
    Q_UNUSED(keyword);
    Part *part = new Part(parent);
    Doc *doc = new Doc(part);
    part->setDocument(doc);
    return part;
}

KAboutData* Factory::aboutData()
{
    if (!s_aboutData)
        s_aboutData = newAboutData();
    return s_aboutData;
}

const KComponentData &Factory::global()
{
    if (!s_global) {
        s_global = new KComponentData(aboutData());

        s_global->dirs()->addResourceType("sheets_template", "data", "sheets/templates/");
        s_global->dirs()->addResourceType("toolbar", "data", "calligra/toolbar/");
        s_global->dirs()->addResourceType("functions", "data", "sheets/functions/");
        s_global->dirs()->addResourceType("sheet-styles", "data", "sheets/sheetstyles/");

        KoDockRegistry *dockRegistry = KoDockRegistry::instance();
        dockRegistry->add(new CellEditorDockerFactory);
    }
    return *s_global;
}

#include "Factory.moc"
