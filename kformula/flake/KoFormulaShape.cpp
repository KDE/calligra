/* This file is part of the KDE project
   Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net>
                 2009 Jeremias Epperlein <jeeree@web.de>

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

#include "FormulaData.h"
#include "KoFormulaShape.h"
#include "FormulaElement.h"
#include "FormulaRenderer.h"
#include <KoShapeSavingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoXmlWriter.h>
#include <KoXmlReader.h>
#include <KoXmlNS.h>
#include <kdebug.h>


KoFormulaShape::KoFormulaShape()
: KoFrameShape( KoXmlNS::draw, "object" )
{
    FormulaElement* element= new FormulaElement();
    m_formulaData = new FormulaData(element);
    m_formulaRenderer = new FormulaRenderer();
}

KoFormulaShape::~KoFormulaShape()
{
    delete m_formulaData;
    delete m_formulaRenderer;
}

void KoFormulaShape::paint( QPainter &painter, const KoViewConverter &converter )
{
    painter.save();
    applyConversion( painter, converter );   // apply zooming and coordinate translation
    m_formulaRenderer->layoutElement(  m_formulaData->formulaElement() );
    m_formulaRenderer->paintElement( painter,  m_formulaData->formulaElement() );  // paint the formula
    painter.restore();
}

void KoFormulaShape::updateLayout() {
    m_formulaRenderer->layoutElement( m_formulaData->formulaElement() );

     KoShape::setSize(m_formulaData->formulaElement()->boundingRect().size());
}


BasicElement* KoFormulaShape::elementAt( const QPointF& p )
{
    return m_formulaData->formulaElement()->childElementAt( p );
}

void KoFormulaShape::resize( const QSizeF& )
{ /* do nothing as FormulaShape is fixed size */ }

FormulaData* KoFormulaShape::formulaData() const
{
    return  m_formulaData;
}

FormulaRenderer* KoFormulaShape::formulaRenderer() const
{
    return m_formulaRenderer;
}

bool KoFormulaShape::loadOdf( const KoXmlElement& element, KoShapeLoadingContext &context )
{
    kDebug() <<"Loading ODF in Formula";
    loadOdfAttributes(element, context, OdfAllAttributes);
    return loadOdfFrame(element, context);
}

bool KoFormulaShape::loadOdfFrameElement( const KoXmlElement & element, KoShapeLoadingContext &/*context*/ )
{
    const KoXmlElement &topLevelElement = KoXml::namedItemNS(element, "http://www.w3.org/1998/Math/MathML", "math");
    // This is only true when loading as embedded in the main doc
    if (topLevelElement.isNull()) {
        kWarning() << "no math element as first child";
        return false;
    }
    delete m_formulaData->formulaElement();
    FormulaElement* formulaElement = new FormulaElement();     // create a new root element
    formulaElement->readMathML( topLevelElement );     // and load the new formula
    m_formulaData->setFormulaElement(formulaElement);
    m_formulaData->notifyDataChange(0,false);
    return true;
}

void KoFormulaShape::saveOdf( KoShapeSavingContext& context ) const
{
    kDebug() <<"Saving ODF in Formula";
    KoXmlWriter& writer = context.xmlWriter();
    writer.startElement("draw:frame");
    saveOdfAttributes(context, OdfAllAttributes);
    writer.startElement( "draw:object" );
    // TODO add some namespace magic to avoid adding "math:" namespace everywhere
    formulaData()->formulaElement()->writeMathML( &context.xmlWriter() );
    writer.endElement(); // draw:object
    writer.endElement(); // draw:frame
}

