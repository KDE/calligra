/* This file is part of the KDE project
 * Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "KoFormulaShapeFactory.h"
#include "KoFormulaShape.h"

#include <KoShapeFactoryBase.h>
#include <klocale.h>
#include <kdebug.h>

KoFormulaShapeFactory::KoFormulaShapeFactory( QObject *parent )
    : KoShapeFactoryBase( parent, KoFormulaShapeId, i18n( "Formula" ) )
{
    setToolTip(i18n( "A formula"));
    setIcon( "x-shape-formula" );
    setOdfElementNames( "http://www.w3.org/1998/Math/MathML", QStringList("math") );
    setLoadingPriority( 1 );
/*    KoShapeTemplate t;
    t.id = KoFormulaShapeId;
    t.name = i18n("Formula");
    t.toolTip = i18n("A formula");
    t.icon = ""; //TODO add it
    props = new KoProperties();
    t.properties = props;
    addTemplate( t );*/
}

KoFormulaShapeFactory::~KoFormulaShapeFactory()
{}

KoShape *KoFormulaShapeFactory::createDefaultShape(KoResourceManager *) const
{
    KoFormulaShape* formula = new KoFormulaShape();
    formula->setShapeId( KoFormulaShapeId );
    return formula;
}

bool KoFormulaShapeFactory::supports(const KoXmlElement& e) const
{
    kDebug() << e.nodeName() << " - "<< e.namespaceURI();
    return ( e.nodeName() == "math" ) && ( e.namespaceURI() == "http://www.w3.org/1998/Math/MathML" );
}


#include "KoFormulaShapeFactory.moc"
