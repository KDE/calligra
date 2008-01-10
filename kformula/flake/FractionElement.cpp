/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
		 2006 Martin Pfeiffer <hubipete@gmx.net>
 
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

#include "FractionElement.h"
#include "FormulaCursor.h"
#include "AttributeManager.h"
#include <KoXmlWriter.h>
#include <KoXmlReader.h>
#include <QPainter>
#include <kdebug.h>

FractionElement::FractionElement( BasicElement* parent ) : BasicElement( parent )
{
    m_numerator = new BasicElement( this );
    m_denominator = new BasicElement( this );
}

FractionElement::~FractionElement()
{
    delete m_numerator;
    delete m_denominator;
}

void FractionElement::paint( QPainter& painter, AttributeManager* am )
{
    QPen pen;
    double linethickness = am->doubleOf( "linethickness", this );
    if( linethickness == 0 )
	    return;  // specification says to not draw a line if thickness is 0

    pen.setWidthF( linethickness );
    painter.setPen( pen );                 // set the line width
    painter.drawLine( m_fractionLine );    // draw the line
}

void FractionElement::layout( const AttributeManager* am )
{
    if( am->boolOf( "bevelled", this ) )
    {
        layoutBevelledFraction( am );
        return;
    }

    QPointF numeratorOrigin;
    QPointF denominatorOrigin;
    double linethickness = am->doubleOf( "linethickness", this );
    double distY = am->layoutSpacing( this );
    Align numalign = am->alignOf( "numalign", this ); 
    Align denomalign = am->alignOf( "denomalign", this ); 
    denominatorOrigin.setY( m_numerator->height() + linethickness + 2*distY );
    
    setWidth( qMax( m_numerator->width(), m_denominator->width() ) + linethickness*2 );
    setHeight( m_numerator->height() + m_denominator->height() +
               linethickness + 2*distY );
    setBaseLine( denominatorOrigin.y() );
 
    if( numalign == Right )  // for Left it is (0.0 /0.0)
        numeratorOrigin.setX( width() - m_numerator->width() - linethickness);
    else
	numeratorOrigin.setX( ( width() - m_numerator->width() ) / 2 );

    if( denomalign == Right )
        denominatorOrigin.setX( width() - m_denominator->width() - linethickness);
    else
	denominatorOrigin.setX( ( width() - m_denominator->width() ) / 2);

    m_numerator->setOrigin( numeratorOrigin );
    m_denominator->setOrigin( denominatorOrigin );
    if(linethickness == 0) 
	    return;  //Specification says to not draw a line if thickness is 0
    float fractionLineY =  m_numerator->height() + linethickness/2 + distY;
    m_fractionLine = QLineF( QPointF( linethickness, fractionLineY ),
                             QPointF( width()-linethickness, fractionLineY ) );
}

void FractionElement::layoutBevelledFraction( const AttributeManager* am )
{
    // the shown line should have a width that has 1/3 of the height
    // the line is heigher as the content by 2*thinmathspace = 2*borderY

    double borderY = am->layoutSpacing( this );
    setHeight( m_numerator->height() + m_denominator->height() + 2*borderY );
    setWidth( m_numerator->width() + m_denominator->width() + height()/3 );
    setBaseLine( height()/2 );

    m_numerator->setOrigin( QPointF( 0.0, borderY ) );
    m_denominator->setOrigin( QPointF( width()-m_denominator->width(),
                                       borderY+m_numerator->height() ) );
    m_fractionLine = QLineF( QPointF( m_numerator->width(), height() ),
                             QPointF( width()-m_denominator->width(), 0.0 ) );
}

const QList<BasicElement*> FractionElement::childElements()
{
    QList<BasicElement*> list;
    list << m_denominator << m_numerator;
    return list;
}

BasicElement* FractionElement::acceptCursor( const FormulaCursor* cursor )
{
/*    switch( cursor->direction() ) {
        case RightToChild:
            return m_numerator;
        case LeftToChild:
            return m_denominator;
        case UpToParent:
            if( cursor->currentElement() == m_denominator )
                return m_numerator;
        case DownToParent:
            if( cursor->currentElement() == m_numerator )
                return m_denominator;
        default:
            return 0;
    }*/
}

void FractionElement::insertChild( FormulaCursor* cursor, BasicElement* child )
{
    BasicElement* tmp = cursor->currentElement();
    if( tmp == m_numerator && m_numerator->elementType() == Basic )
        m_numerator = child;
    else if( tmp == m_denominator && m_denominator->elementType() == Basic )
        m_denominator = child;

    delete tmp;       // finally delete the old BasicElement
}
   
void FractionElement::removeChild( BasicElement* element )
{
    if( element == m_numerator )         
    {
        delete m_numerator;                      // delete the numerator and
        m_numerator = new BasicElement( this );  // assign a new empty BasicElement   
    }
    else if( element == m_denominator )
    {
        delete m_denominator;
        m_denominator = new BasicElement( this );
    }
}

QString FractionElement::attributesDefaultValue( const QString& attribute ) const
{
    if( attribute == "linethickness" )
        return "1";
    else if( attribute == "numalign" || attribute == "denomalign" )
        return "center";
    else if( attribute == "bevelled" )
        return "false";
    else
        return QString();
}

bool FractionElement::readMathMLContent( const KoXmlElement& parent )
{
    KoXmlElement tmp;
    forEachElement( tmp, parent ) {
        if( m_numerator->elementType() == Basic ) {
            delete m_numerator;
            m_numerator = ElementFactory::createElement( tmp.tagName(), this );
            if( !m_numerator->readMathML( tmp ) )
                return false;
        } else if( m_denominator->elementType() == Basic ) {
            delete m_denominator;
            m_denominator = ElementFactory::createElement( tmp.tagName(), this );
            if( !m_denominator->readMathML( tmp ) )
                return false;
        } else {
            kDebug() << "Too many arguments to mfrac" << endl;
	}
    }
    Q_ASSERT( m_numerator );
    Q_ASSERT( m_denominator );

    return true;
}

void FractionElement::writeMathMLContent( KoXmlWriter* writer ) const
{
    m_numerator->writeMathML( writer );
    m_denominator->writeMathML( writer );
}

ElementType FractionElement::elementType() const
{
    return Fraction;
}
