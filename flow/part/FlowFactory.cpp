/* This file is part of the KDE project
   Copyright (C)  2006 Peter Simonsson <peter.simonsson@gmail.com>

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

#include "FlowFactory.h"

#include "FlowDocument.h"
#include "FlowAboutData.h"
#include "FlowPart.h"

#include <KoPluginLoader.h>

#include <kiconloader.h>
#include <KoComponentData.h>

KoComponentData* FlowFactory::s_global = nullptr;

static int factoryCount = 0;

FlowFactory::FlowFactory()
    : KPluginFactory()
{
    (void)global();

    if (factoryCount == 0) {

        // Load the KoPA-specific tools
        KoPluginLoader::load(QStringLiteral("CalligraPageApp/Tool"));

        // Load Flow specific dockers
        KoPluginLoader::load(QStringLiteral("Flow/Dock"));
    }
    factoryCount++;
}

FlowFactory::~FlowFactory()
{
}

QObject* FlowFactory::create( const char* /*iface*/, QWidget* /*parentWidget*/, QObject *parent,
                              const QVariantList& args, const QString& keyword )
{
    Q_UNUSED( args );
    Q_UNUSED( keyword );
    FlowPart *part = new FlowPart(parent);
    FlowDocument* doc = new FlowDocument(part);
    doc->setDefaultStylesResourcePath(QStringLiteral("flow/styles/"));
    part->setDocument(doc);

    return part;
}

const KoComponentData &FlowFactory::global()
{
    if (!s_global) {
        KAboutData *about = newFlowAboutData();
        s_global = new KoComponentData(*about);
        delete about;

        // Add any application-specific resource directories here

        // Tell the iconloader about share/apps/calligra/icons
        KIconLoader::global()->addAppDir(QStringLiteral("calligra"));
    }
    return *s_global;
}
