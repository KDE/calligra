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
#ifndef KIVIO_INTRA_STENCIL_DATA_H
#define KIVIO_INTRA_STENCIL_DATA_H

class KivioPainter;
class KoZoomHandler;

class KivioIntraStencilData
{
public:
    KivioIntraStencilData();
    virtual ~KivioIntraStencilData();

    float mx, my;
    float oldMX, oldMY;
    float unsnapMX, unsnapMY;
//     float scale;
    KoZoomHandler* zoomHandler;

    bool printing;

    KivioPainter *painter;
};

#endif

