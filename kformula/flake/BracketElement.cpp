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

#include "BracketElement.h"
#include <KoXmlWriter.h>

#include <QPainter>
#include <QPen>

#include <kdebug.h>
#include <klocale.h>

#include "fontstyle.h"
#include "FormulaCursor.h"
#include "FormulaElement.h"
#include "RowElement.h"

BracketElement::BracketElement( BasicElement* parent ) : BasicElement( parent ),
                                                         left( 0 ),
                                                         right( 0 ),
                                                         leftType( EmptyBracket ),
                                                         rightType( EmptyBracket ),
                                                         m_operator( false ),
                                                         m_customLeft( false ),
                                                         m_customRight( false )
{
}


BracketElement::~BracketElement()
{
    delete left;
    delete right;
}


const QList<BasicElement*> BracketElement::childElements()
{
    return QList<BasicElement*>();
}

void BracketElement::paint( QPainter& painter, const AttributeManager* am )
{
}

void BracketElement::layout( const AttributeManager* am )
{
}
    
ElementType BracketElement::elementType() const
{
    if ( m_operator ) {
        return Operator;
    }
    else {
        return Fenced;
    }
}

bool BracketElement::readMathMLContent( const KoXmlElement& element )
{
    if ( element.tagName() == "mo" ) {
        m_operator = true;
        // TODO
    }
    else {
        m_operator = false;
        // TODO
    }
    return false;
}
    
void BracketElement::writeMathMLContent( KoXmlWriter* writer) const
{
}

bool BracketElement::operatorType( QDomNode& node, bool open )
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
        // LeftDoubleBracket     0x301A
        // LeftFloor             0x230A
        // OpenCurlyDoubleQuote  0x201C
        // OpenCurlyQuote        0x2018
        // RightCeiling          0x2309
        // RightDoubleBracket    0x301B
        // RightFloor            0x230B
        if ( name == "LeftAngleBracket" ) {
            *type = LeftCornerBracket;
        }
        else if ( name == "RightAngleBracket" ) {
            *type = RightCornerBracket; 
        }
        else {
            if ( open ) {
                *type = LeftRoundBracket;
            }
            else
                *type = RightRoundBracket;
        }
        node = node.nextSibling();
    }
    else {
        return false;
    }
    return true;
}

int BracketElement::searchOperator( const QDomNode& node )
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
                     || name == "RightAngleBracket"
                     || name == "RightCeiling"
                     || name == "RightDoubleBracket"
                     || name == "RightFloor" ) {
                    if ( f.isNull() || f == "postfix" )
                        return i;
                }
                if ( name == "("
                     || name == "["
                     || name == "{"
                     || name == "LeftAngleBracket"
                     || name == "LeftCeiling"
                     || name == "LeftDoubleBracket"
                     || name == "LeftFloor"
                     || name == "OpenCurlyQuote" ) {
                    if ( ! f.isNull() && f == "postfix" )
                        return i;
                }
            }
        }
    }
    return -1;
}
