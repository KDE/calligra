// $Header$

/* This file is part of the KDE project
   Copyright (C) 2001 Nicolas GOUTTE <nicog@snafu.de>

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

/*
    The link between character references and their values are copyrighted by:

    Portions (C) International Organization for Standardization 1986
    Permission to copy in any form is granted for use with
    conforming SGML systems and applications as defined in
    ISO 8879, provided this notice is included in all copies.
*/


#include <qmap.h>
#include "ImportEntities.h"

bool MapEntities::addEntity(const QString& strName, const uint value)
{
    insert(strName,value);
}

bool MapEntities::initMapEntities (void)
{
    // Latin 1
    //  <!ENTITY % HTMLlat1 PUBLIC "-//W3C//ENTITIES Latin 1//EN//HTML">

    addEntity( "nbsp"     ,  160 );
    addEntity( "iexcl"    ,  161 );
    addEntity( "cent"     ,  162 );
    addEntity( "pound"    ,  163 );
    addEntity( "curren"   ,  164 );
    addEntity( "yen"      ,  165 );
    addEntity( "brvbar"   ,  166 );
    addEntity( "sect"     ,  167 );
    addEntity( "uml"      ,  168 );
    addEntity( "copy"     ,  169 );
    addEntity( "ordf"     ,  170 );
    addEntity( "laquo"    ,  171 );
    addEntity( "not"      ,  172 );
    addEntity( "shy"      ,  173 );
    addEntity( "reg"      ,  174 );
    addEntity( "macr"     ,  175 );
    addEntity( "deg"      ,  176 );
    addEntity( "plusmn"   ,  177 );
    addEntity( "sup2"     ,  178 );
    addEntity( "sup3"     ,  179 );
    addEntity( "acute"    ,  180 );
    addEntity( "micro"    ,  181 );
    addEntity( "para"     ,  182 );
    addEntity( "middot"   ,  183 );
    addEntity( "cedil"    ,  184 );
    addEntity( "sup1"     ,  185 );
    addEntity( "ordm"     ,  186 );
    addEntity( "raquo"    ,  187 );
    addEntity( "frac14"   ,  188 );
    addEntity( "frac12"   ,  189 );
    addEntity( "frac34"   ,  190 );
    addEntity( "iquest"   ,  191 );
    addEntity( "Agrave"   ,  192 );
    addEntity( "Aacute"   ,  193 );
    addEntity( "Acirc"    ,  194 );
    addEntity( "Atilde"   ,  195 );
    addEntity( "Auml"     ,  196 );
    addEntity( "Aring"    ,  197 );
    addEntity( "AElig"    ,  198 );
    addEntity( "Ccedil"   ,  199 );
    addEntity( "Egrave"   ,  200 );
    addEntity( "Eacute"   ,  201 );
    addEntity( "Ecirc"    ,  202 );
    addEntity( "Euml"     ,  203 );
    addEntity( "Igrave"   ,  204 );
    addEntity( "Iacute"   ,  205 );
    addEntity( "Icirc"    ,  206 );
    addEntity( "Iuml"     ,  207 );
    addEntity( "ETH"      ,  208 );
    addEntity( "Ntilde"   ,  209 );
    addEntity( "Ograve"   ,  210 );
    addEntity( "Oacute"   ,  211 );
    addEntity( "Ocirc"    ,  212 );
    addEntity( "Otilde"   ,  213 );
    addEntity( "Ouml"     ,  214 );
    addEntity( "times"    ,  215 );
    addEntity( "Oslash"   ,  216 );
    addEntity( "Ugrave"   ,  217 );
    addEntity( "Uacute"   ,  218 );
    addEntity( "Ucirc"    ,  219 );
    addEntity( "Uuml"     ,  220 );
    addEntity( "Yacute"   ,  221 );
    addEntity( "THORN"    ,  222 );
    addEntity( "szlig"    ,  223 );
    addEntity( "agrave"   ,  224 );
    addEntity( "aacute"   ,  225 );
    addEntity( "acirc"    ,  226 );
    addEntity( "atilde"   ,  227 );
    addEntity( "auml"     ,  228 );
    addEntity( "aring"    ,  229 );
    addEntity( "aelig"    ,  230 );
    addEntity( "ccedil"   ,  231 );
    addEntity( "egrave"   ,  232 );
    addEntity( "eacute"   ,  233 );
    addEntity( "ecirc"    ,  234 );
    addEntity( "euml"     ,  235 );
    addEntity( "igrave"   ,  236 );
    addEntity( "iacute"   ,  237 );
    addEntity( "icirc"    ,  238 );
    addEntity( "iuml"     ,  239 );
    addEntity( "eth"      ,  240 );
    addEntity( "ntilde"   ,  241 );
    addEntity( "ograve"   ,  242 );
    addEntity( "oacute"   ,  243 );
    addEntity( "ocirc"    ,  244 );
    addEntity( "otilde"   ,  245 );
    addEntity( "ouml"     ,  246 );
    addEntity( "divide"   ,  247 );
    addEntity( "oslash"   ,  248 );
    addEntity( "ugrave"   ,  249 );
    addEntity( "uacute"   ,  250 );
    addEntity( "ucirc"    ,  251 );
    addEntity( "uuml"     ,  252 );
    addEntity( "yacute"   ,  253 );
    addEntity( "thorn"    ,  254 );
    addEntity( "yuml"     ,  255 );

    // Symbols
    //  <!ENTITY % HTMLsymbol PUBLIC "-//W3C//ENTITIES Symbols//EN//HTML">

    addEntity( "fnof"     ,  402 );
    addEntity( "Alpha"    ,  913 );
    addEntity( "Beta"     ,  914 );
    addEntity( "Gamma"    ,  915 );
    addEntity( "Delta"    ,  916 );
    addEntity( "Epsilon"  ,  917 );
    addEntity( "Zeta"     ,  918 );
    addEntity( "Eta"      ,  919 );
    addEntity( "Theta"    ,  920 );
    addEntity( "Iota"     ,  921 );
    addEntity( "Kappa"    ,  922 );
    addEntity( "Lambda"   ,  923 );
    addEntity( "Mu"       ,  924 );
    addEntity( "Nu"       ,  925 );
    addEntity( "Xi"       ,  926 );
    addEntity( "Omicron"  ,  927 );
    addEntity( "Pi"       ,  928 );
    addEntity( "Rho"      ,  929 );
    addEntity( "Sigma"    ,  931 );
    addEntity( "Tau"      ,  932 );
    addEntity( "Upsilon"  ,  933 );
    addEntity( "Phi"      ,  934 );
    addEntity( "Chi"      ,  935 );
    addEntity( "Psi"      ,  936 );
    addEntity( "Omega"    ,  937 );
    addEntity( "alpha"    ,  945 );
    addEntity( "beta"     ,  946 );
    addEntity( "gamma"    ,  947 );
    addEntity( "delta"    ,  948 );
    addEntity( "epsilon"  ,  949 );
    addEntity( "zeta"     ,  950 );
    addEntity( "eta"      ,  951 );
    addEntity( "theta"    ,  952 );
    addEntity( "iota"     ,  953 );
    addEntity( "kappa"    ,  954 );
    addEntity( "lambda"   ,  955 );
    addEntity( "mu"       ,  956 );
    addEntity( "nu"       ,  957 );
    addEntity( "xi"       ,  958 );
    addEntity( "omicron"  ,  959 );
    addEntity( "pi"       ,  960 );
    addEntity( "rho"      ,  961 );
    addEntity( "sigmaf"   ,  962 );
    addEntity( "sigma"    ,  963 );
    addEntity( "tau"      ,  964 );
    addEntity( "upsilon"  ,  965 );
    addEntity( "phi"      ,  966 );
    addEntity( "chi"      ,  967 );
    addEntity( "psi"      ,  968 );
    addEntity( "omega"    ,  969 );
    addEntity( "thetasym" ,  977 );
    addEntity( "upsih"    ,  978 );
    addEntity( "piv"      ,  982 );
    addEntity( "bull"     , 8226 );
    addEntity( "hellip"   , 8230 );
    addEntity( "prime"    , 8242 );
    addEntity( "Prime"    , 8243 );
    addEntity( "oline"    , 8254 );
    addEntity( "frasl"    , 8260 );
    addEntity( "weierp"   , 8472 );
    addEntity( "image"    , 8465 );
    addEntity( "real"     , 8476 );
    addEntity( "trade"    , 8482 );
    addEntity( "alefsym"  , 8501 );
    addEntity( "larr"     , 8592 );
    addEntity( "uarr"     , 8593 );
    addEntity( "rarr"     , 8594 );
    addEntity( "darr"     , 8595 );
    addEntity( "harr"     , 8596 );
    addEntity( "crarr"    , 8629 );
    addEntity( "lArr"     , 8656 );
    addEntity( "uArr"     , 8657 );
    addEntity( "rArr"     , 8658 );
    addEntity( "dArr"     , 8659 );
    addEntity( "hArr"     , 8660 );
    addEntity( "forall"   , 8704 );
    addEntity( "part"     , 8706 );
    addEntity( "exist"    , 8707 );
    addEntity( "empty"    , 8709 );
    addEntity( "nabla"    , 8711 );
    addEntity( "isin"     , 8712 );
    addEntity( "notin"    , 8713 );
    addEntity( "ni"       , 8715 );
    addEntity( "prod"     , 8719 );
    addEntity( "sum"      , 8721 );
    addEntity( "minus"    , 8722 );
    addEntity( "lowast"   , 8727 );
    addEntity( "radic"    , 8730 );
    addEntity( "prop"     , 8733 );
    addEntity( "infin"    , 8734 );
    addEntity( "ang"      , 8736 );
    addEntity( "and"      , 8743 );
    addEntity( "or"       , 8744 );
    addEntity( "cap"      , 8745 );
    addEntity( "cup"      , 8746 );
    addEntity( "int"      , 8747 );
    addEntity( "there4"   , 8756 );
    addEntity( "sim"      , 8764 );
    addEntity( "cong"     , 8773 );
    addEntity( "asymp"    , 8776 );
    addEntity( "ne"       , 8800 );
    addEntity( "equiv"    , 8801 );
    addEntity( "le"       , 8804 );
    addEntity( "ge"       , 8805 );
    addEntity( "sub"      , 8834 );
    addEntity( "sup"      , 8835 );
    addEntity( "nsub"     , 8836 );
    addEntity( "sube"     , 8838 );
    addEntity( "supe"     , 8839 );
    addEntity( "oplus"    , 8853 );
    addEntity( "otimes"   , 8855 );
    addEntity( "perp"     , 8869 );
    addEntity( "sdot"     , 8901 );
    addEntity( "lceil"    , 8968 );
    addEntity( "rceil"    , 8969 );
    addEntity( "lfloor"   , 8970 );
    addEntity( "rfloor"   , 8971 );
    addEntity( "lang"     , 9001 );
    addEntity( "rang"     , 9002 );
    addEntity( "loz"      , 9674 );
    addEntity( "spades"   , 9824 );
    addEntity( "clubs"    , 9827 );
    addEntity( "hearts"   , 9829 );
    addEntity( "diams"    , 9830 );

    // Special
    //  <!ENTITY % HTMLspecial PUBLIC "-//W3C//ENTITIES Special//EN//HTML">

    addEntity( "quot"     ,   34 );
    addEntity( "amp"      ,   38 );
    addEntity( "lt"       ,   60 );
    addEntity( "gt"       ,   62 );
    addEntity( "OElig"    ,  338 );
    addEntity( "oelig"    ,  339 );
    addEntity( "Scaron"   ,  352 );
    addEntity( "scaron"   ,  353 );
    addEntity( "Yuml"     ,  376 );
    addEntity( "circ"     ,  710 );
    addEntity( "tilde"    ,  732 );
    addEntity( "ensp"     , 8194 );
    addEntity( "emsp"     , 8195 );
    addEntity( "thinsp"   , 8201 );
    addEntity( "zwnj"     , 8204 );
    addEntity( "zwj"      , 8205 );
    addEntity( "lrm"      , 8206 );
    addEntity( "rlm"      , 8207 );
    addEntity( "ndash"    , 8211 );
    addEntity( "mdash"    , 8212 );
    addEntity( "lsquo"    , 8216 );
    addEntity( "rsquo"    , 8217 );
    addEntity( "sbquo"    , 8218 );
    addEntity( "ldquo"    , 8220 );
    addEntity( "rdquo"    , 8221 );
    addEntity( "bdquo"    , 8222 );
    addEntity( "dagger"   , 8224 );
    addEntity( "Dagger"   , 8225 );
    addEntity( "permil"   , 8240 );
    addEntity( "lsaquo"   , 8249 );
    addEntity( "rsaquo"   , 8250 );
    addEntity( "euro"     , 8364 );

    return true;
}