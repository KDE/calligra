/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by  
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include "PStateManager.h"
#include "PStateManager.moc"

#include <kapp.h>
#include <qvector.h>
#include "version.h"
#include "GObject.h"
#include "GText.h"
#include "UnitBox.h"
#include <kconfig.h>

PStateManager* PStateManager::managerInstance = 0L;

PStateManager::PStateManager () {
  defaultUnit = UnitPoint;
  readDefaultSettings ();
}

PStateManager* PStateManager::instance () {
  if (managerInstance == 0L)
    managerInstance = new PStateManager ();
  return managerInstance;
}

void PStateManager::readDefaultSettings () {
#if NEWKDE
  KConfig* config = kapp->config ();
#else
  KConfig* config = kapp->getConfig ();
#endif
  QString oldgroup = config->group ();
  
  config->setGroup ("General");
  QString value = config->readEntry ("DefaultUnit", "pt");
  if (value == "mm")
    defaultUnit = UnitMillimeter;
  else if (value == "inch")
    defaultUnit = UnitInch;
  else if (value == "cm")
    defaultUnit = UnitCentimeter;
  else if (value == "pica")
    defaultUnit = UnitPica;
  else if (value = "didot")
    defaultUnit = UnitDidot;
  else if (value == "cicero")
    defaultUnit = UnitCicero;  
  else
    defaultUnit = UnitPoint;
  UnitBox::setDefaultMeasurementUnit (defaultUnit);

  smallStep = config->readDoubleNumEntry ("SmallStep", 2.0);
  bigStep = config->readDoubleNumEntry ("BigStep", 10.0);
  dupXOff = config->readDoubleNumEntry ("DuplicateXOffset", 10.0);
  dupYOff = config->readDoubleNumEntry ("DuplicateYOffset", 10.0);

  showSplash = config->readBoolEntry ("ShowSplash", true);

  config->setGroup ("DefaultObjectProperties");

  GObject::OutlineInfo oInfo;
  oInfo.color = config->readColorEntry ("OutlineColor", &black);
  oInfo.style = (PenStyle) config->readNumEntry ("OutlineStyle", SolidLine);
  oInfo.width = (BrushStyle) config->readDoubleNumEntry ("OutlineWidth", 1.0);
  oInfo.mask = GObject::OutlineInfo::Color | GObject::OutlineInfo::Style | 
    GObject::OutlineInfo::Width;
  GObject::setDefaultOutlineInfo (oInfo);

  GObject::FillInfo fInfo;
  fInfo.color = config->readColorEntry ("FillColor", &white);
  fInfo.fstyle = GObject::FillInfo::NoFill;
  //  fInfo.style = (BrushStyle) config->readNumEntry ("FillStyle", NoBrush);
  fInfo.mask = GObject::FillInfo::Color | GObject::FillInfo::FillStyle;
  GObject::setDefaultFillInfo (fInfo);

  GText::TextInfo tInfo;
  QFont tmp(QFont::defaultFont ());
  tInfo.font = config->readFontEntry ("Font", &tmp);
  tInfo.mask = GText::TextInfo::Font;
  GText::setDefaultTextInfo (tInfo);

  config->setGroup ("RecentFiles");
  recentFiles.clear ();
  for (unsigned int i = 0; i < 4; i++) {
    QString key;
    key.sprintf ("%d", i + 1);
    QString file = config->readEntry ((const char *) key);
    if (! file.isNull ())
      recentFiles.append ((const char*) file);
  }

  config->setGroup (oldgroup);

}
  
void PStateManager::addRecentFile (const char* fname) {
  unsigned int i;

  for (i = 0; i < recentFiles.count (); i++) {
    if (::strcmp (recentFiles.at (i), fname) == 0) {
      // file already in the list
      if (i == 0)
	// file is most recent file -> ignore it
	return;
      else
	// remove the old entry
	recentFiles.remove (i);
    }
  }
  // now insert the new entry
  recentFiles.insert (0, fname);
  if (recentFiles.count () > 4) 
    recentFiles.removeLast ();
  emit recentFilesChanged ();
}

QStrList PStateManager::getRecentFiles () {
  return recentFiles;
}

MeasurementUnit PStateManager::defaultMeasurementUnit () {
  return defaultUnit;
}

void PStateManager::setDefaultMeasurementUnit (MeasurementUnit unit) {
  MeasurementUnit old = defaultUnit;
  defaultUnit = unit;
  UnitBox::setDefaultMeasurementUnit (defaultUnit);
  if (old != defaultUnit)
    emit settingsChanged ();
}

float PStateManager::smallStepSize () {
  return smallStep;
}

float PStateManager::bigStepSize () {
  return bigStep;
}

void PStateManager::setStepSizes (float small, float big) {
  smallStep = small;
  bigStep = big;
  emit settingsChanged ();
}

float PStateManager::duplicateXOffset () {
  return dupXOff;
}

float PStateManager::duplicateYOffset () {
  return dupYOff;
}

void PStateManager::setDuplicateOffsets (float x, float y) {
  dupXOff = x;
  dupYOff = y;
  emit settingsChanged ();
}

bool PStateManager::showSplashScreen () {
  return showSplash;
}

void PStateManager::saveDefaultSettings () {
#if NEWKDE
  KConfig* config = kapp->config ();
#else
  KConfig* config = kapp->getConfig ();
#endif
  QString oldgroup = config->group ();

  config->setGroup ("General");
  switch (defaultUnit) {
  case UnitPoint:
    config->writeEntry ("DefaultUnit", "pt");
    break;
  case UnitMillimeter:
    config->writeEntry ("DefaultUnit", "mm");
    break;
  case UnitInch:
    config->writeEntry ("DefaultUnit", "inch");
    break;
  case UnitCentimeter:
    config->writeEntry ("DefaultUnit", "cm");
    break;
  case UnitPica:
    config->writeEntry ("DefaultUnit", "pica");
    break;
  case UnitDidot:
    config->writeEntry ("DefaultUnit", "didot");
    break;
  case UnitCicero:
    config->writeEntry ("DefaultUnit", "cicero");
    break;
  }

  config->writeEntry ("SmallStep", smallStep);
  config->writeEntry ("BigStep", bigStep);
  config->writeEntry ("DuplicateXOffset", dupXOff);
  config->writeEntry ("DuplicateYOffset", dupYOff);

  config->setGroup ("DefaultObjectProperties");

  GObject::OutlineInfo oInfo = GObject::getDefaultOutlineInfo ();
  config->writeEntry ("OutlineColor", oInfo.color);
  config->writeEntry ("OutlineStyle", (int) oInfo.style);
  config->writeEntry ("OutlineWidth", (double) oInfo.width);

  GObject::FillInfo fInfo = GObject::getDefaultFillInfo ();
  config->writeEntry ("FillColor", fInfo.color);
  //  config->writeEntry ("FillStyle", (int) fInfo.style);

  GText::TextInfo tInfo = GText::getDefaultTextInfo ();
  config->writeEntry ("Font", tInfo.font);

  config->setGroup ("RecentFiles");
  for (unsigned int i = 0; i < recentFiles.count (); i++) {
    QString key;
    key.sprintf ("%d", i + 1);
    config->writeEntry ((const char *) key, recentFiles.at (i));
  }

  config->setGroup (oldgroup);
  config->sync ();
}
