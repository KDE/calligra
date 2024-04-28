/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@valdyas.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KWFactory.h"
#include "KWAboutData.h"
#include "KWDocument.h"

#include <KIconLoader>

#include <KWPart.h>
#include <KoDockRegistry.h>
#include <KoDocumentRdfBase.h>
#include <KoToolRegistry.h>
#include <KoMainWindow.h>
#include <KoComponentData.h>

#ifdef SHOULD_BUILD_RDF
#include <KoDocumentRdf.h>
#include <KoSemanticStylesheetsEditor.h>
#include "dockers/KWRdfDocker.h"
#include "dockers/KWRdfDockerFactory.h"
#endif

#include "pagetool/KWPageToolFactory.h"
#include "dockers/KWStatisticsDocker.h"
#include "dockers/KWNavigationDockerFactory.h"

#ifndef NDEBUG
#include "dockers/KWDebugDockerFactory.h"
#endif

KoComponentData *KWFactory::s_componentData = 0;

KWFactory::KWFactory()
    : KPluginFactory()
{
    // Create our instance, so that it becomes KGlobal::instance if the
    // main app is Words.
    (void) componentData();
}

KWFactory::~KWFactory()
{
    delete s_componentData;
    s_componentData = 0;
}

QObject* KWFactory::create(const char* /*iface*/, QWidget* /*parentWidget*/, QObject *parent, const QVariantList& args)
{
    Q_UNUSED(args);

    KWPart *part = new KWPart(parent);
    KWDocument *doc = new KWDocument(part);
    part->setDocument(doc);
    KoToolRegistry::instance()->add(new KWPageToolFactory());
    return part;
}

const KoComponentData &KWFactory::componentData()
{
    if (!s_componentData) {
        KAboutData *aboutData = newWordsAboutData();
        s_componentData = new KoComponentData(*aboutData);
        delete aboutData;

        KIconLoader::global()->addAppDir("calligra");

        KoDockRegistry *dockRegistry = KoDockRegistry::instance();
        dockRegistry->add(new KWStatisticsDockerFactory());
        dockRegistry->add(new KWNavigationDockerFactory());
#ifndef NDEBUG
        dockRegistry->add(new KWDebugDockerFactory());
#endif

#ifdef SHOULD_BUILD_RDF
// TODO reenable after release
        dockRegistry->add(new KWRdfDockerFactory());
#endif
    }
    return *s_componentData;
}
