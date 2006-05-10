/* This file is part of the KDE project
   Copyright (C) 2002, Dirk Schönberger <dirk.schoenberger@sz-online.de>

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

#ifndef KARBONAIPARSERBASE_H
#define KARBONAIPARSERBASE_H

#include <aiparserbase.h>
#include <q3ptrlist.h>
#include <QString>
#include <qpair.h>

#include "aicolor.h"

#include <KoPoint.h>
#include <core/vcomposite.h>
#include <core/vdocument.h>
#include <core/vlayer.h>
#include <core/vgroup.h>
#include <core/vfill.h>
#include <core/vstroke.h>
#include <qdom.h>

#include <q3ptrstack.h>

/**
  *@author 
  */
typedef QPair<QString,QString> Parameter;
typedef Q3PtrList<Parameter> Parameters;
typedef Q3PtrList<PathElement> PathElements;

typedef enum { POT_Filled = 1, POT_Stroked = 2, POT_FilledStroked = 3, POT_Clip = 4, POT_Ignore = 8, POT_Leave = -1, POT_Other = 0 } PathOutputType;
typedef enum { PTT_Output = 1, PTT_Combine = 2 } PathTransferType;

// typedef struct { int llx, lly, urx, ury; } BoundingBox;

class KarbonAIParserBase;
class KarbonGStateHandler;
class KarbonStructureHandler;
class KarbonPathHandler;
class KarbonDocumentHandler;

class KarbonDocumentHandler : public DocumentHandlerBase
{
  private:
    KarbonAIParserBase *delegate;
  public:
    KarbonDocumentHandler (KarbonAIParserBase *delegate) : DocumentHandlerBase () { this->delegate = delegate; }

    void gotBoundingBox (int llx, int lly, int urx, int ury);
    void gotCreationDate (const char *val1,const char *val2);
    void gotProcessColors (int colors);
};

class KarbonGStateHandler : public GStateHandlerBase
{
  private:
    KarbonAIParserBase *delegate;
  public:
    KarbonGStateHandler (KarbonAIParserBase *delegate) : GStateHandlerBase() { this->delegate = delegate; }

    void gotFillColor (AIColor &color);
    void gotStrokeColor (AIColor &color);

    void gotFlatness (double val);
    void gotLineWidth (double val);
    void gotLineCaps (int val);
    void gotLineJoin (int val);
    void gotMiterLimit (double val);
    void gotWindingOrder (int val);

};

class KarbonStructureHandler : public StructureHandlerBase
{
  private:
    KarbonAIParserBase *delegate;
  public:
    KarbonStructureHandler (KarbonAIParserBase *delegate) : StructureHandlerBase() { this->delegate = delegate; }

   void gotBeginGroup (bool clipping);
   void gotEndGroup (bool clipping);
   void gotBeginCombination ();
   void gotEndCombination ();

};

class KarbonPathHandler : public PathHandlerBase
{
  private:
    KarbonAIParserBase *delegate;
    FillMode m_fm;
  public:
    KarbonPathHandler (KarbonAIParserBase *delegate) : PathHandlerBase ()
    {
       m_fm = FM_EvenOdd;
       this->delegate = delegate;
    }

  void gotPathElement (PathElement &element);
  void gotFillPath (bool closed, bool reset);
  void gotStrokePath (bool closed);
  void gotIgnorePath (bool closed, bool reset);
  void gotClipPath (bool closed);
  void gotFillMode (FillMode fm);

};

class KarbonAIParserBase : public AIParserBase {
  friend class KarbonDocumentHandler;
  friend class KarbonGStateHandler;
  friend class KarbonStructureHandler;
  friend class KarbonPathHandler;

public: 
	KarbonAIParserBase();
	~KarbonAIParserBase();

  bool parse (QIODevice& fin, QDomDocument &doc);
private:
  VPath *m_curKarbonPath;
  VDocument *m_document;
  VLayer *m_layer;
  VPath *m_combination;
  Q3PtrStack<VGroup> m_groupStack;

  FillMode m_fm;
  PathOutputType m_pot;
  PathTransferType m_ptt;

//  BoundingBox  m_bbox;
  KoRect m_bbox;
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
  void ensureLayer ();

  VFill m_emptyFill;
  VStroke m_emptyStroke;

protected:
  void setupHandlers();
  void teardownHandlers();

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
