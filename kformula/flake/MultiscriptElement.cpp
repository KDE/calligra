/* This file is part of the KDE project
   Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net>

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

#include "MultiscriptElement.h"
#include "AttributeManager.h"
#include <KoXmlWriter.h>
#include <KoXmlReader.h>
#include <QPainter>

MultiscriptElement::MultiscriptElement( BasicElement* parent ) : BasicElement( parent )
{
    m_baseElement = new BasicElement( this );
}

MultiscriptElement::~MultiscriptElement()
{
    delete m_baseElement;
    //Delete all of the scripts
    while (!m_preScripts.isEmpty())
        delete m_preScripts.takeFirst();
    while (!m_postScripts.isEmpty())
        delete m_postScripts.takeFirst();
}

void MultiscriptElement::paint( QPainter& painter, AttributeManager* am )
{ 
    Q_UNUSED(painter)
    Q_UNUSED(am)
    /*do nothing as this element has no visual representation*/
}

void MultiscriptElement::ensureEvenNumberElements() {
    if(m_postScripts.size() % 2 == 1) {
        //Odd number - at a None element on the end
        m_postScripts.append(NULL);
    }
    if(m_preScripts.size() % 2 == 1) {
        //Odd number - at a None element on the end
        m_preScripts.append(NULL);
    }
}

void MultiscriptElement::layout( const AttributeManager* am )
{
    // Get the minimum amount of shifting
    double subscriptshift   = am->doubleOf( "subscriptshift", this ); 
    double superscriptshift = am->doubleOf( "superscriptshift", this );
    //Add half a thin space between both sup and superscript, so there is a minimum
    //of a whole thin space between them.
    double halfthinspace   = am->layoutSpacing( this )/2.0;

    //First make sure that there are an even number of elements in both subscript and superscript
    ensureEvenNumberElements();
    
    // Go through all the superscripts (pre and post) and find the maximum heights;
    // BaseLine is the distance from the top to the baseline.
    // Depth is the distance from the baseline to the bottom
    double maxSuperScriptDepth     = 0.0;
    double maxSuperScriptBaseLine  = 0.0;
    double maxSubScriptDepth       = 0.0;
    double maxSubScriptBaseLine    = 0.0;
    bool isSuperscript = true;  //Toggle after each element time
    foreach( BasicElement *script, m_postScripts ) {
        isSuperscript = !isSuperscript;  //Toggle each time
        if(!script)
            continue;  // Null means no element - just a blank
        if(isSuperscript) {
            maxSuperScriptDepth = qMax( script->height() - script->baseLine(), maxSuperScriptDepth );
            maxSuperScriptBaseLine = qMax( script->baseLine(), maxSuperScriptBaseLine );
        } else {
            //Find out how much the subscript sticks below the baseline
            maxSubScriptDepth = qMax( script->height() - script->baseLine(), maxSubScriptDepth );
            maxSubScriptBaseLine = qMax( script->baseLine(), maxSubScriptBaseLine );
        }
    }
    foreach( BasicElement *script, m_preScripts ) {
        isSuperscript = !isSuperscript;  //Toggle each time
        if(!script)
            continue;  // Null means no element - just a blank
        if(isSuperscript) {
            maxSuperScriptDepth = qMax( script->height() - script->baseLine(), maxSuperScriptDepth );
            maxSuperScriptBaseLine = qMax( script->baseLine(), maxSuperScriptBaseLine );
        } else {
            //Find out how much the subscript sticks below the baseline
            maxSubScriptDepth = qMax( script->height() - script->baseLine(), maxSubScriptDepth );
            maxSubScriptBaseLine = qMax( script->baseLine(), maxSubScriptBaseLine );
        }
    }
    // The yOffsetBase is the amount the base element is moved down to make
    // room for the superscript
    double yOffsetBase = 0;
    if(maxSuperScriptDepth + maxSuperScriptBaseLine > 0) {
        yOffsetBase = maxSuperScriptDepth + maxSuperScriptBaseLine - m_baseElement->height()/2.0 + halfthinspace;
        yOffsetBase = qMax( yOffsetBase, superscriptshift );
    }
    // The yOffsetSub is the amount the subscript elements /baseline/ are moved down.
    double yOffsetSub = yOffsetBase + maxSubScriptBaseLine +
                qMax( m_baseElement->height()/2 + halfthinspace, 
                      m_baseElement->height() - maxSubScriptBaseLine
                          + subscriptshift );
    
    double xOffset = 0.0;  //We increment this as we go along, to keep track of where to place elements
    double lastSuperScriptWidth= 0.0;
    // Now we have all the information needed to start putting elements in place.
    // We start from the far left, and work to the far right.
    for( int i = m_preScripts.size()-1; i >= 0; i--) {
        //We start from the end, and work in.
        //m_preScripts[0] is subscript etc.  So even i is subscript, odd i is superscript
        if( i%2 == 0) {
            // i is even, so subscript
            if(!m_preScripts[i]) {
                xOffset += lastSuperScriptWidth;
            } else {
                // For a given vertical line, this is processed after the superscript
                double offset = qMax(0.0, (lastSuperScriptWidth - m_preScripts[i]->width())/2.0);
                m_preScripts[i]->setOrigin( QPointF( 
                            offset + xOffset, 
                            yOffsetSub - m_preScripts[i]->baseLine() ) );
                xOffset += qMax(lastSuperScriptWidth, m_preScripts[i]->width());
            }
            if(i!=0)  //No halfthinspace between the first element and the base element
                xOffset += halfthinspace;
        } else {
            // i is odd, so superscript
            // For a given vertical line, we process the superscript first, then 
            // the subscript.  We need to look at the subscript (i-1) as well
            // to find out how to align them
            if( !m_preScripts[i] )
                lastSuperScriptWidth = 0.0;
            else {
                lastSuperScriptWidth = m_preScripts[i]->width();
                double offset = 0.0;
                if(m_preScripts[i-1]) //the subscript directly below us. 
                    offset = qMax(0.0, (m_preScripts[i-1]->width() - lastSuperScriptWidth)/2.0);
                m_preScripts[i]->setOrigin( QPointF(
                            offset + xOffset,
                            maxSuperScriptBaseLine - m_preScripts[i]->baseLine()));
            }
        }
    }

    //We have placed all the prescripts now.  So now place the base element
    m_baseElement->setOrigin( QPointF( xOffset, yOffsetBase ) );
    xOffset += m_baseElement->width();
    double lastSubScriptWidth = 0.0;
    //Now we can draw the post scripts.  This code is very similar, but this time we will parse
    //the subscript before the superscript
    for( int i = 0; i < m_postScripts.size(); i++) {
        //We start from the start, and work out.
        //m_preScripts[0] is subscript etc.  So even i is subscript, odd i is superscript
        if( i%2 == 0) {
            // i is even, so subscript
            // For a given vertical line, we process the subscript first, then 
            // the superscript.  We need to look at the superscript (i+1) as well
            // to find out how to align them
 
            if(!m_postScripts[i]) {
                lastSubScriptWidth = 0.0;
            } else {
                lastSubScriptWidth = m_postScripts[i]->width();
                // For a given vertical line, this is processed after the superscript
                double offset = 0.0;
                if(m_postScripts.size() > i+1 && m_postScripts[i+1] != NULL) //the subscript directly below us. 
                    offset = qMax(0.0, (m_postScripts[i+1]->width() - lastSubScriptWidth)/2.0);
                m_postScripts[i]->setOrigin( QPointF( 
                            offset + xOffset,
                            yOffsetSub - m_postScripts[i]->baseLine() ) );
            }
        } else {
            // i is odd, so superscript
           if( !m_postScripts[i] )
                xOffset += lastSubScriptWidth;
           else {
                double offset = qMax(0.0, (lastSubScriptWidth - m_postScripts[i]->width())/2.0);
                m_postScripts[i]->setOrigin( QPointF(
                            offset + xOffset,
                            maxSuperScriptBaseLine - m_postScripts[i]->baseLine()));
                xOffset += qMax(lastSubScriptWidth, m_postScripts[i]->width());
            }
           if(i != m_postScripts.size()-1)
               xOffset += halfthinspace; //Don't add an unneeded space at the very end
        }
    }


    //Finally, set our boundingbox
    setWidth( xOffset );
    setHeight( yOffsetSub + maxSubScriptDepth );
    setBaseLine( yOffsetBase + m_baseElement->baseLine() );
}

