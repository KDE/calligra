/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2011 Boudewijn Rempt <boud@valdyas.org>

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
#include <kdebug.h>
#include "KWAboutData.h"
#include "KWDocument.h"
#include <kcomponentdata.h>

#include <kiconloader.h>

#include <KWPart.h>
#include <KoDockRegistry.h>
#include <KoDocumentRdfBase.h>
#include <KoToolRegistry.h>
#include <KoMainWindow.h>

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

KComponentData *KWFactory::s_instance = 0;
K4AboutData *KWFactory::s_aboutData = 0;

KWFactory::KWFactory()
    : KPluginFactory()
{
    // Create our instance, so that it becomes KGlobal::instance if the
    // main app is Words.
    (void) componentData();
}

KWFactory::~KWFactory()
{
    delete s_aboutData;
    s_aboutData = 0;
    delete s_instance;
    s_instance = 0;
}

QObject* KWFactory::create(const char* /*iface*/, QWidget* /*parentWidget*/, QObject *parent, const QVariantList& args, const QString& keyword)
{
    Q_UNUSED(args);
    Q_UNUSED(keyword);

    KWPart *part = new KWPart(parent);
    KWDocument *doc = new KWDocument(part);
    part->setDocument(doc);
    KoToolRegistry::instance()->add(new KWPageToolFactory());
    return part;
}

K4AboutData *KWFactory::aboutData()
{
    if (!s_aboutData) {
        s_aboutData = newWordsAboutData();
    }
    return s_aboutData;
}

const KComponentData &KWFactory::componentData()
{
    if (!s_instance) {
        s_instance = new KComponentData(aboutData());

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
    return *s_instance;
}
