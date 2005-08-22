/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2005 Peter Simonsson
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef KIVIOPEN_H
#define KIVIOPEN_H

#include <qpen.h>

class KoZoomHandler;

namespace Kivio {

/**
 * Pen that handles line widths in points
 */
class Pen : public QPen
{
  public:
    Pen();
    ~Pen();

    /// Set the pen width in points
    void setPointWidth(double width);
    /// Pen width in points
    double pointWidth() const { return m_pointWidth; }

    /// Returns a zoomed QPen
    QPen zoomedPen(KoZoomHandler* zoomHandler);

  private:
    double m_pointWidth;
};

}

#endif
