/* This file is part of the KDE project
   Copyright (C) 2005 Yolla Indria <yolla.indria@gmail.com>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef POWERPOINTIMPORT_H
#define POWERPOINTIMPORT_H

#include <KoFilter.h>
#include <KoStore.h>

#include <q3cstring.h>

#include "libppt.h"

using namespace Libppt;

class KoXmlWriter;

class PowerPointImport : public KoFilter 
{
Q_OBJECT

public:
  PowerPointImport ( QObject *parent, const char* name, const QStringList& );
  virtual ~PowerPointImport();
  virtual KoFilter::ConversionStatus convert( const QByteArray& from, 
        const QByteArray& to );

private:
  class Private;
  Private* d;

  QByteArray createManifest();
  QByteArray createStyles();
  QByteArray createContent();

  int drawingObjectCounter;

  void processSlideForBody( unsigned slideNo, Slide* slide, KoXmlWriter* xmlWriter );
  void processObjectForBody( Object* object, KoXmlWriter* xmlWriter );
  void processGroupObjectForBody( GroupObject* groupObject, KoXmlWriter* xmlWriter );
  void processDrawingObjectForBody( DrawObject* drawObject, KoXmlWriter* xmlWriter );
  void processTextObjectForBody( TextObject* textObject, KoXmlWriter* xmlWriter );


  void processSlideForStyle( unsigned slideNo, Slide* slide, KoXmlWriter* xmlWriter );
  void processObjectForStyle( Object* object, KoXmlWriter* xmlWriter );
  void processGroupObjectForStyle( GroupObject* groupObject, KoXmlWriter* xmlWriter );
  void processDrawingObjectForStyle( DrawObject* drawObject, KoXmlWriter* xmlWriter );
  void processTextObjectForStyle( TextObject* textObject, KoXmlWriter* xmlWriter );

  void processEllipse(DrawObject* drawObject, KoXmlWriter* xmlWriter); 
  void processRectangle(DrawObject* drawObject, KoXmlWriter* xmlWriter); 
  void processRoundRectangle(DrawObject* drawObject, KoXmlWriter* xmlWriter); 
  void processDiamond(DrawObject* drawObject, KoXmlWriter* xmlWriter); 
  void processTriangle(DrawObject* drawObject, KoXmlWriter* xmlWriter); 
  void processTrapezoid(DrawObject* drawObject, KoXmlWriter* xmlWriter); 
  void processParallelogram(DrawObject* drawObject, KoXmlWriter* xmlWriter); 
  void processHexagon(DrawObject* drawObject, KoXmlWriter* xmlWriter); 
  void processOctagon(DrawObject* drawObject, KoXmlWriter* xmlWriter); 
  void processArrow(DrawObject* drawObject, KoXmlWriter* xmlWriter); 
  void processLine(DrawObject* drawObject, KoXmlWriter* xmlWriter); 
  void processSmiley(DrawObject* drawObject, KoXmlWriter* xmlWriter); 
  void processHeart(DrawObject* drawObject, KoXmlWriter* xmlWriter); 
  void processFreeLine(DrawObject* drawObject, KoXmlWriter* xmlWriter); 
};

#endif // POWERPOINTIMPORT_H
