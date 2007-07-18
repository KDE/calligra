/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
		 2006 Martin Pfeiffer <hubipete@gmx.net>
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
   Boston, MA 02110-1301, USA.
*/

#include "FencedElement.h"
#include <QPainter>

FencedElement::FencedElement( BasicElement* parent ) : RowElement( parent )
{}

void FencedElement::paint( QPainter& painter, AttributeManager* am )
{
    painter.drawPath( m_buffer );
}

void FencedElement::layout( const AttributeManager* am )
{
/*    m_buffer = QPainterPath();
    QStringList separators = am->stringListOf( "separators" );
    m_buffer.addText( am->stringOf( "open" ) );

    foreach( BasicElement* tmp, childElements() )
        if( tmp != childElements().last() )
        {
            m_buffer.addText( separators[ ] );
        }

    m_buffer.addText( am->stringOf( "close" ) );

    setWidth( m_buffer.boundingRect().width() );
    setHeight( m_buffer.boundingRect().height() );*/
}
    
ElementType FencedElement::elementType() const
{
    return Fenced;
}
/*
bool FencedElement::operatorType( QDomNode& node, bool open )
{
    SymbolType* type = open ? &leftType : &rightType;
    if ( node.isElement() ) {
        QDomElement e = node.toElement();
        QString s =  e.text();
        if ( s.isNull() )
            return false;
        *type = static_cast<SymbolType>( QString::number( s.at( 0 ).toLatin1() ).toInt() );
        node = node.nextSibling();
    }
    else if ( node.isEntityReference() ) {
        QString name = node.nodeName();
        // TODO: To fully support these, SymbolType has to be extended,
        //       and better Unicode support is a must
        // CloseCurlyDoubleQuote 0x201D
        // CloseCurlyQoute       0x2019
        // LeftCeiling           0x2308
        // LeftDoubleFenced     0x301A
        // LeftFloor             0x230A
        // OpenCurlyDoubleQuote  0x201C
        // OpenCurlyQuote        0x2018
        // RightCeiling          0x2309
        // RightDoubleFenced    0x301B
        // RightFloor            0x230B
        if ( name == "LeftAngleFenced" ) {
            *type = LeftCornerFenced;
        }
        else if ( name == "RightAngleFenced" ) {
            *type = RightCornerFenced; 
        }
        else {
            if ( open ) {
                *type = LeftRoundFenced;
            }
            else
                *type = RightRoundFenced;
        }
        node = node.nextSibling();
    }
    else {
        return false;
    }
    return true;
}

int FencedElement::searchOperator( const QDomNode& node )
{
    QDomNode n = node;
    for ( int i = 0; ! n.isNull(); n = n.nextSibling(), i++ ) {
        if ( n.isElement() ) {
            QDomElement e = n.toElement();
            if ( e.tagName().toLower() == "mo" ) {
                // Try to guess looking at attributes
                QString form = e.attribute( "form" );
                QString f;
                if ( ! form.isNull() ) {
                    f = form.trimmed().toLower();
                }
                QString fence = e.attribute( "fence" );
                if ( ! fence.isNull() ) {
                    if ( fence.trimmed().toLower() == "false" ) {
                        continue;
                    }
                    if ( ! f.isNull() ) {
                        if ( f == "postfix" ) {
                            return i;
                        }
                        else {
                            continue;
                        }
                    }
                }
                
                // Guess looking at contents
                QDomNode child = e.firstChild();
                QString name;
                if ( child.isText() )
                    name = child.toText().data().trimmed();
                else if ( child.isEntityReference() )
                    name = child.nodeName();
                else 
                    continue;
                if ( name == ")"
                     || name == "]"
                     || name == "}"
                     || name == "CloseCurlyDoubleQuote"
                     || name == "CloseCurlyQuote"
                     || name == "RightAngleFenced"
                     || name == "RightCeiling"
                     || name == "RightDoubleFenced"
                     || name == "RightFloor" ) {
                    if ( f.isNull() || f == "postfix" )
                        return i;
                }
                if ( name == "("
                     || name == "["
                     || name == "{"
                     || name == "LeftAngleFenced"
                     || name == "LeftCeiling"
                     || name == "LeftDoubleFenced"
                     || name == "LeftFloor"
                     || name == "OpenCurlyQuote" ) {
                    if ( ! f.isNull() && f == "postfix" )
                        return i;
                }
            }
        }
    }
    return -1;
}*/

