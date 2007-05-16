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
#include "KFormulaPartDocument.h"
#include <KoXmlWriter.h>

KoFormulaShape::KoFormulaShape() :  m_document( 0 ), m_formulaElement( 0 )
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
    applyConversion( painter, converter );   // apply zooming and coordinate translation
    m_formulaRenderer->paintElement( painter, m_formulaElement );  // paint the formula
}

BasicElement* KoFormulaShape::elementAt( const QPointF& p )
{
    return m_formulaElement->childElementAt( p );
}

QSizeF KoFormulaShape::size() const
{
    return m_formulaElement->boundingRect().size();
}

void KoFormulaShape::resize( const QSizeF& )
{ /* do nothing as FormulaShape is fixed size */ }

QRectF KoFormulaShape::boundingRect() const
{
    return matrix().mapRect( m_formulaElement->boundingRect() );
}

BasicElement* KoFormulaShape::formulaElement() const
{
    return m_formulaElement;
}

void KoFormulaShape::loadMathML( const QDomDocument &doc, bool )
{
    delete m_formulaElement;                                // delete the old formula
    m_formulaElement = new FormulaElement();                // create a new root element
    m_formulaElement->readMathML( doc.documentElement() );  // and load the new formula
}

void KoFormulaShape::saveMathML( KoXmlWriter* writer, bool oasisFormat )
{
    if( m_formulaElement->childElements().isEmpty() )  // if the formula is empty
	return;                                        // do not save it

/*
    if( oasisFormat )
    {
        writer->startElement( "math:semantics" )
    } // TODO write the correct namespace that is inherited to all children
    else
        writer->startDocument( "math", "http://www.w3.org/1998/Math/MathML" );

	inherited::writeMathMLContent( writer, oasisFormat);

    m_formulaElement->writeMathML( writer );

    if( oasisFormat )
        writer->endElement();
    else
        writer->endDocument();*/
}

void KoFormulaShape::importFormula( const KUrl& url )
{
    delete m_document;
	delete m_formulaElement;
    m_document = new KFormulaPartDocument();
    m_document->openURL(url);
	m_formulaElement = m_document->formulaElement();
	m_formulaRenderer->layoutElement( m_formulaElement );
}

void KoFormulaShape::saveOdf( KoShapeSavingContext * context ) const {
    // TODO
}

bool KoFormulaShape::loadOdf( const KoXmlElement & element, KoShapeLoadingContext &context ) {
    return false; // TODO
}

