/*
    Copyright (C) 2001, Rob Buis <rwlbuis@wanadoo.nl>.
    This file is part of the KDE project

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

DESCRIPTION
    Filter to convert a svg file to a killustrator document using koDocument
*/

#include "style.h"
#include <qstring.h>
#include <kdebug.h>

unsigned int rgb(short r, short g, short b) {
    return r * ( 256 * 256 ) + g * 256 + b;
}

struct SVGColor {
const char *colorID;
unsigned int colorRef;
} SVGColorTable[] = {  // table from www.w3.org/tr/svg/types.html#ColorKeywords (4.2)
{"aliceblue",              rgb(240, 248, 255)},    
{"antiquewhite",           rgb(250, 235, 215)},
{"aqua",                   rgb(  0, 255, 255)},
{"aquamarine",             rgb(127, 255, 212)},
{"azure",                  rgb(240, 255, 255)},
{"beige",                  rgb(245, 245, 220)},
{"bisque",                 rgb(255, 228, 196)},
{"black",                  rgb(  0,   0,   0)},
{"blanchedalmond",         rgb(255, 235, 205)},
{"blue",                   rgb(  0,   0, 255)},
{"blueviolet",             rgb(138,  43, 226)},
{"brown",                  rgb(165,  42,  42)},
{"burlywood",              rgb(222, 184, 135)},
{"cadetblue",              rgb( 95, 158, 160)},
{"chartreuse",             rgb(127, 255,   0)},
{"chocolate",              rgb(210, 105,  30)},
{"coral",                  rgb(255, 127,  80)},
{"cornflowerblue",         rgb(100, 149, 237)},
{"cornsilk",               rgb(255, 248, 220)},
{"crimson",                rgb(220,  20,  60)},
{"cyan",                   rgb(  0, 255, 255)},
{"darkblue",               rgb(  0,   0, 139)},
{"darkcyan",               rgb(  0, 139, 139)},
{"darkgoldenrod",          rgb(184, 134,  11)},
{"darkgray",               rgb(169, 169, 169)},
{"darkgreen",              rgb(  0, 100,   0)},
{"darkgrey",               rgb(169, 169, 169)},
{"darkkhaki",              rgb(189, 183, 107)},
{"darkmagenta",            rgb(139,   0, 139)},
{"darkolivegreen",         rgb( 85, 107,  47)},
{"darkorange",             rgb(255, 140,   0)},
{"darkorchid",             rgb(153,  50, 204)},
{"darkred",                rgb(139,   0,   0)},
{"darksalmon",             rgb(233, 150, 122)},
{"darkseagreen",           rgb(143, 188, 143)},
{"darkslateblue",          rgb( 72,  61, 139)},
{"darkslategrey",          rgb( 47,  79,  79)},
{"darkturquoise",          rgb(  0, 206, 209)},
{"darkviolet",             rgb(148,   0, 211)},
{"deeppink",               rgb(255,  20, 147)},
{"deepskyblue",            rgb(  0, 191, 255)},
{"dimgray",                rgb(105, 105, 105)},
{"dimgrey",                rgb(105, 105, 105)},
{"dodgerblue",             rgb( 30, 144, 255)},
{"firebrick",              rgb(178,  34,  34)},
{"floralwhite",            rgb(255, 250, 240)},
{"forestgreen",            rgb( 34, 139,  34)},
{"fuchsia",                rgb(255,   0, 255)},
{"gainsboro",              rgb(220, 220, 220)},
{"ghostwhite",             rgb(248, 248, 255)},
{"gold",                   rgb(255, 215,   0)},
{"goldenrod",              rgb(218, 165,  32)},
{"gray",                   rgb(128, 128, 128)},
{"grey",                   rgb(128, 128, 128)},
{"green",                  rgb(  0, 128,   0)},
{"greenyellow",            rgb(173, 255,  47)},
{"honeydew",               rgb(240, 255, 240)},
{"hotpink",                rgb(255, 105, 180)},
{"indianred",              rgb(205,  92,  92)},
{"indigo",                 rgb( 75,   0, 130)},
{"ivory",                  rgb(255, 255, 240)},
{"khaki",                  rgb(240, 230, 140)},
{"lavender",               rgb(230, 230, 250)},
{"lavenderblush",          rgb(255, 240, 245)},
{"lawngreen",              rgb(124, 252,   0)},
{"lemonchiffon",           rgb(255, 250, 205)},
{"lightblue",              rgb(173, 216, 230)},
{"lightcoral",             rgb(240, 128, 128)},
{"lightcyan",              rgb(224, 255, 255)},
{"lightgoldenrodyellow",   rgb(250, 250, 210)},
{"lightgray",              rgb(211, 211, 211)},
{"lightgreen",             rgb(144, 238, 144)},
{"lightgrey",              rgb(211, 211, 211)},
{"lightpink",              rgb(255, 182, 193)},
{"lightsalmon",            rgb(255, 160, 122)},
{"lightseagreen",          rgb( 32, 178, 170)},
{"lightskyblue",           rgb(135, 206, 250)},
{"lightslategray",         rgb(119, 136, 153)},
{"lightslategrey",         rgb(119, 136, 153)},
{"lightsteelblue",         rgb(176, 196, 222)},
{"lightyellow",            rgb(255, 255, 224)},
{"lime",                   rgb(  0, 255,   0)},
{"limegreen",              rgb( 50, 205,  50)},
{"linen",                  rgb(250, 240, 230)},
{"magenta",                rgb(255,   0, 255)},
{"maroon",                 rgb(128,   0,   0)},
{"mediumaquamarine",       rgb(102, 205, 170)},
{"mediumblue",             rgb(  0,   0, 205)},
{"mediumorchid",           rgb(186,  85, 211)},
{"mediumpurple",           rgb(147, 112, 219)},
{"mediumseagreen",         rgb( 60, 179, 113)},
{"mediumslateblue",        rgb(123, 104, 238)},
{"mediumspringgreen",      rgb(  0, 250, 154)},
{"mediumturquoise",        rgb( 72, 209, 204)},
{"mediumvioletred",        rgb(199,  21, 133)},
{"midnightblue",           rgb( 25,  25, 112)},
{"mintcream",              rgb(245, 255, 250)},
{"mistyrose",              rgb(255, 228, 225)},
{"moccasin",               rgb(255, 228, 181)},
{"navajowhite",            rgb(255, 222, 173)},
{"navy",                   rgb(  0,   0, 128)},
{"oldlace",                rgb(253, 245, 230)},
{"olive",                  rgb(128, 128,   0)},
{"olivedrab",              rgb(107, 142,  35)},
{"orange",                 rgb(255, 165,   0)},
{"orangered",              rgb(255,  69,   0)},
{"orchid",                 rgb(218, 112, 214)},
{"palegoldenrod",          rgb(238, 232, 170)},
{"palegreen",              rgb(152, 251, 152)},
{"paleturquoise",          rgb(175, 238, 238)},
{"palevioletred",          rgb(219, 112, 147)},
{"papayawhip",             rgb(255, 239, 213)},
{"peachpuff",              rgb(255, 218, 185)},
{"peru",                   rgb(205, 133,  63)},
{"pink",                   rgb(255, 192, 203)},
{"plum",                   rgb(221, 160, 221)},
{"powderblue",             rgb(176, 224, 230)},
{"purple",                 rgb(128,   0, 128)},
{"red",                    rgb(255,   0,   0)},
{"rosybrown",              rgb(188, 143, 143)},
{"royalblue",              rgb( 65, 105, 225)},
{"saddlebrown",            rgb(139,  69,  19)},
{"salmon",                 rgb(250, 128, 114)},
{"sandybrown",             rgb(244, 164,  96)},
{"seagreen",               rgb( 46, 139,  87)},
{"seashell",               rgb(255, 245, 238)},
{"sienna",                 rgb(160,  82,  45)},
{"silver",                 rgb(192, 192, 192)},
{"skyblue",                rgb(135, 206, 235)},
{"slateblue",              rgb(106,  90, 205)},
{"slategray",              rgb(112, 128, 144)},
{"snow",                   rgb(255, 250, 250)},
{"springgreen",            rgb(  0, 255, 127)},
{"steelblue",              rgb( 70, 130, 180)},
{"tan",                    rgb(210, 180, 140)},
{"teal",                   rgb(  0, 128, 128)},
{"thistle",                rgb(216, 191, 216)},
{"tomato",                 rgb(255,  99,  71)},
{"turquoise",              rgb( 64, 224, 208)},
{"violet",                 rgb(238, 130, 238)},
{"wheat",                  rgb(245, 222, 179)},
{"white",                  rgb(255, 255, 255)},
{"whitesmoke",             rgb(245, 245, 245)},
{"yellow",                 rgb(255, 255,   0)},
{"yellowgreen",            rgb(154, 205,  50)},
{0,    0},	// end of color table
};

