/* This file is part of the Calligra project, made with-in the KDE community

   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>
   SPDX-FileCopyrightText: 2013 Friedrich W. H. Kossebau <kossebau@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KoLocationSemanticItemPlugin.h"

// plugin
#include "KoLocationSemanticItemFactory.h"
// rdf
#include <KoRdfSemanticItemRegistry.h>
// KF5
#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "calligra_semanticitem_location.json", registerPlugin<KoLocationSemanticItemPlugin>();)

KoLocationSemanticItemPlugin::KoLocationSemanticItemPlugin(QObject *parent, const QVariantList & /*args */)
    : QObject(parent)
{
    KoRdfSemanticItemRegistry::instance()->add(new KoLocationSemanticItemFactory());
}

#include "KoLocationSemanticItemPlugin.moc" // prevents vtable error
