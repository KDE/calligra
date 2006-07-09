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

#include "kivio_grid_data.h"
#include "kivioglobal.h"
#include "kivio_common.h"
#include <qdom.h>

KivioGridData::KivioGridData()
{
  color = QColor(200, 200, 200);
  freq = KoSize(10.0, 10.0);
  snap = KoSize(10.0, 10.0);
  isSnap = true;
  isShow = true;
}

KivioGridData::~KivioGridData()
{
}

void KivioGridData::save(QDomElement& element, const QString& name)
{
  Kivio::saveSize(element, name + "Freg", freq);
  Kivio::saveSize(element, name + "Snap", snap);
  XmlWriteColor(element, name + "Color", color);
  element.setAttribute(name + "IsSnap", (int)isSnap);
  element.setAttribute(name + "IsShow", (int)isShow);
}

void KivioGridData::load(const QDomElement& element, const QString& name)
{
  KoSize sdef;
  sdef = KoSize(10.0, 10.0);
  freq = Kivio::loadSize(element, name + "Freg", sdef);
  snap = Kivio::loadSize(element, name + "Snap", sdef);

  QColor def(QColor(228, 228, 228));
  color = XmlReadColor(element, name + "Color", def);

  isSnap = (bool)element.attribute(name + "IsSnap", "1").toInt();
  isShow = (bool)element.attribute(name + "IsShow", "1").toInt();
}