bool MultiscriptElement::acceptCursor( const FormulaCursor* cursor )
{
    return false;
}

const QList<BasicElement*> MultiscriptElement::childElements()
{
    QList<BasicElement*> list;
    list << m_baseElement;
    
    foreach( BasicElement* tmp, m_postScripts ) {
        if(tmp)
            list << tmp;
    }

    foreach( BasicElement* tmp, m_preScripts ) {
        if(tmp)
            list << tmp;
    }
    
    return list;
}

QString MultiscriptElement::attributesDefaultValue( const QString& attribute ) const
{
    return QString();
}

ElementType MultiscriptElement::elementType() const
{
    return MultiScript;
}

bool MultiscriptElement::readMathMLContent( const KoXmlElement& parent )
{
    QString name = parent.tagName().toLower();
    BasicElement* tmpElement = 0;
    KoXmlElement tmp;
    bool prescript = false; //When we see a mprescripts tag, we enable this
    forEachElement( tmp, parent ) { 
        if(tmp.tagName() == "none") {
            //In mathml, we read subscript, then superscript, etc.  To skip one,
            //you use "none"
            //To represent "none" we use a NULL pointer
            if(prescript)
                m_preScripts.append(NULL);
            else
                m_postScripts.append(NULL);
            continue;
        } else if(tmp.tagName() == "mprescripts") {
            prescript = true;  
            //In mathml, when we see this tag, all the elements after it are
            // for prescripts
            continue;
        }
        
        tmpElement = ElementFactory::createElement( tmp.tagName(), this );
        if( !tmpElement->readMathML( tmp ) )
            return false;
        if( m_baseElement->elementType() == Basic ) {  //Very first element is the base
            delete m_baseElement; 
            m_baseElement = tmpElement;
        }
        else if( prescript)
            m_preScripts.append( tmpElement );
        else 
            m_postScripts.append( tmpElement );
    }
    ensureEvenNumberElements();
    Q_ASSERT(m_baseElement);  //We should have at least a BasicElement for the base
    return true;
}

void MultiscriptElement::writeMathMLContent( KoXmlWriter* writer ) const
{
    m_baseElement->writeMathML( writer );        // Just save the children in
                                                 // the right order
    foreach( BasicElement* tmp, m_postScripts ) {
        if(tmp)
            tmp->writeMathML( writer );
        else {
            //We need to use a none element for missing elements in the super/sub scripts
            writer->startElement("none");
            writer->endElement();
        }
    }
    if( m_preScripts.isEmpty() ) return;
    writer->startElement("mprescripts");
    writer->endElement();
    foreach( BasicElement* tmp, m_preScripts ) {
        if(tmp)
            tmp->writeMathML( writer );
        else {
            //We need to use a none element for missing elements in the super/sub scripts
            writer->startElement("none");
            writer->endElement();
        }
    }
}

