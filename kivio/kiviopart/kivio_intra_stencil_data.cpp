/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "kivio_intra_stencil_data.h"

#include <KoZoomHandler.h>

#include "kivio_painter.h"

KivioIntraStencilData::KivioIntraStencilData()
{
    mx = my = 0.0f;
    oldMX = oldMY = 0.0f;
    unsnapMX = unsnapMY = 0.0f;
//     scale = 1.0f;
    zoomHandler = 0;

    painter = 0;

    printing = false;
}

KivioIntraStencilData::~KivioIntraStencilData()
{
}
