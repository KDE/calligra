/* This file is part of the KDE project
   Copyright (C) 2003 Peter Simonsson <psn@linux.se>

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

#include "kivioglobal.h"
#include "kivio_common.h"

#include <qdom.h>

struct PaperSizeDef {
  const char* title;
  float width;
  float height;
  int unit;
};

void Kivio::savePageLayout(QDomElement& e, KoPageLayout layout)
{
  XmlWriteFloat(e, "width", layout.ptWidth);
  XmlWriteFloat(e, "height", layout.ptHeight);
  XmlWriteFloat(e, "marginLeft", layout.ptLeft);
  XmlWriteFloat(e, "marginRight", layout.ptRight);
  XmlWriteFloat(e, "marginTop", layout.ptTop);
  XmlWriteFloat(e, "marginBottom", layout.ptBottom);
  XmlWriteString(e, "format", KoPageFormat::formatString(layout.format));
  XmlWriteString(e, "orientation", Kivio::orientationString(layout.orientation));
}

KoPageLayout Kivio::loadPageLayout(const QDomElement& e)
{
  KoPageLayout layout;
  
  if(e.hasAttribute("unit")) {
    // Compatability with Kivio <= 1.2.x
    KoUnit::Unit unit = Kivio::convToKoUnit(XmlReadInt(e, "unit", 0));
    layout.ptWidth = KoUnit::ptFromUnit(XmlReadFloat(e, "width", 0.0), unit);
    layout.ptHeight = KoUnit::ptFromUnit(XmlReadFloat(e, "height", 0.0), unit);
    layout.ptLeft = KoUnit::ptFromUnit(XmlReadFloat(e, "marginLeft", 0.0), unit);
    layout.ptRight = KoUnit::ptFromUnit(XmlReadFloat(e, "marginRight", 0.0), unit);
    layout.ptTop = KoUnit::ptFromUnit(XmlReadFloat(e, "marginTop", 0.0), unit);
    layout.ptBottom = KoUnit::ptFromUnit(XmlReadFloat(e, "marginBottom", 0.0), unit);
    Kivio::setFormatOrientation(layout);
  } else {
    layout.ptWidth = XmlReadFloat(e, "width", 0.0);
    layout.ptHeight = XmlReadFloat(e, "height", 0.0);
    layout.ptLeft = XmlReadFloat(e, "marginLeft", 0.0);
    layout.ptRight = XmlReadFloat(e, "marginRight", 0.0);
    layout.ptTop = XmlReadFloat(e, "marginTop", 0.0);
    layout.ptBottom = XmlReadFloat(e, "marginBottom", 0.0);
    layout.format = KoPageFormat::formatFromString(XmlReadString(e, "format", "A4"));
    layout.orientation = Kivio::orientationFromString(XmlReadString(e, "orientation", "Portrait"));
  }
  
  return layout;
}

KoUnit::Unit Kivio::convToKoUnit(int tkUnit)
{
  switch(tkUnit) {
    case 0:
      return KoUnit::U_PT;
    case 1:
      return KoUnit::U_MM;
    case 2:
      return KoUnit::U_INCH;
    case 3:
      return KoUnit::U_PI;
    case 4:
      return KoUnit::U_CM;
    case 5:
      return KoUnit::U_DD;
    case 6:
      return KoUnit::U_CC;
  }
  
  return KoUnit::U_PT;
}

QString Kivio::orientationString(KoOrientation o)
{
  if(o == PG_LANDSCAPE) {
    return "Landscape";
  }
  
  return "Portrait";
}

KoOrientation Kivio::orientationFromString(const QString& s)
{
  if(s == "Landscape") {
    return PG_LANDSCAPE;
  }
  
  return PG_PORTRAIT;
}

void Kivio::setFormatOrientation(KoPageLayout& layout)
{
  const PaperSizeDef PapersTable[] = {
    {"A0",841,1189,KoUnit::U_MM},
    {"A1",594,841,KoUnit::U_MM},
    {"A2",420,594,KoUnit::U_MM},
    {"A3",297,420,KoUnit::U_MM},
    {"A4",210,297,KoUnit::U_MM},
    {"A5",148,210,KoUnit::U_MM},

    {"B4",250,353,KoUnit::U_MM},
    {"B6",125,176,KoUnit::U_MM},

    {"C4",229,324,KoUnit::U_MM},
    {"C5",162,229,KoUnit::U_MM},
    {"C5",114,162,KoUnit::U_MM},

    {"DL",110,220,KoUnit::U_MM},

    {"Letter",8.5,11,KoUnit::U_INCH},
    {"Legal",8.5,14,KoUnit::U_INCH},

    {"A",8.5,11,KoUnit::U_INCH},
    {"B",11,17,KoUnit::U_INCH},
    {"C",17,22,KoUnit::U_INCH},
    {"D",22,34,KoUnit::U_INCH},
    {"E",34,44,KoUnit::U_INCH},
    
    {"null",0,0,-2}     // -2 for end of list
  };
  PaperSizeDef sizeDef = PapersTable[0];
  bool found = false;
  float width, height;
  layout.format = PG_CUSTOM;
  layout.orientation = PG_PORTRAIT;
  
  while((sizeDef.unit != -2) && !found) {
    width = KoUnit::ptFromUnit(sizeDef.width, static_cast<KoUnit::Unit>(sizeDef.unit));
    height = KoUnit::ptFromUnit(sizeDef.height, static_cast<KoUnit::Unit>(sizeDef.unit));
    
    if((layout.ptWidth == width) && (layout.ptHeight == height)) {
      layout.format = KoPageFormat::formatFromString(sizeDef.title);
      layout.orientation = PG_PORTRAIT;
      found = true;
    } else if((layout.ptWidth == height) && (layout.ptHeight == width)) {
      layout.format = KoPageFormat::formatFromString(sizeDef.title);
      layout.orientation = PG_LANDSCAPE;
      found = true;
    }
  }
}
