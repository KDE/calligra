/*
 * Copyright (c) 2010 Adam Celarek <kdedev at xibo dot at>
 * Copyright (C) 2012 Cyrille Berger <cberger@cberger.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 sure
 * USA
 */

#ifndef _KIS_COLOR_SELECTOR_INTERFACE_H_
#define _KIS_COLOR_SELECTOR_INTERFACE_H_

class KoColorSpace;

class KisColorSelectorInterface
{
public:
    KisColorSelectorInterface() : m_blipDisplay(true) {}
    virtual ~KisColorSelectorInterface() {}
    virtual const KoColorSpace* colorSpace() const = 0;

public:
    void setDisplayBlip(bool disp) {m_blipDisplay = disp;}
    bool displayBlip() const {return m_blipDisplay;}

private:
    bool m_blipDisplay;
  
};

#endif
