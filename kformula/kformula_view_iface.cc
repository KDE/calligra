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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kformula_view_iface.h"
#include <kformuladocument.h>
#include <kformulaview.h>

#include "kformula_view.h"
#include "kformula_doc.h"
#include <dcopclient.h>

KformulaViewIface::KformulaViewIface( KFormulaPartView *view_ )
    : KoViewIface( view_ )
{
    m_view = view_;
}

void KformulaViewIface::addThinSpace()
{
  m_view->document()->getDocument()->addThinSpace();
}

void KformulaViewIface::addMediumSpace()
{
    m_view->document()->getDocument()->addMediumSpace();
}

void KformulaViewIface::addThickSpace()
{
    m_view->document()->getDocument()->addThickSpace();
}

void KformulaViewIface::addQuadSpace()
{
    m_view->document()->getDocument()->addQuadSpace();
}

void KformulaViewIface::addBracket( int left, int right )
{
    m_view->document()->getDocument()->addBracket( static_cast<KFormula::SymbolType>( left ),
                                                   static_cast<KFormula::SymbolType>( right ) );
}

void KformulaViewIface::addParenthesis()
{
    m_view->document()->getDocument()->addParenthesis();
}

void KformulaViewIface::addSquareBracket()
{
    m_view->document()->getDocument()->addSquareBracket();
}

void KformulaViewIface::addCurlyBracket()
{
    m_view->document()->getDocument()->addCurlyBracket();
}

void KformulaViewIface::addLineBracket()
{
    m_view->document()->getDocument()->addLineBracket();
}

void KformulaViewIface::addFraction()
{
    m_view->document()->getDocument()->addFraction();
}

void KformulaViewIface::addRoot()
{
    m_view->document()->getDocument()->addRoot();
}

void KformulaViewIface::addIntegral()
{
    m_view->document()->getDocument()->addIntegral();
}

void KformulaViewIface::addProduct()
{
    m_view->document()->getDocument()->addProduct();
}

void KformulaViewIface::addSum()
{
    m_view->document()->getDocument()->addSum();
}

void KformulaViewIface::addMatrix()
{
    m_view->document()->getDocument()->addMatrix();
}

void KformulaViewIface::addMatrix( uint rows, uint columns )
{
    m_view->document()->getDocument()->addMatrix( rows, columns );
}

void KformulaViewIface::addOneByTwoMatrix()
{
    m_view->document()->getDocument()->addOneByTwoMatrix();
}

void KformulaViewIface::addNameSequence()
{
    m_view->document()->getDocument()->addNameSequence();
}

void KformulaViewIface::addLowerLeftIndex()
{
    m_view->document()->getDocument()->addLowerLeftIndex();
}

void KformulaViewIface::addUpperLeftIndex()
{
    m_view->document()->getDocument()->addUpperLeftIndex();
}

void KformulaViewIface::addLowerRightIndex()
{
    m_view->document()->getDocument()->addLowerRightIndex();
}

void KformulaViewIface::addUpperRightIndex()
{
    m_view->document()->getDocument()->addUpperRightIndex();
}

void KformulaViewIface::addGenericLowerIndex()
{
    m_view->document()->getDocument()->addGenericLowerIndex();
}

void KformulaViewIface::addGenericUpperIndex()
{
    m_view->document()->getDocument()->addGenericUpperIndex();
}

void KformulaViewIface::removeEnclosing()
{
    m_view->document()->getDocument()->removeEnclosing();
}

void KformulaViewIface::makeGreek()
{
    m_view->document()->getDocument()->makeGreek();
}

void KformulaViewIface::insertSymbol( QString name )
{
    m_view->document()->getDocument()->insertSymbol( name );
}

void KformulaViewIface::appendColumn()
{
    m_view->document()->getDocument()->appendColumn();
}

void KformulaViewIface::insertColumn()
{
    m_view->document()->getDocument()->insertColumn();
}

void KformulaViewIface::removeColumn()
{
    m_view->document()->getDocument()->removeColumn();
}

void KformulaViewIface::appendRow()
{
    m_view->document()->getDocument()->appendRow();
}

void KformulaViewIface::insertRow()
{
    m_view->document()->getDocument()->insertRow();
}

void KformulaViewIface::removeRow()
{
    m_view->document()->getDocument()->removeRow();
}

void KformulaViewIface::moveRight( int flag )
{
    m_view->formulaView()->moveRight( flag );
}

void KformulaViewIface::moveUp( int flag )
{
    m_view->formulaView()->moveUp( flag );
}

void KformulaViewIface::moveDown( int flag )
{
    m_view->formulaView()->moveDown( flag );
}

void KformulaViewIface::moveLeft( int flag )
{
    m_view->formulaView()->moveLeft( flag );
}

void KformulaViewIface::moveHome( int flag )
{
    m_view->formulaView()->moveHome( flag );
}

void KformulaViewIface::moveEnd( int flag )
{
    m_view->formulaView()->moveEnd( flag );
}

bool KformulaViewIface::isHome() const
{
    return m_view->formulaView()->isHome();
}

bool KformulaViewIface::isEnd() const
{
    return m_view->formulaView()->isEnd();
}

void KformulaViewIface::eraseSelection( int direction )
{
    m_view->formulaView()->eraseSelection( static_cast<KFormula::Direction>( direction ) );
}

void KformulaViewIface::addText( QString str )
{
    m_view->formulaView()->addText( str );
}

QStringList KformulaViewIface::readFormulaString( QString text )
{
    return m_view->readFormulaString( text );
}
