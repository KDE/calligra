/* This file is part of the KDE project
   Copyright (C) 2003 Ulrich Kuettler <ulrich.kuettler@gmx.de>
                 2006 Martin Pfeiffer <hubipete@gmx.net>
                 2006 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

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

#include "ElementFactory.h"
#include "kformuladefs.h"
#include <kdebug.h>

//#include "ActionElement.h"
//#include "BracketElement.h"
//#include "EncloseElement.h"
//#include "ErrorElement.h"
#include "FractionElement.h"
#include "GlyphElement.h"
#include "IdentifierElement.h"
//#include "MatrixElement.h"
//#include "MatrixRowElement.h"
//#include "MatrixEntryElement.h"
//#include "MultiscriptElement.h"
#include "NumberElement.h"
#include "OperatorElement.h"
//#include "PaddedElement.h"
//#include "PhantomElement.h"
#include "RootElement.h"
#include "RowElement.h"
#include "SpaceElement.h"
//#include "StringElement.h"
//#include "StyleElement.h"
#include "TextElement.h"
//#include "UnderOverElement.h"

BasicElement* ElementFactory::createElement( const QString& tagName,
                                             BasicElement* parent )
{
    kWarning( DEBUGID ) << "Creating element: " << tagName << endl;
    if( tagName == "mi" )
        return new IdentifierElement( parent );
    else if ( tagName == "mo" )
        return new OperatorElement( parent );
    else if ( tagName == "mn" )
        return new NumberElement( parent );
    else if ( tagName == "mtext" )
        return new TokenElement( parent );
    //else if ( tagName == "ms" )
    //        return new StringElement( parent );
    else if ( tagName == "mspace" )
        return new SpaceElement( parent );
    else if ( tagName == "mglyph" )
        return new GlyphElement( parent );
    else if ( tagName == "mrow" )
          return new RowElement( parent );
    else if ( tagName == "mfrac" )
          return new FractionElement( parent );
    else if ( tagName == "msqrt" || tagName == "mroot" )
          return new RootElement( parent );
    //else if ( tagName == "mstyle" )
    //      return new StyleElement( parent );
    //else if ( tagName == "merror" )
    //      return new ErrorElement( parent );
    //else if ( tagName == "mpadded" )
    //      return new PaddedElement( parent );
    //else if ( tagName == "mphantom" )
    //      return new PhantomElement( parent );
    //else if ( tagName == "mfenced" )
    //      return new BracketElement( parent );
    //else if ( tagName == "menclose" )
    //      return new EncloseElement( parent );
    //else if ( tagName == "msub" || tagName == "msup" || tagName == "msubsup" )
    //      return new MultiscriptElement( parent );
    //else if ( tagName == "munder" || tagName == "mover" || tagName == "munderover" )
    //      return new UnderOverElement( parent );
    //else if ( tagName == "mmultiscripts" )
    //      return new MultiscriptElement( parent );

    return 0;
}

QString ElementFactory::elementName( ElementType type )
{
    if( type == Identifier )
        return "mi";
    else if( type == Operator )
        return "mo";
    else if( type == Number )
        return "mn";
    else if( type == Text )
        return "mtext";
    else if( type == Glyph )
        return "mglyph";
    else if( type == String )
        return "ms";
    else if( type == Space )
        return "mspace";
    else if( type == Row )
        return "mrow";
    else if( type == Fraction )
        return "mfrac";
    else if( type == Phantom )
        return "mphantom";
    else if( type == Style )
        return "mstyle";
    else if( type == Padded )
        return "mpadded";
    else if( type == Error )
        return "merror";
    else if( type == Fenced )
        return "mfenced";
    else if( type == Enclose )
        return "menclose";
    else if( type == Space )
        return "mspace";
    else if( type == UnderOver )
        return "munderover";
    else if( type == Under )
        return "munder";
    else if( type == Over )
        return "mover";
    else if( type == SubScript )
        return "msub";
    else if( type == SupScript )
        return "msup";
    else if( type == SubSupScript )
        return "msubsup";
    else if( type == MultiScript )
        return "mmultiscripts";
    else if( type == Root )
        return "mroot";
    else if( type == SquareRoot )
        return "msqrt";

    return QString();
}
