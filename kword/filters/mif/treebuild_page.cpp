/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "treebuild_page.h"

#include "unitconv.h"

// This is where the pages are collected.
PageList pages;

Page::Page( PageElementList * elements)
{
	_elements = elements;
}


PageElement::PageElement(Unique * element)
{
	_type = T_Unique;
	_unique = element;
}


PageElement::PageElement(PageType * element)
{
	_type = T_PageType;
	_pagetype = element;
}


PageElement::PageElement(PageTag * element)
{
	_type = T_PageTag;
	_pagetag = element;
}


PageElement::PageElement(PageSize * element)
{
	_type = T_PageSize;
	_pagesize = element;
}


PageElement::PageElement(PageOrientation * element)
{
	_type = T_PageOrientation;
	_pageorientation = element;
}


PageElement::PageElement(PageAngle * element)
{
	_type = T_PageAngle;
	_pageangle = element;
}


PageElement::PageElement(TextRect * element)
{
	_type = T_TextRect;
	_textrect = element;
}


PageElement::PageElement(TextLine * element)
{
	_type = T_TextLine;
	_textline = element;
}


PageElement::PageElement(PolyLine * element)
{
	_type = T_PolyLine;
	_polyline = element;
}


PageElement::PageElement(Frame * element)
{
	_type = T_Frame;
	_frame = element;
}


PageElement::PageElement(PageNum * element)
{
	_type = T_PageNum;
	_pagenum = element;
}


PageElement::PageElement(PageBackground * element)
{
	_type = T_PageBackground;
	_pagebackground = element;
}


PageElement::PageElement(Rectangle * element)
{
	_type = T_Rectangle;
	_rectangle = element;
}


PageElement::PageElement(RunAroundType * element)
{
	_type = T_RunAroundType;
	_runaroundtype = element;
}


PageElement::PageElement(RunAroundGap * element)
{
	_type = T_RunAroundGap;
	_runaroundgap = element;
}


PageElement::PageElement(Polygon * element)
{
	_type = T_Polygon;
	_polygon = element;
}


PageElement::PageElement(Group * element)
{
	_type = T_Group;
	_group = element;
}


PageElement::PageElement(Ellipse * element)
{
	_type = T_Ellipse;
	_ellipse = element;
}


PageNum::PageNum(char const * value)
{
	_value = value;
}


PageBackground::PageBackground(char const * value)
{
	_value = value;
}


PageType::PageType( const char* value)
{
	_value = value;
}


PageTag::PageTag( const char* value)
{
	_value = value;
}


PageSize::PageSize(double width, char const * unitw, 
				   double height, char const * unith)
{
	_width = unitconversion( width, unitw );
	_height = unitconversion( height, unith );
}


PageOrientation::PageOrientation( const char* value)
{
	_value = value;
}


PageAngle::PageAngle(double value)
{
	_value = value;
}



