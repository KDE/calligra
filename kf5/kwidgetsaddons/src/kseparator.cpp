/*
 *   Copyright (C) 1997  Michael Roth <mroth@wirlweb.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "kseparator.h"
#include <QStyle>
#include <QStyleOption>
#include <QPainter>

KSeparator::KSeparator(QWidget* parent, Qt::WindowFlags f) : QFrame(parent, f)
{
   setLineWidth(1);
   setMidLineWidth(0);
   setOrientation( Qt::Horizontal );
}


KSeparator::KSeparator(Qt::Orientation orientation, QWidget* parent, Qt::WindowFlags f)
   : QFrame(parent, f)
{
   setLineWidth(1);
   setMidLineWidth(0);
   setOrientation( orientation );
}


void KSeparator::setOrientation(Qt::Orientation orientation)
{
   if (orientation == Qt::Vertical) {
      setFrameShape ( QFrame::VLine );
      setFrameShadow( QFrame::Sunken );
      setMinimumSize(2, 0);
   }
   else {
      setFrameShape ( QFrame::HLine );
      setFrameShadow( QFrame::Sunken );
      setMinimumSize(0, 2);
   }
   updateGeometry();
}

Qt::Orientation KSeparator::orientation() const
{
   return ((frameStyle() & VLine) == VLine) ? Qt::Vertical : Qt::Horizontal;
}

