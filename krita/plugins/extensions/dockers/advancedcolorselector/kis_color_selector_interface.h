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

class QColor;
class KoColor;
class KoCanvasResourceManager;
class KoColorSpace;

/**
 * This class is used as a base class for QWidget-based and QML-based color selector 
 */
class KisColorSelectorInterface
{
public:
    enum ColorRole {Foreground, Background};
public:
    KisColorSelectorInterface();
    virtual ~KisColorSelectorInterface();
    virtual const KoColorSpace* colorSpace() const = 0;

public:
    void setDisplayBlip(bool disp);
    bool displayBlip() const;
    void commitColor(const KoColor& koColor, ColorRole role);
    QColor findGeneratingColor(const KoColor& ref) const;
protected:
    virtual KoCanvasResourceManager* resourceManager() const = 0;
    /// finds a QColor, that will be ref.toQColor(), if converting it to the color space of ref
private:
    bool m_blipDisplay;
protected:
    bool m_colorUpdateAllowed;
};

#endif
