/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)                  
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */
#include "krobjectdata.h"
#include <kdebug.h>

KRObjectData::KRObjectData() 
{ 
Z = 0;
_name = new KoProperty::Property ( "Name", "", "Name", "Object Name");
_name->setAutoSync(0);
}

KRObjectData::~KRObjectData() { }

KRLineData * KRObjectData::toLine() { return 0; }
KRLabelData * KRObjectData::toLabel() { return 0; }
KRFieldData * KRObjectData::toField() { return 0; }
KRTextData * KRObjectData::toText() { return 0; }
KRBarcodeData * KRObjectData::toBarcode() { return 0; }
KRImageData * KRObjectData::toImage() { return 0; }
KRChartData * KRObjectData::toChart() { return 0; }