ColorTable colorTable; // used to look up colorname -> value pairs

ColorTable::ColorTable() : QDict<unsigned int>()
{
    // initialize color table
    int i = 0;
    const char *colorID = SVGColorTable[i].colorID;
    while( colorID ) {
        insert( colorID, &(SVGColorTable[i].colorRef) );
        colorID = SVGColorTable[++i].colorID;
    }
}


ColorTable::~ColorTable()
{
    clear();
}


StyleProperty::StyleProperty()
{
    outlineInfo.mask = GObject::OutlineInfo::All ;
    outlineInfo.startArrowId = 0;
    outlineInfo.endArrowId   = 0;
    outlineInfo.width = 1;
    outlineInfo.style = Qt::SolidLine;
 
    fillInfo.mask = GObject::FillInfo::Color | GObject::FillInfo::FillStyle;
    fillInfo.color  = QColor( 0, 0, 0 ); // black fill color as default
    fillInfo.fstyle = GObject::FillInfo::SolidFill;
 
    font.setWeight( QFont::Normal );
}


StyleProperty::~StyleProperty() {
}


void StyleProperty::styleCopy( const StyleProperty &prop ) {
    outlineInfo  = prop.outlineInfo;
    fillInfo     = prop.fillInfo;
    font         = prop.font;
}

