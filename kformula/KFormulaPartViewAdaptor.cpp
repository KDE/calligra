/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
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

#include "KFormulaPartViewAdaptor.h"
#include "KFormulaPartView.h"


KFormulaPartViewAdaptor::KFormulaPartViewAdaptor( KFormulaPartView *view_ )
                    : QDBusAbstractAdaptor( view_ )
{
    setAutoRelaySignals( true );
    m_partView = view_;
}

void KFormulaPartViewAdaptor::addThinSpace()
{
//    m_view->document()->getDocument()->wrapper()->addThinSpace();
}

void KFormulaPartViewAdaptor::addMediumSpace()
{
//    m_view->document()->getDocument()->wrapper()->addMediumSpace();
}

void KFormulaPartViewAdaptor::addThickSpace()
{
//    m_view->document()->getDocument()->wrapper()->addThickSpace();
}

void KFormulaPartViewAdaptor::addQuadSpace()
{
//    m_view->document()->getDocument()->wrapper()->addQuadSpace();
}

void KFormulaPartViewAdaptor::addBracket( int left, int right )
{
/*    m_view->document()->getDocument()->
        wrapper()->addBracket( static_cast<KFormula::SymbolType>( left ),
                               static_cast<KFormula::SymbolType>( right ) );*/
}

void KFormulaPartViewAdaptor::addParenthesis()
{
//    m_view->document()->getDocument()->wrapper()->addParenthesis();
}

void KFormulaPartViewAdaptor::addSquareBracket()
{
//    m_view->document()->getDocument()->wrapper()->addSquareBracket();
}

void KFormulaPartViewAdaptor::addCurlyBracket()
{
//    m_view->document()->getDocument()->wrapper()->addCurlyBracket();
}

void KFormulaPartViewAdaptor::addLineBracket()
{
//    m_view->document()->getDocument()->wrapper()->addLineBracket();
}

void KFormulaPartViewAdaptor::addFraction()
{
//    m_view->document()->getDocument()->wrapper()->addFraction();
}

void KFormulaPartViewAdaptor::addRoot()
{
//    m_view->document()->getDocument()->wrapper()->addRoot();
}

void KFormulaPartViewAdaptor::addIntegral()
{
//    m_view->document()->getDocument()->wrapper()->addIntegral();
}

void KFormulaPartViewAdaptor::addProduct()
{
//    m_view->document()->getDocument()->wrapper()->addProduct();
}

void KFormulaPartViewAdaptor::addSum()
{
//    m_view->document()->getDocument()->wrapper()->addSum();
}

void KFormulaPartViewAdaptor::addMatrix()
{
//    m_view->document()->getDocument()->wrapper()->addMatrix();
}

void KFormulaPartViewAdaptor::addMatrix( uint rows, uint columns )
{
//    m_view->document()->getDocument()->wrapper()->addMatrix( rows, columns );
}

void KFormulaPartViewAdaptor::addOneByTwoMatrix()
{
//    m_view->document()->getDocument()->wrapper()->addOneByTwoMatrix();
}

void KFormulaPartViewAdaptor::addNameSequence()
{
//    m_view->document()->getDocument()->wrapper()->addNameSequence();
}

void KFormulaPartViewAdaptor::addLowerLeftIndex()
{
//    m_view->document()->getDocument()->wrapper()->addLowerLeftIndex();
}

void KFormulaPartViewAdaptor::addUpperLeftIndex()
{
//    m_view->document()->getDocument()->wrapper()->addUpperLeftIndex();
}

void KFormulaPartViewAdaptor::addLowerRightIndex()
{
//    m_view->document()->getDocument()->wrapper()->addLowerRightIndex();
}

void KFormulaPartViewAdaptor::addUpperRightIndex()
{
//    m_view->document()->getDocument()->wrapper()->addUpperRightIndex();
}

void KFormulaPartViewAdaptor::addGenericLowerIndex()
{
//    m_view->document()->getDocument()->wrapper()->addGenericLowerIndex();
}

void KFormulaPartViewAdaptor::addGenericUpperIndex()
{
//    m_view->document()->getDocument()->wrapper()->addGenericUpperIndex();
}

void KFormulaPartViewAdaptor::removeEnclosing()
{
//    m_view->document()->getDocument()->wrapper()->removeEnclosing();
}

void KFormulaPartViewAdaptor::makeGreek()
{
//    m_view->document()->getDocument()->wrapper()->makeGreek();
}

void KFormulaPartViewAdaptor::insertSymbol( QString name )
{
//    m_view->document()->getDocument()->wrapper()->insertSymbol( name );
}

void KFormulaPartViewAdaptor::appendColumn()
{
//    m_view->document()->getDocument()->wrapper()->appendColumn();
}

void KFormulaPartViewAdaptor::insertColumn()
{
//    m_view->document()->getDocument()->wrapper()->insertColumn();
}

void KFormulaPartViewAdaptor::removeColumn()
{
//    m_view->document()->getDocument()->wrapper()->removeColumn();
}

void KFormulaPartViewAdaptor::appendRow()
{
//    m_view->document()->getDocument()->wrapper()->appendRow();
}

void KFormulaPartViewAdaptor::insertRow()
{
//    m_view->document()->getDocument()->wrapper()->insertRow();
}

void KFormulaPartViewAdaptor::removeRow()
{
//    m_view->document()->getDocument()->wrapper()->removeRow();
}

void KFormulaPartViewAdaptor::moveRight( int flag )
{
//    m_view->formulaView()->moveRight( flag );
}

void KFormulaPartViewAdaptor::moveUp( int flag )
{
//    m_view->formulaView()->moveUp( flag );
}

void KFormulaPartViewAdaptor::moveDown( int flag )
{
//    m_view->formulaView()->moveDown( flag );
}

void KFormulaPartViewAdaptor::moveLeft( int flag )
{
//    m_view->formulaView()->moveLeft( flag );
}

void KFormulaPartViewAdaptor::moveHome( int flag )
{
//    m_view->formulaView()->moveHome( flag );
}

void KFormulaPartViewAdaptor::moveEnd( int flag )
{
//    m_view->formulaView()->moveEnd( flag );
}

bool KFormulaPartViewAdaptor::isHome() const
{
  //  return m_view->formulaView()->isHome();
    return true;
}

bool KFormulaPartViewAdaptor::isEnd() const
{
//    return m_view->formulaView()->isEnd();
    return true;
}

void KFormulaPartViewAdaptor::eraseSelection( int direction )
{
//    m_view->formulaView()->eraseSelection( static_cast<KFormula::Direction>( direction ) );
}

void KFormulaPartViewAdaptor::addText( QString str )
{
//    m_view->formulaView()->addText( str );
}

QStringList KFormulaPartViewAdaptor::readFormulaString( QString text )
{
//    return m_view->readFormulaString( text );
    return QStringList();
}

#include "KFormulaPartViewAdaptor.moc"
