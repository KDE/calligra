/*
** A program to convert the XML rendered by KWord into LATEX.
**
** Copyright (C) 2000 Robert JACOLIN
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** To receive a copy of the GNU Library General Public License, write to the
** Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
**
*/

#include <stdlib.h>
#include <kdebug.h>
#include "element.h"

/*******************************************/
/* Constructor                             */
/*******************************************/
Element::Element()
{
	_type      = ST_NONE;
	_hinfo     = SI_NONE;
	_section   = SS_NONE;
	_name      = "";
	_removable = false;
	_visible   = true;
	_row       = 0;
	_col       = 0;
	_rows      = 0;
	_cols      = 0;
	setGrpMgr("");
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Element::~Element()
{
	kDebug(30522) << "Element Destructor" << endl;
}

/*******************************************/
/* Analyze                                 */
/*******************************************/
void Element::analyze(const QDomNode node)
{
	/* Analyze a frameset markup */
	
	/* Parameter analysis */
	kDebug(30522) << "FRAMESET PARAMETER ANALYSIS (Element)" << endl;
	analyzeParam(node);
}

/*******************************************/
/* AnalyzeParam                            */
/*******************************************/
void Element::analyzeParam(const QDomNode node)
{
	/* <FRAMESET frameType="1" frameInfo="0" removable="0"
	 * visible="1" name="Supercadre 1">
	 */
	_name = getAttr(node, "name");
	_type = (SType) getAttr(node, "frameType").toInt();
	switch(getAttr(node, "frameInfo").toInt())
	{
		case 0: _section = SS_BODY;
			break;
		case 1: _section = SS_HEADERS;
			_hinfo   = SI_FIRST;
			break;
		case 2: _section = SS_HEADERS;
			_hinfo   = SI_ODD;
			break;
		case 3: _section = SS_HEADERS;
			_hinfo   = SI_EVEN;
			break;
		case 4: _section = SS_FOOTERS;
			_hinfo   = SI_FIRST;
			break;
		case 5: _section = SS_FOOTERS;
			_hinfo   = SI_ODD;
			break;
		case 6: _section = SS_FOOTERS;
			_hinfo   = SI_EVEN;
			break;
		case 7: _section = SS_FOOTNOTES;
			break;
		default:
			_section = SS_NONE;
			kDebug(30522) << "Error: Frame info unknown!" << endl;
	}
	setRemovable(getAttr(node, "removable").toInt());
	setVisible(getAttr(node, "visible").toInt());
	if(getAttr(node, "grpMgr")!= 0)
	{
		_section = SS_TABLE;
		setGrpMgr(getAttr(node, "grpMgr"));
	}
	setRow(getAttr(node, "row").toInt());
	setCol(getAttr(node, "col").toInt());
	setRows(getAttr(node, "rows").toInt());
	setCols(getAttr(node, "cols").toInt());

	kDebug(30522) << "END PARAM" << endl;
}
