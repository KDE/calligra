/* This file is part of the KDE project
   Copyright (C) 2002 Laurent MONTEL <lmontel@mandrakesoft.com>

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

#include "KWordFormulaFrameSetEditIface.h"

#include "kwframe.h"
#include "kwdoc.h"
#include <kapplication.h>
#include <dcopclient.h>
#include <kformulacontainer.h>
#include <kformuladocument.h>
#include <kformulaview.h>

KWordFormulaFrameSetEditIface::KWordFormulaFrameSetEditIface( KWFormulaFrameSetEdit *_framesetedit )
    :DCOPObject()
{
   m_framesetedit = _framesetedit;
}

void KWordFormulaFrameSetEditIface::copy()
{
    m_framesetedit->copy();
}

void KWordFormulaFrameSetEditIface::cut()
{
    m_framesetedit->cut();
}

void KWordFormulaFrameSetEditIface::paste()
{
    m_framesetedit->paste();
}

void KWordFormulaFrameSetEditIface::selectAll()
{
    m_framesetedit->selectAll();
}

void KWordFormulaFrameSetEditIface::addThinSpace()
{
   m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->addThinSpace();
}

void KWordFormulaFrameSetEditIface::addMediumSpace()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->addMediumSpace();
}

void KWordFormulaFrameSetEditIface::addThickSpace()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->addThickSpace();
}

void KWordFormulaFrameSetEditIface::addQuadSpace()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->addQuadSpace();
}

void KWordFormulaFrameSetEditIface::addDefaultBracket()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->addDefaultBracket();
}

void KWordFormulaFrameSetEditIface::addSquareBracket()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->addSquareBracket();
}

void KWordFormulaFrameSetEditIface::addCurlyBracket()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->addCurlyBracket();
}

void KWordFormulaFrameSetEditIface::addLineBracket()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->addLineBracket();
}

void KWordFormulaFrameSetEditIface::addFraction()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->addFraction();
}

void KWordFormulaFrameSetEditIface::addRoot()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->addRoot();
}

void KWordFormulaFrameSetEditIface::addIntegral()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->addIntegral();
}

void KWordFormulaFrameSetEditIface::addProduct()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->addProduct();
}

void KWordFormulaFrameSetEditIface::addSum()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->addSum();
}

void KWordFormulaFrameSetEditIface::addMatrix()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->addMatrix();
}

void KWordFormulaFrameSetEditIface::addLowerLeftIndex()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->addLowerLeftIndex();
}

void KWordFormulaFrameSetEditIface::addUpperLeftIndex()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->addUpperLeftIndex();
}

void KWordFormulaFrameSetEditIface::addLowerRightIndex()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->addLowerRightIndex();
}

void KWordFormulaFrameSetEditIface::addUpperRightIndex()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->addUpperRightIndex();
}

void KWordFormulaFrameSetEditIface::addGenericLowerIndex()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->addGenericLowerIndex();
}

void KWordFormulaFrameSetEditIface::addGenericUpperIndex()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->addGenericUpperIndex();
}

void KWordFormulaFrameSetEditIface::removeEnclosing()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->removeEnclosing();
}

void KWordFormulaFrameSetEditIface::makeGreek()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->makeGreek();
}

void KWordFormulaFrameSetEditIface::insertSymbol()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->insertSymbol();
}

void KWordFormulaFrameSetEditIface::appendColumn()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->appendColumn();
}

void KWordFormulaFrameSetEditIface::insertColumn()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->insertColumn();
}

void KWordFormulaFrameSetEditIface::removeColumn()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->removeColumn();
}

void KWordFormulaFrameSetEditIface::appendRow()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->appendRow();
}

void KWordFormulaFrameSetEditIface::insertRow()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->insertRow();
}

void KWordFormulaFrameSetEditIface::removeRow()
{
    m_framesetedit->formulaFrameSet()->kWordDocument()->getFormulaDocument()->removeRow();
}

