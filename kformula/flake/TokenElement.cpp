/* This file is part of the KDE project
   Copyright (C) 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

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
 * Boston, MA 02110-1301, USA.
*/

#include <algorithm>

#include <qpainter.h>

#include <klocale.h>

#include "Entities.h"
#include "TextElement.h"
#include "GlyphElement.h"
#include "TokenElement.h"
#include "FormulaElement.h"
#include "ElementFactory.h"
#include "AttributeManager.h"

KFORMULA_NAMESPACE_BEGIN

TokenElement::TokenElement( BasicElement* parent ) : TokenStyleElement( parent ),
                                                     m_textOnly( true )
{
}

QFont TokenElement::font( const AttributeManager* am )
{
    QFont font;
    QVariant mathVariant = am->valueOf( "mathvariant" );
    if ( mathVariant.canConvert( QVariant::Int ) ) {
        MathVariant variant = static_cast<MathVariant>(mathVariant.toInt());
        switch ( variant ) {
        case Normal:
            font.setItalic( false );
            font.setBold( false );
            break;
        case Bold:
            font.setItalic( false );
            font.setBold( true );
            break;
        case Italic:
            font.setItalic( true );
            font.setBold( false );
            break;
        case BoldItalic:
            font.setItalic( true );
            font.setBold( true );
            break;
        }
    }
    else {
        QVariant fontFamily = am->valueOf( "fontfamily" );
        if ( fontFamily.canConvert( QVariant::String ) ) {
            QString family = fontFamily.toString();
            font = QFont( family );
        }
    }

    /*
    if ( style.customFont() ) {
        font = style.font();
    }
    else {
        font = context.getDefaultFont();
    }
    */
//    return fontStyle.symbolTable()->font( character(), font );
    return font;
}

void TokenElement::paint( QPainter& painter, const AttributeManager* am)
{
    // Color handling.
    // TODO: most of this should be handled by AttributeManager
    QVariant color = am->valueOf( "mathcolor" );
    if ( color.isNull() )
        color = am->valueOf( "color" );
    if ( color.canConvert( QVariant::Color ) )
        painter.setPen( color.value<QColor>() );
    else
        painter.setPen( QColor( "black" ) );

    // TODO: Default fonts should be read from settings
    // Font style handling
    painter.setFont( font( am ) );
    inherited::paint( painter, am );
}

bool TokenElement::readMathMLContent( const KoXmlElement& parent )
{
    QDomNode tmp = parent.firstChild();
    while( !tmp.isNull() ) {   // for each child element, create a element
        if ( tmp.isText() ) {
            QString textelements = tmp.toText().data();
            textelements = textelements.trimmed();

            for (uint i = 0; i < textelements.length(); i++) {
                TextElement* child = new TextElement(textelements[i]);
                child->setParentElement( this );
                appendChild( child );
            }
        }
        else if ( tmp.isEntityReference() ) {
            QString entity = tmp.toEntityReference().nodeName();
            const entityMap* begin = entities;
            const entityMap* end = entities + entityMap::size();
            const entityMap* pos = std::lower_bound( begin, end, entity.toAscii() );
            if ( pos == end || QString( pos->name ) != entity ) {
                kWarning() << "Invalid entity refererence: " << entity << endl;
            }
            else {
                TextElement* child = new TextElement( QChar( pos->unicode ) );
                child->setParentElement(this);
                appendChild( child );
            }
        }
        else if ( tmp.isElement() ) {
            m_textOnly = false;
            // Only mglyph element is allowed
            QDomElement e = tmp.toElement();
            if ( e.tagName().toLower() != "mglyph" ) {
                kWarning( DEBUGID ) << "Invalid element inside Token Element\n";
                return -1;
            }
            GlyphElement* child = new GlyphElement();
            child->setParentElement(this);
            /*
            if ( child->buildFromMathMLDom( e ) == -1 ) {
                return -1;
            }
            */
            appendChild( child );
        }
        else {
            kWarning() << "Invalid content in TokenElement\n";
        }
        tmp = tmp.nextSibling();
    }
//	parse();
	kWarning() << "Num of children " << childElements().count() << endl;
    return true;
}

luPt TokenElement::getSpaceBefore( const ContextStyle& context,
                                   ContextStyle::TextStyle tstyle,
                                   double factor )
{
    if ( !context.isScript( tstyle ) ) {
        return context.getMediumSpace( tstyle, factor );
    }
    return 0;
}

luPt TokenElement::getSpaceAfter( const ContextStyle& context,
                                  ContextStyle::TextStyle tstyle,
                                  double factor )
{
    if ( !context.isScript( tstyle ) ) {
        return context.getThinSpace( tstyle, factor );
    }
    return 0;
}

KFORMULA_NAMESPACE_END
