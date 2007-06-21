/*
 *  Copyright (c) 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef _KIS_VISCOSITY_MASK_
#define _KIS_VISCOSITY_MASK_

#include "kis_types.h"

#include "kis_mask.h"
/**
   Viscosity affects both the drying and spreading of paint. If paint is very
   viscous, it will not travel easily with gravity or dry very fast to
   the air.

   See http://gamma.cs.unc.edu/viscous/Baxter-ViscousPaint-CASA04.pdf
   for a scholarly discussion.
*/

class KRITAIMAGE_EXPORT KisViscosityMask : public KisMask
{
    KisViscosityMask( KisPaintDeviceSP device );
    virtual ~KisViscosityMask();
    KisViscosityMask( const KisViscosityMask& rhs );
    QString id() { return "KisViscosityMask"; }
};

#endif //_KIS_VISCOSITY_MASK_
