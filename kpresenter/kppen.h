/* This file is part of the KDE project
   Copyright (C) 2005 Peter Simonsson

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#ifndef KPPEN_H
#define KPPEN_H

#include <qpen.h>

class KoZoomHandler;

/**
 * Pen that handles line widths in points
 */
class KPPen : public QPen
{
  public:
    KPPen();
    KPPen(const QColor& _color, double _pointWidth, Qt::PenStyle _style);
    KPPen(const QColor& _color);
    ~KPPen();

    /// Set the pen width in points
    void setPointWidth(double width);
    /// KPPen width in points
    double pointWidth() const { return m_pointWidth; }

    /// Returns a zoomed QPen
    QPen zoomedPen(KoZoomHandler* zoomHandler);

  private:
    double m_pointWidth;
};

#endif