/********************* Style handling *******************************/

void StyleProperty::setStyleProperties( GObject *obj ) { 
    processStyle();
    if( obj)
    {
        obj->setOutlineInfo( outlineInfo );
        obj->setFillInfo( fillInfo );
    }
}


void StyleProperty::processStyle( ) {
    unsigned int FillColor;

    QStringList substyles = QStringList::split( ';', style );
    for( QStringList::Iterator it = substyles.begin(); it != substyles.end(); ++it ) {
        QStringList substyle = QStringList::split( ':', (*it) );
        QString command = substyle[0];
	QString params  = substyle[1];
	
        command = command.stripWhiteSpace();
        params  = params.stripWhiteSpace();
        kdDebug() << "command: " << command.local8Bit() << endl;
        kdDebug() << "rest:    " << params.local8Bit() << endl;
 

        if(command == "stroke-width") {
            outlineInfo.width = params.toFloat();
        }
        else if(command == "fill" || command == "stroke") {
            int ReferencePos = params.find( '#' );
            int RGBPos = params.find( "rgb", 0, FALSE );
            if( params == "none" ) {
	        if( command == "fill" )
                    fillInfo.fstyle = GObject::FillInfo::NoFill;
		else
		    outlineInfo.width = 0;// = GObject::FillInfo::NoFill;
            } else {
                // if ReferencePos != -1, that means the format = #xxxxxx
                if( ReferencePos != -1 ) {
                    params.remove( ReferencePos, 1 );
                    bool gelukt;
                    FillColor = params.toUInt( &gelukt, 16 );
                    kdDebug() << "FillColor: " << FillColor << endl;
                }
                // if the string rgb is somewhere in there, then the format is rgb(aa, bb, cc)
                else if( RGBPos != -1 ) {
                    params = params.simplifyWhiteSpace();
                    QRegExp reg( "[a-zA-Z,() ]" );
                    QStringList kleuren = QStringList::split( reg, params );
                    FillColor = rgb( kleuren[0].toUInt(), kleuren[1].toUInt(), kleuren[2].toUInt() );
                    kdDebug() << "FillColor: " << FillColor << endl;
                }
                // if all else fails, it must mean the format is the name of the colour
                else {
                    FillColor = colorTable[params] ? *(colorTable[params]) : 0;
                }

                if( command == "fill" )
                {
		    kdDebug() << "Fill " << endl;
                    QColor FillColor2( FillColor );
                    fillInfo.color = FillColor2;
                    fillInfo.fstyle = GObject::FillInfo::SolidFill;
		    if( !outlineInfo.color.isValid() ) {   // if not already set strokecolor == fillcolor
		        outlineInfo.color = FillColor2;
			//outlineInfo.style = FillColor2;
		    }
                }
                else
                {
                    QColor StrokeColor( FillColor );
                    outlineInfo.color = StrokeColor;
		    kdDebug() << "No Fill " << endl;
                }
            }
        }
	// TOSO subStyle not necessary?
//        else processSubStyle(command, params);
    else if( command == "font-family" )
    {
        font.setFamily( params );
    }
    else if( command == "font-size" )
    {
//        font.setPointSize( (int) convertNumToPt( params ) );
    }
    else if( command == "font-weight" )
    {
        if( params == "bold" )
            font.setWeight( QFont::Bold );
        else if( params == "normal" )
            font.setWeight( QFont::Normal );

    }
    else if( command == "font-style" )
    {
        if( params == "italic" )
            font.setItalic( true );
    }
    }
}
