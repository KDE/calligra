// Created: Wed Sep 12 13:13:23 2007
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

QChar Dictionary::mapEntity( const QString& entity )
{
    if( entity.isEmpty() ) return QChar();
    else if( entity == "Aacute" ) return QChar( 0x000C1 );
    else if( entity == "aacute" ) return QChar( 0x000E1 );
    else if( entity == "Abreve" ) return QChar( 0x00102 );
    else if( entity == "abreve" ) return QChar( 0x00103 );
    else if( entity == "ac" ) return QChar( 0x0223E );
    else if( entity == "acd" ) return QChar( 0x0223F );
    else if( entity == "Acirc" ) return QChar( 0x000C2 );
    else if( entity == "acirc" ) return QChar( 0x000E2 );
    else if( entity == "acute" ) return QChar( 0x000B4 );
    else if( entity == "Acy" ) return QChar( 0x00410 );
    else if( entity == "acy" ) return QChar( 0x00430 );
    else if( entity == "AElig" ) return QChar( 0x000C6 );
    else if( entity == "aelig" ) return QChar( 0x000E6 );
    else if( entity == "af" ) return QChar( 0x02061 );
    else if( entity == "Afr" ) return QChar( 0x1D504 );
    else if( entity == "afr" ) return QChar( 0x1D51E );
    else if( entity == "Agrave" ) return QChar( 0x000C0 );
    else if( entity == "agrave" ) return QChar( 0x000E0 );
    else if( entity == "aleph" ) return QChar( 0x02135 );
    else if( entity == "alpha" ) return QChar( 0x003B1 );
    else if( entity == "Amacr" ) return QChar( 0x00100 );
    else if( entity == "amacr" ) return QChar( 0x00101 );
    else if( entity == "amalg" ) return QChar( 0x02A3F );
    else if( entity == "amp" ) return QChar( 0x00026 );
    else if( entity == "And" ) return QChar( 0x02A53 );
    else if( entity == "and" ) return QChar( 0x02227 );
    else if( entity == "andand" ) return QChar( 0x02A55 );
    else if( entity == "andd" ) return QChar( 0x02A5C );
    else if( entity == "andslope" ) return QChar( 0x02A58 );
    else if( entity == "andv" ) return QChar( 0x02A5A );
    else if( entity == "ang" ) return QChar( 0x02220 );
    else if( entity == "ange" ) return QChar( 0x029A4 );
    else if( entity == "angle" ) return QChar( 0x02220 );
    else if( entity == "angmsd" ) return QChar( 0x02221 );
    else if( entity == "angmsdaa" ) return QChar( 0x029A8 );
    else if( entity == "angmsdab" ) return QChar( 0x029A9 );
    else if( entity == "angmsdac" ) return QChar( 0x029AA );
    else if( entity == "angmsdad" ) return QChar( 0x029AB );
    else if( entity == "angmsdae" ) return QChar( 0x029AC );
    else if( entity == "angmsdaf" ) return QChar( 0x029AD );
    else if( entity == "angmsdag" ) return QChar( 0x029AE );
    else if( entity == "angmsdah" ) return QChar( 0x029AF );
    else if( entity == "angrt" ) return QChar( 0x0221F );
    else if( entity == "angrtvb" ) return QChar( 0x022BE );
    else if( entity == "angrtvbd" ) return QChar( 0x0299D );
    else if( entity == "angsph" ) return QChar( 0x02222 );
    else if( entity == "angst" ) return QChar( 0x0212B );
    else if( entity == "angzarr" ) return QChar( 0x0237C );
    else if( entity == "Aogon" ) return QChar( 0x00104 );
    else if( entity == "aogon" ) return QChar( 0x00105 );
    else if( entity == "Aopf" ) return QChar( 0x1D538 );
    else if( entity == "aopf" ) return QChar( 0x1D552 );
    else if( entity == "ap" ) return QChar( 0x02248 );
    else if( entity == "apacir" ) return QChar( 0x02A6F );
    else if( entity == "apE" ) return QChar( 0x02A70 );
    else if( entity == "ape" ) return QChar( 0x0224A );
    else if( entity == "apid" ) return QChar( 0x0224B );
    else if( entity == "apos" ) return QChar( 0x00027 );
    else if( entity == "ApplyFunction" ) return QChar( 0x02061 );
    else if( entity == "approx" ) return QChar( 0x02248 );
    else if( entity == "approxeq" ) return QChar( 0x0224A );
    else if( entity == "Aring" ) return QChar( 0x000C5 );
    else if( entity == "aring" ) return QChar( 0x000E5 );
    else if( entity == "Ascr" ) return QChar( 0x1D49C );
    else if( entity == "ascr" ) return QChar( 0x1D4B6 );
    else if( entity == "Assign" ) return QChar( 0x02254 );
    else if( entity == "ast" ) return QChar( 0x0002A );
    else if( entity == "asymp" ) return QChar( 0x02248 );
    else if( entity == "asympeq" ) return QChar( 0x0224D );
    else if( entity == "Atilde" ) return QChar( 0x000C3 );
    else if( entity == "atilde" ) return QChar( 0x000E3 );
    else if( entity == "Auml" ) return QChar( 0x000C4 );
    else if( entity == "auml" ) return QChar( 0x000E4 );
    else if( entity == "awconint" ) return QChar( 0x02233 );
    else if( entity == "awint" ) return QChar( 0x02A11 );
    else if( entity == "backcong" ) return QChar( 0x0224C );
    else if( entity == "backepsilon" ) return QChar( 0x003F6 );
    else if( entity == "backprime" ) return QChar( 0x02035 );
    else if( entity == "backsim" ) return QChar( 0x0223D );
    else if( entity == "backsimeq" ) return QChar( 0x022CD );
    else if( entity == "Backslash" ) return QChar( 0x02216 );
    else if( entity == "Barv" ) return QChar( 0x02AE7 );
    else if( entity == "barvee" ) return QChar( 0x022BD );
    else if( entity == "Barwed" ) return QChar( 0x02306 );
    else if( entity == "barwed" ) return QChar( 0x02305 );
    else if( entity == "barwedge" ) return QChar( 0x02305 );
    else if( entity == "bbrk" ) return QChar( 0x023B5 );
    else if( entity == "bbrktbrk" ) return QChar( 0x023B6 );
    else if( entity == "bcong" ) return QChar( 0x0224C );
    else if( entity == "Bcy" ) return QChar( 0x00411 );
    else if( entity == "bcy" ) return QChar( 0x00431 );
    else if( entity == "becaus" ) return QChar( 0x02235 );
    else if( entity == "Because" ) return QChar( 0x02235 );
    else if( entity == "because" ) return QChar( 0x02235 );
    else if( entity == "bemptyv" ) return QChar( 0x029B0 );
    else if( entity == "bepsi" ) return QChar( 0x003F6 );
    else if( entity == "bernou" ) return QChar( 0x0212C );
    else if( entity == "Bernoullis" ) return QChar( 0x0212C );
    else if( entity == "beta" ) return QChar( 0x003B2 );
    else if( entity == "beth" ) return QChar( 0x02136 );
    else if( entity == "between" ) return QChar( 0x0226C );
    else if( entity == "Bfr" ) return QChar( 0x1D505 );
    else if( entity == "bfr" ) return QChar( 0x1D51F );
    else if( entity == "bigcap" ) return QChar( 0x022C2 );
    else if( entity == "bigcirc" ) return QChar( 0x025EF );
    else if( entity == "bigcup" ) return QChar( 0x022C3 );
    else if( entity == "bigodot" ) return QChar( 0x02A00 );
    else if( entity == "bigoplus" ) return QChar( 0x02A01 );
    else if( entity == "bigotimes" ) return QChar( 0x02A02 );
    else if( entity == "bigsqcup" ) return QChar( 0x02A06 );
    else if( entity == "bigstar" ) return QChar( 0x02605 );
    else if( entity == "bigtriangledown" ) return QChar( 0x025BD );
    else if( entity == "bigtriangleup" ) return QChar( 0x025B3 );
    else if( entity == "biguplus" ) return QChar( 0x02A04 );
    else if( entity == "bigvee" ) return QChar( 0x022C1 );
    else if( entity == "bigwedge" ) return QChar( 0x022C0 );
    else if( entity == "bkarow" ) return QChar( 0x0290D );
    else if( entity == "blacklozenge" ) return QChar( 0x029EB );
    else if( entity == "blacksquare" ) return QChar( 0x025AA );
    else if( entity == "blacktriangle" ) return QChar( 0x025B4 );
    else if( entity == "blacktriangledown" ) return QChar( 0x025BE );
    else if( entity == "blacktriangleleft" ) return QChar( 0x025C2 );
    else if( entity == "blacktriangleright" ) return QChar( 0x025B8 );
    else if( entity == "blank" ) return QChar( 0x02423 );
    else if( entity == "blk12" ) return QChar( 0x02592 );
    else if( entity == "blk14" ) return QChar( 0x02591 );
    else if( entity == "blk34" ) return QChar( 0x02593 );
    else if( entity == "block" ) return QChar( 0x02588 );
    else if( entity == "bNot" ) return QChar( 0x02AED );
    else if( entity == "bnot" ) return QChar( 0x02310 );
    else if( entity == "Bopf" ) return QChar( 0x1D539 );
    else if( entity == "bopf" ) return QChar( 0x1D553 );
    else if( entity == "bot" ) return QChar( 0x022A5 );
    else if( entity == "bottom" ) return QChar( 0x022A5 );
    else if( entity == "bowtie" ) return QChar( 0x022C8 );
    else if( entity == "boxbox" ) return QChar( 0x029C9 );
    else if( entity == "boxDL" ) return QChar( 0x02557 );
    else if( entity == "boxDl" ) return QChar( 0x02556 );
    else if( entity == "boxdL" ) return QChar( 0x02555 );
    else if( entity == "boxdl" ) return QChar( 0x02510 );
    else if( entity == "boxDR" ) return QChar( 0x02554 );
    else if( entity == "boxDr" ) return QChar( 0x02553 );
    else if( entity == "boxdR" ) return QChar( 0x02552 );
    else if( entity == "boxdr" ) return QChar( 0x0250C );
    else if( entity == "boxH" ) return QChar( 0x02550 );
    else if( entity == "boxh" ) return QChar( 0x02500 );
    else if( entity == "boxHD" ) return QChar( 0x02566 );
    else if( entity == "boxHd" ) return QChar( 0x02564 );
    else if( entity == "boxhD" ) return QChar( 0x02565 );
    else if( entity == "boxhd" ) return QChar( 0x0252C );
    else if( entity == "boxHU" ) return QChar( 0x02569 );
    else if( entity == "boxHu" ) return QChar( 0x02567 );
    else if( entity == "boxhU" ) return QChar( 0x02568 );
    else if( entity == "boxhu" ) return QChar( 0x02534 );
    else if( entity == "boxminus" ) return QChar( 0x0229F );
    else if( entity == "boxplus" ) return QChar( 0x0229E );
    else if( entity == "boxtimes" ) return QChar( 0x022A0 );
    else if( entity == "boxUL" ) return QChar( 0x0255D );
    else if( entity == "boxUl" ) return QChar( 0x0255C );
    else if( entity == "boxuL" ) return QChar( 0x0255B );
    else if( entity == "boxul" ) return QChar( 0x02518 );
    else if( entity == "boxUR" ) return QChar( 0x0255A );
    else if( entity == "boxUr" ) return QChar( 0x02559 );
    else if( entity == "boxuR" ) return QChar( 0x02558 );
    else if( entity == "boxur" ) return QChar( 0x02514 );
    else if( entity == "boxV" ) return QChar( 0x02551 );
    else if( entity == "boxv" ) return QChar( 0x02502 );
    else if( entity == "boxVH" ) return QChar( 0x0256C );
    else if( entity == "boxVh" ) return QChar( 0x0256B );
    else if( entity == "boxvH" ) return QChar( 0x0256A );
    else if( entity == "boxvh" ) return QChar( 0x0253C );
    else if( entity == "boxVL" ) return QChar( 0x02563 );
    else if( entity == "boxVl" ) return QChar( 0x02562 );
    else if( entity == "boxvL" ) return QChar( 0x02561 );
    else if( entity == "boxvl" ) return QChar( 0x02524 );
    else if( entity == "boxVR" ) return QChar( 0x02560 );
    else if( entity == "boxVr" ) return QChar( 0x0255F );
    else if( entity == "boxvR" ) return QChar( 0x0255E );
    else if( entity == "boxvr" ) return QChar( 0x0251C );
    else if( entity == "bprime" ) return QChar( 0x02035 );
    else if( entity == "Breve" ) return QChar( 0x002D8 );
    else if( entity == "breve" ) return QChar( 0x002D8 );
    else if( entity == "brvbar" ) return QChar( 0x000A6 );
    else if( entity == "Bscr" ) return QChar( 0x0212C );
    else if( entity == "bscr" ) return QChar( 0x1D4B7 );
    else if( entity == "bsemi" ) return QChar( 0x0204F );
    else if( entity == "bsim" ) return QChar( 0x0223D );
    else if( entity == "bsime" ) return QChar( 0x022CD );
    else if( entity == "bsol" ) return QChar( 0x0005C );
    else if( entity == "bsolb" ) return QChar( 0x029C5 );
    else if( entity == "bull" ) return QChar( 0x02022 );
    else if( entity == "bullet" ) return QChar( 0x02022 );
    else if( entity == "bump" ) return QChar( 0x0224E );
    else if( entity == "bumpE" ) return QChar( 0x02AAE );
    else if( entity == "bumpe" ) return QChar( 0x0224F );
    else if( entity == "Bumpeq" ) return QChar( 0x0224E );
    else if( entity == "bumpeq" ) return QChar( 0x0224F );
    else if( entity == "Cacute" ) return QChar( 0x00106 );
    else if( entity == "cacute" ) return QChar( 0x00107 );
    else if( entity == "Cap" ) return QChar( 0x022D2 );
    else if( entity == "cap" ) return QChar( 0x02229 );
    else if( entity == "capand" ) return QChar( 0x02A44 );
    else if( entity == "capbrcup" ) return QChar( 0x02A49 );
    else if( entity == "capcap" ) return QChar( 0x02A4B );
    else if( entity == "capcup" ) return QChar( 0x02A47 );
    else if( entity == "capdot" ) return QChar( 0x02A40 );
    else if( entity == "CapitalDifferentialD" ) return QChar( 0x02145 );
    else if( entity == "caret" ) return QChar( 0x02041 );
    else if( entity == "caron" ) return QChar( 0x002C7 );
    else if( entity == "Cayleys" ) return QChar( 0x0212D );
    else if( entity == "ccaps" ) return QChar( 0x02A4D );
    else if( entity == "Ccaron" ) return QChar( 0x0010C );
    else if( entity == "ccaron" ) return QChar( 0x0010D );
    else if( entity == "Ccedil" ) return QChar( 0x000C7 );
    else if( entity == "ccedil" ) return QChar( 0x000E7 );
    else if( entity == "Ccirc" ) return QChar( 0x00108 );
    else if( entity == "ccirc" ) return QChar( 0x00109 );
    else if( entity == "Cconint" ) return QChar( 0x02230 );
    else if( entity == "ccups" ) return QChar( 0x02A4C );
    else if( entity == "ccupssm" ) return QChar( 0x02A50 );
    else if( entity == "Cdot" ) return QChar( 0x0010A );
    else if( entity == "cdot" ) return QChar( 0x0010B );
    else if( entity == "cedil" ) return QChar( 0x000B8 );
    else if( entity == "Cedilla" ) return QChar( 0x000B8 );
    else if( entity == "cemptyv" ) return QChar( 0x029B2 );
    else if( entity == "cent" ) return QChar( 0x000A2 );
    else if( entity == "CenterDot" ) return QChar( 0x000B7 );
    else if( entity == "centerdot" ) return QChar( 0x000B7 );
    else if( entity == "Cfr" ) return QChar( 0x0212D );
    else if( entity == "cfr" ) return QChar( 0x1D520 );
    else if( entity == "CHcy" ) return QChar( 0x00427 );
    else if( entity == "chcy" ) return QChar( 0x00447 );
    else if( entity == "check" ) return QChar( 0x02713 );
    else if( entity == "checkmark" ) return QChar( 0x02713 );
    else if( entity == "chi" ) return QChar( 0x003C7 );
    else if( entity == "cir" ) return QChar( 0x025CB );
    else if( entity == "circ" ) return QChar( 0x002C6 );
    else if( entity == "circeq" ) return QChar( 0x02257 );
    else if( entity == "circlearrowleft" ) return QChar( 0x021BA );
    else if( entity == "circlearrowright" ) return QChar( 0x021BB );
    else if( entity == "circledast" ) return QChar( 0x0229B );
    else if( entity == "circledcirc" ) return QChar( 0x0229A );
    else if( entity == "circleddash" ) return QChar( 0x0229D );
    else if( entity == "CircleDot" ) return QChar( 0x02299 );
    else if( entity == "circledR" ) return QChar( 0x000AE );
    else if( entity == "circledS" ) return QChar( 0x024C8 );
    else if( entity == "CircleMinus" ) return QChar( 0x02296 );
    else if( entity == "CirclePlus" ) return QChar( 0x02295 );
    else if( entity == "CircleTimes" ) return QChar( 0x02297 );
    else if( entity == "cirE" ) return QChar( 0x029C3 );
    else if( entity == "cire" ) return QChar( 0x02257 );
    else if( entity == "cirfnint" ) return QChar( 0x02A10 );
    else if( entity == "cirmid" ) return QChar( 0x02AEF );
    else if( entity == "cirscir" ) return QChar( 0x029C2 );
    else if( entity == "ClockwiseContourIntegral" ) return QChar( 0x02232 );
    else if( entity == "CloseCurlyDoubleQuote" ) return QChar( 0x0201D );
    else if( entity == "CloseCurlyQuote" ) return QChar( 0x02019 );
    else if( entity == "clubs" ) return QChar( 0x02663 );
    else if( entity == "clubsuit" ) return QChar( 0x02663 );
    else if( entity == "Colon" ) return QChar( 0x02237 );
    else if( entity == "colon" ) return QChar( 0x0003A );
    else if( entity == "Colone" ) return QChar( 0x02A74 );
    else if( entity == "colone" ) return QChar( 0x02254 );
    else if( entity == "coloneq" ) return QChar( 0x02254 );
    else if( entity == "comma" ) return QChar( 0x0002C );
    else if( entity == "commat" ) return QChar( 0x00040 );
    else if( entity == "comp" ) return QChar( 0x02201 );
    else if( entity == "compfn" ) return QChar( 0x02218 );
    else if( entity == "complement" ) return QChar( 0x02201 );
    else if( entity == "complexes" ) return QChar( 0x02102 );
    else if( entity == "cong" ) return QChar( 0x02245 );
    else if( entity == "congdot" ) return QChar( 0x02A6D );
    else if( entity == "Congruent" ) return QChar( 0x02261 );
    else if( entity == "Conint" ) return QChar( 0x0222F );
    else if( entity == "conint" ) return QChar( 0x0222E );
    else if( entity == "ContourIntegral" ) return QChar( 0x0222E );
    else if( entity == "Copf" ) return QChar( 0x02102 );
    else if( entity == "copf" ) return QChar( 0x1D554 );
    else if( entity == "coprod" ) return QChar( 0x02210 );
    else if( entity == "Coproduct" ) return QChar( 0x02210 );
    else if( entity == "copy" ) return QChar( 0x000A9 );
    else if( entity == "copysr" ) return QChar( 0x02117 );
    else if( entity == "CounterClockwiseContourIntegral" ) return QChar( 0x02233 );
    else if( entity == "Cross" ) return QChar( 0x02A2F );
    else if( entity == "cross" ) return QChar( 0x02717 );
    else if( entity == "Cscr" ) return QChar( 0x1D49E );
    else if( entity == "cscr" ) return QChar( 0x1D4B8 );
    else if( entity == "csub" ) return QChar( 0x02ACF );
    else if( entity == "csube" ) return QChar( 0x02AD1 );
    else if( entity == "csup" ) return QChar( 0x02AD0 );
    else if( entity == "csupe" ) return QChar( 0x02AD2 );
    else if( entity == "ctdot" ) return QChar( 0x022EF );
    else if( entity == "cudarrl" ) return QChar( 0x02938 );
    else if( entity == "cudarrr" ) return QChar( 0x02935 );
    else if( entity == "cuepr" ) return QChar( 0x022DE );
    else if( entity == "cuesc" ) return QChar( 0x022DF );
    else if( entity == "cularr" ) return QChar( 0x021B6 );
    else if( entity == "cularrp" ) return QChar( 0x0293D );
    else if( entity == "Cup" ) return QChar( 0x022D3 );
    else if( entity == "cup" ) return QChar( 0x0222A );
    else if( entity == "cupbrcap" ) return QChar( 0x02A48 );
    else if( entity == "CupCap" ) return QChar( 0x0224D );
    else if( entity == "cupcap" ) return QChar( 0x02A46 );
    else if( entity == "cupcup" ) return QChar( 0x02A4A );
    else if( entity == "cupdot" ) return QChar( 0x0228D );
    else if( entity == "cupor" ) return QChar( 0x02A45 );
    else if( entity == "curarr" ) return QChar( 0x021B7 );
    else if( entity == "curarrm" ) return QChar( 0x0293C );
    else if( entity == "curlyeqprec" ) return QChar( 0x022DE );
    else if( entity == "curlyeqsucc" ) return QChar( 0x022DF );
    else if( entity == "curlyvee" ) return QChar( 0x022CE );
    else if( entity == "curlywedge" ) return QChar( 0x022CF );
    else if( entity == "curren" ) return QChar( 0x000A4 );
    else if( entity == "curvearrowleft" ) return QChar( 0x021B6 );
    else if( entity == "curvearrowright" ) return QChar( 0x021B7 );
    else if( entity == "cuvee" ) return QChar( 0x022CE );
    else if( entity == "cuwed" ) return QChar( 0x022CF );
    else if( entity == "cwconint" ) return QChar( 0x02232 );
    else if( entity == "cwint" ) return QChar( 0x02231 );
    else if( entity == "cylcty" ) return QChar( 0x0232D );
    else if( entity == "Dagger" ) return QChar( 0x02021 );
    else if( entity == "Dagger" ) return QChar( 0x02021 );
    else if( entity == "dagger" ) return QChar( 0x02020 );
    else if( entity == "dagger" ) return QChar( 0x02020 );
    else if( entity == "daleth" ) return QChar( 0x02138 );
    else if( entity == "Darr" ) return QChar( 0x021A1 );
    else if( entity == "dArr" ) return QChar( 0x021D3 );
    else if( entity == "darr" ) return QChar( 0x02193 );
    else if( entity == "dash" ) return QChar( 0x02010 );
    else if( entity == "Dashv" ) return QChar( 0x02AE4 );
    else if( entity == "dashv" ) return QChar( 0x022A3 );
    else if( entity == "dbkarow" ) return QChar( 0x0290F );
    else if( entity == "dblac" ) return QChar( 0x002DD );
    else if( entity == "Dcaron" ) return QChar( 0x0010E );
    else if( entity == "dcaron" ) return QChar( 0x0010F );
    else if( entity == "Dcy" ) return QChar( 0x00414 );
    else if( entity == "dcy" ) return QChar( 0x00434 );
    else if( entity == "DD" ) return QChar( 0x02145 );
    else if( entity == "dd" ) return QChar( 0x02146 );
    else if( entity == "ddagger" ) return QChar( 0x02021 );
    else if( entity == "ddarr" ) return QChar( 0x021CA );
    else if( entity == "DDotrahd" ) return QChar( 0x02911 );
    else if( entity == "ddotseq" ) return QChar( 0x02A77 );
    else if( entity == "deg" ) return QChar( 0x000B0 );
    else if( entity == "Del" ) return QChar( 0x02207 );
    else if( entity == "Delta" ) return QChar( 0x00394 );
    else if( entity == "delta" ) return QChar( 0x003B4 );
    else if( entity == "demptyv" ) return QChar( 0x029B1 );
    else if( entity == "dfisht" ) return QChar( 0x0297F );
    else if( entity == "Dfr" ) return QChar( 0x1D507 );
    else if( entity == "dfr" ) return QChar( 0x1D521 );
    else if( entity == "dHar" ) return QChar( 0x02965 );
    else if( entity == "dharl" ) return QChar( 0x021C3 );
    else if( entity == "dharr" ) return QChar( 0x021C2 );
    else if( entity == "DiacriticalAcute" ) return QChar( 0x000B4 );
    else if( entity == "DiacriticalDot" ) return QChar( 0x002D9 );
    else if( entity == "DiacriticalDoubleAcute" ) return QChar( 0x002DD );
    else if( entity == "DiacriticalGrave" ) return QChar( 0x00060 );
    else if( entity == "DiacriticalTilde" ) return QChar( 0x002DC );
    else if( entity == "diam" ) return QChar( 0x022C4 );
    else if( entity == "Diamond" ) return QChar( 0x022C4 );
    else if( entity == "diamond" ) return QChar( 0x022C4 );
    else if( entity == "diamondsuit" ) return QChar( 0x02666 );
    else if( entity == "diams" ) return QChar( 0x02666 );
    else if( entity == "die" ) return QChar( 0x000A8 );
    else if( entity == "DifferentialD" ) return QChar( 0x02146 );
    else if( entity == "digamma" ) return QChar( 0x003DD );
    else if( entity == "disin" ) return QChar( 0x022F2 );
    else if( entity == "div" ) return QChar( 0x000F7 );
    else if( entity == "divide" ) return QChar( 0x000F7 );
    else if( entity == "divideontimes" ) return QChar( 0x022C7 );
    else if( entity == "divonx" ) return QChar( 0x022C7 );
    else if( entity == "DJcy" ) return QChar( 0x00402 );
    else if( entity == "djcy" ) return QChar( 0x00452 );
    else if( entity == "dlcorn" ) return QChar( 0x0231E );
    else if( entity == "dlcrop" ) return QChar( 0x0230D );
    else if( entity == "dollar" ) return QChar( 0x00024 );
    else if( entity == "Dopf" ) return QChar( 0x1D53B );
    else if( entity == "dopf" ) return QChar( 0x1D555 );
    else if( entity == "Dot" ) return QChar( 0x000A8 );
    else if( entity == "dot" ) return QChar( 0x002D9 );
    else if( entity == "DotDot" ) return QChar( 0x020DC );
    else if( entity == "doteq" ) return QChar( 0x02250 );
    else if( entity == "doteqdot" ) return QChar( 0x02251 );
    else if( entity == "DotEqual" ) return QChar( 0x02250 );
    else if( entity == "dotminus" ) return QChar( 0x02238 );
    else if( entity == "dotplus" ) return QChar( 0x02214 );
    else if( entity == "dotsquare" ) return QChar( 0x022A1 );
    else if( entity == "doublebarwedge" ) return QChar( 0x02306 );
    else if( entity == "DoubleContourIntegral" ) return QChar( 0x0222F );
    else if( entity == "DoubleDot" ) return QChar( 0x000A8 );
    else if( entity == "DoubleDownArrow" ) return QChar( 0x021D3 );
    else if( entity == "DoubleLeftArrow" ) return QChar( 0x021D0 );
    else if( entity == "DoubleLeftRightArrow" ) return QChar( 0x021D4 );
    else if( entity == "DoubleLeftTee" ) return QChar( 0x02AE4 );
    else if( entity == "DoubleLongLeftArrow" ) return QChar( 0x027F8 );
    else if( entity == "DoubleLongLeftRightArrow" ) return QChar( 0x027FA );
    else if( entity == "DoubleLongRightArrow" ) return QChar( 0x027F9 );
    else if( entity == "DoubleRightArrow" ) return QChar( 0x021D2 );
    else if( entity == "DoubleRightTee" ) return QChar( 0x022A8 );
    else if( entity == "DoubleUpArrow" ) return QChar( 0x021D1 );
    else if( entity == "DoubleUpDownArrow" ) return QChar( 0x021D5 );
    else if( entity == "DoubleVerticalBar" ) return QChar( 0x02225 );
    else if( entity == "DownArrow" ) return QChar( 0x02193 );
    else if( entity == "Downarrow" ) return QChar( 0x021D3 );
    else if( entity == "downarrow" ) return QChar( 0x02193 );
    else if( entity == "DownArrowBar" ) return QChar( 0x02913 );
    else if( entity == "DownArrowUpArrow" ) return QChar( 0x021F5 );
    else if( entity == "DownBreve" ) return QChar( 0x00311 );
    else if( entity == "downdownarrows" ) return QChar( 0x021CA );
    else if( entity == "downharpoonleft" ) return QChar( 0x021C3 );
    else if( entity == "downharpoonright" ) return QChar( 0x021C2 );
    else if( entity == "DownLeftRightVector" ) return QChar( 0x02950 );
    else if( entity == "DownLeftTeeVector" ) return QChar( 0x0295E );
    else if( entity == "DownLeftVector" ) return QChar( 0x021BD );
    else if( entity == "DownLeftVectorBar" ) return QChar( 0x02956 );
    else if( entity == "DownRightTeeVector" ) return QChar( 0x0295F );
    else if( entity == "DownRightVector" ) return QChar( 0x021C1 );
    else if( entity == "DownRightVectorBar" ) return QChar( 0x02957 );
    else if( entity == "DownTee" ) return QChar( 0x022A4 );
    else if( entity == "DownTeeArrow" ) return QChar( 0x021A7 );
    else if( entity == "drbkarow" ) return QChar( 0x02910 );
    else if( entity == "drcorn" ) return QChar( 0x0231F );
    else if( entity == "drcrop" ) return QChar( 0x0230C );
    else if( entity == "Dscr" ) return QChar( 0x1D49F );
    else if( entity == "dscr" ) return QChar( 0x1D4B9 );
    else if( entity == "DScy" ) return QChar( 0x00405 );
    else if( entity == "dscy" ) return QChar( 0x00455 );
    else if( entity == "dsol" ) return QChar( 0x029F6 );
    else if( entity == "Dstrok" ) return QChar( 0x00110 );
    else if( entity == "dstrok" ) return QChar( 0x00111 );
    else if( entity == "dtdot" ) return QChar( 0x022F1 );
    else if( entity == "dtri" ) return QChar( 0x025BF );
    else if( entity == "dtrif" ) return QChar( 0x025BE );
    else if( entity == "duarr" ) return QChar( 0x021F5 );
    else if( entity == "duhar" ) return QChar( 0x0296F );
    else if( entity == "dwangle" ) return QChar( 0x029A6 );
    else if( entity == "DZcy" ) return QChar( 0x0040F );
    else if( entity == "dzcy" ) return QChar( 0x0045F );
    else if( entity == "dzigrarr" ) return QChar( 0x027FF );
    else if( entity == "Eacute" ) return QChar( 0x000C9 );
    else if( entity == "eacute" ) return QChar( 0x000E9 );
    else if( entity == "easter" ) return QChar( 0x02A6E );
    else if( entity == "Ecaron" ) return QChar( 0x0011A );
    else if( entity == "ecaron" ) return QChar( 0x0011B );
    else if( entity == "ecir" ) return QChar( 0x02256 );
    else if( entity == "Ecirc" ) return QChar( 0x000CA );
    else if( entity == "ecirc" ) return QChar( 0x000EA );
    else if( entity == "ecolon" ) return QChar( 0x02255 );
    else if( entity == "Ecy" ) return QChar( 0x0042D );
    else if( entity == "ecy" ) return QChar( 0x0044D );
    else if( entity == "eDDot" ) return QChar( 0x02A77 );
    else if( entity == "Edot" ) return QChar( 0x00116 );
    else if( entity == "eDot" ) return QChar( 0x02251 );
    else if( entity == "edot" ) return QChar( 0x00117 );
    else if( entity == "ee" ) return QChar( 0x02147 );
    else if( entity == "efDot" ) return QChar( 0x02252 );
    else if( entity == "Efr" ) return QChar( 0x1D508 );
    else if( entity == "efr" ) return QChar( 0x1D522 );
    else if( entity == "eg" ) return QChar( 0x02A9A );
    else if( entity == "Egrave" ) return QChar( 0x000C8 );
    else if( entity == "egrave" ) return QChar( 0x000E8 );
    else if( entity == "egs" ) return QChar( 0x02A96 );
    else if( entity == "egsdot" ) return QChar( 0x02A98 );
    else if( entity == "el" ) return QChar( 0x02A99 );
    else if( entity == "Element" ) return QChar( 0x02208 );
    else if( entity == "elinters" ) return QChar( 0x0FFFD );
    else if( entity == "ell" ) return QChar( 0x02113 );
    else if( entity == "els" ) return QChar( 0x02A95 );
    else if( entity == "elsdot" ) return QChar( 0x02A97 );
    else if( entity == "Emacr" ) return QChar( 0x00112 );
    else if( entity == "emacr" ) return QChar( 0x00113 );
    else if( entity == "empty" ) return QChar( 0x02205 );
    else if( entity == "emptyset" ) return QChar( 0x02205 );
    else if( entity == "EmptySmallSquare" ) return QChar( 0x025FB );
    else if( entity == "emptyv" ) return QChar( 0x02205 );
    else if( entity == "EmptyVerySmallSquare" ) return QChar( 0x025AB );
    else if( entity == "emsp" ) return QChar( 0x02003 );
    else if( entity == "emsp13" ) return QChar( 0x02004 );
    else if( entity == "emsp14" ) return QChar( 0x02005 );
    else if( entity == "ENG" ) return QChar( 0x0014A );
    else if( entity == "eng" ) return QChar( 0x0014B );
    else if( entity == "ensp" ) return QChar( 0x02002 );
    else if( entity == "Eogon" ) return QChar( 0x00118 );
    else if( entity == "eogon" ) return QChar( 0x00119 );
    else if( entity == "Eopf" ) return QChar( 0x1D53C );
    else if( entity == "eopf" ) return QChar( 0x1D556 );
    else if( entity == "epar" ) return QChar( 0x022D5 );
    else if( entity == "eparsl" ) return QChar( 0x029E3 );
    else if( entity == "eplus" ) return QChar( 0x02A71 );
    else if( entity == "epsi" ) return QChar( 0x003F5 );
    else if( entity == "epsiv" ) return QChar( 0x003B5 );
    else if( entity == "eqcirc" ) return QChar( 0x02256 );
    else if( entity == "eqcolon" ) return QChar( 0x02255 );
    else if( entity == "eqsim" ) return QChar( 0x02242 );
    else if( entity == "eqslantgtr" ) return QChar( 0x02A96 );
    else if( entity == "eqslantless" ) return QChar( 0x02A95 );
    else if( entity == "Equal" ) return QChar( 0x02A75 );
    else if( entity == "equals" ) return QChar( 0x0003D );
    else if( entity == "EqualTilde" ) return QChar( 0x02242 );
    else if( entity == "equest" ) return QChar( 0x0225F );
    else if( entity == "Equilibrium" ) return QChar( 0x021CC );
    else if( entity == "equiv" ) return QChar( 0x02261 );
    else if( entity == "equivDD" ) return QChar( 0x02A78 );
    else if( entity == "eqvparsl" ) return QChar( 0x029E5 );
    else if( entity == "erarr" ) return QChar( 0x02971 );
    else if( entity == "erDot" ) return QChar( 0x02253 );
    else if( entity == "Escr" ) return QChar( 0x02130 );
    else if( entity == "escr" ) return QChar( 0x0212F );
    else if( entity == "esdot" ) return QChar( 0x02250 );
    else if( entity == "Esim" ) return QChar( 0x02A73 );
    else if( entity == "esim" ) return QChar( 0x02242 );
    else if( entity == "eta" ) return QChar( 0x003B7 );
    else if( entity == "ETH" ) return QChar( 0x000D0 );
    else if( entity == "eth" ) return QChar( 0x000F0 );
    else if( entity == "Euml" ) return QChar( 0x000CB );
    else if( entity == "euml" ) return QChar( 0x000EB );
    else if( entity == "excl" ) return QChar( 0x00021 );
    else if( entity == "exist" ) return QChar( 0x02203 );
    else if( entity == "Exists" ) return QChar( 0x02203 );
    else if( entity == "expectation" ) return QChar( 0x02130 );
    else if( entity == "ExponentialE" ) return QChar( 0x02147 );
    else if( entity == "exponentiale" ) return QChar( 0x02147 );
    else if( entity == "fallingdotseq" ) return QChar( 0x02252 );
    else if( entity == "Fcy" ) return QChar( 0x00424 );
    else if( entity == "fcy" ) return QChar( 0x00444 );
    else if( entity == "female" ) return QChar( 0x02640 );
    else if( entity == "ffilig" ) return QChar( 0x0FB03 );
    else if( entity == "fflig" ) return QChar( 0x0FB00 );
    else if( entity == "ffllig" ) return QChar( 0x0FB04 );
    else if( entity == "Ffr" ) return QChar( 0x1D509 );
    else if( entity == "ffr" ) return QChar( 0x1D523 );
    else if( entity == "filig" ) return QChar( 0x0FB01 );
    else if( entity == "FilledSmallSquare" ) return QChar( 0x025FC );
    else if( entity == "FilledVerySmallSquare" ) return QChar( 0x025AA );
    else if( entity == "flat" ) return QChar( 0x0266D );
    else if( entity == "fllig" ) return QChar( 0x0FB02 );
    else if( entity == "fltns" ) return QChar( 0x025B1 );
    else if( entity == "fnof" ) return QChar( 0x00192 );
    else if( entity == "Fopf" ) return QChar( 0x1D53D );
    else if( entity == "fopf" ) return QChar( 0x1D557 );
    else if( entity == "ForAll" ) return QChar( 0x02200 );
    else if( entity == "forall" ) return QChar( 0x02200 );
    else if( entity == "fork" ) return QChar( 0x022D4 );
    else if( entity == "forkv" ) return QChar( 0x02AD9 );
    else if( entity == "Fouriertrf" ) return QChar( 0x02131 );
    else if( entity == "fpartint" ) return QChar( 0x02A0D );
    else if( entity == "frac12" ) return QChar( 0x000BD );
    else if( entity == "frac13" ) return QChar( 0x02153 );
    else if( entity == "frac14" ) return QChar( 0x000BC );
    else if( entity == "frac15" ) return QChar( 0x02155 );
    else if( entity == "frac16" ) return QChar( 0x02159 );
    else if( entity == "frac18" ) return QChar( 0x0215B );
    else if( entity == "frac23" ) return QChar( 0x02154 );
    else if( entity == "frac25" ) return QChar( 0x02156 );
    else if( entity == "frac34" ) return QChar( 0x000BE );
    else if( entity == "frac35" ) return QChar( 0x02157 );
    else if( entity == "frac38" ) return QChar( 0x0215C );
    else if( entity == "frac45" ) return QChar( 0x02158 );
    else if( entity == "frac56" ) return QChar( 0x0215A );
    else if( entity == "frac58" ) return QChar( 0x0215D );
    else if( entity == "frac78" ) return QChar( 0x0215E );
    else if( entity == "frown" ) return QChar( 0x02322 );
    else if( entity == "Fscr" ) return QChar( 0x02131 );
    else if( entity == "fscr" ) return QChar( 0x1D4BB );
    else if( entity == "gacute" ) return QChar( 0x001F5 );
    else if( entity == "Gamma" ) return QChar( 0x00393 );
    else if( entity == "gamma" ) return QChar( 0x003B3 );
    else if( entity == "Gammad" ) return QChar( 0x003DC );
    else if( entity == "gammad" ) return QChar( 0x003DD );
    else if( entity == "gap" ) return QChar( 0x02A86 );
    else if( entity == "Gbreve" ) return QChar( 0x0011E );
    else if( entity == "gbreve" ) return QChar( 0x0011F );
    else if( entity == "Gcedil" ) return QChar( 0x00122 );
    else if( entity == "Gcirc" ) return QChar( 0x0011C );
    else if( entity == "gcirc" ) return QChar( 0x0011D );
    else if( entity == "Gcy" ) return QChar( 0x00413 );
    else if( entity == "gcy" ) return QChar( 0x00433 );
    else if( entity == "Gdot" ) return QChar( 0x00120 );
    else if( entity == "gdot" ) return QChar( 0x00121 );
    else if( entity == "gE" ) return QChar( 0x02267 );
    else if( entity == "ge" ) return QChar( 0x02265 );
    else if( entity == "gEl" ) return QChar( 0x02A8C );
    else if( entity == "gel" ) return QChar( 0x022DB );
    else if( entity == "geq" ) return QChar( 0x02265 );
    else if( entity == "geqq" ) return QChar( 0x02267 );
    else if( entity == "geqslant" ) return QChar( 0x02A7E );
    else if( entity == "ges" ) return QChar( 0x02A7E );
    else if( entity == "gescc" ) return QChar( 0x02AA9 );
    else if( entity == "gesdot" ) return QChar( 0x02A80 );
    else if( entity == "gesdoto" ) return QChar( 0x02A82 );
    else if( entity == "gesdotol" ) return QChar( 0x02A84 );
    else if( entity == "gesles" ) return QChar( 0x02A94 );
    else if( entity == "Gfr" ) return QChar( 0x1D50A );
    else if( entity == "gfr" ) return QChar( 0x1D524 );
    else if( entity == "Gg" ) return QChar( 0x022D9 );
    else if( entity == "gg" ) return QChar( 0x0226B );
    else if( entity == "ggg" ) return QChar( 0x022D9 );
    else if( entity == "gimel" ) return QChar( 0x02137 );
    else if( entity == "GJcy" ) return QChar( 0x00403 );
    else if( entity == "gjcy" ) return QChar( 0x00453 );
    else if( entity == "gl" ) return QChar( 0x02277 );
    else if( entity == "gla" ) return QChar( 0x02AA5 );
    else if( entity == "glE" ) return QChar( 0x02A92 );
    else if( entity == "glj" ) return QChar( 0x02AA4 );
    else if( entity == "gnap" ) return QChar( 0x02A8A );
    else if( entity == "gnapprox" ) return QChar( 0x02A8A );
    else if( entity == "gnE" ) return QChar( 0x02269 );
    else if( entity == "gne" ) return QChar( 0x02A88 );
    else if( entity == "gneq" ) return QChar( 0x02A88 );
    else if( entity == "gneqq" ) return QChar( 0x02269 );
    else if( entity == "gnsim" ) return QChar( 0x022E7 );
    else if( entity == "Gopf" ) return QChar( 0x1D53E );
    else if( entity == "gopf" ) return QChar( 0x1D558 );
    else if( entity == "grave" ) return QChar( 0x00060 );
    else if( entity == "GreaterEqual" ) return QChar( 0x02265 );
    else if( entity == "GreaterEqualLess" ) return QChar( 0x022DB );
    else if( entity == "GreaterFullEqual" ) return QChar( 0x02267 );
    else if( entity == "GreaterGreater" ) return QChar( 0x02AA2 );
    else if( entity == "GreaterLess" ) return QChar( 0x02277 );
    else if( entity == "GreaterSlantEqual" ) return QChar( 0x02A7E );
    else if( entity == "GreaterTilde" ) return QChar( 0x02273 );
    else if( entity == "Gscr" ) return QChar( 0x1D4A2 );
    else if( entity == "gscr" ) return QChar( 0x0210A );
    else if( entity == "gsim" ) return QChar( 0x02273 );
    else if( entity == "gsime" ) return QChar( 0x02A8E );
    else if( entity == "gsiml" ) return QChar( 0x02A90 );
    else if( entity == "Gt" ) return QChar( 0x0226B );
    else if( entity == "gt" ) return QChar( 0x0003E );
    else if( entity == "gtcc" ) return QChar( 0x02AA7 );
    else if( entity == "gtcir" ) return QChar( 0x02A7A );
    else if( entity == "gtdot" ) return QChar( 0x022D7 );
    else if( entity == "gtlPar" ) return QChar( 0x02995 );
    else if( entity == "gtquest" ) return QChar( 0x02A7C );
    else if( entity == "gtrapprox" ) return QChar( 0x02A86 );
    else if( entity == "gtrarr" ) return QChar( 0x02978 );
    else if( entity == "gtrdot" ) return QChar( 0x022D7 );
    else if( entity == "gtreqless" ) return QChar( 0x022DB );
    else if( entity == "gtreqqless" ) return QChar( 0x02A8C );
    else if( entity == "gtrless" ) return QChar( 0x02277 );
    else if( entity == "gtrsim" ) return QChar( 0x02273 );
    else if( entity == "Hacek" ) return QChar( 0x002C7 );
    else if( entity == "hairsp" ) return QChar( 0x0200A );
    else if( entity == "half" ) return QChar( 0x000BD );
    else if( entity == "hamilt" ) return QChar( 0x0210B );
    else if( entity == "HARDcy" ) return QChar( 0x0042A );
    else if( entity == "hardcy" ) return QChar( 0x0044A );
    else if( entity == "hArr" ) return QChar( 0x021D4 );
    else if( entity == "harr" ) return QChar( 0x02194 );
    else if( entity == "harrcir" ) return QChar( 0x02948 );
    else if( entity == "harrw" ) return QChar( 0x021AD );
    else if( entity == "Hat" ) return QChar( 0x0005E );
    else if( entity == "hbar" ) return QChar( 0x0210F );
    else if( entity == "Hcirc" ) return QChar( 0x00124 );
    else if( entity == "hcirc" ) return QChar( 0x00125 );
    else if( entity == "hearts" ) return QChar( 0x02665 );
    else if( entity == "heartsuit" ) return QChar( 0x02665 );
    else if( entity == "hellip" ) return QChar( 0x02026 );
    else if( entity == "hercon" ) return QChar( 0x022B9 );
    else if( entity == "Hfr" ) return QChar( 0x0210C );
    else if( entity == "hfr" ) return QChar( 0x1D525 );
    else if( entity == "HilbertSpace" ) return QChar( 0x0210B );
    else if( entity == "hksearow" ) return QChar( 0x02925 );
    else if( entity == "hkswarow" ) return QChar( 0x02926 );
    else if( entity == "hoarr" ) return QChar( 0x021FF );
    else if( entity == "homtht" ) return QChar( 0x0223B );
    else if( entity == "hookleftarrow" ) return QChar( 0x021A9 );
    else if( entity == "hookrightarrow" ) return QChar( 0x021AA );
    else if( entity == "Hopf" ) return QChar( 0x0210D );
    else if( entity == "hopf" ) return QChar( 0x1D559 );
    else if( entity == "horbar" ) return QChar( 0x02015 );
    else if( entity == "HorizontalLine" ) return QChar( 0x02500 );
    else if( entity == "Hscr" ) return QChar( 0x0210B );
    else if( entity == "hscr" ) return QChar( 0x1D4BD );
    else if( entity == "hslash" ) return QChar( 0x0210F );
    else if( entity == "Hstrok" ) return QChar( 0x00126 );
    else if( entity == "hstrok" ) return QChar( 0x00127 );
    else if( entity == "HumpDownHump" ) return QChar( 0x0224E );
    else if( entity == "HumpEqual" ) return QChar( 0x0224F );
    else if( entity == "hybull" ) return QChar( 0x02043 );
    else if( entity == "hyphen" ) return QChar( 0x02010 );
    else if( entity == "Iacute" ) return QChar( 0x000CD );
    else if( entity == "iacute" ) return QChar( 0x000ED );
    else if( entity == "ic" ) return QChar( 0x02063 );
    else if( entity == "Icirc" ) return QChar( 0x000CE );
    else if( entity == "icirc" ) return QChar( 0x000EE );
    else if( entity == "Icy" ) return QChar( 0x00418 );
    else if( entity == "icy" ) return QChar( 0x00438 );
    else if( entity == "Idot" ) return QChar( 0x00130 );
    else if( entity == "IEcy" ) return QChar( 0x00415 );
    else if( entity == "iecy" ) return QChar( 0x00435 );
    else if( entity == "iexcl" ) return QChar( 0x000A1 );
    else if( entity == "iff" ) return QChar( 0x021D4 );
    else if( entity == "Ifr" ) return QChar( 0x02111 );
    else if( entity == "ifr" ) return QChar( 0x1D526 );
    else if( entity == "Igrave" ) return QChar( 0x000CC );
    else if( entity == "igrave" ) return QChar( 0x000EC );
    else if( entity == "ii" ) return QChar( 0x02148 );
    else if( entity == "iiiint" ) return QChar( 0x02A0C );
    else if( entity == "iiint" ) return QChar( 0x0222D );
    else if( entity == "iinfin" ) return QChar( 0x029DC );
    else if( entity == "iiota" ) return QChar( 0x02129 );
    else if( entity == "IJlig" ) return QChar( 0x00132 );
    else if( entity == "ijlig" ) return QChar( 0x00133 );
    else if( entity == "Im" ) return QChar( 0x02111 );
    else if( entity == "Imacr" ) return QChar( 0x0012A );
    else if( entity == "imacr" ) return QChar( 0x0012B );
    else if( entity == "image" ) return QChar( 0x02111 );
    else if( entity == "ImaginaryI" ) return QChar( 0x02148 );
    else if( entity == "imagline" ) return QChar( 0x02110 );
    else if( entity == "imagpart" ) return QChar( 0x02111 );
    else if( entity == "imath" ) return QChar( 0x00131 );
    else if( entity == "imof" ) return QChar( 0x022B7 );
    else if( entity == "imped" ) return QChar( 0x001B5 );
    else if( entity == "Implies" ) return QChar( 0x021D2 );
    else if( entity == "in" ) return QChar( 0x02208 );
    else if( entity == "incare" ) return QChar( 0x02105 );
    else if( entity == "infin" ) return QChar( 0x0221E );
    else if( entity == "infintie" ) return QChar( 0x029DD );
    else if( entity == "inodot" ) return QChar( 0x00131 );
    else if( entity == "Int" ) return QChar( 0x0222C );
    else if( entity == "int" ) return QChar( 0x0222B );
    else if( entity == "intcal" ) return QChar( 0x022BA );
    else if( entity == "integers" ) return QChar( 0x02124 );
    else if( entity == "Integral" ) return QChar( 0x0222B );
    else if( entity == "intercal" ) return QChar( 0x022BA );
    else if( entity == "Intersection" ) return QChar( 0x022C2 );
    else if( entity == "intlarhk" ) return QChar( 0x02A17 );
    else if( entity == "intprod" ) return QChar( 0x02A3C );
    else if( entity == "InvisibleComma" ) return QChar( 0x02063 );
    else if( entity == "InvisibleTimes" ) return QChar( 0x02062 );
    else if( entity == "IOcy" ) return QChar( 0x00401 );
    else if( entity == "iocy" ) return QChar( 0x00451 );
    else if( entity == "Iogon" ) return QChar( 0x0012E );
    else if( entity == "iogon" ) return QChar( 0x0012F );
    else if( entity == "Iopf" ) return QChar( 0x1D540 );
    else if( entity == "iopf" ) return QChar( 0x1D55A );
    else if( entity == "iota" ) return QChar( 0x003B9 );
    else if( entity == "iprod" ) return QChar( 0x02A3C );
    else if( entity == "iquest" ) return QChar( 0x000BF );
    else if( entity == "Iscr" ) return QChar( 0x02110 );
    else if( entity == "iscr" ) return QChar( 0x1D4BE );
    else if( entity == "isin" ) return QChar( 0x02208 );
    else if( entity == "isindot" ) return QChar( 0x022F5 );
    else if( entity == "isinE" ) return QChar( 0x022F9 );
    else if( entity == "isins" ) return QChar( 0x022F4 );
    else if( entity == "isinsv" ) return QChar( 0x022F3 );
    else if( entity == "isinv" ) return QChar( 0x02208 );
    else if( entity == "it" ) return QChar( 0x02062 );
    else if( entity == "Itilde" ) return QChar( 0x00128 );
    else if( entity == "itilde" ) return QChar( 0x00129 );
    else if( entity == "Iukcy" ) return QChar( 0x00406 );
    else if( entity == "iukcy" ) return QChar( 0x00456 );
    else if( entity == "Iuml" ) return QChar( 0x000CF );
    else if( entity == "iuml" ) return QChar( 0x000EF );
    else if( entity == "Jcirc" ) return QChar( 0x00134 );
    else if( entity == "jcirc" ) return QChar( 0x00135 );
    else if( entity == "Jcy" ) return QChar( 0x00419 );
    else if( entity == "jcy" ) return QChar( 0x00439 );
    else if( entity == "Jfr" ) return QChar( 0x1D50D );
    else if( entity == "jfr" ) return QChar( 0x1D527 );
    else if( entity == "jmath" ) return QChar( 0x0006A );
    else if( entity == "Jopf" ) return QChar( 0x1D541 );
    else if( entity == "jopf" ) return QChar( 0x1D55B );
    else if( entity == "Jscr" ) return QChar( 0x1D4A5 );
    else if( entity == "jscr" ) return QChar( 0x1D4BF );
    else if( entity == "Jsercy" ) return QChar( 0x00408 );
    else if( entity == "jsercy" ) return QChar( 0x00458 );
    else if( entity == "Jukcy" ) return QChar( 0x00404 );
    else if( entity == "jukcy" ) return QChar( 0x00454 );
    else if( entity == "kappa" ) return QChar( 0x003BA );
    else if( entity == "kappav" ) return QChar( 0x003F0 );
    else if( entity == "Kcedil" ) return QChar( 0x00136 );
    else if( entity == "kcedil" ) return QChar( 0x00137 );
    else if( entity == "Kcy" ) return QChar( 0x0041A );
    else if( entity == "kcy" ) return QChar( 0x0043A );
    else if( entity == "Kfr" ) return QChar( 0x1D50E );
    else if( entity == "kfr" ) return QChar( 0x1D528 );
    else if( entity == "kgreen" ) return QChar( 0x00138 );
    else if( entity == "KHcy" ) return QChar( 0x00425 );
    else if( entity == "khcy" ) return QChar( 0x00445 );
    else if( entity == "KJcy" ) return QChar( 0x0040C );
    else if( entity == "kjcy" ) return QChar( 0x0045C );
    else if( entity == "Kopf" ) return QChar( 0x1D542 );
    else if( entity == "kopf" ) return QChar( 0x1D55C );
    else if( entity == "Kscr" ) return QChar( 0x1D4A6 );
    else if( entity == "kscr" ) return QChar( 0x1D4C0 );
    else if( entity == "lAarr" ) return QChar( 0x021DA );
    else if( entity == "Lacute" ) return QChar( 0x00139 );
    else if( entity == "lacute" ) return QChar( 0x0013A );
    else if( entity == "laemptyv" ) return QChar( 0x029B4 );
    else if( entity == "lagran" ) return QChar( 0x02112 );
    else if( entity == "Lambda" ) return QChar( 0x0039B );
    else if( entity == "lambda" ) return QChar( 0x003BB );
    else if( entity == "Lang" ) return QChar( 0x0300A );
    else if( entity == "lang" ) return QChar( 0x02329 );
    else if( entity == "langd" ) return QChar( 0x02991 );
    else if( entity == "langle" ) return QChar( 0x02329 );
    else if( entity == "lap" ) return QChar( 0x02A85 );
    else if( entity == "Laplacetrf" ) return QChar( 0x02112 );
    else if( entity == "laquo" ) return QChar( 0x000AB );
    else if( entity == "Larr" ) return QChar( 0x0219E );
    else if( entity == "lArr" ) return QChar( 0x021D0 );
    else if( entity == "larr" ) return QChar( 0x02190 );
    else if( entity == "larrb" ) return QChar( 0x021E4 );
    else if( entity == "larrbfs" ) return QChar( 0x0291F );
    else if( entity == "larrfs" ) return QChar( 0x0291D );
    else if( entity == "larrhk" ) return QChar( 0x021A9 );
    else if( entity == "larrlp" ) return QChar( 0x021AB );
    else if( entity == "larrpl" ) return QChar( 0x02939 );
    else if( entity == "larrsim" ) return QChar( 0x02973 );
    else if( entity == "larrtl" ) return QChar( 0x021A2 );
    else if( entity == "lat" ) return QChar( 0x02AAB );
    else if( entity == "lAtail" ) return QChar( 0x0291B );
    else if( entity == "latail" ) return QChar( 0x02919 );
    else if( entity == "late" ) return QChar( 0x02AAD );
    else if( entity == "lBarr" ) return QChar( 0x0290E );
    else if( entity == "lbarr" ) return QChar( 0x0290C );
    else if( entity == "lbbrk" ) return QChar( 0x03014 );
    else if( entity == "lbrace" ) return QChar( 0x0007B );
    else if( entity == "lbrack" ) return QChar( 0x0005B );
    else if( entity == "lbrke" ) return QChar( 0x0298B );
    else if( entity == "lbrksld" ) return QChar( 0x0298F );
    else if( entity == "lbrkslu" ) return QChar( 0x0298D );
    else if( entity == "Lcaron" ) return QChar( 0x0013D );
    else if( entity == "lcaron" ) return QChar( 0x0013E );
    else if( entity == "Lcedil" ) return QChar( 0x0013B );
    else if( entity == "lcedil" ) return QChar( 0x0013C );
    else if( entity == "lceil" ) return QChar( 0x02308 );
    else if( entity == "lcub" ) return QChar( 0x0007B );
    else if( entity == "Lcy" ) return QChar( 0x0041B );
    else if( entity == "lcy" ) return QChar( 0x0043B );
    else if( entity == "ldca" ) return QChar( 0x02936 );
    else if( entity == "ldquo" ) return QChar( 0x0201C );
    else if( entity == "ldquor" ) return QChar( 0x0201E );
    else if( entity == "ldrdhar" ) return QChar( 0x02967 );
    else if( entity == "ldrushar" ) return QChar( 0x0294B );
    else if( entity == "ldsh" ) return QChar( 0x021B2 );
    else if( entity == "lE" ) return QChar( 0x02266 );
    else if( entity == "le" ) return QChar( 0x02264 );
    else if( entity == "LeftAngleBracket" ) return QChar( 0x02329 );
    else if( entity == "LeftArrow" ) return QChar( 0x02190 );
    else if( entity == "Leftarrow" ) return QChar( 0x021D0 );
    else if( entity == "leftarrow" ) return QChar( 0x02190 );
    else if( entity == "LeftArrowBar" ) return QChar( 0x021E4 );
    else if( entity == "LeftArrowRightArrow" ) return QChar( 0x021C6 );
    else if( entity == "leftarrowtail" ) return QChar( 0x021A2 );
    else if( entity == "LeftCeiling" ) return QChar( 0x02308 );
    else if( entity == "LeftDoubleBracket" ) return QChar( 0x0301A );
    else if( entity == "LeftDownTeeVector" ) return QChar( 0x02961 );
    else if( entity == "LeftDownVector" ) return QChar( 0x021C3 );
    else if( entity == "LeftDownVectorBar" ) return QChar( 0x02959 );
    else if( entity == "LeftFloor" ) return QChar( 0x0230A );
    else if( entity == "leftharpoondown" ) return QChar( 0x021BD );
    else if( entity == "leftharpoonup" ) return QChar( 0x021BC );
    else if( entity == "leftleftarrows" ) return QChar( 0x021C7 );
    else if( entity == "LeftRightArrow" ) return QChar( 0x02194 );
    else if( entity == "Leftrightarrow" ) return QChar( 0x021D4 );
    else if( entity == "leftrightarrow" ) return QChar( 0x02194 );
    else if( entity == "leftrightarrows" ) return QChar( 0x021C6 );
    else if( entity == "leftrightharpoons" ) return QChar( 0x021CB );
    else if( entity == "leftrightsquigarrow" ) return QChar( 0x021AD );
    else if( entity == "LeftRightVector" ) return QChar( 0x0294E );
    else if( entity == "LeftTee" ) return QChar( 0x022A3 );
    else if( entity == "LeftTeeArrow" ) return QChar( 0x021A4 );
    else if( entity == "LeftTeeVector" ) return QChar( 0x0295A );
    else if( entity == "leftthreetimes" ) return QChar( 0x022CB );
    else if( entity == "LeftTriangle" ) return QChar( 0x022B2 );
    else if( entity == "LeftTriangleBar" ) return QChar( 0x029CF );
    else if( entity == "LeftTriangleEqual" ) return QChar( 0x022B4 );
    else if( entity == "LeftUpDownVector" ) return QChar( 0x02951 );
    else if( entity == "LeftUpTeeVector" ) return QChar( 0x02960 );
    else if( entity == "LeftUpVector" ) return QChar( 0x021BF );
    else if( entity == "LeftUpVectorBar" ) return QChar( 0x02958 );
    else if( entity == "LeftVector" ) return QChar( 0x021BC );
    else if( entity == "LeftVectorBar" ) return QChar( 0x02952 );
    else if( entity == "lEg" ) return QChar( 0x02A8B );
    else if( entity == "leg" ) return QChar( 0x022DA );
    else if( entity == "leq" ) return QChar( 0x02264 );
    else if( entity == "leqq" ) return QChar( 0x02266 );
    else if( entity == "leqslant" ) return QChar( 0x02A7D );
    else if( entity == "les" ) return QChar( 0x02A7D );
    else if( entity == "lescc" ) return QChar( 0x02AA8 );
    else if( entity == "lesdot" ) return QChar( 0x02A7F );
    else if( entity == "lesdoto" ) return QChar( 0x02A81 );
    else if( entity == "lesdotor" ) return QChar( 0x02A83 );
    else if( entity == "lesges" ) return QChar( 0x02A93 );
    else if( entity == "lessapprox" ) return QChar( 0x02A85 );
    else if( entity == "lessdot" ) return QChar( 0x022D6 );
    else if( entity == "lesseqgtr" ) return QChar( 0x022DA );
    else if( entity == "lesseqqgtr" ) return QChar( 0x02A8B );
    else if( entity == "LessEqualGreater" ) return QChar( 0x022DA );
    else if( entity == "LessFullEqual" ) return QChar( 0x02266 );
    else if( entity == "LessGreater" ) return QChar( 0x02276 );
    else if( entity == "lessgtr" ) return QChar( 0x02276 );
    else if( entity == "LessLess" ) return QChar( 0x02AA1 );
    else if( entity == "lesssim" ) return QChar( 0x02272 );
    else if( entity == "LessSlantEqual" ) return QChar( 0x02A7D );
    else if( entity == "LessTilde" ) return QChar( 0x02272 );
    else if( entity == "lfisht" ) return QChar( 0x0297C );
    else if( entity == "lfloor" ) return QChar( 0x0230A );
    else if( entity == "Lfr" ) return QChar( 0x1D50F );
    else if( entity == "lfr" ) return QChar( 0x1D529 );
    else if( entity == "lg" ) return QChar( 0x02276 );
    else if( entity == "lgE" ) return QChar( 0x02A91 );
    else if( entity == "lHar" ) return QChar( 0x02962 );
    else if( entity == "lhard" ) return QChar( 0x021BD );
    else if( entity == "lharu" ) return QChar( 0x021BC );
    else if( entity == "lharul" ) return QChar( 0x0296A );
    else if( entity == "lhblk" ) return QChar( 0x02584 );
    else if( entity == "LJcy" ) return QChar( 0x00409 );
    else if( entity == "ljcy" ) return QChar( 0x00459 );
    else if( entity == "Ll" ) return QChar( 0x022D8 );
    else if( entity == "ll" ) return QChar( 0x0226A );
    else if( entity == "llarr" ) return QChar( 0x021C7 );
    else if( entity == "llcorner" ) return QChar( 0x0231E );
    else if( entity == "Lleftarrow" ) return QChar( 0x021DA );
    else if( entity == "llhard" ) return QChar( 0x0296B );
    else if( entity == "lltri" ) return QChar( 0x025FA );
    else if( entity == "Lmidot" ) return QChar( 0x0013F );
    else if( entity == "lmidot" ) return QChar( 0x00140 );
    else if( entity == "lmoust" ) return QChar( 0x023B0 );
    else if( entity == "lmoustache" ) return QChar( 0x023B0 );
    else if( entity == "lnap" ) return QChar( 0x02A89 );
    else if( entity == "lnapprox" ) return QChar( 0x02A89 );
    else if( entity == "lnE" ) return QChar( 0x02268 );
    else if( entity == "lne" ) return QChar( 0x02A87 );
    else if( entity == "lneq" ) return QChar( 0x02A87 );
    else if( entity == "lneqq" ) return QChar( 0x02268 );
    else if( entity == "lnsim" ) return QChar( 0x022E6 );
    else if( entity == "loang" ) return QChar( 0x03018 );
    else if( entity == "loarr" ) return QChar( 0x021FD );
    else if( entity == "lobrk" ) return QChar( 0x0301A );
    else if( entity == "LongLeftArrow" ) return QChar( 0x027F5 );
    else if( entity == "Longleftarrow" ) return QChar( 0x027F8 );
    else if( entity == "longleftarrow" ) return QChar( 0x027F5 );
    else if( entity == "LongLeftRightArrow" ) return QChar( 0x027F7 );
    else if( entity == "Longleftrightarrow" ) return QChar( 0x027FA );
    else if( entity == "longleftrightarrow" ) return QChar( 0x027F7 );
    else if( entity == "longmapsto" ) return QChar( 0x027FC );
    else if( entity == "LongRightArrow" ) return QChar( 0x027F6 );
    else if( entity == "Longrightarrow" ) return QChar( 0x027F9 );
    else if( entity == "longrightarrow" ) return QChar( 0x027F6 );
    else if( entity == "looparrowleft" ) return QChar( 0x021AB );
    else if( entity == "looparrowright" ) return QChar( 0x021AC );
    else if( entity == "lopar" ) return QChar( 0x02985 );
    else if( entity == "Lopf" ) return QChar( 0x1D543 );
    else if( entity == "lopf" ) return QChar( 0x1D55D );
    else if( entity == "loplus" ) return QChar( 0x02A2D );
    else if( entity == "lotimes" ) return QChar( 0x02A34 );
    else if( entity == "lowast" ) return QChar( 0x02217 );
    else if( entity == "lowbar" ) return QChar( 0x0005F );
    else if( entity == "LowerLeftArrow" ) return QChar( 0x02199 );
    else if( entity == "LowerRightArrow" ) return QChar( 0x02198 );
    else if( entity == "loz" ) return QChar( 0x025CA );
    else if( entity == "lozenge" ) return QChar( 0x025CA );
    else if( entity == "lozf" ) return QChar( 0x029EB );
    else if( entity == "lpar" ) return QChar( 0x00028 );
    else if( entity == "lparlt" ) return QChar( 0x02993 );
    else if( entity == "lrarr" ) return QChar( 0x021C6 );
    else if( entity == "lrcorner" ) return QChar( 0x0231F );
    else if( entity == "lrhar" ) return QChar( 0x021CB );
    else if( entity == "lrhard" ) return QChar( 0x0296D );
    else if( entity == "lrtri" ) return QChar( 0x022BF );
    else if( entity == "Lscr" ) return QChar( 0x02112 );
    else if( entity == "lscr" ) return QChar( 0x1D4C1 );
    else if( entity == "Lsh" ) return QChar( 0x021B0 );
    else if( entity == "lsh" ) return QChar( 0x021B0 );
    else if( entity == "lsim" ) return QChar( 0x02272 );
    else if( entity == "lsime" ) return QChar( 0x02A8D );
    else if( entity == "lsimg" ) return QChar( 0x02A8F );
    else if( entity == "lsqb" ) return QChar( 0x0005B );
    else if( entity == "lsquo" ) return QChar( 0x02018 );
    else if( entity == "lsquor" ) return QChar( 0x0201A );
    else if( entity == "Lstrok" ) return QChar( 0x00141 );
    else if( entity == "lstrok" ) return QChar( 0x00142 );
    else if( entity == "Lt" ) return QChar( 0x0226A );
    else if( entity == "lt" ) return QChar( 0x0003C );
    else if( entity == "ltcc" ) return QChar( 0x02AA6 );
    else if( entity == "ltcir" ) return QChar( 0x02A79 );
    else if( entity == "ltdot" ) return QChar( 0x022D6 );
    else if( entity == "lthree" ) return QChar( 0x022CB );
    else if( entity == "ltimes" ) return QChar( 0x022C9 );
    else if( entity == "ltlarr" ) return QChar( 0x02976 );
    else if( entity == "ltquest" ) return QChar( 0x02A7B );
    else if( entity == "ltri" ) return QChar( 0x025C3 );
    else if( entity == "ltrie" ) return QChar( 0x022B4 );
    else if( entity == "ltrif" ) return QChar( 0x025C2 );
    else if( entity == "ltrPar" ) return QChar( 0x02996 );
    else if( entity == "lurdshar" ) return QChar( 0x0294A );
    else if( entity == "luruhar" ) return QChar( 0x02966 );
    else if( entity == "macr" ) return QChar( 0x000AF );
    else if( entity == "male" ) return QChar( 0x02642 );
    else if( entity == "malt" ) return QChar( 0x02720 );
    else if( entity == "maltese" ) return QChar( 0x02720 );
    else if( entity == "Map" ) return QChar( 0x02905 );
    else if( entity == "map" ) return QChar( 0x021A6 );
    else if( entity == "mapsto" ) return QChar( 0x021A6 );
    else if( entity == "mapstodown" ) return QChar( 0x021A7 );
    else if( entity == "mapstoleft" ) return QChar( 0x021A4 );
    else if( entity == "mapstoup" ) return QChar( 0x021A5 );
    else if( entity == "marker" ) return QChar( 0x025AE );
    else if( entity == "mcomma" ) return QChar( 0x02A29 );
    else if( entity == "Mcy" ) return QChar( 0x0041C );
    else if( entity == "mcy" ) return QChar( 0x0043C );
    else if( entity == "mdash" ) return QChar( 0x02014 );
    else if( entity == "mDDot" ) return QChar( 0x0223A );
    else if( entity == "measuredangle" ) return QChar( 0x02221 );
    else if( entity == "MediumSpace" ) return QChar( 0x0205F );
    else if( entity == "Mellintrf" ) return QChar( 0x02133 );
    else if( entity == "Mfr" ) return QChar( 0x1D510 );
    else if( entity == "mfr" ) return QChar( 0x1D52A );
    else if( entity == "mho" ) return QChar( 0x02127 );
    else if( entity == "micro" ) return QChar( 0x000B5 );
    else if( entity == "mid" ) return QChar( 0x02223 );
    else if( entity == "midast" ) return QChar( 0x0002A );
    else if( entity == "midcir" ) return QChar( 0x02AF0 );
    else if( entity == "middot" ) return QChar( 0x000B7 );
    else if( entity == "minus" ) return QChar( 0x02212 );
    else if( entity == "minusb" ) return QChar( 0x0229F );
    else if( entity == "minusd" ) return QChar( 0x02238 );
    else if( entity == "minusdu" ) return QChar( 0x02A2A );
    else if( entity == "MinusPlus" ) return QChar( 0x02213 );
    else if( entity == "mlcp" ) return QChar( 0x02ADB );
    else if( entity == "mldr" ) return QChar( 0x02026 );
    else if( entity == "mnplus" ) return QChar( 0x02213 );
    else if( entity == "models" ) return QChar( 0x022A7 );
    else if( entity == "Mopf" ) return QChar( 0x1D544 );
    else if( entity == "mopf" ) return QChar( 0x1D55E );
    else if( entity == "mp" ) return QChar( 0x02213 );
    else if( entity == "Mscr" ) return QChar( 0x02133 );
    else if( entity == "mscr" ) return QChar( 0x1D4C2 );
    else if( entity == "mstpos" ) return QChar( 0x0223E );
    else if( entity == "mu" ) return QChar( 0x003BC );
    else if( entity == "multimap" ) return QChar( 0x022B8 );
    else if( entity == "mumap" ) return QChar( 0x022B8 );
    else if( entity == "nabla" ) return QChar( 0x02207 );
    else if( entity == "Nacute" ) return QChar( 0x00143 );
    else if( entity == "nacute" ) return QChar( 0x00144 );
    else if( entity == "nap" ) return QChar( 0x02249 );
    else if( entity == "napos" ) return QChar( 0x00149 );
    else if( entity == "napprox" ) return QChar( 0x02249 );
    else if( entity == "natur" ) return QChar( 0x0266E );
    else if( entity == "natural" ) return QChar( 0x0266E );
    else if( entity == "naturals" ) return QChar( 0x02115 );
    else if( entity == "nbsp" ) return QChar( 0x000A0 );
    else if( entity == "ncap" ) return QChar( 0x02A43 );
    else if( entity == "Ncaron" ) return QChar( 0x00147 );
    else if( entity == "ncaron" ) return QChar( 0x00148 );
    else if( entity == "Ncedil" ) return QChar( 0x00145 );
    else if( entity == "ncedil" ) return QChar( 0x00146 );
    else if( entity == "ncong" ) return QChar( 0x02247 );
    else if( entity == "ncup" ) return QChar( 0x02A42 );
    else if( entity == "Ncy" ) return QChar( 0x0041D );
    else if( entity == "ncy" ) return QChar( 0x0043D );
    else if( entity == "ndash" ) return QChar( 0x02013 );
    else if( entity == "ne" ) return QChar( 0x02260 );
    else if( entity == "nearhk" ) return QChar( 0x02924 );
    else if( entity == "neArr" ) return QChar( 0x021D7 );
    else if( entity == "nearr" ) return QChar( 0x02197 );
    else if( entity == "nearrow" ) return QChar( 0x02197 );
    else if( entity == "NegativeMediumSpace" ) return QChar( 0x0200B );
    else if( entity == "NegativeThickSpace" ) return QChar( 0x0200B );
    else if( entity == "NegativeThinSpace" ) return QChar( 0x0200B );
    else if( entity == "NegativeVeryThinSpace" ) return QChar( 0x0200B );
    else if( entity == "nequiv" ) return QChar( 0x02262 );
    else if( entity == "nesear" ) return QChar( 0x02928 );
    else if( entity == "NestedGreaterGreater" ) return QChar( 0x0226B );
    else if( entity == "NestedLessLess" ) return QChar( 0x0226A );
    else if( entity == "NewLine" ) return QChar( 0x0000A );
    else if( entity == "nexist" ) return QChar( 0x02204 );
    else if( entity == "nexists" ) return QChar( 0x02204 );
    else if( entity == "Nfr" ) return QChar( 0x1D511 );
    else if( entity == "nfr" ) return QChar( 0x1D52B );
    else if( entity == "nge" ) return QChar( 0x02271 );
    else if( entity == "ngeq" ) return QChar( 0x02271 );
    else if( entity == "ngsim" ) return QChar( 0x02275 );
    else if( entity == "ngt" ) return QChar( 0x0226F );
    else if( entity == "ngtr" ) return QChar( 0x0226F );
    else if( entity == "nhArr" ) return QChar( 0x021CE );
    else if( entity == "nharr" ) return QChar( 0x021AE );
    else if( entity == "nhpar" ) return QChar( 0x02AF2 );
    else if( entity == "ni" ) return QChar( 0x0220B );
    else if( entity == "nis" ) return QChar( 0x022FC );
    else if( entity == "nisd" ) return QChar( 0x022FA );
    else if( entity == "niv" ) return QChar( 0x0220B );
    else if( entity == "NJcy" ) return QChar( 0x0040A );
    else if( entity == "njcy" ) return QChar( 0x0045A );
    else if( entity == "nlArr" ) return QChar( 0x021CD );
    else if( entity == "nlarr" ) return QChar( 0x0219A );
    else if( entity == "nldr" ) return QChar( 0x02025 );
    else if( entity == "nle" ) return QChar( 0x02270 );
    else if( entity == "nLeftarrow" ) return QChar( 0x021CD );
    else if( entity == "nleftarrow" ) return QChar( 0x0219A );
    else if( entity == "nLeftrightarrow" ) return QChar( 0x021CE );
    else if( entity == "nleftrightarrow" ) return QChar( 0x021AE );
    else if( entity == "nleq" ) return QChar( 0x02270 );
    else if( entity == "nless" ) return QChar( 0x0226E );
    else if( entity == "nlsim" ) return QChar( 0x02274 );
    else if( entity == "nlt" ) return QChar( 0x0226E );
    else if( entity == "nltri" ) return QChar( 0x022EA );
    else if( entity == "nltrie" ) return QChar( 0x022EC );
    else if( entity == "nmid" ) return QChar( 0x02224 );
    else if( entity == "NoBreak" ) return QChar( 0x02060 );
    else if( entity == "NonBreakingSpace" ) return QChar( 0x000A0 );
    else if( entity == "Nopf" ) return QChar( 0x02115 );
    else if( entity == "nopf" ) return QChar( 0x1D55F );
    else if( entity == "Not" ) return QChar( 0x02AEC );
    else if( entity == "not" ) return QChar( 0x000AC );
    else if( entity == "NotCongruent" ) return QChar( 0x02262 );
    else if( entity == "NotCupCap" ) return QChar( 0x0226D );
    else if( entity == "NotDoubleVerticalBar" ) return QChar( 0x02226 );
    else if( entity == "NotElement" ) return QChar( 0x02209 );
    else if( entity == "NotEqual" ) return QChar( 0x02260 );
    else if( entity == "NotExists" ) return QChar( 0x02204 );
    else if( entity == "NotGreater" ) return QChar( 0x0226F );
    else if( entity == "NotGreaterEqual" ) return QChar( 0x02271 );
    else if( entity == "NotGreaterLess" ) return QChar( 0x02279 );
    else if( entity == "NotGreaterTilde" ) return QChar( 0x02275 );
    else if( entity == "notin" ) return QChar( 0x02209 );
    else if( entity == "notinva" ) return QChar( 0x02209 );
    else if( entity == "notinvb" ) return QChar( 0x022F7 );
    else if( entity == "notinvc" ) return QChar( 0x022F6 );
    else if( entity == "NotLeftTriangle" ) return QChar( 0x022EA );
    else if( entity == "NotLeftTriangleEqual" ) return QChar( 0x022EC );
    else if( entity == "NotLess" ) return QChar( 0x0226E );
    else if( entity == "NotLessEqual" ) return QChar( 0x02270 );
    else if( entity == "NotLessGreater" ) return QChar( 0x02278 );
    else if( entity == "NotLessTilde" ) return QChar( 0x02274 );
    else if( entity == "notni" ) return QChar( 0x0220C );
    else if( entity == "notniva" ) return QChar( 0x0220C );
    else if( entity == "notnivb" ) return QChar( 0x022FE );
    else if( entity == "notnivc" ) return QChar( 0x022FD );
    else if( entity == "NotPrecedes" ) return QChar( 0x02280 );
    else if( entity == "NotPrecedesSlantEqual" ) return QChar( 0x022E0 );
    else if( entity == "NotReverseElement" ) return QChar( 0x0220C );
    else if( entity == "NotRightTriangle" ) return QChar( 0x022EB );
    else if( entity == "NotRightTriangleEqual" ) return QChar( 0x022ED );
    else if( entity == "NotSquareSubsetEqual" ) return QChar( 0x022E2 );
    else if( entity == "NotSquareSupersetEqual" ) return QChar( 0x022E3 );
    else if( entity == "NotSubsetEqual" ) return QChar( 0x02288 );
    else if( entity == "NotSucceeds" ) return QChar( 0x02281 );
    else if( entity == "NotSucceedsSlantEqual" ) return QChar( 0x022E1 );
    else if( entity == "NotSupersetEqual" ) return QChar( 0x02289 );
    else if( entity == "NotTilde" ) return QChar( 0x02241 );
    else if( entity == "NotTildeEqual" ) return QChar( 0x02244 );
    else if( entity == "NotTildeFullEqual" ) return QChar( 0x02247 );
    else if( entity == "NotTildeTilde" ) return QChar( 0x02249 );
    else if( entity == "NotVerticalBar" ) return QChar( 0x02224 );
    else if( entity == "npar" ) return QChar( 0x02226 );
    else if( entity == "nparallel" ) return QChar( 0x02226 );
    else if( entity == "npolint" ) return QChar( 0x02A14 );
    else if( entity == "npr" ) return QChar( 0x02280 );
    else if( entity == "nprcue" ) return QChar( 0x022E0 );
    else if( entity == "nprec" ) return QChar( 0x02280 );
    else if( entity == "nrArr" ) return QChar( 0x021CF );
    else if( entity == "nrarr" ) return QChar( 0x0219B );
    else if( entity == "nRightarrow" ) return QChar( 0x021CF );
    else if( entity == "nrightarrow" ) return QChar( 0x0219B );
    else if( entity == "nrtri" ) return QChar( 0x022EB );
    else if( entity == "nrtrie" ) return QChar( 0x022ED );
    else if( entity == "nsc" ) return QChar( 0x02281 );
    else if( entity == "nsccue" ) return QChar( 0x022E1 );
    else if( entity == "Nscr" ) return QChar( 0x1D4A9 );
    else if( entity == "nscr" ) return QChar( 0x1D4C3 );
    else if( entity == "nshortmid" ) return QChar( 0x02224 );
    else if( entity == "nshortparallel" ) return QChar( 0x02226 );
    else if( entity == "nsim" ) return QChar( 0x02241 );
    else if( entity == "nsime" ) return QChar( 0x02244 );
    else if( entity == "nsimeq" ) return QChar( 0x02244 );
    else if( entity == "nsmid" ) return QChar( 0x02224 );
    else if( entity == "nspar" ) return QChar( 0x02226 );
    else if( entity == "nsqsube" ) return QChar( 0x022E2 );
    else if( entity == "nsqsupe" ) return QChar( 0x022E3 );
    else if( entity == "nsub" ) return QChar( 0x02284 );
    else if( entity == "nsube" ) return QChar( 0x02288 );
    else if( entity == "nsubseteq" ) return QChar( 0x02288 );
    else if( entity == "nsucc" ) return QChar( 0x02281 );
    else if( entity == "nsup" ) return QChar( 0x02285 );
    else if( entity == "nsupe" ) return QChar( 0x02289 );
    else if( entity == "nsupseteq" ) return QChar( 0x02289 );
    else if( entity == "ntgl" ) return QChar( 0x02279 );
    else if( entity == "Ntilde" ) return QChar( 0x000D1 );
    else if( entity == "ntilde" ) return QChar( 0x000F1 );
    else if( entity == "ntlg" ) return QChar( 0x02278 );
    else if( entity == "ntriangleleft" ) return QChar( 0x022EA );
    else if( entity == "ntrianglelefteq" ) return QChar( 0x022EC );
    else if( entity == "ntriangleright" ) return QChar( 0x022EB );
    else if( entity == "ntrianglerighteq" ) return QChar( 0x022ED );
    else if( entity == "nu" ) return QChar( 0x003BD );
    else if( entity == "num" ) return QChar( 0x00023 );
    else if( entity == "numero" ) return QChar( 0x02116 );
    else if( entity == "numsp" ) return QChar( 0x02007 );
    else if( entity == "nVDash" ) return QChar( 0x022AF );
    else if( entity == "nVdash" ) return QChar( 0x022AE );
    else if( entity == "nvDash" ) return QChar( 0x022AD );
    else if( entity == "nvdash" ) return QChar( 0x022AC );
    else if( entity == "nvHarr" ) return QChar( 0x02904 );
    else if( entity == "nvinfin" ) return QChar( 0x029DE );
    else if( entity == "nvlArr" ) return QChar( 0x02902 );
    else if( entity == "nvrArr" ) return QChar( 0x02903 );
    else if( entity == "nwarhk" ) return QChar( 0x02923 );
    else if( entity == "nwArr" ) return QChar( 0x021D6 );
    else if( entity == "nwarr" ) return QChar( 0x02196 );
    else if( entity == "nwarrow" ) return QChar( 0x02196 );
    else if( entity == "nwnear" ) return QChar( 0x02927 );
    else if( entity == "Oacute" ) return QChar( 0x000D3 );
    else if( entity == "oacute" ) return QChar( 0x000F3 );
    else if( entity == "oast" ) return QChar( 0x0229B );
    else if( entity == "ocir" ) return QChar( 0x0229A );
    else if( entity == "Ocirc" ) return QChar( 0x000D4 );
    else if( entity == "ocirc" ) return QChar( 0x000F4 );
    else if( entity == "Ocy" ) return QChar( 0x0041E );
    else if( entity == "ocy" ) return QChar( 0x0043E );
    else if( entity == "odash" ) return QChar( 0x0229D );
    else if( entity == "Odblac" ) return QChar( 0x00150 );
    else if( entity == "odblac" ) return QChar( 0x00151 );
    else if( entity == "odiv" ) return QChar( 0x02A38 );
    else if( entity == "odot" ) return QChar( 0x02299 );
    else if( entity == "odsold" ) return QChar( 0x029BC );
    else if( entity == "OElig" ) return QChar( 0x00152 );
    else if( entity == "oelig" ) return QChar( 0x00153 );
    else if( entity == "ofcir" ) return QChar( 0x029BF );
    else if( entity == "Ofr" ) return QChar( 0x1D512 );
    else if( entity == "ofr" ) return QChar( 0x1D52C );
    else if( entity == "ogon" ) return QChar( 0x002DB );
    else if( entity == "Ograve" ) return QChar( 0x000D2 );
    else if( entity == "ograve" ) return QChar( 0x000F2 );
    else if( entity == "ogt" ) return QChar( 0x029C1 );
    else if( entity == "ohbar" ) return QChar( 0x029B5 );
    else if( entity == "ohm" ) return QChar( 0x02126 );
    else if( entity == "oint" ) return QChar( 0x0222E );
    else if( entity == "olarr" ) return QChar( 0x021BA );
    else if( entity == "olcir" ) return QChar( 0x029BE );
    else if( entity == "olcross" ) return QChar( 0x029BB );
    else if( entity == "olt" ) return QChar( 0x029C0 );
    else if( entity == "Omacr" ) return QChar( 0x0014C );
    else if( entity == "omacr" ) return QChar( 0x0014D );
    else if( entity == "Omega" ) return QChar( 0x003A9 );
    else if( entity == "omega" ) return QChar( 0x003C9 );
    else if( entity == "omid" ) return QChar( 0x029B6 );
    else if( entity == "ominus" ) return QChar( 0x02296 );
    else if( entity == "Oopf" ) return QChar( 0x1D546 );
    else if( entity == "oopf" ) return QChar( 0x1D560 );
    else if( entity == "opar" ) return QChar( 0x029B7 );
    else if( entity == "OpenCurlyDoubleQuote" ) return QChar( 0x0201C );
    else if( entity == "OpenCurlyQuote" ) return QChar( 0x02018 );
    else if( entity == "operp" ) return QChar( 0x029B9 );
    else if( entity == "oplus" ) return QChar( 0x02295 );
    else if( entity == "Or" ) return QChar( 0x02A54 );
    else if( entity == "or" ) return QChar( 0x02228 );
    else if( entity == "orarr" ) return QChar( 0x021BB );
    else if( entity == "ord" ) return QChar( 0x02A5D );
    else if( entity == "order" ) return QChar( 0x02134 );
    else if( entity == "orderof" ) return QChar( 0x02134 );
    else if( entity == "ordf" ) return QChar( 0x000AA );
    else if( entity == "ordm" ) return QChar( 0x000BA );
    else if( entity == "origof" ) return QChar( 0x022B6 );
    else if( entity == "oror" ) return QChar( 0x02A56 );
    else if( entity == "orslope" ) return QChar( 0x02A57 );
    else if( entity == "orv" ) return QChar( 0x02A5B );
    else if( entity == "oS" ) return QChar( 0x024C8 );
    else if( entity == "Oscr" ) return QChar( 0x1D4AA );
    else if( entity == "oscr" ) return QChar( 0x02134 );
    else if( entity == "Oslash" ) return QChar( 0x000D8 );
    else if( entity == "oslash" ) return QChar( 0x000F8 );
    else if( entity == "osol" ) return QChar( 0x02298 );
    else if( entity == "Otilde" ) return QChar( 0x000D5 );
    else if( entity == "otilde" ) return QChar( 0x000F5 );
    else if( entity == "Otimes" ) return QChar( 0x02A37 );
    else if( entity == "otimes" ) return QChar( 0x02297 );
    else if( entity == "otimesas" ) return QChar( 0x02A36 );
    else if( entity == "Ouml" ) return QChar( 0x000D6 );
    else if( entity == "ouml" ) return QChar( 0x000F6 );
    else if( entity == "ovbar" ) return QChar( 0x0233D );
    else if( entity == "OverBar" ) return QChar( 0x000AF );
    else if( entity == "OverBrace" ) return QChar( 0x0FE37 );
    else if( entity == "OverBracket" ) return QChar( 0x023B4 );
    else if( entity == "OverParenthesis" ) return QChar( 0x0FE35 );
    else if( entity == "par" ) return QChar( 0x02225 );
    else if( entity == "para" ) return QChar( 0x000B6 );
    else if( entity == "parallel" ) return QChar( 0x02225 );
    else if( entity == "parsim" ) return QChar( 0x02AF3 );
    else if( entity == "parsl" ) return QChar( 0x02AFD );
    else if( entity == "part" ) return QChar( 0x02202 );
    else if( entity == "PartialD" ) return QChar( 0x02202 );
    else if( entity == "Pcy" ) return QChar( 0x0041F );
    else if( entity == "pcy" ) return QChar( 0x0043F );
    else if( entity == "percnt" ) return QChar( 0x00025 );
    else if( entity == "period" ) return QChar( 0x0002E );
    else if( entity == "permil" ) return QChar( 0x02030 );
    else if( entity == "perp" ) return QChar( 0x022A5 );
    else if( entity == "pertenk" ) return QChar( 0x02031 );
    else if( entity == "Pfr" ) return QChar( 0x1D513 );
    else if( entity == "pfr" ) return QChar( 0x1D52D );
    else if( entity == "Phi" ) return QChar( 0x003A6 );
    else if( entity == "phi" ) return QChar( 0x003D5 );
    else if( entity == "phiv" ) return QChar( 0x003C6 );
    else if( entity == "phmmat" ) return QChar( 0x02133 );
    else if( entity == "phone" ) return QChar( 0x0260E );
    else if( entity == "Pi" ) return QChar( 0x003A0 );
    else if( entity == "pi" ) return QChar( 0x003C0 );
    else if( entity == "pitchfork" ) return QChar( 0x022D4 );
    else if( entity == "piv" ) return QChar( 0x003D6 );
    else if( entity == "planck" ) return QChar( 0x0210F );
    else if( entity == "planckh" ) return QChar( 0x0210E );
    else if( entity == "plankv" ) return QChar( 0x0210F );
    else if( entity == "plus" ) return QChar( 0x0002B );
    else if( entity == "plusacir" ) return QChar( 0x02A23 );
    else if( entity == "plusb" ) return QChar( 0x0229E );
    else if( entity == "pluscir" ) return QChar( 0x02A22 );
    else if( entity == "plusdo" ) return QChar( 0x02214 );
    else if( entity == "plusdu" ) return QChar( 0x02A25 );
    else if( entity == "pluse" ) return QChar( 0x02A72 );
    else if( entity == "PlusMinus" ) return QChar( 0x000B1 );
    else if( entity == "plusmn" ) return QChar( 0x000B1 );
    else if( entity == "plussim" ) return QChar( 0x02A26 );
    else if( entity == "plustwo" ) return QChar( 0x02A27 );
    else if( entity == "pm" ) return QChar( 0x000B1 );
    else if( entity == "Poincareplane" ) return QChar( 0x0210C );
    else if( entity == "pointint" ) return QChar( 0x02A15 );
    else if( entity == "Popf" ) return QChar( 0x02119 );
    else if( entity == "popf" ) return QChar( 0x1D561 );
    else if( entity == "pound" ) return QChar( 0x000A3 );
    else if( entity == "Pr" ) return QChar( 0x02ABB );
    else if( entity == "pr" ) return QChar( 0x0227A );
    else if( entity == "prap" ) return QChar( 0x02AB7 );
    else if( entity == "prcue" ) return QChar( 0x0227C );
    else if( entity == "prE" ) return QChar( 0x02AB3 );
    else if( entity == "pre" ) return QChar( 0x02AAF );
    else if( entity == "prec" ) return QChar( 0x0227A );
    else if( entity == "precapprox" ) return QChar( 0x02AB7 );
    else if( entity == "preccurlyeq" ) return QChar( 0x0227C );
    else if( entity == "Precedes" ) return QChar( 0x0227A );
    else if( entity == "PrecedesEqual" ) return QChar( 0x02AAF );
    else if( entity == "PrecedesSlantEqual" ) return QChar( 0x0227C );
    else if( entity == "PrecedesTilde" ) return QChar( 0x0227E );
    else if( entity == "preceq" ) return QChar( 0x02AAF );
    else if( entity == "precnapprox" ) return QChar( 0x02AB9 );
    else if( entity == "precneqq" ) return QChar( 0x02AB5 );
    else if( entity == "precnsim" ) return QChar( 0x022E8 );
    else if( entity == "precsim" ) return QChar( 0x0227E );
    else if( entity == "Prime" ) return QChar( 0x02033 );
    else if( entity == "prime" ) return QChar( 0x02032 );
    else if( entity == "primes" ) return QChar( 0x02119 );
    else if( entity == "prnap" ) return QChar( 0x02AB9 );
    else if( entity == "prnE" ) return QChar( 0x02AB5 );
    else if( entity == "prnsim" ) return QChar( 0x022E8 );
    else if( entity == "prod" ) return QChar( 0x0220F );
    else if( entity == "Product" ) return QChar( 0x0220F );
    else if( entity == "profalar" ) return QChar( 0x0232E );
    else if( entity == "profline" ) return QChar( 0x02312 );
    else if( entity == "profsurf" ) return QChar( 0x02313 );
    else if( entity == "prop" ) return QChar( 0x0221D );
    else if( entity == "Proportion" ) return QChar( 0x02237 );
    else if( entity == "Proportional" ) return QChar( 0x0221D );
    else if( entity == "propto" ) return QChar( 0x0221D );
    else if( entity == "prsim" ) return QChar( 0x0227E );
    else if( entity == "prurel" ) return QChar( 0x022B0 );
    else if( entity == "Pscr" ) return QChar( 0x1D4AB );
    else if( entity == "pscr" ) return QChar( 0x1D4C5 );
    else if( entity == "Psi" ) return QChar( 0x003A8 );
    else if( entity == "psi" ) return QChar( 0x003C8 );
    else if( entity == "puncsp" ) return QChar( 0x02008 );
    else if( entity == "Qfr" ) return QChar( 0x1D514 );
    else if( entity == "qfr" ) return QChar( 0x1D52E );
    else if( entity == "qint" ) return QChar( 0x02A0C );
    else if( entity == "Qopf" ) return QChar( 0x0211A );
    else if( entity == "qopf" ) return QChar( 0x1D562 );
    else if( entity == "qprime" ) return QChar( 0x02057 );
    else if( entity == "Qscr" ) return QChar( 0x1D4AC );
    else if( entity == "qscr" ) return QChar( 0x1D4C6 );
    else if( entity == "quaternions" ) return QChar( 0x0210D );
    else if( entity == "quatint" ) return QChar( 0x02A16 );
    else if( entity == "quest" ) return QChar( 0x0003F );
    else if( entity == "questeq" ) return QChar( 0x0225F );
    else if( entity == "quot" ) return QChar( 0x00022 );
    else if( entity == "rAarr" ) return QChar( 0x021DB );
    else if( entity == "race" ) return QChar( 0x029DA );
    else if( entity == "Racute" ) return QChar( 0x00154 );
    else if( entity == "racute" ) return QChar( 0x00155 );
    else if( entity == "radic" ) return QChar( 0x0221A );
    else if( entity == "raemptyv" ) return QChar( 0x029B3 );
    else if( entity == "Rang" ) return QChar( 0x0300B );
    else if( entity == "rang" ) return QChar( 0x0232A );
    else if( entity == "rangd" ) return QChar( 0x02992 );
    else if( entity == "range" ) return QChar( 0x029A5 );
    else if( entity == "rangle" ) return QChar( 0x0232A );
    else if( entity == "raquo" ) return QChar( 0x000BB );
    else if( entity == "Rarr" ) return QChar( 0x021A0 );
    else if( entity == "rArr" ) return QChar( 0x021D2 );
    else if( entity == "rarr" ) return QChar( 0x02192 );
    else if( entity == "rarrap" ) return QChar( 0x02975 );
    else if( entity == "rarrb" ) return QChar( 0x021E5 );
    else if( entity == "rarrbfs" ) return QChar( 0x02920 );
    else if( entity == "rarrc" ) return QChar( 0x02933 );
    else if( entity == "rarrfs" ) return QChar( 0x0291E );
    else if( entity == "rarrhk" ) return QChar( 0x021AA );
    else if( entity == "rarrlp" ) return QChar( 0x021AC );
    else if( entity == "rarrpl" ) return QChar( 0x02945 );
    else if( entity == "rarrsim" ) return QChar( 0x02974 );
    else if( entity == "Rarrtl" ) return QChar( 0x02916 );
    else if( entity == "rarrtl" ) return QChar( 0x021A3 );
    else if( entity == "rarrw" ) return QChar( 0x0219D );
    else if( entity == "rAtail" ) return QChar( 0x0291C );
    else if( entity == "ratail" ) return QChar( 0x0291A );
    else if( entity == "ratio" ) return QChar( 0x02236 );
    else if( entity == "rationals" ) return QChar( 0x0211A );
    else if( entity == "RBarr" ) return QChar( 0x02910 );
    else if( entity == "rBarr" ) return QChar( 0x0290F );
    else if( entity == "rbarr" ) return QChar( 0x0290D );
    else if( entity == "rbbrk" ) return QChar( 0x03015 );
    else if( entity == "rbrace" ) return QChar( 0x0007D );
    else if( entity == "rbrack" ) return QChar( 0x0005D );
    else if( entity == "rbrke" ) return QChar( 0x0298C );
    else if( entity == "rbrksld" ) return QChar( 0x0298E );
    else if( entity == "rbrkslu" ) return QChar( 0x02990 );
    else if( entity == "Rcaron" ) return QChar( 0x00158 );
    else if( entity == "rcaron" ) return QChar( 0x00159 );
    else if( entity == "Rcedil" ) return QChar( 0x00156 );
    else if( entity == "rcedil" ) return QChar( 0x00157 );
    else if( entity == "rceil" ) return QChar( 0x02309 );
    else if( entity == "rcub" ) return QChar( 0x0007D );
    else if( entity == "Rcy" ) return QChar( 0x00420 );
    else if( entity == "rcy" ) return QChar( 0x00440 );
    else if( entity == "rdca" ) return QChar( 0x02937 );
    else if( entity == "rdldhar" ) return QChar( 0x02969 );
    else if( entity == "rdquo" ) return QChar( 0x0201D );
    else if( entity == "rdquor" ) return QChar( 0x0201D );
    else if( entity == "rdsh" ) return QChar( 0x021B3 );
    else if( entity == "Re" ) return QChar( 0x0211C );
    else if( entity == "real" ) return QChar( 0x0211C );
    else if( entity == "realine" ) return QChar( 0x0211B );
    else if( entity == "realpart" ) return QChar( 0x0211C );
    else if( entity == "reals" ) return QChar( 0x0211D );
    else if( entity == "rect" ) return QChar( 0x025AD );
    else if( entity == "reg" ) return QChar( 0x000AE );
    else if( entity == "ReverseElement" ) return QChar( 0x0220B );
    else if( entity == "ReverseEquilibrium" ) return QChar( 0x021CB );
    else if( entity == "ReverseUpEquilibrium" ) return QChar( 0x0296F );
    else if( entity == "rfisht" ) return QChar( 0x0297D );
    else if( entity == "rfloor" ) return QChar( 0x0230B );
    else if( entity == "Rfr" ) return QChar( 0x0211C );
    else if( entity == "rfr" ) return QChar( 0x1D52F );
    else if( entity == "rHar" ) return QChar( 0x02964 );
    else if( entity == "rhard" ) return QChar( 0x021C1 );
    else if( entity == "rharu" ) return QChar( 0x021C0 );
    else if( entity == "rharul" ) return QChar( 0x0296C );
    else if( entity == "rho" ) return QChar( 0x003C1 );
    else if( entity == "rhov" ) return QChar( 0x003F1 );
    else if( entity == "RightAngleBracket" ) return QChar( 0x0232A );
    else if( entity == "RightArrow" ) return QChar( 0x02192 );
    else if( entity == "Rightarrow" ) return QChar( 0x021D2 );
    else if( entity == "rightarrow" ) return QChar( 0x02192 );
    else if( entity == "RightArrowBar" ) return QChar( 0x021E5 );
    else if( entity == "RightArrowLeftArrow" ) return QChar( 0x021C4 );
    else if( entity == "rightarrowtail" ) return QChar( 0x021A3 );
    else if( entity == "RightCeiling" ) return QChar( 0x02309 );
    else if( entity == "RightDoubleBracket" ) return QChar( 0x0301B );
    else if( entity == "RightDownTeeVector" ) return QChar( 0x0295D );
    else if( entity == "RightDownVector" ) return QChar( 0x021C2 );
    else if( entity == "RightDownVectorBar" ) return QChar( 0x02955 );
    else if( entity == "RightFloor" ) return QChar( 0x0230B );
    else if( entity == "rightharpoondown" ) return QChar( 0x021C1 );
    else if( entity == "rightharpoonup" ) return QChar( 0x021C0 );
    else if( entity == "rightleftarrows" ) return QChar( 0x021C4 );
    else if( entity == "rightleftharpoons" ) return QChar( 0x021CC );
    else if( entity == "rightrightarrows" ) return QChar( 0x021C9 );
    else if( entity == "rightsquigarrow" ) return QChar( 0x0219D );
    else if( entity == "RightTee" ) return QChar( 0x022A2 );
    else if( entity == "RightTeeArrow" ) return QChar( 0x021A6 );
    else if( entity == "RightTeeVector" ) return QChar( 0x0295B );
    else if( entity == "rightthreetimes" ) return QChar( 0x022CC );
    else if( entity == "RightTriangle" ) return QChar( 0x022B3 );
    else if( entity == "RightTriangleBar" ) return QChar( 0x029D0 );
    else if( entity == "RightTriangleEqual" ) return QChar( 0x022B5 );
    else if( entity == "RightUpDownVector" ) return QChar( 0x0294F );
    else if( entity == "RightUpTeeVector" ) return QChar( 0x0295C );
    else if( entity == "RightUpVector" ) return QChar( 0x021BE );
    else if( entity == "RightUpVectorBar" ) return QChar( 0x02954 );
    else if( entity == "RightVector" ) return QChar( 0x021C0 );
    else if( entity == "RightVectorBar" ) return QChar( 0x02953 );
    else if( entity == "ring" ) return QChar( 0x002DA );
    else if( entity == "risingdotseq" ) return QChar( 0x02253 );
    else if( entity == "rlarr" ) return QChar( 0x021C4 );
    else if( entity == "rlhar" ) return QChar( 0x021CC );
    else if( entity == "rmoust" ) return QChar( 0x023B1 );
    else if( entity == "rmoustache" ) return QChar( 0x023B1 );
    else if( entity == "rnmid" ) return QChar( 0x02AEE );
    else if( entity == "roang" ) return QChar( 0x03019 );
    else if( entity == "roarr" ) return QChar( 0x021FE );
    else if( entity == "robrk" ) return QChar( 0x0301B );
    else if( entity == "ropar" ) return QChar( 0x02986 );
    else if( entity == "Ropf" ) return QChar( 0x0211D );
    else if( entity == "ropf" ) return QChar( 0x1D563 );
    else if( entity == "roplus" ) return QChar( 0x02A2E );
    else if( entity == "rotimes" ) return QChar( 0x02A35 );
    else if( entity == "RoundImplies" ) return QChar( 0x02970 );
    else if( entity == "rpar" ) return QChar( 0x00029 );
    else if( entity == "rpargt" ) return QChar( 0x02994 );
    else if( entity == "rppolint" ) return QChar( 0x02A12 );
    else if( entity == "rrarr" ) return QChar( 0x021C9 );
    else if( entity == "Rrightarrow" ) return QChar( 0x021DB );
    else if( entity == "Rscr" ) return QChar( 0x0211B );
    else if( entity == "rscr" ) return QChar( 0x1D4C7 );
    else if( entity == "Rsh" ) return QChar( 0x021B1 );
    else if( entity == "rsh" ) return QChar( 0x021B1 );
    else if( entity == "rsqb" ) return QChar( 0x0005D );
    else if( entity == "rsquo" ) return QChar( 0x02019 );
    else if( entity == "rsquor" ) return QChar( 0x02019 );
    else if( entity == "rthree" ) return QChar( 0x022CC );
    else if( entity == "rtimes" ) return QChar( 0x022CA );
    else if( entity == "rtri" ) return QChar( 0x025B9 );
    else if( entity == "rtrie" ) return QChar( 0x022B5 );
    else if( entity == "rtrif" ) return QChar( 0x025B8 );
    else if( entity == "rtriltri" ) return QChar( 0x029CE );
    else if( entity == "RuleDelayed" ) return QChar( 0x029F4 );
    else if( entity == "ruluhar" ) return QChar( 0x02968 );
    else if( entity == "rx" ) return QChar( 0x0211E );
    else if( entity == "Sacute" ) return QChar( 0x0015A );
    else if( entity == "sacute" ) return QChar( 0x0015B );
    else if( entity == "Sc" ) return QChar( 0x02ABC );
    else if( entity == "sc" ) return QChar( 0x0227B );
    else if( entity == "scap" ) return QChar( 0x02AB8 );
    else if( entity == "Scaron" ) return QChar( 0x00160 );
    else if( entity == "scaron" ) return QChar( 0x00161 );
    else if( entity == "sccue" ) return QChar( 0x0227D );
    else if( entity == "scE" ) return QChar( 0x02AB4 );
    else if( entity == "sce" ) return QChar( 0x02AB0 );
    else if( entity == "Scedil" ) return QChar( 0x0015E );
    else if( entity == "scedil" ) return QChar( 0x0015F );
    else if( entity == "Scirc" ) return QChar( 0x0015C );
    else if( entity == "scirc" ) return QChar( 0x0015D );
    else if( entity == "scnap" ) return QChar( 0x02ABA );
    else if( entity == "scnE" ) return QChar( 0x02AB6 );
    else if( entity == "scnsim" ) return QChar( 0x022E9 );
    else if( entity == "scpolint" ) return QChar( 0x02A13 );
    else if( entity == "scsim" ) return QChar( 0x0227F );
    else if( entity == "Scy" ) return QChar( 0x00421 );
    else if( entity == "scy" ) return QChar( 0x00441 );
    else if( entity == "sdot" ) return QChar( 0x022C5 );
    else if( entity == "sdotb" ) return QChar( 0x022A1 );
    else if( entity == "sdote" ) return QChar( 0x02A66 );
    else if( entity == "searhk" ) return QChar( 0x02925 );
    else if( entity == "seArr" ) return QChar( 0x021D8 );
    else if( entity == "searr" ) return QChar( 0x02198 );
    else if( entity == "searrow" ) return QChar( 0x02198 );
    else if( entity == "sect" ) return QChar( 0x000A7 );
    else if( entity == "semi" ) return QChar( 0x0003B );
    else if( entity == "seswar" ) return QChar( 0x02929 );
    else if( entity == "setminus" ) return QChar( 0x02216 );
    else if( entity == "setmn" ) return QChar( 0x02216 );
    else if( entity == "sext" ) return QChar( 0x02736 );
    else if( entity == "Sfr" ) return QChar( 0x1D516 );
    else if( entity == "sfr" ) return QChar( 0x1D530 );
    else if( entity == "sfrown" ) return QChar( 0x02322 );
    else if( entity == "sharp" ) return QChar( 0x0266F );
    else if( entity == "SHCHcy" ) return QChar( 0x00429 );
    else if( entity == "shchcy" ) return QChar( 0x00449 );
    else if( entity == "SHcy" ) return QChar( 0x00428 );
    else if( entity == "shcy" ) return QChar( 0x00448 );
    else if( entity == "ShortDownArrow" ) return QChar( 0x02193 );
    else if( entity == "ShortLeftArrow" ) return QChar( 0x02190 );
    else if( entity == "shortmid" ) return QChar( 0x02223 );
    else if( entity == "shortparallel" ) return QChar( 0x02225 );
    else if( entity == "ShortRightArrow" ) return QChar( 0x02192 );
    else if( entity == "ShortUpArrow" ) return QChar( 0x02191 );
    else if( entity == "shy" ) return QChar( 0x000AD );
    else if( entity == "Sigma" ) return QChar( 0x003A3 );
    else if( entity == "sigma" ) return QChar( 0x003C3 );
    else if( entity == "sigmav" ) return QChar( 0x003C2 );
    else if( entity == "sim" ) return QChar( 0x0223C );
    else if( entity == "simdot" ) return QChar( 0x02A6A );
    else if( entity == "sime" ) return QChar( 0x02243 );
    else if( entity == "simeq" ) return QChar( 0x02243 );
    else if( entity == "simg" ) return QChar( 0x02A9E );
    else if( entity == "simgE" ) return QChar( 0x02AA0 );
    else if( entity == "siml" ) return QChar( 0x02A9D );
    else if( entity == "simlE" ) return QChar( 0x02A9F );
    else if( entity == "simne" ) return QChar( 0x02246 );
    else if( entity == "simplus" ) return QChar( 0x02A24 );
    else if( entity == "simrarr" ) return QChar( 0x02972 );
    else if( entity == "slarr" ) return QChar( 0x02190 );
    else if( entity == "SmallCircle" ) return QChar( 0x02218 );
    else if( entity == "smallsetminus" ) return QChar( 0x02216 );
    else if( entity == "smashp" ) return QChar( 0x02A33 );
    else if( entity == "smeparsl" ) return QChar( 0x029E4 );
    else if( entity == "smid" ) return QChar( 0x02223 );
    else if( entity == "smile" ) return QChar( 0x02323 );
    else if( entity == "smt" ) return QChar( 0x02AAA );
    else if( entity == "smte" ) return QChar( 0x02AAC );
    else if( entity == "SOFTcy" ) return QChar( 0x0042C );
    else if( entity == "softcy" ) return QChar( 0x0044C );
    else if( entity == "sol" ) return QChar( 0x0002F );
    else if( entity == "solb" ) return QChar( 0x029C4 );
    else if( entity == "solbar" ) return QChar( 0x0233F );
    else if( entity == "Sopf" ) return QChar( 0x1D54A );
    else if( entity == "sopf" ) return QChar( 0x1D564 );
    else if( entity == "spades" ) return QChar( 0x02660 );
    else if( entity == "spadesuit" ) return QChar( 0x02660 );
    else if( entity == "spar" ) return QChar( 0x02225 );
    else if( entity == "sqcap" ) return QChar( 0x02293 );
    else if( entity == "sqcup" ) return QChar( 0x02294 );
    else if( entity == "Sqrt" ) return QChar( 0x0221A );
    else if( entity == "sqsub" ) return QChar( 0x0228F );
    else if( entity == "sqsube" ) return QChar( 0x02291 );
    else if( entity == "sqsubset" ) return QChar( 0x0228F );
    else if( entity == "sqsubseteq" ) return QChar( 0x02291 );
    else if( entity == "sqsup" ) return QChar( 0x02290 );
    else if( entity == "sqsupe" ) return QChar( 0x02292 );
    else if( entity == "sqsupset" ) return QChar( 0x02290 );
    else if( entity == "sqsupseteq" ) return QChar( 0x02292 );
    else if( entity == "squ" ) return QChar( 0x025A1 );
    else if( entity == "Square" ) return QChar( 0x025A1 );
    else if( entity == "square" ) return QChar( 0x025A1 );
    else if( entity == "SquareIntersection" ) return QChar( 0x02293 );
    else if( entity == "SquareSubset" ) return QChar( 0x0228F );
    else if( entity == "SquareSubsetEqual" ) return QChar( 0x02291 );
    else if( entity == "SquareSuperset" ) return QChar( 0x02290 );
    else if( entity == "SquareSupersetEqual" ) return QChar( 0x02292 );
    else if( entity == "SquareUnion" ) return QChar( 0x02294 );
    else if( entity == "squarf" ) return QChar( 0x025AA );
    else if( entity == "squf" ) return QChar( 0x025AA );
    else if( entity == "srarr" ) return QChar( 0x02192 );
    else if( entity == "Sscr" ) return QChar( 0x1D4AE );
    else if( entity == "sscr" ) return QChar( 0x1D4C8 );
    else if( entity == "ssetmn" ) return QChar( 0x02216 );
    else if( entity == "ssmile" ) return QChar( 0x02323 );
    else if( entity == "sstarf" ) return QChar( 0x022C6 );
    else if( entity == "Star" ) return QChar( 0x022C6 );
    else if( entity == "star" ) return QChar( 0x02606 );
    else if( entity == "starf" ) return QChar( 0x02605 );
    else if( entity == "straightepsilon" ) return QChar( 0x003F5 );
    else if( entity == "straightphi" ) return QChar( 0x003D5 );
    else if( entity == "strns" ) return QChar( 0x000AF );
    else if( entity == "Sub" ) return QChar( 0x022D0 );
    else if( entity == "sub" ) return QChar( 0x02282 );
    else if( entity == "subdot" ) return QChar( 0x02ABD );
    else if( entity == "subE" ) return QChar( 0x02AC5 );
    else if( entity == "sube" ) return QChar( 0x02286 );
    else if( entity == "subedot" ) return QChar( 0x02AC3 );
    else if( entity == "submult" ) return QChar( 0x02AC1 );
    else if( entity == "subnE" ) return QChar( 0x02ACB );
    else if( entity == "subne" ) return QChar( 0x0228A );
    else if( entity == "subplus" ) return QChar( 0x02ABF );
    else if( entity == "subrarr" ) return QChar( 0x02979 );
    else if( entity == "Subset" ) return QChar( 0x022D0 );
    else if( entity == "subset" ) return QChar( 0x02282 );
    else if( entity == "subseteq" ) return QChar( 0x02286 );
    else if( entity == "subseteqq" ) return QChar( 0x02AC5 );
    else if( entity == "SubsetEqual" ) return QChar( 0x02286 );
    else if( entity == "subsetneq" ) return QChar( 0x0228A );
    else if( entity == "subsetneqq" ) return QChar( 0x02ACB );
    else if( entity == "subsim" ) return QChar( 0x02AC7 );
    else if( entity == "subsub" ) return QChar( 0x02AD5 );
    else if( entity == "subsup" ) return QChar( 0x02AD3 );
    else if( entity == "succ" ) return QChar( 0x0227B );
    else if( entity == "succapprox" ) return QChar( 0x02AB8 );
    else if( entity == "succcurlyeq" ) return QChar( 0x0227D );
    else if( entity == "Succeeds" ) return QChar( 0x0227B );
    else if( entity == "SucceedsEqual" ) return QChar( 0x02AB0 );
    else if( entity == "SucceedsSlantEqual" ) return QChar( 0x0227D );
    else if( entity == "SucceedsTilde" ) return QChar( 0x0227F );
    else if( entity == "succeq" ) return QChar( 0x02AB0 );
    else if( entity == "succnapprox" ) return QChar( 0x02ABA );
    else if( entity == "succneqq" ) return QChar( 0x02AB6 );
    else if( entity == "succnsim" ) return QChar( 0x022E9 );
    else if( entity == "succsim" ) return QChar( 0x0227F );
    else if( entity == "SuchThat" ) return QChar( 0x0220B );
    else if( entity == "Sum" ) return QChar( 0x02211 );
    else if( entity == "sum" ) return QChar( 0x02211 );
    else if( entity == "sung" ) return QChar( 0x0266A );
    else if( entity == "Sup" ) return QChar( 0x022D1 );
    else if( entity == "sup" ) return QChar( 0x02283 );
    else if( entity == "sup1" ) return QChar( 0x000B9 );
    else if( entity == "sup2" ) return QChar( 0x000B2 );
    else if( entity == "sup3" ) return QChar( 0x000B3 );
    else if( entity == "supdot" ) return QChar( 0x02ABE );
    else if( entity == "supdsub" ) return QChar( 0x02AD8 );
    else if( entity == "supE" ) return QChar( 0x02AC6 );
    else if( entity == "supe" ) return QChar( 0x02287 );
    else if( entity == "supedot" ) return QChar( 0x02AC4 );
    else if( entity == "Superset" ) return QChar( 0x02283 );
    else if( entity == "SupersetEqual" ) return QChar( 0x02287 );
    else if( entity == "suphsub" ) return QChar( 0x02AD7 );
    else if( entity == "suplarr" ) return QChar( 0x0297B );
    else if( entity == "supmult" ) return QChar( 0x02AC2 );
    else if( entity == "supnE" ) return QChar( 0x02ACC );
    else if( entity == "supne" ) return QChar( 0x0228B );
    else if( entity == "supplus" ) return QChar( 0x02AC0 );
    else if( entity == "Supset" ) return QChar( 0x022D1 );
    else if( entity == "supset" ) return QChar( 0x02283 );
    else if( entity == "supseteq" ) return QChar( 0x02287 );
    else if( entity == "supseteqq" ) return QChar( 0x02AC6 );
    else if( entity == "supsetneq" ) return QChar( 0x0228B );
    else if( entity == "supsetneqq" ) return QChar( 0x02ACC );
    else if( entity == "supsim" ) return QChar( 0x02AC8 );
    else if( entity == "supsub" ) return QChar( 0x02AD4 );
    else if( entity == "supsup" ) return QChar( 0x02AD6 );
    else if( entity == "swarhk" ) return QChar( 0x02926 );
    else if( entity == "swArr" ) return QChar( 0x021D9 );
    else if( entity == "swarr" ) return QChar( 0x02199 );
    else if( entity == "swarrow" ) return QChar( 0x02199 );
    else if( entity == "swnwar" ) return QChar( 0x0292A );
    else if( entity == "szlig" ) return QChar( 0x000DF );
    else if( entity == "Tab" ) return QChar( 0x00009 );
    else if( entity == "target" ) return QChar( 0x02316 );
    else if( entity == "tau" ) return QChar( 0x003C4 );
    else if( entity == "tbrk" ) return QChar( 0x023B4 );
    else if( entity == "Tcaron" ) return QChar( 0x00164 );
    else if( entity == "tcaron" ) return QChar( 0x00165 );
    else if( entity == "Tcedil" ) return QChar( 0x00162 );
    else if( entity == "tcedil" ) return QChar( 0x00163 );
    else if( entity == "Tcy" ) return QChar( 0x00422 );
    else if( entity == "tcy" ) return QChar( 0x00442 );
    else if( entity == "tdot" ) return QChar( 0x020DB );
    else if( entity == "telrec" ) return QChar( 0x02315 );
    else if( entity == "Tfr" ) return QChar( 0x1D517 );
    else if( entity == "tfr" ) return QChar( 0x1D531 );
    else if( entity == "there4" ) return QChar( 0x02234 );
    else if( entity == "Therefore" ) return QChar( 0x02234 );
    else if( entity == "therefore" ) return QChar( 0x02234 );
    else if( entity == "Theta" ) return QChar( 0x00398 );
    else if( entity == "theta" ) return QChar( 0x003B8 );
    else if( entity == "thetav" ) return QChar( 0x003D1 );
    else if( entity == "thickapprox" ) return QChar( 0x02248 );
    else if( entity == "thicksim" ) return QChar( 0x0223C );
    else if( entity == "thinsp" ) return QChar( 0x02009 );
    else if( entity == "ThinSpace" ) return QChar( 0x02009 );
    else if( entity == "thkap" ) return QChar( 0x02248 );
    else if( entity == "thksim" ) return QChar( 0x0223C );
    else if( entity == "THORN" ) return QChar( 0x000DE );
    else if( entity == "thorn" ) return QChar( 0x000FE );
    else if( entity == "Tilde" ) return QChar( 0x0223C );
    else if( entity == "tilde" ) return QChar( 0x002DC );
    else if( entity == "TildeEqual" ) return QChar( 0x02243 );
    else if( entity == "TildeFullEqual" ) return QChar( 0x02245 );
    else if( entity == "TildeTilde" ) return QChar( 0x02248 );
    else if( entity == "times" ) return QChar( 0x000D7 );
    else if( entity == "timesb" ) return QChar( 0x022A0 );
    else if( entity == "timesbar" ) return QChar( 0x02A31 );
    else if( entity == "timesd" ) return QChar( 0x02A30 );
    else if( entity == "tint" ) return QChar( 0x0222D );
    else if( entity == "toea" ) return QChar( 0x02928 );
    else if( entity == "top" ) return QChar( 0x022A4 );
    else if( entity == "topbot" ) return QChar( 0x02336 );
    else if( entity == "topcir" ) return QChar( 0x02AF1 );
    else if( entity == "Topf" ) return QChar( 0x1D54B );
    else if( entity == "topf" ) return QChar( 0x1D565 );
    else if( entity == "topfork" ) return QChar( 0x02ADA );
    else if( entity == "tosa" ) return QChar( 0x02929 );
    else if( entity == "tprime" ) return QChar( 0x02034 );
    else if( entity == "trade" ) return QChar( 0x02122 );
    else if( entity == "triangle" ) return QChar( 0x025B5 );
    else if( entity == "triangledown" ) return QChar( 0x025BF );
    else if( entity == "triangleleft" ) return QChar( 0x025C3 );
    else if( entity == "trianglelefteq" ) return QChar( 0x022B4 );
    else if( entity == "triangleq" ) return QChar( 0x0225C );
    else if( entity == "triangleright" ) return QChar( 0x025B9 );
    else if( entity == "trianglerighteq" ) return QChar( 0x022B5 );
    else if( entity == "tridot" ) return QChar( 0x025EC );
    else if( entity == "trie" ) return QChar( 0x0225C );
    else if( entity == "triminus" ) return QChar( 0x02A3A );
    else if( entity == "TripleDot" ) return QChar( 0x020DB );
    else if( entity == "triplus" ) return QChar( 0x02A39 );
    else if( entity == "trisb" ) return QChar( 0x029CD );
    else if( entity == "tritime" ) return QChar( 0x02A3B );
    else if( entity == "trpezium" ) return QChar( 0x0FFFD );
    else if( entity == "Tscr" ) return QChar( 0x1D4AF );
    else if( entity == "tscr" ) return QChar( 0x1D4C9 );
    else if( entity == "TScy" ) return QChar( 0x00426 );
    else if( entity == "tscy" ) return QChar( 0x00446 );
    else if( entity == "TSHcy" ) return QChar( 0x0040B );
    else if( entity == "tshcy" ) return QChar( 0x0045B );
    else if( entity == "Tstrok" ) return QChar( 0x00166 );
    else if( entity == "tstrok" ) return QChar( 0x00167 );
    else if( entity == "twixt" ) return QChar( 0x0226C );
    else if( entity == "twoheadleftarrow" ) return QChar( 0x0219E );
    else if( entity == "twoheadrightarrow" ) return QChar( 0x021A0 );
    else if( entity == "Uacute" ) return QChar( 0x000DA );
    else if( entity == "uacute" ) return QChar( 0x000FA );
    else if( entity == "Uarr" ) return QChar( 0x0219F );
    else if( entity == "uArr" ) return QChar( 0x021D1 );
    else if( entity == "uarr" ) return QChar( 0x02191 );
    else if( entity == "Uarrocir" ) return QChar( 0x02949 );
    else if( entity == "Ubrcy" ) return QChar( 0x0040E );
    else if( entity == "ubrcy" ) return QChar( 0x0045E );
    else if( entity == "Ubreve" ) return QChar( 0x0016C );
    else if( entity == "ubreve" ) return QChar( 0x0016D );
    else if( entity == "Ucirc" ) return QChar( 0x000DB );
    else if( entity == "ucirc" ) return QChar( 0x000FB );
    else if( entity == "Ucy" ) return QChar( 0x00423 );
    else if( entity == "ucy" ) return QChar( 0x00443 );
    else if( entity == "udarr" ) return QChar( 0x021C5 );
    else if( entity == "Udblac" ) return QChar( 0x00170 );
    else if( entity == "udblac" ) return QChar( 0x00171 );
    else if( entity == "udhar" ) return QChar( 0x0296E );
    else if( entity == "ufisht" ) return QChar( 0x0297E );
    else if( entity == "Ufr" ) return QChar( 0x1D518 );
    else if( entity == "ufr" ) return QChar( 0x1D532 );
    else if( entity == "Ugrave" ) return QChar( 0x000D9 );
    else if( entity == "ugrave" ) return QChar( 0x000F9 );
    else if( entity == "uHar" ) return QChar( 0x02963 );
    else if( entity == "uharl" ) return QChar( 0x021BF );
    else if( entity == "uharr" ) return QChar( 0x021BE );
    else if( entity == "uhblk" ) return QChar( 0x02580 );
    else if( entity == "ulcorn" ) return QChar( 0x0231C );
    else if( entity == "ulcorner" ) return QChar( 0x0231C );
    else if( entity == "ulcrop" ) return QChar( 0x0230F );
    else if( entity == "ultri" ) return QChar( 0x025F8 );
    else if( entity == "Umacr" ) return QChar( 0x0016A );
    else if( entity == "umacr" ) return QChar( 0x0016B );
    else if( entity == "uml" ) return QChar( 0x000A8 );
    else if( entity == "UnderBar" ) return QChar( 0x00332 );
    else if( entity == "UnderBrace" ) return QChar( 0x0FE38 );
    else if( entity == "UnderBracket" ) return QChar( 0x023B5 );
    else if( entity == "UnderParenthesis" ) return QChar( 0x0FE36 );
    else if( entity == "Union" ) return QChar( 0x022C3 );
    else if( entity == "UnionPlus" ) return QChar( 0x0228E );
    else if( entity == "Uogon" ) return QChar( 0x00172 );
    else if( entity == "uogon" ) return QChar( 0x00173 );
    else if( entity == "Uopf" ) return QChar( 0x1D54C );
    else if( entity == "uopf" ) return QChar( 0x1D566 );
    else if( entity == "UpArrow" ) return QChar( 0x02191 );
    else if( entity == "Uparrow" ) return QChar( 0x021D1 );
    else if( entity == "uparrow" ) return QChar( 0x02191 );
    else if( entity == "UpArrowBar" ) return QChar( 0x02912 );
    else if( entity == "UpArrowDownArrow" ) return QChar( 0x021C5 );
    else if( entity == "UpDownArrow" ) return QChar( 0x02195 );
    else if( entity == "Updownarrow" ) return QChar( 0x021D5 );
    else if( entity == "updownarrow" ) return QChar( 0x02195 );
    else if( entity == "UpEquilibrium" ) return QChar( 0x0296E );
    else if( entity == "upharpoonleft" ) return QChar( 0x021BF );
    else if( entity == "upharpoonright" ) return QChar( 0x021BE );
    else if( entity == "uplus" ) return QChar( 0x0228E );
    else if( entity == "UpperLeftArrow" ) return QChar( 0x02196 );
    else if( entity == "UpperRightArrow" ) return QChar( 0x02197 );
    else if( entity == "Upsi" ) return QChar( 0x003D2 );
    else if( entity == "upsi" ) return QChar( 0x003C5 );
    else if( entity == "Upsilon" ) return QChar( 0x003A5 );
    else if( entity == "upsilon" ) return QChar( 0x003C5 );
    else if( entity == "UpTee" ) return QChar( 0x022A5 );
    else if( entity == "UpTeeArrow" ) return QChar( 0x021A5 );
    else if( entity == "upuparrows" ) return QChar( 0x021C8 );
    else if( entity == "urcorn" ) return QChar( 0x0231D );
    else if( entity == "urcorner" ) return QChar( 0x0231D );
    else if( entity == "urcrop" ) return QChar( 0x0230E );
    else if( entity == "Uring" ) return QChar( 0x0016E );
    else if( entity == "uring" ) return QChar( 0x0016F );
    else if( entity == "urtri" ) return QChar( 0x025F9 );
    else if( entity == "Uscr" ) return QChar( 0x1D4B0 );
    else if( entity == "uscr" ) return QChar( 0x1D4CA );
    else if( entity == "utdot" ) return QChar( 0x022F0 );
    else if( entity == "Utilde" ) return QChar( 0x00168 );
    else if( entity == "utilde" ) return QChar( 0x00169 );
    else if( entity == "utri" ) return QChar( 0x025B5 );
    else if( entity == "utrif" ) return QChar( 0x025B4 );
    else if( entity == "uuarr" ) return QChar( 0x021C8 );
    else if( entity == "Uuml" ) return QChar( 0x000DC );
    else if( entity == "uuml" ) return QChar( 0x000FC );
    else if( entity == "uwangle" ) return QChar( 0x029A7 );
    else if( entity == "vangrt" ) return QChar( 0x0299C );
    else if( entity == "varepsilon" ) return QChar( 0x003B5 );
    else if( entity == "varkappa" ) return QChar( 0x003F0 );
    else if( entity == "varnothing" ) return QChar( 0x02205 );
    else if( entity == "varphi" ) return QChar( 0x003C6 );
    else if( entity == "varpi" ) return QChar( 0x003D6 );
    else if( entity == "varpropto" ) return QChar( 0x0221D );
    else if( entity == "vArr" ) return QChar( 0x021D5 );
    else if( entity == "varr" ) return QChar( 0x02195 );
    else if( entity == "varrho" ) return QChar( 0x003F1 );
    else if( entity == "varsigma" ) return QChar( 0x003C2 );
    else if( entity == "vartheta" ) return QChar( 0x003D1 );
    else if( entity == "vartriangleleft" ) return QChar( 0x022B2 );
    else if( entity == "vartriangleright" ) return QChar( 0x022B3 );
    else if( entity == "Vbar" ) return QChar( 0x02AEB );
    else if( entity == "vBar" ) return QChar( 0x02AE8 );
    else if( entity == "vBarv" ) return QChar( 0x02AE9 );
    else if( entity == "Vcy" ) return QChar( 0x00412 );
    else if( entity == "vcy" ) return QChar( 0x00432 );
    else if( entity == "VDash" ) return QChar( 0x022AB );
    else if( entity == "Vdash" ) return QChar( 0x022A9 );
    else if( entity == "vDash" ) return QChar( 0x022A8 );
    else if( entity == "vdash" ) return QChar( 0x022A2 );
    else if( entity == "Vdashl" ) return QChar( 0x02AE6 );
    else if( entity == "Vee" ) return QChar( 0x022C1 );
    else if( entity == "vee" ) return QChar( 0x02228 );
    else if( entity == "veebar" ) return QChar( 0x022BB );
    else if( entity == "veeeq" ) return QChar( 0x0225A );
    else if( entity == "vellip" ) return QChar( 0x022EE );
    else if( entity == "Verbar" ) return QChar( 0x02016 );
    else if( entity == "verbar" ) return QChar( 0x0007C );
    else if( entity == "Vert" ) return QChar( 0x02016 );
    else if( entity == "vert" ) return QChar( 0x0007C );
    else if( entity == "VerticalBar" ) return QChar( 0x02223 );
    else if( entity == "VerticalLine" ) return QChar( 0x0007C );
    else if( entity == "VerticalSeparator" ) return QChar( 0x02758 );
    else if( entity == "VerticalTilde" ) return QChar( 0x02240 );
    else if( entity == "VeryThinSpace" ) return QChar( 0x0200A );
    else if( entity == "Vfr" ) return QChar( 0x1D519 );
    else if( entity == "vfr" ) return QChar( 0x1D533 );
    else if( entity == "vltri" ) return QChar( 0x022B2 );
    else if( entity == "Vopf" ) return QChar( 0x1D54D );
    else if( entity == "vopf" ) return QChar( 0x1D567 );
    else if( entity == "vprop" ) return QChar( 0x0221D );
    else if( entity == "vrtri" ) return QChar( 0x022B3 );
    else if( entity == "Vscr" ) return QChar( 0x1D4B1 );
    else if( entity == "vscr" ) return QChar( 0x1D4CB );
    else if( entity == "Vvdash" ) return QChar( 0x022AA );
    else if( entity == "vzigzag" ) return QChar( 0x0299A );
    else if( entity == "Wcirc" ) return QChar( 0x00174 );
    else if( entity == "wcirc" ) return QChar( 0x00175 );
    else if( entity == "wedbar" ) return QChar( 0x02A5F );
    else if( entity == "Wedge" ) return QChar( 0x022C0 );
    else if( entity == "wedge" ) return QChar( 0x02227 );
    else if( entity == "wedgeq" ) return QChar( 0x02259 );
    else if( entity == "weierp" ) return QChar( 0x02118 );
    else if( entity == "Wfr" ) return QChar( 0x1D51A );
    else if( entity == "wfr" ) return QChar( 0x1D534 );
    else if( entity == "Wopf" ) return QChar( 0x1D54E );
    else if( entity == "wopf" ) return QChar( 0x1D568 );
    else if( entity == "wp" ) return QChar( 0x02118 );
    else if( entity == "wr" ) return QChar( 0x02240 );
    else if( entity == "wreath" ) return QChar( 0x02240 );
    else if( entity == "Wscr" ) return QChar( 0x1D4B2 );
    else if( entity == "wscr" ) return QChar( 0x1D4CC );
    else if( entity == "xcap" ) return QChar( 0x022C2 );
    else if( entity == "xcirc" ) return QChar( 0x025EF );
    else if( entity == "xcup" ) return QChar( 0x022C3 );
    else if( entity == "xdtri" ) return QChar( 0x025BD );
    else if( entity == "Xfr" ) return QChar( 0x1D51B );
    else if( entity == "xfr" ) return QChar( 0x1D535 );
    else if( entity == "xhArr" ) return QChar( 0x027FA );
    else if( entity == "xharr" ) return QChar( 0x027F7 );
    else if( entity == "Xi" ) return QChar( 0x0039E );
    else if( entity == "xi" ) return QChar( 0x003BE );
    else if( entity == "xlArr" ) return QChar( 0x027F8 );
    else if( entity == "xlarr" ) return QChar( 0x027F5 );
    else if( entity == "xmap" ) return QChar( 0x027FC );
    else if( entity == "xnis" ) return QChar( 0x022FB );
    else if( entity == "xodot" ) return QChar( 0x02A00 );
    else if( entity == "Xopf" ) return QChar( 0x1D54F );
    else if( entity == "xopf" ) return QChar( 0x1D569 );
    else if( entity == "xoplus" ) return QChar( 0x02A01 );
    else if( entity == "xotime" ) return QChar( 0x02A02 );
    else if( entity == "xrArr" ) return QChar( 0x027F9 );
    else if( entity == "xrarr" ) return QChar( 0x027F6 );
    else if( entity == "Xscr" ) return QChar( 0x1D4B3 );
    else if( entity == "xscr" ) return QChar( 0x1D4CD );
    else if( entity == "xsqcup" ) return QChar( 0x02A06 );
    else if( entity == "xuplus" ) return QChar( 0x02A04 );
    else if( entity == "xutri" ) return QChar( 0x025B3 );
    else if( entity == "xvee" ) return QChar( 0x022C1 );
    else if( entity == "xwedge" ) return QChar( 0x022C0 );
    else if( entity == "Yacute" ) return QChar( 0x000DD );
    else if( entity == "yacute" ) return QChar( 0x000FD );
    else if( entity == "YAcy" ) return QChar( 0x0042F );
    else if( entity == "yacy" ) return QChar( 0x0044F );
    else if( entity == "Ycirc" ) return QChar( 0x00176 );
    else if( entity == "ycirc" ) return QChar( 0x00177 );
    else if( entity == "Ycy" ) return QChar( 0x0042B );
    else if( entity == "ycy" ) return QChar( 0x0044B );
    else if( entity == "yen" ) return QChar( 0x000A5 );
    else if( entity == "Yfr" ) return QChar( 0x1D51C );
    else if( entity == "yfr" ) return QChar( 0x1D536 );
    else if( entity == "YIcy" ) return QChar( 0x00407 );
    else if( entity == "yicy" ) return QChar( 0x00457 );
    else if( entity == "Yopf" ) return QChar( 0x1D550 );
    else if( entity == "yopf" ) return QChar( 0x1D56A );
    else if( entity == "Yscr" ) return QChar( 0x1D4B4 );
    else if( entity == "yscr" ) return QChar( 0x1D4CE );
    else if( entity == "YUcy" ) return QChar( 0x0042E );
    else if( entity == "yucy" ) return QChar( 0x0044E );
    else if( entity == "Yuml" ) return QChar( 0x00178 );
    else if( entity == "yuml" ) return QChar( 0x000FF );
    else if( entity == "Zacute" ) return QChar( 0x00179 );
    else if( entity == "zacute" ) return QChar( 0x0017A );
    else if( entity == "Zcaron" ) return QChar( 0x0017D );
    else if( entity == "zcaron" ) return QChar( 0x0017E );
    else if( entity == "Zcy" ) return QChar( 0x00417 );
    else if( entity == "zcy" ) return QChar( 0x00437 );
    else if( entity == "Zdot" ) return QChar( 0x0017B );
    else if( entity == "zdot" ) return QChar( 0x0017C );
    else if( entity == "zeetrf" ) return QChar( 0x02128 );
    else if( entity == "ZeroWidthSpace" ) return QChar( 0x0200B );
    else if( entity == "zeta" ) return QChar( 0x003B6 );
    else if( entity == "Zfr" ) return QChar( 0x02128 );
    else if( entity == "zfr" ) return QChar( 0x1D537 );
    else if( entity == "ZHcy" ) return QChar( 0x00416 );
    else if( entity == "zhcy" ) return QChar( 0x00436 );
    else if( entity == "zigrarr" ) return QChar( 0x021DD );
    else if( entity == "Zopf" ) return QChar( 0x02124 );
    else if( entity == "zopf" ) return QChar( 0x1D56B );
    else if( entity == "Zscr" ) return QChar( 0x1D4B5 );
    else if( entity == "zscr" ) return QChar( 0x1D4CF );
    else return QChar();
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
