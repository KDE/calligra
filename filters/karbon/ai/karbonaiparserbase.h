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
#include <qptrlist.h>
#include <qstring.h>
#include <qpair.h>

#include "aicolor.h"

#include <koPoint.h>
#include <core/vpath.h>
#include <core/vlayer.h>
#include <core/vgroup.h>
#include <core/vfill.h>
#include <core/vstroke.h>
#include <qdom.h>

#include <qptrstack.h>

#include "vdocument.h"

/**
  *@author 
  */
typedef QPair<QString,QString> Parameter;
typedef QPtrList<Parameter> Parameters;
typedef QPtrList<PathElement> PathElements;

typedef enum { POT_Filled = 1, POT_Stroked = 2, POT_FilledStroked = 3, POT_Clip = 4, POT_Ignore = 8, POT_Leave = -1, POT_Other = 0 } PathOutputType;
typedef enum { PTT_Output = 1, PTT_Combine = 2 } PathTransferType;

typedef struct { int llx, lly, urx, ury; } BoundingBox;

class KarbonAIParserBase : public AIParserBase  {
public: 
	KarbonAIParserBase();
	~KarbonAIParserBase();

  bool parse (QIODevice& fin, QDomDocument &doc);
private:
  VPath *m_curKarbonPath;
  VDocument *m_document;
  VLayer *m_layer;
  VPath *m_combination;
  QPtrStack<VGroup> m_groupStack;

  FillMode m_fm;
  PathOutputType m_pot;
  PathTransferType m_ptt;

  BoundingBox  m_bbox;
  VFill m_fill;
  VStroke m_stroke;
/**  AIColor m_strokeColor;
  AIColor m_fillColor;
  double m_lineWidth;
  double m_flatness;
  int m_lineCaps;
  int m_lineJoin;
  double m_miterLimit; */
  int m_windingOrder;

  void doOutputCurrentPath2(PathOutputType type);
  const VColor toKarbonColor (const AIColor &color);

protected:
  void parsingStarted();
  void parsingFinished();

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
  void gotBeginGroup (bool clipping);
  void gotEndGroup (bool clipping);
  void gotBeginCombination ();
  void gotEndCombination ();

  virtual void gotStartTag (const char *tagName, Parameters& params);
  virtual void gotEndTag (const char *tagName);
  virtual void gotSimpleTag (const char *tagName, Parameters& params);
};

#endif
