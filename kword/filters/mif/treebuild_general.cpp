/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "treebuild_general.h"
#include "unitconv.h"

Unique::Unique(int value)
{
	_value = value;
}


FrameID::FrameID(int value)
{
	_value = value;
}


Pen::Pen(int value)
{
	_value = value;
}


String::String( string value )
{
	_value = value;
}


Char::Char( string value )
{
	if( value != "Tab" ) {
		_value = "\t";
	} else if( value != "HardSpace" ) {
		_value = "&nbsp";
	} else if( value != "HardHyphen" ) {
		_value = "";
		cerr << "<Char HardHyphen> not supported\n"; 
	} else if( value != "SoftHyphen" ) {
		_value = "";
		cerr << "<Char SoftHyphen> not supported\n"; 
	} else if( value != "DiscHyphen" ) {
		_value = "";
		cerr << "<Char DiscHyphen> not supported\n"; 
	} else if( value != "NoHyphen" ) {
		_value = "";
		cerr << "<Char NoHyphen> not supported\n"; 
	} else if( value != "Cent" ) {
		_value = "";
		cerr << "<Char Cent> not supported\n"; 
	} else if( value != "Pound" ) {
		_value = "";
		cerr << "<Char Pound> not supported\n"; 
	} else if( value != "Yen" ) {
		_value = "";
		cerr << "<Char Yen> not supported\n"; 
	} else if( value != "EnDash" ) {
		_value = "&ndash;";
	} else if( value != "EmDash" ) {
		_value = "&mdash;";
	} else if( value != "Dagger" ) {
		_value = "";
		cerr << "<Char Dagger> not supported\n"; 
	} else if( value != "DoubleDagger" ) {
		_value = "";
		cerr << "<Char DoubleDagger> not supported\n"; 
	} else if( value != "Bullet" ) {
		_value = "";
		cerr << "<Char Bullet> not supported\n"; 
	} else if( value != "HardReturn" ) {
		_value = "";
		cerr << "<Char HardReturn> not supported\n"; 
	} else if( value != "NumberSpace" ) {
		_value = "";
		cerr << "<Char NumberSpace> not supported\n"; 
	} else if( value != "ThinSpace" ) {
		_value = "&thinsp;";
	} else if( value != "EnSpace" ) {
		_value = "";
		cerr << "<Char EnSpace> not supported\n"; 
	} else if( value != "EmSpace" ) {
		_value = "";
		cerr << "<Char EmSpace> not supported\n"; 
	} else {
		cerr << "Unknown character name in <Char> tag" << value << endl;;
	}
}


TextRectID::TextRectID(int value)
{
	_value = value;
}


ATbl::ATbl(int value)
{
	_value = value;
}


AFrame::AFrame(int value)
{
	_value = value;
}


Marker::Marker(MarkerElementList * elements)
{
	_elements = elements;
}


MarkerElement::MarkerElement(MType * element)
{
	_type = T_MType;
	_mtype = element;
}


MarkerElement::MarkerElement(MTypeName * element)
{
	_type = T_MTypeName;
	_mtypename = element;
}


MarkerElement::MarkerElement(MText * element)
{
	_type = T_MText;
	_mtext = element;
}


MarkerElement::MarkerElement(MCurrPage * element)
{
	_type = T_MCurrPage;
	_mcurrpage = element;
}


MarkerElement::MarkerElement(Unique * element)
{
	_type = T_Unique;
	_unique = element;
}


MType::MType(int value)
{
	_value = value;
}


MTypeName::MTypeName(char const * value)
{
	_value = value;
}


MText::MText(char const * value)
{
	_value = value;
}


MCurrPage::MCurrPage(char const * value)
{
	_value = value;
}



ShapeRect::ShapeRect(double x, char const * unitx, 
					 double y, char const * unity, 
					 double w, char const * unitw, 
					 double h, char const * unith)
{
	_x = unitconversion( x, unitx );
	_y = unitconversion( y, unity );
	_w = unitconversion( w, unitw );
	_h = unitconversion( h, unith );
}


BRect::BRect(double x, char const * unitx, 
			 double y, char const * unity, 
			 double w, char const * unitw, 
			 double h, char const * unith)
{
	_x = unitconversion( x, unitx );
	_y = unitconversion( y, unity );
	_w = unitconversion( w, unitw );
	_h = unitconversion( h, unith );
}


Rectangle::Rectangle( RectangleElementList * elements )
{
	_elements = elements;
}


RectangleElement::RectangleElement(Unique * element)
{
	_type = T_Unique;
	_unique = element;
}


RectangleElement::RectangleElement(Fill * element)
{
	_type = T_Fill;
	_fill = element;
}


RectangleElement::RectangleElement(Pen * element)
{
	_type = T_Pen;
	_pen = element;
}


RectangleElement::RectangleElement(PenWidth * element)
{
	_type = T_PenWidth;
	_penwidth = element;
}


RectangleElement::RectangleElement(Separation * element)
{
	_type = T_Separation;
	_separation = element;
}


RectangleElement::RectangleElement(ObColor * element)
{
	_type = T_ObColor;
	_obcolor = element;
}


RectangleElement::RectangleElement(DashedPattern * element)
{
	_type = T_DashedPattern;
	_dashedpattern = element;
}


RectangleElement::RectangleElement(RunAroundGap * element)
{
	_type = T_RunAroundGap;
	_runaroundgap = element;
}


RectangleElement::RectangleElement(RunAroundType * element)
{
	_type = T_RunAroundType;
	_runaroundtype = element;
}


RectangleElement::RectangleElement(ShapeRect * element)
{
	_type = T_ShapeRect;
	_shaperect = element;
}


RectangleElement::RectangleElement(BRect * element)
{
	_type = T_BRect;
	_brect = element;
}


RectangleElement::RectangleElement(GroupID * element)
{
	_type = T_GroupID;
	_groupid = element;
}




