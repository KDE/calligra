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
    m_lineThickness = 1.0;
}

FractionElement::~FractionElement()
{
    delete m_numerator;
    delete m_denominator;
}

void FractionElement::paint( QPainter& painter, AttributeManager* am )
{
    Q_UNUSED( am )
    // return if there is nothing to paint
    if( m_lineThickness == 0.0 )
        return;

    // paint the fraction line with the specified line width
    QPen pen;
    pen.setWidthF( m_lineThickness );
    painter.setPen( pen );
    painter.drawLine( m_fractionLine );
}

void FractionElement::layout( const AttributeManager* am )
{
    // get values of all attributes
    QString value = am->findValue( "linethickness", this );
    Length length;
    if(value == "thick")
        length.value = 2;
    else if(value == "medium")
        length.value = 1;
    else if(value == "thin")
        length.value = 0.5;
    else
        length = am->parseUnit( value, this );

    if(length.unit == Length::None)
        m_lineThickness = am->lineThickness(this) * length.value;
    else 
        m_lineThickness = am->lengthToPixels(length, this, "linethickness");

    // decide which layout is wanted
    if( am->boolOf( "bevelled", this ) )
    {
        layoutBevelledFraction( am );
        return;
    }

    double distY = am->layoutSpacing( this );
    Align numalign = am->alignOf( "numalign", this ); 
    Align denomalign = am->alignOf( "denomalign", this );

    // align the numerator and the denominator
    QPointF numeratorOrigin;
    QPointF denominatorOrigin( 0.0, m_numerator->height() + m_lineThickness + 2*distY );
    setWidth( qMax( m_numerator->width(), m_denominator->width() ) + m_lineThickness*2 );
    
    if( numalign == Right )
        numeratorOrigin.setX( width() - m_numerator->width() - m_lineThickness );
    else if( numalign == Center )
	numeratorOrigin.setX( ( width() - m_numerator->width() ) / 2 );

    if( denomalign == Right )
        denominatorOrigin.setX( width() - m_denominator->width() - m_lineThickness );
    else if( numalign == Center )
	denominatorOrigin.setX( ( width() - m_denominator->width() ) / 2 );

    m_numerator->setOrigin( numeratorOrigin );
    m_denominator->setOrigin( denominatorOrigin );

    // construct the fraction's line    
    double fractionLineY =  m_numerator->height() + m_lineThickness/2 + distY;
    m_fractionLine = QLineF( QPointF( m_lineThickness, fractionLineY ),
                             QPointF( width()-m_lineThickness, fractionLineY ) );

    setHeight( m_numerator->height() + m_denominator->height() +
               m_lineThickness + 2*distY );
    setBaseLine( denominatorOrigin.y() ); 
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
    return this;
}

int FractionElement::length() const {
    return 3;
}

QLineF FractionElement::cursorLine(const FormulaCursor* cursor) {
    QPointF top=absoluteBoundingRect().topLeft();
    QPointF bottom;
    switch (cursor->position()) {
	case 0:
	    top+=m_numerator->origin();
	    break;
	case 1:
	    top+=m_numerator->origin()+QPointF(m_numerator->width(),0.0);
	    break;
	case 2:
	    top+=m_denominator->origin();
	    break;
	case 3:
	    top+=m_denominator->origin()+QPointF(m_denominator->width(),0.0);
	    break;
    }
    if (cursor->position()<=1) {
	bottom=top+QPointF(0.0,m_numerator->height());
    }
    else {
	bottom=top+QPointF(0.0,m_denominator->height());
    }
    return QLineF(top, bottom);
}

int FractionElement::positionOfChild(BasicElement* child) const {
    if (m_numerator==child){
	return 0;
    }
    else if (m_denominator==child) {
	return 2;
    }
    return -1;
}

bool FractionElement::moveCursor(FormulaCursor* cursor) {
    if ( ((cursor->position()==0 || cursor->position()==1) && cursor->direction()==MoveUp) ||
	 ((cursor->position()==2 || cursor->position()==3) && cursor->direction()==MoveDown)) {
	return false;
    }
    if ( ((cursor->position()==0 || cursor->position()==2) && cursor->direction()==MoveLeft) ||
	 ((cursor->position()==1 || cursor->position()==3) && cursor->direction()==MoveRight) ) {
	return BasicElement::moveCursor(cursor);
    }
    else {
	switch (cursor->direction()) {
	    case MoveLeft:
		cursor->setCurrentElement(cursor->position()==1 ? m_numerator : m_denominator );
		cursor->moveEnd();
		break;
	    case MoveRight:
		cursor->setCurrentElement(cursor->position()==0 ? m_numerator : m_denominator );
		cursor->moveHome();
		break;
	    case MoveUp:
		cursor->setCurrentElement(m_numerator );
		cursor->moveHome();
		break;
	    case MoveDown:
		cursor->setCurrentElement(m_denominator );
		cursor->moveHome();
		break;
	}
	return true;
    }
}

bool FractionElement::setCursorTo( FormulaCursor* cursor, QPointF point ) 
{
    kDebug()<<"Punkt: "<<point;
    //check if the point is above the fration line, the origin is in the top left corner
    bool inNumerator=point.y() < (m_numerator->boundingRect().bottom() +  m_denominator->boundingRect().top())/2 ;
    if (point.x() > width()) {
	if ( inNumerator ) {
	    cursor->setPosition(1);
	    cursor->setCurrentElement(this);
	    return true;
	} else {
	    cursor->setPosition(3);
	    cursor->setCurrentElement(this);
	    return true;
	}
    }
    if (point.x() < 0) {
	if ( inNumerator) {
	    cursor->setPosition(0);
	    cursor->setCurrentElement(this);
	    return true;
	} else {
	    cursor->setPosition(2);
    	    cursor->setCurrentElement(this);
	    return true;
	}
    }
    if ( inNumerator ) {
	point-=m_numerator->origin();
	return m_numerator->setCursorTo(cursor,point);
    } else {
	point-=m_denominator->origin();
	return m_denominator->setCursorTo(cursor,point);
    }
}


void FractionElement::insertChild( FormulaCursor* cursor, BasicElement* child )
{
    if( cursor->currentElement() == m_numerator )
        m_numerator = child;
    else if( cursor->currentElement() == m_denominator )
        m_denominator = child;
//    else
        // TODO make some error
}
   
void FractionElement::removeChild( FormulaCursor* cursor, BasicElement* element )
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
            kDebug(39001) << "Too many arguments to mfrac" << endl;
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
