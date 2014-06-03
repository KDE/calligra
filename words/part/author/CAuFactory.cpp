/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2011 Boudewijn Rempt <boud@valdyas.org>
   Copyright (C) 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>

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

#include "CAuFactory.h"

#include "CAuPart.h"
#include "CAuAboutData.h"

#include <kdebug.h>
#include "KWDocument.h"
#include <kcomponentdata.h>
#include <kstandarddirs.h>

#include <kiconloader.h>

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
#include "dockers/KWStatisticsDocker.h"
#include "pagetool/KWPageToolFactory.h"

KComponentData *CAuFactory::s_instance = 0;
KAboutData *CAuFactory::s_aboutData = 0;

CAuFactory::CAuFactory(QObject *parent)
        : KPluginFactory(*aboutData(), parent)
{
    // Create our instance, so that it becomes KGlobal::instance if the
    // main app is Author.
    (void) componentData();
}

CAuFactory::~CAuFactory()
{
    delete s_aboutData;
    s_aboutData = 0;
    delete s_instance;
    s_instance = 0;
}

QObject* CAuFactory::create(const char* /*iface*/, QWidget* /*parentWidget*/, QObject *parent, const QVariantList& args, const QString& keyword)
{
    Q_UNUSED(args);
    Q_UNUSED(keyword);

    CAuPart *part = new CAuPart(parent);
    KWDocument *doc = new KWDocument(part);
    part->setDocument(doc);
    KoToolRegistry::instance()->add(new KWPageToolFactory());
    return part;
}

KAboutData *CAuFactory::aboutData()
{
    if (!s_aboutData) {
        s_aboutData = newAuthorAboutData();
    }
    return s_aboutData;
}

const KComponentData &CAuFactory::componentData()
{
    if (!s_instance) {
        s_instance = new KComponentData(aboutData());

        s_instance->dirs()->addResourceType("words_template",
                                            "data", "words/templates/");
        s_instance->dirs()->addResourceType("styles", "data", "words/styles/");

        KIconLoader::global()->addAppDir("calligra");


        KoDockRegistry *dockRegistry = KoDockRegistry::instance();
        dockRegistry->add(new KWStatisticsDockerFactory());
#ifdef SHOULD_BUILD_RDF
// TODO reenable after release
        dockRegistry->add(new KWRdfDockerFactory());
#endif

    }
    return *s_instance;
}
