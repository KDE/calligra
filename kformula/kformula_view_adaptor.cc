/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>

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

#include "kformula_view_adaptor.h"
#include <kformuladocument.h>
#include <kformulaview.h>

#include "kformula_view.h"
#include "kformula_doc.h"

KformulaViewAdaptor::KformulaViewAdaptor( KFormulaPartView *view_ )
    : QDBusAbstractAdaptor(view_)
{
    setAutoRelaySignals(true);
    m_view = view_;
}

void KformulaViewAdaptor::addThinSpace()
{
    m_view->document()->getDocument()->wrapper()->addThinSpace();
}

void KformulaViewAdaptor::addMediumSpace()
{
    m_view->document()->getDocument()->wrapper()->addMediumSpace();
}

void KformulaViewAdaptor::addThickSpace()
{
    m_view->document()->getDocument()->wrapper()->addThickSpace();
}

void KformulaViewAdaptor::addQuadSpace()
{
    m_view->document()->getDocument()->wrapper()->addQuadSpace();
}

void KformulaViewAdaptor::addBracket( int left, int right )
{
    m_view->document()->getDocument()->
        wrapper()->addBracket( static_cast<KFormula::SymbolType>( left ),
                               static_cast<KFormula::SymbolType>( right ) );
}

void KformulaViewAdaptor::addParenthesis()
{
    m_view->document()->getDocument()->wrapper()->addParenthesis();
}

void KformulaViewAdaptor::addSquareBracket()
{
    m_view->document()->getDocument()->wrapper()->addSquareBracket();
}

void KformulaViewAdaptor::addCurlyBracket()
{
    m_view->document()->getDocument()->wrapper()->addCurlyBracket();
}

void KformulaViewAdaptor::addLineBracket()
{
    m_view->document()->getDocument()->wrapper()->addLineBracket();
}

void KformulaViewAdaptor::addFraction()
{
    m_view->document()->getDocument()->wrapper()->addFraction();
}

void KformulaViewAdaptor::addRoot()
{
    m_view->document()->getDocument()->wrapper()->addRoot();
}

void KformulaViewAdaptor::addIntegral()
{
    m_view->document()->getDocument()->wrapper()->addIntegral();
}

void KformulaViewAdaptor::addProduct()
{
    m_view->document()->getDocument()->wrapper()->addProduct();
}

void KformulaViewAdaptor::addSum()
{
    m_view->document()->getDocument()->wrapper()->addSum();
}

void KformulaViewAdaptor::addMatrix()
{
    m_view->document()->getDocument()->wrapper()->addMatrix();
}

void KformulaViewAdaptor::addMatrix( uint rows, uint columns )
{
    m_view->document()->getDocument()->wrapper()->addMatrix( rows, columns );
}

void KformulaViewAdaptor::addOneByTwoMatrix()
{
    m_view->document()->getDocument()->wrapper()->addOneByTwoMatrix();
}

void KformulaViewAdaptor::addNameSequence()
{
    m_view->document()->getDocument()->wrapper()->addNameSequence();
}

void KformulaViewAdaptor::addLowerLeftIndex()
{
    m_view->document()->getDocument()->wrapper()->addLowerLeftIndex();
}

void KformulaViewAdaptor::addUpperLeftIndex()
{
    m_view->document()->getDocument()->wrapper()->addUpperLeftIndex();
}

void KformulaViewAdaptor::addLowerRightIndex()
{
    m_view->document()->getDocument()->wrapper()->addLowerRightIndex();
}

void KformulaViewAdaptor::addUpperRightIndex()
{
    m_view->document()->getDocument()->wrapper()->addUpperRightIndex();
}

void KformulaViewAdaptor::addGenericLowerIndex()
{
    m_view->document()->getDocument()->wrapper()->addGenericLowerIndex();
}

void KformulaViewAdaptor::addGenericUpperIndex()
{
    m_view->document()->getDocument()->wrapper()->addGenericUpperIndex();
}

void KformulaViewAdaptor::removeEnclosing()
{
    m_view->document()->getDocument()->wrapper()->removeEnclosing();
}

void KformulaViewAdaptor::makeGreek()
{
    m_view->document()->getDocument()->wrapper()->makeGreek();
}

void KformulaViewAdaptor::insertSymbol( QString name )
{
    m_view->document()->getDocument()->wrapper()->insertSymbol( name );
}

void KformulaViewAdaptor::appendColumn()
{
    m_view->document()->getDocument()->wrapper()->appendColumn();
}

void KformulaViewAdaptor::insertColumn()
{
    m_view->document()->getDocument()->wrapper()->insertColumn();
}

void KformulaViewAdaptor::removeColumn()
{
    m_view->document()->getDocument()->wrapper()->removeColumn();
}

void KformulaViewAdaptor::appendRow()
{
    m_view->document()->getDocument()->wrapper()->appendRow();
}

void KformulaViewAdaptor::insertRow()
{
    m_view->document()->getDocument()->wrapper()->insertRow();
}

void KformulaViewAdaptor::removeRow()
{
    m_view->document()->getDocument()->wrapper()->removeRow();
}

void KformulaViewAdaptor::moveRight( int flag )
{
    m_view->formulaView()->moveRight( flag );
}

void KformulaViewAdaptor::moveUp( int flag )
{
    m_view->formulaView()->moveUp( flag );
}

void KformulaViewAdaptor::moveDown( int flag )
{
    m_view->formulaView()->moveDown( flag );
}

void KformulaViewAdaptor::moveLeft( int flag )
{
    m_view->formulaView()->moveLeft( flag );
}

void KformulaViewAdaptor::moveHome( int flag )
{
    m_view->formulaView()->moveHome( flag );
}

void KformulaViewAdaptor::moveEnd( int flag )
{
    m_view->formulaView()->moveEnd( flag );
}

bool KformulaViewAdaptor::isHome() const
{
    return m_view->formulaView()->isHome();
}

bool KformulaViewAdaptor::isEnd() const
{
    return m_view->formulaView()->isEnd();
}

void KformulaViewAdaptor::eraseSelection( int direction )
{
    m_view->formulaView()->eraseSelection( static_cast<KFormula::Direction>( direction ) );
}

void KformulaViewAdaptor::addText( QString str )
{
    m_view->formulaView()->addText( str );
}

QStringList KformulaViewAdaptor::readFormulaString( QString text )
{
    return m_view->readFormulaString( text );
}

#include "kformula_view_adaptor.moc"
