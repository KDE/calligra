/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "treebuild_variable.h"

VariableName::VariableName(char const * value)
{
	_value = value;
}


VariableDef::VariableDef(char const * value)
{
	_value = value;
}


Variable::Variable(VariableElementList * elements)
{
	_elements = elements;
}


VariableElement::VariableElement(VariableName * element)
{
	_type = T_VariableName;
	_variablename = element;
}


VariableElement::VariableElement(Unique * element)
{
	_type = T_Unique;
	_unique = element;
}



