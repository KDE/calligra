/* This file is part of the KDE project
   Copyright (C) 2009-2010 Adam Pigg <adam@piggz.co.uk>
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "kexirelationdesignshapeplugin.h"
#include "kexirelationdesignfactory.h"
#include "kexirelationdesigntoolfactory.h"
#include <KoShapeRegistry.h>
#include <kgenericfactory.h>
#include <KoToolRegistry.h>

K_EXPORT_COMPONENT_FACTORY ( kexirelationdesignshape,
                             KGenericFactory<KexiRelationDesignShapePlugin> ( "KexiRelationDesignShapePlugin" ) )

KexiRelationDesignShapePlugin::KexiRelationDesignShapePlugin ( QObject *parent, const QStringList& )
        : QObject ( parent ) {
    // register the shape's factory
    KoShapeRegistry::instance()->add (new KexiRelationDesignFactory ( parent ) );
    KoToolRegistry::instance()->add (new KexiRelationDesignToolFactory ( parent ) );
    // we could register more things here in this same plugin.
}
#include "kexirelationdesignshapeplugin.moc"

