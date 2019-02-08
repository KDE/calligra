// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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

#include "KPrFactory.h"
#include "KPrDocument.h"
#include "KPrAboutData.h"
#include "KPrPart.h"

#include <KoComponentData.h>
#include <KoPluginLoader.h>
#include <KoDockRegistry.h>

KoComponentData* KPrFactory::s_instance = 0;
KAboutData* KPrFactory::s_aboutData = 0;

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
    s_aboutData = 0;
    delete s_instance;
    s_instance = 0;
}

QObject* KPrFactory::create( const char* /*iface*/, QWidget* /*parentWidget*/, QObject *parent,
                             const QVariantList& args, const QString& keyword )
{
    Q_UNUSED( args );
    Q_UNUSED( keyword );
    KPrPart *part = new KPrPart(parent);
    KPrDocument *doc = new KPrDocument(part);
    doc->setDefaultStylesResourcePath(QLatin1String("calligrastage/styles/"));
    part->setDocument(doc);
    return part;
}

KAboutData* KPrFactory::aboutData()
{
    if( !s_aboutData )
        s_aboutData = newKPresenterAboutData();

    return s_aboutData;
}

const KoComponentData &KPrFactory::componentData()
{
    if ( !s_instance )
    {
        s_instance = new KoComponentData(*aboutData());
    }
    return *s_instance;
}
