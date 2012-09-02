/*
 *  Copyright (c) 2012 José Luis Vergara <pentalis@gmail.com>
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

#include "kis_quickpix_paintop_settings.h"

#include <kis_quickpix_paintop_options.h>

KisQuickPixPaintOpSettings::KisQuickPixPaintOpSettings()
{
}

#if defined(HAVE_OPENGL)
QString KisQuickPixPaintOpSettings::modelName() const
{
    return "YOUR_LOGO_HERE";
}
#endif

