/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "unitconv.h"
#include <stdio.h>

// Concert value to point
double unitconversion( double value, const char* unit )
{
	if( !strcmp( unit, "pc" ) )
		// Pica
		return value * 12.0; // 1 pica = 12 points
	else if( !strcmp( unit, "pt" ) )
		// Point, base unit
		return value;
	else if( !strcmp( unit, "in" ) )
		// Inch
		return value * 72.0;
	else if( !strcmp( unit, "mm" ) )
		// Millimeter
		return value / 25.4 * 72.0;
	else if( !strcmp( unit, "cm" ) )
		// Centimeter
		return value / 2.54 * 72.0;
	else if( !strcmp( unit, "dd" ) )
		// Didot
		return value * 0.01483 * 72.0;
	else if( !strcmp( unit, "cc" ) )
		// Cicero
		return value * 12.0 * 0.01483 * 72.0;
	else {
		fprintf( stderr, "Internal error: unknown unit: %s", unit );
		return 0.0;
	}
}


double point2mm( double point )
{
	double inch = point / 72.0;
	return inch * 25.4;
}

