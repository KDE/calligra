/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "treebuild_math.h"
#include "unitconv.h"

Math::Math( MathElementList * elements )
{
	_elements = elements;
}


MathElement::MathElement(Unique * element)
{
	_type = T_Unique;
	_unique = element;
}


MathElement::MathElement(BRect * element)
{
	_type = T_BRect;
	_brect = element;
}


MathElement::MathElement(MathFullForm * element)
{
	_type = T_MathFullForm;
	_mathfullform = element;
}


MathElement::MathElement(MathLineBreak * element)
{
	_type = T_MathLineBreak;
	_mathlinebreak = element;
}


MathElement::MathElement(MathOrigin * element)
{
	_type = T_MathOrigin;
	_mathorigin = element;
}


MathElement::MathElement(MathAlignment * element)
{
	_type = T_MathAlignment;
	_mathalignment = element;
}


MathElement::MathElement(MathSize * element)
{
	_type = T_MathSize;
	_mathsize = element;
}


MathFullForm::MathFullForm(char const *value)
{
	_value = value;
}


MathLineBreak::MathLineBreak(double value, char const * unit)
{
	_value = unitconversion( value, unit );
}


MathOrigin::MathOrigin(double x, char const * unitx, 
					   double y, char const * unity)
{
	_x = unitconversion( x, unitx );
	_y = unitconversion( y, unity );
}


MathAlignment::MathAlignment(char const *value)
{
	_value = value;
}


MathSize::MathSize(char const *value)
{
	_value = value;
}



