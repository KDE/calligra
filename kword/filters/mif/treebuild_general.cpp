/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "treebuild_general.h"

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



