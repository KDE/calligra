/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#ifndef _TREEBUILD_VARIABLE
#define _TREEBUILD_VARIABLE

#include <list>
#include <string>

class Unique;

class VariableName
{
public:
	VariableName( const char* );

private:
	string _value;
};

class VariableDef
{
public:
	VariableDef( const char* );

private:
	string _value;
};


class VariableElement
{
public:
	enum VariableElementType { T_VariableName, T_Unique };

	VariableElement( VariableName* element );
	VariableElement( Unique* element );

private:
	VariableElementType _type;

	union {
		VariableName*_variablename;
		Unique* _unique;
	};
};

typedef list<VariableElement*> VariableElementList;

class Variable
{
public:
	Variable( VariableElementList* elements );

private:
	VariableElementList* _elements;
};


#endif

