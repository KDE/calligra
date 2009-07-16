/* This file is part of the KDE project
   Copyright (C) Martin Pfeiffer <hubipete@gmx.net>

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
#include <kdebug.h>

KoFormulaShape::KoFormulaShape()
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
    kDebug() << "before:" << KoShape::size()<<"," <<size();
    m_formulaRenderer->layoutElement( m_formulaData->formulaElement() );

     KoShape::setSize(m_formulaData->formulaElement()->boundingRect().size());
     kDebug() << "after:" << KoShape::size()<<"," <<size();
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

bool KoFormulaShape::loadOdf( const KoXmlElement& element, KoShapeLoadingContext &context )
{
    Q_UNUSED( context )
    update();
    // delete the old formula
    FormulaElement* formulaElement = new FormulaElement();     // create a new root element
    formulaElement->readMathML( element );     // and load the new formula
    m_formulaData->setFormulaElement(formulaElement);
    m_formulaData->notifyDataChange(0,false);
    return true;
}

void KoFormulaShape::saveOdf( KoShapeSavingContext& context ) const
{
    if( m_formulaData->formulaElement()->childElements().isEmpty() )  // if the formula is empty
	return;                                        // do not save it

    bool odfFormat = true;
    if( context.xmlWriter().tagHierarchy().isEmpty() ) // hack to determine if saving is
        odfFormat = false;                             // for odf or not

    if( odfFormat )
        context.xmlWriter().startElement( "math:semantics" );
        // TODO add some namespace magic to avoid adding "math:" namespace everywhere
    else
        context.xmlWriter().startDocument( "math", "http://www.w3.org/1998/Math/MathML" );

    m_formulaData->formulaElement()->writeMathML( &context.xmlWriter() );

    if( odfFormat )
        context.xmlWriter().endElement();
    else
        context.xmlWriter().endDocument();       
}

