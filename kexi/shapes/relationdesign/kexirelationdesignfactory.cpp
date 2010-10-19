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

#include "kexirelationdesignfactory.h"
#include "kexirelationdesignshape.h"
#include <klocale.h>
#include <kdebug.h>

KexiRelationDesignFactory::KexiRelationDesignFactory ( QObject* parent )
        : KoShapeFactoryBase ( parent, "KexiRelationDesignShape",
                           i18n ( "Kexi Relation Design" ) ) {
    setToolTip ( i18n ( "A kexi relation design shape" ) );
    setIcon( "kexi" );
    setOdfElementNames("http://www.koffice.org/kexirelationdesign", QStringList("shape") );
    setLoadingPriority( 1 );
}

KoShape *KexiRelationDesignFactory::createDefaultShape(KoResourceManager *) const
{
    kDebug();
    KexiRelationDesignShape* fooShape = new KexiRelationDesignShape();
    // set defaults
    return fooShape;
}

bool KexiRelationDesignFactory::supports(const KoXmlElement & e, KoShapeLoadingContext &context) const
{
    Q_UNUSED(context);
    kDebug();
    return ( e.localName() == "shape" ) && ( e.namespaceURI() == "http://www.koffice.org/kexirelationdesign" );
}

