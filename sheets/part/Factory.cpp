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

#include "SheetsDebug.h"

#include <KoDockRegistry.h>
#include <KoComponentData.h>
#include <KoResourcePaths.h>

#include "AboutData.h"
#include "Doc.h"
#include "Part.h"
#include "ui/CellEditorDocker.h"

using namespace Calligra::Sheets;

KoComponentData* Factory::s_global = 0;
KAboutData* Factory::s_aboutData = 0;

Factory::Factory()
    : KPluginFactory()
{
    //debugSheets <<"Factory::Factory()";
    // Create our instance, so that it becomes KGlobal::instance if the
    // main app is Calligra Sheets.
    (void)global();
}

Factory::~Factory()
{
    //debugSheets <<"Factory::~Factory()";
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

const KoComponentData &Factory::global()
{
    if (!s_global) {
        s_global = new KoComponentData(*aboutData());

        KoResourcePaths::addResourceType("sheet-styles", "data", "calligrasheets/sheetstyles/");

        KoDockRegistry *dockRegistry = KoDockRegistry::instance();
        dockRegistry->add(new CellEditorDockerFactory);
    }
    return *s_global;
}
