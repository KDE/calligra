/* $Id$
 *
 * This file is part of MIFParse, a MIF parser for Unix.
 *
 * Copyright (C) 1998 by Matthias Kalle Dalheimer <kalle@dalheimer.de>
 */

#include "treebuild_external.h"
#include "unitconv.h"

ImportObject::ImportObject( ImportObjectElementList * elements )
{
	_elements = elements;
}


ImportObjectElement::ImportObjectElement(Unique *element)
{
	_type = T_Unique;
	_unique = element;
}


ImportObjectElement::ImportObjectElement(ImportObFile *element)
{
	_type = T_ImportObFile;
	_importobfile = element;
}


ImportObjectElement::ImportObjectElement(ShapeRect *element)
{
	_type = T_ShapeRect;
	_shaperect = element;
}


ImportObjectElement::ImportObjectElement(BRect *element)
{
	_type = T_BRect;
	_brect = element;
}


ImportObjectElement::ImportObjectElement(FlipLR *element)
{
	_type = T_FlipLR;
	_fliplr = element;
}


ImportObjectElement::ImportObjectElement(NativeOrigin *element)
{
	_type = T_NativeOrigin;
	_nativeorigin = element;
}


ImportObjectElement::ImportObjectElement(BitmapDPI *element)
{
	_type = T_BitmapDPI;
	_bitmapdpi = element;
}


ImportObjectElement::ImportObjectElement(RunAroundType *element)
{
	_type = T_RunAroundType;
	_runaroundtype = element;
}


ImportObjectElement::ImportObjectElement(RunAroundGap *element)
{
	_type = T_RunAroundGap;
	_runaroundgap = element;
}


ImportObjectElement::ImportObjectElement(ExternalData *element)
{
	_type = T_ExternalData;
	_externaldata = element;
}



BitmapDPI::BitmapDPI(int value)
{
	_value = value;
}


ImportObFile::ImportObFile(char const * value)
{
	_value = value;
}


FlipLR::FlipLR(char const * value)
{
	_value = value;
}


NativeOrigin::NativeOrigin(double x, char const * unitx, 
						   double y, char const * unity)
{
	_x = unitconversion( x, unitx );
	_y = unitconversion( y, unity );
}






