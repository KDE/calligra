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

#include "KoFormulaShape.h"
#include "FormulaElement.h"
#include "FormulaRenderer.h"
#include <KoShapeSavingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoXmlWriter.h>
#include <kdebug.h>

KoFormulaShape::KoFormulaShape()
{
    m_formulaElement = new FormulaElement();
    m_formulaRenderer = new FormulaRenderer();
}

KoFormulaShape::~KoFormulaShape()
{
    delete m_formulaElement;
    delete m_formulaRenderer;
}

void KoFormulaShape::paint( QPainter &painter, const KoViewConverter &converter )
{
    painter.save();
    applyConversion( painter, converter );   // apply zooming and coordinate translation
    m_formulaRenderer->layoutElement( m_formulaElement );
    m_formulaRenderer->paintElement( painter, m_formulaElement );  // paint the formula
    painter.restore();
}

void KoFormulaShape::KoFormulaShape::updateLayout() {
    kDebug() << "before:" << KoShape::size()<<"," <<size(); 
    m_formulaRenderer->layoutElement( m_formulaElement );
     
     KoShape::setSize(m_formulaElement->boundingRect().size());
     kDebug() << "after:" << KoShape::size()<<"," <<size();
}


BasicElement* KoFormulaShape::elementAt( const QPointF& p )
{
    return m_formulaElement->childElementAt( p );
}

void KoFormulaShape::resize( const QSizeF& )
{ /* do nothing as FormulaShape is fixed size */ }

// QRectF KoFormulaShape::boundingRect() const
// {
//     return matrix().mapRect( m_formulaElement->boundingRect() );
// }

BasicElement* KoFormulaShape::formulaElement() const
{
    return m_formulaElement;
}

bool KoFormulaShape::loadOdf( const KoXmlElement& element, KoShapeLoadingContext &context )
{
    Q_UNUSED( context )

    delete m_formulaElement;                     // delete the old formula
    m_formulaElement = new FormulaElement();     // create a new root element
    m_formulaElement->readMathML( element );     // and load the new formula
    updateLayout();
    update();
    return true;
}

void KoFormulaShape::saveOdf( KoShapeSavingContext& context ) const
{
    if( m_formulaElement->childElements().isEmpty() )  // if the formula is empty
	return;                                        // do not save it

    bool odfFormat = true;
    if( context.xmlWriter().tagHierarchy().isEmpty() ) // hack to determine if saving is
        odfFormat = false;                             // for odf or not

    if( odfFormat )
        context.xmlWriter().startElement( "math:semantics" );
        // TODO add some namespace magic to avoid adding "math:" namespace everywhere
    else
        context.xmlWriter().startDocument( "math", "http://www.w3.org/1998/Math/MathML" );

    m_formulaElement->writeMathML( &context.xmlWriter() );

    if( odfFormat )
        context.xmlWriter().endElement();
    else
        context.xmlWriter().endDocument();       
}

