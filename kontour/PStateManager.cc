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

#include <PStateManager.h>

//#include <qvector.h>
#include <qobject.h>
#include <kapp.h>
#include <kconfig.h>
#include <kdebug.h>

#include <GObject.h>
#include <GText.h>
#include <UnitBox.h>
#include <KIllustrator_factory.h>

PStateManager* PStateManager::managerInstance = 0L;

PStateManager::PStateManager ()
{
   defaultUnit = UnitPoint;
   readDefaultSettings ();
}

PStateManager* PStateManager::instance ()
{
   if (managerInstance == 0L)
      managerInstance = new PStateManager ();
   return managerInstance;
}

void PStateManager::readDefaultSettings ()
{

   kdDebug(38000)<<"PStateManager::readDefaultSettings()"<<endl;
   KConfig* config = KIllustratorFactory::global()->config ();

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
   else if (value == "didot")
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

   showSplash = config->readBoolEntry ("ShowSplash", false);

   config->setGroup ("DefaultObjectProperties");

   GObject::OutlineInfo oInfo;
   oInfo.color = config->readColorEntry ("OutlineColor", &Qt::black);
   oInfo.style = (Qt::PenStyle) config->readNumEntry ("OutlineStyle", Qt::SolidLine);
   QString test=config->readEntry("OutlineWidth", "hiho");
   oInfo.width = config->readDoubleNumEntry ("OutlineWidth", 1.0);
   oInfo.startArrowId=config->readNumEntry("StartArrowID", 0);
   oInfo.endArrowId=config->readNumEntry("EndArrowID", 0);
   oInfo.mask = GObject::OutlineInfo::Color|GObject::OutlineInfo::Style|GObject::OutlineInfo::Width;
   GObject::setDefaultOutlineInfo (oInfo);

   GObject::FillInfo fInfo;
   fInfo.color = config->readColorEntry ("FillColor", &Qt::white);
   fInfo.pattern = (Qt::BrushStyle)config->readNumEntry("FillPattern", 0);
   fInfo.fstyle = (GObject::FillInfo::Style)config->readNumEntry("FillStyle", 0);
   fInfo.gradient.setColor1(config->readColorEntry("GradientColor1", &Qt::black));
   fInfo.gradient.setColor2(config->readColorEntry("GradientColor2", &Qt::white));
   fInfo.gradient.setStyle((Gradient::Style)config->readNumEntry("GradientStyle", 0));
   fInfo.gradient.setAngle(config->readNumEntry("GradientAngle", 0));
   fInfo.mask = GObject::FillInfo::All;
   GObject::setDefaultFillInfo(fInfo);

   GText::TextInfo tInfo;
   QFont tmp(QFont::defaultFont ());
   tInfo.font = config->readFontEntry ("Font", &tmp);
   tInfo.mask = GText::TextInfo::Font;
   GText::setDefaultTextInfo (tInfo);
}

void PStateManager::setDefaultMeasurementUnit (MeasurementUnit unit)
{
//   MeasurementUnit old = defaultUnit;
   defaultUnit = unit;
   UnitBox::setDefaultMeasurementUnit (defaultUnit);
/*   if (old != defaultUnit)
      emit settingsChanged ();*/
}

void PStateManager::setStepSizes (float small, float big)
{
   smallStep = small;
   bigStep = big;
//   emit settingsChanged ();
}

void PStateManager::setDuplicateOffsets (float x, float y)
{
   dupXOff = x;
   dupYOff = y;
//   emit settingsChanged ();
}

void PStateManager::saveDefaultSettings ()
{

   kdDebug(38000)<<"PStateManager::saveDefaultSettings()"<<endl;
   KConfig* config = KIllustratorFactory::global()->config ();

   config->setGroup ("General");
   switch (defaultUnit)
   {
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
   config->writeEntry ("OutlineWidth", QString::number(oInfo.width));
   config->writeEntry("StartArrowID", oInfo.startArrowId);
   config->writeEntry("EndArrowID", oInfo.endArrowId);

   GObject::FillInfo fInfo = GObject::getDefaultFillInfo ();
   config->writeEntry("FillColor", fInfo.color);
   config->writeEntry("FillPattern", (int)fInfo.pattern);
   config->writeEntry("FillStyle", (int)fInfo.fstyle);
   config->writeEntry("GradientColor1", fInfo.gradient.getColor1());
   config->writeEntry("GradientColor2", fInfo.gradient.getColor2());
   config->writeEntry("GradientStyle", (int)fInfo.gradient.getStyle());
   config->writeEntry("GradientAngle", fInfo.gradient.getAngle());

   GText::TextInfo tInfo = GText::getDefaultTextInfo ();
   config->writeEntry ("Font", tInfo.font);

   config->sync ();
}

//#include <PStateManager.moc>
