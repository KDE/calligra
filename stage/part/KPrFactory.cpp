// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrFactory.h"
#include "KPrAboutData.h"
#include "KPrDocument.h"
#include "KPrPart.h"

#include <KoComponentData.h>
#include <KoDockRegistry.h>
#include <KoPluginLoader.h>

KoComponentData *KPrFactory::s_instance = nullptr;
KAboutData *KPrFactory::s_aboutData = nullptr;

static int factoryCount = 0;

KPrFactory::KPrFactory()
    : KPluginFactory()
{
    (void)componentData();

    if (factoryCount == 0) {
        // Load the KoPA-specific tools
        KoPluginLoader::load(QStringLiteral("calligra/pageapptools"));

        // Load the Stage-specific tools
        KoPluginLoader::load(QStringLiteral("calligrastage/tools"));
    }
    factoryCount++;
}

KPrFactory::~KPrFactory()
{
    delete s_aboutData;
    s_aboutData = nullptr;
    delete s_instance;
    s_instance = nullptr;
}

QObject *KPrFactory::create(const char * /*iface*/, QWidget * /*parentWidget*/, QObject *parent, const QVariantList &args)
{
    Q_UNUSED(args);
    KPrPart *part = new KPrPart(parent);
    KPrDocument *doc = new KPrDocument(part);
    doc->setDefaultStylesResourcePath(QLatin1String("calligrastage/styles/"));
    part->setDocument(doc);
    return part;
}

KAboutData *KPrFactory::aboutData()
{
    if (!s_aboutData)
        s_aboutData = newKPresenterAboutData();

    return s_aboutData;
}

const KoComponentData &KPrFactory::componentData()
{
    if (!s_instance) {
        s_instance = new KoComponentData(*aboutData());
    }
    return *s_instance;
}
