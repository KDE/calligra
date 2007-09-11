// Created: Tue Sep 11 13:09:34 2007
// WARNING! All changes made in this file will be lost!

/* This file is part of the KDE project
   Copyright (C) 2007 <hubipete@gmx.net>

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

#include "Dictionary.h"

Dictionary::Dictionary()
{
    m_lspace = "thickmathspace";
    m_rspace = "thickmathspace";
    m_maxsize = "infinity";
    m_minsize = "1";
    m_fence = false;
    m_separator = false;
    m_stretchy = false;
    m_symmetric = true;
    m_largeop = false;
    m_movablelimits = false;
    m_accent = false;
}


bool Dictionary::queryOperator( const QString& queriedOperator, Form form )
{
    if( queriedOperator.isEmpty() || queriedOperator.isNull() )
        return false;
    else if( queriedOperator == "(" && form == Prefix ) {
        m_fence = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == ")" && form == Postfix ) {
        m_fence = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "[" && form == Prefix ) {
        m_fence = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "]" && form == Postfix ) {
        m_fence = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "{" && form == Prefix ) {
        m_fence = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "}" && form == Postfix ) {
        m_fence = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&CloseCurlyDoubleQuote;" && form == Postfix ) {
        m_fence = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&CloseCurlyQuote;" && form == Postfix ) {
        m_fence = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&LeftAngleBracket;" && form == Prefix ) {
        m_fence = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&LeftCeiling;" && form == Prefix ) {
        m_fence = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&LeftDoubleBracket;" && form == Prefix ) {
        m_fence = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&LeftFloor;" && form == Prefix ) {
        m_fence = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&OpenCurlyDoubleQuote;" && form == Prefix ) {
        m_fence = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&OpenCurlyQuote;" && form == Prefix ) {
        m_fence = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&RightAngleBracket;" && form == Postfix ) {
        m_fence = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&RightCeiling;" && form == Postfix ) {
        m_fence = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&RightDoubleBracket;" && form == Postfix ) {
        m_fence = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&RightFloor;" && form == Postfix ) {
        m_fence = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&InvisibleComma;" && form == Infix ) {
        m_separator = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "," && form == Infix ) {
        m_separator = true;
        m_lspace = "0em";
        m_rspace = "verythickmathspace";
        return true;
    }
    else if( queriedOperator == "&HorizontalLine;" && form == Infix ) {
        m_stretchy = true;
        m_minsize = "0";
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&VerticalLine;" && form == Infix ) {
        m_stretchy = true;
        m_minsize = "0";
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == ";" && form == Infix ) {
        m_separator = true;
        m_lspace = "0em";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == ";" && form == Postfix ) {
        m_separator = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == ":=" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&Assign;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&Because;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&Therefore;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&VerticalSeparator;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "//" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&Colon;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&amp;" && form == Prefix ) {
        m_lspace = "0em";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&amp;" && form == Postfix ) {
        m_lspace = "thickmathspace";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "*=" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "-=" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "+=" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "/=" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "->" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == ":" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == ".." && form == Postfix ) {
        m_lspace = "mediummathspace";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "..." && form == Postfix ) {
        m_lspace = "mediummathspace";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&SuchThat;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&DoubleLeftTee;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&DoubleRightTee;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&DownTee;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&LeftTee;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&RightTee;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&Implies;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&RoundImplies;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "|" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "||" && form == Infix ) {
        m_lspace = "mediummathspace";
        m_rspace = "mediummathspace";
        return true;
    }
    else if( queriedOperator == "&Or;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "mediummathspace";
        m_rspace = "mediummathspace";
        return true;
    }
    else if( queriedOperator == "&amp;&amp;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&And;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "mediummathspace";
        m_rspace = "mediummathspace";
        return true;
    }
    else if( queriedOperator == "&amp;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "!" && form == Prefix ) {
        m_lspace = "0em";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&Not;" && form == Prefix ) {
        m_lspace = "0em";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&Exists;" && form == Prefix ) {
        m_lspace = "0em";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&ForAll;" && form == Prefix ) {
        m_lspace = "0em";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotExists;" && form == Prefix ) {
        m_lspace = "0em";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&Element;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotElement;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotReverseElement;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotSquareSubset;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotSquareSubsetEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotSquareSuperset;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotSquareSupersetEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotSubset;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotSubsetEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotSuperset;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotSupersetEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&ReverseElement;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&SquareSubset;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&SquareSubsetEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&SquareSuperset;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&SquareSupersetEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&Subset;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&SubsetEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&Superset;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&SupersetEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&DoubleLeftArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&DoubleLeftRightArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&DoubleRightArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&DownLeftRightVector;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&DownLeftTeeVector;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&DownLeftVector;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&DownLeftVectorBar;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&DownRightTeeVector;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&DownRightVector;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&DownRightVectorBar;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&LeftArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&LeftArrowBar;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&LeftArrowRightArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&LeftRightArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&LeftRightVector;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&LeftTeeArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&LeftTeeVector;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&LeftVector;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&LeftVectorBar;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&LowerLeftArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&LowerRightArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&RightArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&RightArrowBar;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&RightArrowLeftArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&RightTeeArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&RightTeeVector;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&RightVector;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&RightVectorBar;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&ShortLeftArrow;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&ShortRightArrow;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&UpperLeftArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&UpperRightArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "=" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&lt;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == ">" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "!=" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "==" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&lt;=" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == ">=" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&Congruent;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&CupCap;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&DotEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&DoubleVerticalBar;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&Equal;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&EqualTilde;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&Equilibrium;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&GreaterEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&GreaterEqualLess;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&GreaterFullEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&GreaterGreater;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&GreaterLess;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&GreaterSlantEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&GreaterTilde;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&HumpDownHump;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&HumpEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&LeftTriangle;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&LeftTriangleBar;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&LeftTriangleEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&le;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&LessEqualGreater;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&LessFullEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&LessGreater;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&LessLess;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&LessSlantEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&LessTilde;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NestedGreaterGreater;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NestedLessLess;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotCongruent;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotCupCap;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotDoubleVerticalBar;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotEqualTilde;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotGreater;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotGreaterEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotGreaterFullEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotGreaterGreater;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotGreaterLess;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotGreaterSlantEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotGreaterTilde;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotHumpDownHump;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotHumpEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotLeftTriangle;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotLeftTriangleBar;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotLeftTriangleEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotLess;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotLessEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotLessGreater;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotLessLess;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotLessSlantEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotLessTilde;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotNestedGreaterGreater;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotNestedLessLess;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotPrecedes;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotPrecedesEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotPrecedesSlantEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotRightTriangle;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotRightTriangleBar;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotRightTriangleEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotSucceeds;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotSucceedsEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotSucceedsSlantEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotSucceedsTilde;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotTilde;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotTildeEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotTildeFullEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotTildeTilde;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&NotVerticalBar;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&Precedes;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&PrecedesEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&PrecedesSlantEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&PrecedesTilde;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&Proportion;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&Proportional;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&ReverseEquilibrium;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&RightTriangle;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&RightTriangleBar;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&RightTriangleEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&Succeeds;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&SucceedsEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&SucceedsSlantEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&SucceedsTilde;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&Tilde;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&TildeEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&TildeFullEqual;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&TildeTilde;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&UpTee;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&VerticalBar;" && form == Infix ) {
        m_lspace = "thickmathspace";
        m_rspace = "thickmathspace";
        return true;
    }
    else if( queriedOperator == "&SquareUnion;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "mediummathspace";
        m_rspace = "mediummathspace";
        return true;
    }
    else if( queriedOperator == "&Union;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "mediummathspace";
        m_rspace = "mediummathspace";
        return true;
    }
    else if( queriedOperator == "&UnionPlus;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "mediummathspace";
        m_rspace = "mediummathspace";
        return true;
    }
    else if( queriedOperator == "-" && form == Infix ) {
        m_lspace = "mediummathspace";
        m_rspace = "mediummathspace";
        return true;
    }
    else if( queriedOperator == "+" && form == Infix ) {
        m_lspace = "mediummathspace";
        m_rspace = "mediummathspace";
        return true;
    }
    else if( queriedOperator == "&Intersection;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "mediummathspace";
        m_rspace = "mediummathspace";
        return true;
    }
    else if( queriedOperator == "&MinusPlus;" && form == Infix ) {
        m_lspace = "mediummathspace";
        m_rspace = "mediummathspace";
        return true;
    }
    else if( queriedOperator == "&PlusMinus;" && form == Infix ) {
        m_lspace = "mediummathspace";
        m_rspace = "mediummathspace";
        return true;
    }
    else if( queriedOperator == "&SquareIntersection;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "mediummathspace";
        m_rspace = "mediummathspace";
        return true;
    }
    else if( queriedOperator == "&Vee;" && form == Prefix ) {
        m_largeop = true;
        m_movablelimits = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&CircleMinus;" && form == Prefix ) {
        m_largeop = true;
        m_movablelimits = true;
        m_lspace = "0em";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&CirclePlus;" && form == Prefix ) {
        m_largeop = true;
        m_movablelimits = true;
        m_lspace = "0em";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&Sum;" && form == Prefix ) {
        m_largeop = true;
        m_movablelimits = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&Union;" && form == Prefix ) {
        m_largeop = true;
        m_movablelimits = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&UnionPlus;" && form == Prefix ) {
        m_largeop = true;
        m_movablelimits = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "lim" && form == Prefix ) {
        m_movablelimits = true;
        m_lspace = "0em";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "max" && form == Prefix ) {
        m_movablelimits = true;
        m_lspace = "0em";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "min" && form == Prefix ) {
        m_movablelimits = true;
        m_lspace = "0em";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&CircleMinus;" && form == Infix ) {
        m_lspace = "thinmathspace";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&CirclePlus;" && form == Infix ) {
        m_lspace = "thinmathspace";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&ClockwiseContourIntegral;" && form == Prefix ) {
        m_largeop = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&ContourIntegral;" && form == Prefix ) {
        m_largeop = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&CounterClockwiseContourIntegral;" && form == Prefix ) {
        m_largeop = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&DoubleContourIntegral;" && form == Prefix ) {
        m_largeop = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&Integral;" && form == Prefix ) {
        m_largeop = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&Cup;" && form == Infix ) {
        m_lspace = "thinmathspace";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&Cap;" && form == Infix ) {
        m_lspace = "thinmathspace";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&VerticalTilde;" && form == Infix ) {
        m_lspace = "thinmathspace";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&Wedge;" && form == Prefix ) {
        m_largeop = true;
        m_movablelimits = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&CircleTimes;" && form == Prefix ) {
        m_largeop = true;
        m_movablelimits = true;
        m_lspace = "0em";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&Coproduct;" && form == Prefix ) {
        m_largeop = true;
        m_movablelimits = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&Product;" && form == Prefix ) {
        m_largeop = true;
        m_movablelimits = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&Intersection;" && form == Prefix ) {
        m_largeop = true;
        m_movablelimits = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&Coproduct;" && form == Infix ) {
        m_lspace = "thinmathspace";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&Star;" && form == Infix ) {
        m_lspace = "thinmathspace";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&CircleDot;" && form == Prefix ) {
        m_largeop = true;
        m_movablelimits = true;
        m_lspace = "0em";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "*" && form == Infix ) {
        m_lspace = "thinmathspace";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&InvisibleTimes;" && form == Infix ) {
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&CenterDot;" && form == Infix ) {
        m_lspace = "thinmathspace";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&CircleTimes;" && form == Infix ) {
        m_lspace = "thinmathspace";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&Vee;" && form == Infix ) {
        m_lspace = "thinmathspace";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&Wedge;" && form == Infix ) {
        m_lspace = "thinmathspace";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&Diamond;" && form == Infix ) {
        m_lspace = "thinmathspace";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "&Backslash;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thinmathspace";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "/" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "thinmathspace";
        m_rspace = "thinmathspace";
        return true;
    }
    else if( queriedOperator == "-" && form == Prefix ) {
        m_lspace = "0em";
        m_rspace = "veryverythinmathspace";
        return true;
    }
    else if( queriedOperator == "+" && form == Prefix ) {
        m_lspace = "0em";
        m_rspace = "veryverythinmathspace";
        return true;
    }
    else if( queriedOperator == "&MinusPlus;" && form == Prefix ) {
        m_lspace = "0em";
        m_rspace = "veryverythinmathspace";
        return true;
    }
    else if( queriedOperator == "&PlusMinus;" && form == Prefix ) {
        m_lspace = "0em";
        m_rspace = "veryverythinmathspace";
        return true;
    }
    else if( queriedOperator == "." && form == Infix ) {
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&Cross;" && form == Infix ) {
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "**" && form == Infix ) {
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&CircleDot;" && form == Infix ) {
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&SmallCircle;" && form == Infix ) {
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&Square;" && form == Prefix ) {
        m_lspace = "0em";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&Del;" && form == Prefix ) {
        m_lspace = "0em";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&PartialD;" && form == Prefix ) {
        m_lspace = "0em";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&CapitalDifferentialD;" && form == Prefix ) {
        m_lspace = "0em";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&DifferentialD;" && form == Prefix ) {
        m_lspace = "0em";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&Sqrt;" && form == Prefix ) {
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&DoubleDownArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&DoubleLongLeftArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&DoubleLongLeftRightArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&DoubleLongRightArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&DoubleUpArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&DoubleUpDownArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&DownArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&DownArrowBar;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&DownArrowUpArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&DownTeeArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&LeftDownTeeVector;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&LeftDownVector;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&LeftDownVectorBar;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&LeftUpDownVector;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&LeftUpTeeVector;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&LeftUpVector;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&LeftUpVectorBar;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&LongLeftArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&LongLeftRightArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&LongRightArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&ReverseUpEquilibrium;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&RightDownTeeVector;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&RightDownVector;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&RightDownVectorBar;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&RightUpDownVector;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&RightUpTeeVector;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&RightUpVector;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&RightUpVectorBar;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&ShortDownArrow;" && form == Infix ) {
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&ShortUpArrow;" && form == Infix ) {
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&UpArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&UpArrowBar;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&UpArrowDownArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&UpDownArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&UpEquilibrium;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&UpTeeArrow;" && form == Infix ) {
        m_stretchy = true;
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "^" && form == Infix ) {
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&lt;>" && form == Infix ) {
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "'" && form == Postfix ) {
        m_lspace = "verythinmathspace";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "!" && form == Postfix ) {
        m_lspace = "verythinmathspace";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "!!" && form == Postfix ) {
        m_lspace = "verythinmathspace";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "~" && form == Infix ) {
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "@" && form == Infix ) {
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "--" && form == Postfix ) {
        m_lspace = "verythinmathspace";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "--" && form == Prefix ) {
        m_lspace = "0em";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "++" && form == Postfix ) {
        m_lspace = "verythinmathspace";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "++" && form == Prefix ) {
        m_lspace = "0em";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&ApplyFunction;" && form == Infix ) {
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "?" && form == Infix ) {
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "_" && form == Infix ) {
        m_lspace = "verythinmathspace";
        m_rspace = "verythinmathspace";
        return true;
    }
    else if( queriedOperator == "&Breve;" && form == Postfix ) {
        m_accent = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&Cedilla;" && form == Postfix ) {
        m_accent = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&DiacriticalGrave;" && form == Postfix ) {
        m_accent = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&DiacriticalDot;" && form == Postfix ) {
        m_accent = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&DiacriticalDoubleAcute;" && form == Postfix ) {
        m_accent = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&LeftArrow;" && form == Postfix ) {
        m_accent = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&LeftRightArrow;" && form == Postfix ) {
        m_accent = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&LeftRightVector;" && form == Postfix ) {
        m_accent = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&LeftVector;" && form == Postfix ) {
        m_accent = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&DiacriticalAcute;" && form == Postfix ) {
        m_accent = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&RightArrow;" && form == Postfix ) {
        m_accent = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&RightVector;" && form == Postfix ) {
        m_accent = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&DiacriticalTilde;" && form == Postfix ) {
        m_accent = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&DoubleDot;" && form == Postfix ) {
        m_accent = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&DownBreve;" && form == Postfix ) {
        m_accent = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&Hacek;" && form == Postfix ) {
        m_accent = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&Hat;" && form == Postfix ) {
        m_accent = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&OverBar;" && form == Postfix ) {
        m_accent = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&OverBrace;" && form == Postfix ) {
        m_accent = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&OverBracket;" && form == Postfix ) {
        m_accent = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&OverParenthesis;" && form == Postfix ) {
        m_accent = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&TripleDot;" && form == Postfix ) {
        m_accent = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&UnderBar;" && form == Postfix ) {
        m_accent = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&UnderBrace;" && form == Postfix ) {
        m_accent = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&UnderBracket;" && form == Postfix ) {
        m_accent = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }
    else if( queriedOperator == "&UnderParenthesis;" && form == Postfix ) {
        m_accent = true;
        m_stretchy = true;
        m_lspace = "0em";
        m_rspace = "0em";
        return true;
    }

    return false;
}
