/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "treebuild_textflow.h"

/*********************************************************************
 * This is where the TextFlows are collected during tree construction.
 ********************************************************************/
TextFlowList textflows;

TextFlow::TextFlow(TextFlowElementList * elements )
{
	_elements = elements;
}


TextFlowElement::TextFlowElement(Notes * element) 
{
	_type = T_Notes;
	_notes = element;
}


TextFlowElement::TextFlowElement(Para * element)
{
	_type = T_Para;
	_para = element;
}


TextFlowElement::TextFlowElement(TFTag * element) 
{
	_type = T_TFTag;
	_tftag = element;
}


TextFlowElement::TextFlowElement(TFAutoConnect * element)
{
	_type = T_TFAutoConnect;
	_tfautoconnect = element;
}


TextFlowElement::TextFlowElement( TFSynchronized* element )
{
	_type = T_TFSynchronized;
	_tfsynchronized = element;
}


TextFlowElement::TextFlowElement( TFMinHangHeight* element )
{
	_type = T_TFMinHangHeight;
	_tfminhangheight = element;
}


TextFlowElement::TextFlowElement( TFLineSpacing* element )
{
	_type = T_TFLineSpacing;
	_tflinespacing = element;
}



TFTag::TFTag(char const * value) 
{
	_value = value;
}


TFAutoConnect::TFAutoConnect(char const * value)
{
	_value = value;
}


TFLineSpacing::TFLineSpacing( double value )
{
	_value = value;
}


TFMinHangHeight::TFMinHangHeight( double value )
{
	_value = value;
}


TFSynchronized::TFSynchronized( const char* value )
{
	if( strcmp( value, "Yes" ) == 0 )
		_value = true;
	else
		_value = false;
}



