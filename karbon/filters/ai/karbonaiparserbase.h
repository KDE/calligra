/***************************************************************************
                          karbonaiparserbase.h  -  description
                             -------------------
    begin                : Wed Feb 20 2002
    copyright            : (C) 2002 by 
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KARBONAIPARSERBASE_H
#define KARBONAIPARSERBASE_H

#include <aiparserbase.h>
#include <qstring.h>
#include <qpair.h>
#include <qptrlist.h>

#include "aicolor.h"

/**
  *@author 
  */
typedef QPair<QString,QString> Parameter;
typedef QPtrList<Parameter> Parameters;
typedef QPtrList<PathElement> PathElements;

typedef enum { POT_Filled, POT_Stroked, POT_FilledStroked, POT_Clip, POT_Ignore, POT_Other } PathOutputType;

typedef struct { int llx, lly, urx, ury; } BoundingBox;

class KarbonAIParserBase : public AIParserBase  {
public: 
	KarbonAIParserBase();
	~KarbonAIParserBase();

private:
  PathElements m_curPath;
  FillMode m_fm;

  BoundingBox  m_bbox;
  AIColor m_strokeColor;
  AIColor m_fillColor;
  double m_lineWidth;
  double m_flatness;
  int m_lineCaps;
  int m_lineJoin;
  double m_miterLimit;
  int m_windingOrder;

  void doOutputCurrentPath(PathOutputType type, bool closed);
  void fillColorAttributes(Parameters &parameters, AIColor &color);

protected:
  void parsingStarted();
  void parsingFinished();

  QString getHeader();
  QString getFooter();
  QString getParamList(Parameters& params);

  void gotPathElement (PathElement &element);
  void gotFillPath (bool closed, bool reset, FillMode fm = FM_NonZero);
  void gotStrokePath (bool closed);
  void gotIgnorePath (bool closed, bool reset);
  void gotClipPath (bool closed);

  void gotFillColor (AIColor &color);
  void gotStrokeColor (AIColor &color);
  void gotBoundingBox (int llx, int lly, int urx, int ury);

  void gotFlatness (double val);
  void gotLineWidth (double val);
  void gotLineCaps (int val);
  void gotLineJoin (int val);
  void gotMiterLimit (double val);
  void gotWindingOrder (int val);

  virtual void gotStartTag (const char *tagName, Parameters& params);
  virtual void gotEndTag (const char *tagName);
  virtual void gotSimpleTag (const char *tagName, Parameters& params);
};

#endif
