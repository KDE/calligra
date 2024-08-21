/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "Factory.h"
#include "AboutData.h"
#include "Doc.h"
#include "Part.h"

#include <KoComponentData.h>
#include <KoDockRegistry.h>
#include <KoResourcePaths.h>

using namespace Calligra::Sheets;

KoComponentData *Factory::s_global = nullptr;
KAboutData *Factory::s_aboutData = nullptr;

Factory::Factory()
    : KPluginFactory()
{
    // debugSheets <<"Factory::Factory()";
    //  Create our instance, so that it becomes KGlobal::instance if the
    //  main app is Calligra Sheets.
    (void)global();
}

Factory::~Factory()
{
    // debugSheets <<"Factory::~Factory()";
    delete s_aboutData;
    s_aboutData = nullptr;
    delete s_global;
    s_global = nullptr;
}

QObject *Factory::create(const char * /*iface*/, QWidget * /*parentWidget*/, QObject *parent, const QVariantList &args)
{
    Q_UNUSED(args);
    Part *part = new Part(parent);
    Doc *doc = new Doc(part);
    part->setDocument(doc);
    return part;
}

KAboutData *Factory::aboutData()
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
    }
    return *s_global;
}
