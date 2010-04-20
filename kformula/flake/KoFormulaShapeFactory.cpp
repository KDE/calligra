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

#include <klocale.h>
#include <kdebug.h>

#include <KoShapeFactoryBase.h>
#include <KoXmlNS.h>

#include "KoFormulaShape.h"


KoFormulaShapeFactory::KoFormulaShapeFactory( QObject *parent )
    : KoShapeFactoryBase( parent, KoFormulaShapeId, i18n( "Formula" ) )
{
    setToolTip(i18n( "A formula"));
    setIcon( "x-shape-formula" );

#if 0
    QStringList elementNames;
    elementNames << "math";
    elementNames << "object";
    // FIXME: How can I set this so that it supports both inline
    //        formulas and embedded ones in an embedded object?
    //setOdfElementNames("http://www.w3.org/1998/Math/MathML", elementNames);
#endif

    // The following two alternatives let the formula shape load
    // either embedded formulas or inline formulas.  Inline formulas
    // is what KOffice 2.x has supported since 2.0.  Formulas in an
    // embedded document is what OOo supports, and what I *think* is
    // the only supported mode in ODF.
    //
    // The line below tells the shape registry which XML elements that
    // the shape supports.  As far as I understand right now there is
    // only one element that can be supported here.  If I am right,
    // this needs to be fixed.  Note that the loading code in the
    // formula shape can load either way.
    //
    // FIXME: Find out if inline formulas are supported by ODF and if
    //        there is a way to tell the shape registry that this
    //        shape supports both.
#if 1
    // Formulas in an embedded document.
    setOdfElementNames(KoXmlNS::draw, QStringList("object"));
#else
    // Inline formulas
    setOdfElementNames( "http://www.w3.org/1998/Math/MathML", QStringList("math"));
#endif

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

KoShape *KoFormulaShapeFactory::createDefaultShape(KoResourceManager *resourceManager) const
{
    KoFormulaShape* formula = new KoFormulaShape(resourceManager);
    formula->setShapeId( KoFormulaShapeId );
    return formula;
}

bool KoFormulaShapeFactory::supports(const KoXmlElement& e) const
{
    bool retval = ((e.nodeName() == "math"
                    && e.namespaceURI() == "http://www.w3.org/1998/Math/MathML")
                   || (e.tagName() == "object"
                       && e.namespaceURI() == "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0"));

    // Should be 39001 (kformula) instead of 31000 (koffice)
    //kDebug(31000) << e.nodeName() << " - "<< e.namespaceURI(); 
    //kDebug(31000) << "Return value = " << retval;

    return retval;
}


#include "KoFormulaShapeFactory.moc"
