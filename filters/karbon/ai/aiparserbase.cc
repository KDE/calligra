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

#include "aiparserbase.h"
#include "ai88handler.h"
#include "ai3handler.h"
#include <QRegExp>
#include <QStringList>

typedef struct {
  char* op;
  AIOperation action;
} AIOperationMapping;

typedef struct {
  char* op;
  PSOperation action;
} PSOperationMapping;

typedef struct {
  char* op;
  CommentOperation action;
} CommentOperationMapping;

static AIOperationMapping aiMappings[] = {
  { "k", AIO_SetFillColorCMYK },
  { "K", AIO_SetStrokeColorCMYK },
  { "g", AIO_SetFillColorGray },
  { "G", AIO_SetStrokeColorGray },
  { "x", AIO_SetFillColorCustom },
  { "X", AIO_SetStrokeColorCustom },
  { "p", AIO_SetFillPattern },
  { "P", AIO_SetStrokePattern },
  { "O", AIO_SetFillOverprinting },
  { "R", AIO_SetStrokeOverprinting },
  { "i", AIO_SetFlatness },
  { "J", AIO_SetLineCap },
  { "j", AIO_SetLineJoin },
  { "M", AIO_SetMiterLimit },
  { "w", AIO_SetLineWidth },
  { "d", AIO_SetDash },
  { "q", AIO_BeginGroupClip },
  { "Q", AIO_EndGroupClip },
  { "m", AIO_MoveTo },
  { "l", AIO_LineToSmooth },
  { "L", AIO_LineToCorner },
  { "c", AIO_CurveToSmooth },
  { "C", AIO_CurveToCorner },

  { "v", AIO_CurveToOmitC1Smooth },
  { "V", AIO_CurveToOmitC1Corner },
  { "y", AIO_CurveToOmitC2Smooth },
  { "Y", AIO_CurveToOmitC2Corner },

  { "H", AIO_PathIgnoreNoReset },
  { "h", AIO_PathIgnoreNoResetClose },
  { "W", AIO_PathClipPath },
  { "N", AIO_PathIgnoreReset },
  { "n", AIO_PathIgnoreResetClose },
  { "F", AIO_PathFillNonZero },
  { "f", AIO_PathFillNonZeroClose },
  { "B", AIO_PathFillNoReset },
  { "b", AIO_PathFillNoResetClose },
  { "S", AIO_PathStroke },
  { "s", AIO_PathStrokeClose },
  { "D", AIO_SetWindingOrder },
  { "Z", AIO_FontEncoding },
  { "E", AIO_PatternDefinition },
  { "A", AIO_LockElement },

  { "z", AIO_SetCurrentText },
  { "a", AIO_TextBlockFillStroke },
  { "e", AIO_TextBlockFill },
  { "I", AIO_TextBlockAppend },
  { "o", AIO_TextBlockIgnore },
  { "r", AIO_TextBlockStroke },
  { "t", AIO_TextOutput },
  { "T", AIO_TextBlockEnd },
  { "`", AIO_GsaveIncludeDocument },
  { "~", AIO_Grestore },

  { "u", AIO_BeginGroupNoClip },
  { "U", AIO_EndGroupNoClip },
  { "*u", AIO_BeginCombination },
  { "*U", AIO_EndCombination },

  { "XR", AIO_SetFillMode },

  { NULL, AIO_Other }
};

static PSOperationMapping psMappings[] = {
  { "get", PSO_Get },
  { "exec", PSO_Exec },
  { "string", PSO_String },
  { "bind", PSO_Bind },
  { "def", PSO_Def },
  { "userdict", PSO_Userdict },
  { "dict", PSO_Dict },
  { "dup", PSO_Dup },
  { "begin", PSO_Begin },
  { "put", PSO_Put },
  { NULL, PSO_Other },
};

static CommentOperationMapping commentMappings[] = {
  { "BeginProlog", CO_BeginProlog },
  { "BeginSetup", CO_BeginSetup },
  { "BeginProcSet", CO_BeginProcSet },
  { "BeginResource", CO_BeginResource },
  { "BeginEncoding", CO_BeginEncoding },
  { "BeginPattern", CO_BeginPattern },
  { "BeginDocument", CO_BeginPattern },
  { "Trailer", CO_Trailer },
  { "EndProlog", CO_EndProlog },
  { "EndSetup", CO_EndSetup },
  { "EndProcSet", CO_EndProcSet },
  { "EndResource", CO_EndResource },
  { "EndEncoding", CO_EndEncoding },
  { "EndPattern", CO_EndPattern },
  { "EndDocument", CO_EndDocument },

  { "Title", CO_Title },
  { "Creator", CO_Creator },

  { "EOF", CO_Ignore },
  { "Note", CO_Ignore },
  { "EndComments", CO_Ignore },
  { "PS-Adobe", CO_Ignore },

  { "BoundingBox", CO_BoundingBox },
  { "TemplateBox", CO_TemplateBox },
  { "AI3_Margin", CO_Margin },

  { "For", CO_For },
  { "CreationDate",  CO_CreationDate },
  { "DocumentFonts",  CO_DocumentFonts },
  { "DocumentFiles",  CO_DocumentFiles },
  { "ColorUsage",  CO_ColorUsage },
  { "DocumentProcSets",  CO_DocumentProcSets },
  { "DocumentSuppliedProcSets", CO_DocumentSuppliedProcSets },

  { "DocumentProcessColors",  CO_DocumentProcessColors },
  { "DocumentCustomColors",  CO_DocumentCustomColors },
  { "CMYKCustomColor", CO_CMYKCustomColor },
  { "TileBox",  CO_TileBox },
  { "%+",  CO_Continuation },

  { "Template",  CO_Template },
  { "PageOrigin",  CO_PageOrigin },
  { "PrinterName",  CO_PrinterName },
  { "PrinterRect",  CO_PrinterRect },
  { "Note",  CO_Note },

  { "DocumentNeededResources", CO_DocumentNeededResources },


  { "IncludeFont",  CO_IncludeFont },
  { "BeginBrushPattern", CO_BeginBrushPattern },
  { "EndBrushPattern", CO_EndBrushPattern },
  { "BeginGradient", CO_BeginGradient },
  { "EndGradient", CO_EndGradient },
  { "BeginPalette", CO_BeginPalette },
  { "EndPalette", CO_EndPalette },

  { "IncludeFile", CO_IncludeFile },
  { "IncludeResource", CO_IncludeResource },

  { NULL, CO_Other }
};

AIParserBase::AIParserBase() : m_debug(false), m_ignoring(false), m_sink (DS_Other), m_continuationMode(CM_None)
 {
  m_gstateHandler = NULL;
  m_structureHandler = NULL;
  m_pathHandler = NULL;
  m_miscGStateHandler = NULL;
  m_documentHandler = NULL;
  m_moduleHandler = NULL;
  m_embeddedHandler = NULL;
  m_ai88Handler = new AI88Handler(this);
  m_ai3Handler = new AI3Handler(this);
}

AIParserBase::~AIParserBase(){
  delete m_ai88Handler;
  delete m_ai3Handler;

}

bool AIParserBase::parse (QIODevice& fin){
  return AILexer::parse (fin);
}

void AIParserBase::gotComment (const char *value) {
  int llx, lly, urx, ury;

  CommentOperation cop = getCommentOperation (value);
  switch (cop) {
    case CO_BeginDocument :
       if (m_moduleHandler) m_moduleHandler->gotBeginSection (ST_Document, value);
       break;
    case CO_EndDocument :
       if (m_moduleHandler) m_moduleHandler->gotBeginSection (ST_Document, value);
       break;
    case CO_BeginPattern :
       if (m_moduleHandler) m_moduleHandler->gotBeginSection (ST_Pattern, value);
       break;
    case CO_EndPattern :
       if (m_moduleHandler) m_moduleHandler->gotBeginSection (ST_Pattern, value);
       break;
    case CO_BeginProlog :
       if (m_moduleHandler) m_moduleHandler->gotBeginSection (ST_Prolog, value);
       break;
    case CO_BeginProcSet :
       if (m_moduleHandler) m_moduleHandler->gotBeginSection (ST_ProcSet, value);
       if (m_debug) qDebug ("start ignoring");
       m_ignoring = true;
       break;
    case CO_BeginResource :
       if (m_moduleHandler) m_moduleHandler->gotBeginSection (ST_Resource, value);
       if (m_debug) qDebug ("start ignoring");
       m_ignoring = true;
       break;
    case CO_BeginEncoding :
       if (m_moduleHandler) m_moduleHandler->gotBeginSection (ST_Encoding, value);
       m_ignoring = false;
       break;
    case CO_IncludeFont :
       if (m_debug) qDebug ("start ignoring");
       m_ignoring = true;
       break;
    case CO_BeginBrushPattern :
       if (m_moduleHandler) m_moduleHandler->gotBeginSection (ST_BrushPattern, value);
       break;
    case CO_BeginGradient :
       if (m_moduleHandler) m_moduleHandler->gotBeginSection (ST_Gradient, value);
       break;
    case CO_Trailer :
       if (m_debug) qDebug ("start ignoring");
       m_ignoring = true;
       break;
    case CO_BeginPalette :
       if (m_moduleHandler) m_moduleHandler->gotBeginSection (ST_Palette, value);
       break;
    case CO_BeginSetup :
       if (m_moduleHandler) m_moduleHandler->gotBeginSection (ST_Setup, value);
       break;
    case CO_EndSetup :
       cleanupArrays();
       if (m_moduleHandler) m_moduleHandler->gotEndSection (ST_Setup, value);
       break;
    case CO_EndProlog :
       if (m_moduleHandler) m_moduleHandler->gotEndSection (ST_Prolog, value);
       break;
    case CO_EndProcSet :
       if (m_moduleHandler) m_moduleHandler->gotEndSection (ST_ProcSet, value);
       if (m_debug) qDebug ("stop ignoring");
       m_ignoring = false;
       break;
    case CO_EndResource :
       if (m_moduleHandler) m_moduleHandler->gotEndSection (ST_Resource, value);
       if (m_debug) qDebug ("stop ignoring");
       m_ignoring = false;
       break;
    case CO_EndEncoding :
       cleanupArrays();
       if (m_moduleHandler) m_moduleHandler->gotEndSection (ST_Encoding, value);
       break;
    case CO_EndBrushPattern :
       cleanupArrays();
       if (m_moduleHandler) m_moduleHandler->gotEndSection (ST_BrushPattern, value);
       break;
    case CO_EndGradient :
       cleanupArrays();
       if (m_moduleHandler) m_moduleHandler->gotEndSection (ST_Gradient, value);
       break;
    case CO_EndPalette :
       cleanupArrays();
       if (m_moduleHandler) m_moduleHandler->gotEndSection (ST_Palette, value);
       break;
    case CO_Ignore :
      break;
    case CO_BoundingBox :
      if (getRectangle (value, llx, lly, urx, ury))
      {
        if (m_documentHandler) m_documentHandler->gotBoundingBox (llx, lly, urx, ury);
      }
      break;
    case CO_TemplateBox :
      if (getRectangle (value, llx, lly, urx, ury))
      {
        if (m_documentHandler) m_documentHandler->gotTemplateBox (llx, lly, urx, ury);
      }
      break;
    case CO_Margin :
      if (getRectangle (value, llx, lly, urx, ury))
      {
        if (m_documentHandler) m_documentHandler->gotMargin (llx, lly, urx, ury);
      }
      break;
    case CO_Title :
      if (m_documentHandler) m_documentHandler->gotTitle (getValue (value));
      break;
    case CO_Creator :
      if (m_documentHandler) m_documentHandler->gotCreator (getValue (value));
      break;
    case CO_DocumentFonts :
      _handleDocumentFonts (value);
      m_continuationMode = CM_DocumentFonts;
      break;
    case CO_DocumentFiles :
      _handleDocumentFiles (value);
      m_continuationMode = CM_DocumentFiles;
      break;
    case CO_DocumentCustomColors :
      _handleDocumentCustomColors (value);
      m_continuationMode = CM_DocumentFiles;
      break;
    case CO_CMYKCustomColor :
      _handleCMYKCustomColor (value);
      m_continuationMode = CM_CMYKCustomColor;
      break;
    case CO_DocumentNeededResources :
      _handleDocumentNeededResources (value);
      m_continuationMode = CM_DocumentNeededResources;
      break;
    case CO_DocumentProcessColors :
      _handleDocumentProcessColors (value);
      break;
    case CO_CreationDate :
      _handleCreationDate (value);
      break;
    case CO_IncludeFile :
      break;
    case CO_IncludeResource :
      _handleIncludeResource (value);
      break;
    case CO_Continuation :
      switch (m_continuationMode) {
        case CM_DocumentFonts : _handleDocumentFonts (value); break;
        case CM_DocumentFiles : _handleDocumentFiles (value); break;
        case CM_DocumentCustomColors : _handleDocumentCustomColors (value); break;
        case CM_CMYKCustomColor :  _handleCMYKCustomColor (value); break;
        case CM_DocumentNeededResources :  _handleDocumentNeededResources (value); break;

        default : qWarning ("unknown continuation mode %d",m_continuationMode);
      }
      break;

    default :
      qWarning( "unhandled comment: %s", value );
  }
}

void AIParserBase::handleElement (AIElement &element)
{
  if (m_ignoring) return;

  if (m_sink == DS_Array)
  {
    if (m_debug) qDebug ("in mode array");
    Q3ValueVector<AIElement> &elementArray = m_arrayStack.top();
    elementArray.push_back(element);
  }
  if (m_sink == DS_Block)
  {
    if (m_debug) qDebug ("in mode block");
    Q3ValueVector<AIElement> &elementArray = m_blockStack.top();
    elementArray.push_back(element);
  }
  else
  {
    if (m_debug) qDebug ("in mode stack");
    m_stack.push (element);
  }
}

void AIParserBase::gotIntValue (int value) {
  if (m_debug) qDebug ("got int value");
  if (m_ignoring) return;
  AIElement element (value);
  handleElement (element);
  if (m_debug) qDebug ("/got int value");
}

void AIParserBase::gotDoubleValue (double value) {
  if (m_debug) qDebug ("got double value");
  if (m_ignoring) return;
  AIElement element (value);
  handleElement (element);
  if (m_debug) qDebug ("/got double value");
}

void AIParserBase::gotStringValue (const char *value) {
  if (m_debug) qDebug ("got string value");
  if (m_ignoring) return;
  if (value == NULL) value = "";
  if (m_debug) qDebug ("string: %s", value);
  AIElement element (value);
  handleElement (element);
  if (m_debug) qDebug ("/got string value");

}

void AIParserBase::gotReference (const char *value) {
  if (m_debug) qDebug ("got reference value");

  if (m_ignoring) return;
  if (value == NULL) value = "";
  if (m_debug) qDebug ("reference: %s", value);
  QString string(value);
  AIElement element (string, AIElement::Reference);
  handleElement (element);
  if (m_debug) qDebug ("/got reference value");

}

void AIParserBase::gotByte (uchar value) {
  if (m_debug) qDebug ("got byte value");

  if (m_ignoring) return;
  AIElement element (value);
  handleElement (element);
  if (m_debug) qDebug ("/got byte value");
}

void AIParserBase::gotByteArray (const QByteArray &data) {
  if (m_ignoring) return;
  AIElement element (data);
  handleElement (element);
}


void AIParserBase::gotArrayStart () {
  if (m_ignoring) return;
  if (m_debug) qDebug ("got array start");

  Q3ValueVector<AIElement> array;
  m_arrayStack.push (array);

  m_sink = DS_Array;
}

void AIParserBase::gotBlockStart () {
  if (m_ignoring) return;
  if (m_debug) qDebug ("got block start");

  Q3ValueVector<AIElement> array;
  m_blockStack.push (array);

  m_sink = DS_Block;
}

void AIParserBase::gotArrayEnd () {
  if (m_ignoring) return;
  if (m_debug) qDebug ("got array end");

  Q3ValueVector<AIElement> stackArray = m_arrayStack.pop();

  if (m_arrayStack.empty())
  {
    if (m_debug) qDebug ("put elements to stack");
    AIElement realElement (stackArray);

    if (m_debug) {
      qDebug ("going to stack");
      elementtoa (realElement);
      qDebug ("done");
    }
    m_stack.push (realElement);

    m_sink = DS_Other;
  }
  else
  {
    if (m_debug) qDebug ("put elements to nest stack level");
    Q3ValueVector<AIElement> currentTOS = m_arrayStack.top();
    currentTOS.push_back (stackArray);
  }
}

void AIParserBase::gotBlockEnd () {
  if (m_ignoring) return;
  if (m_debug) qDebug ("got block end");

  Q3ValueVector<AIElement> stackArray = m_blockStack.pop();

  if (m_blockStack.empty())
  {
    if (m_debug) qDebug ("put elements to stack");
    AIElement realElement (stackArray, AIElement::Block);

    if (m_debug) {
      qDebug ("going to stack");
      elementtoa (realElement);
      qDebug ("done");
    }
    m_stack.push (realElement);

    m_sink = DS_Other;
  }
  else
  {
    if (m_debug) qDebug ("put elements to nest stack level");
    Q3ValueVector<AIElement> currentTOS = m_blockStack.top();
    currentTOS.push_back (stackArray);
  }
}

/*Ai88*/ /* void AIParserBase::_handleSetDash()
{
  double fval = getDoubleValue();

  AIElement elem (m_stack.top());
  m_stack.pop();

  const QValueVector<AIElement> aval = elem.toElementArray();
  if (m_gstateHandler) m_gstateHandler->gotDash (aval, fval);
} */

/*Ai88*/ /* void AIParserBase::_handleSetFillColorCMYK()
{
  double k = getDoubleValue();
  double y = getDoubleValue();
  double m = getDoubleValue();
  double c = getDoubleValue();

  if (m_debug) qDebug ("values 1 are %f %f %f %f",c,m,y,k);
  AIColor color (c,m,y,k);

  if (m_gstateHandler) m_gstateHandler->gotFillColor (color);
} */

/*Ai88*/ /* void AIParserBase::_handleSetFillPattern()
{
  AIElement elem (m_stack.top());
  m_stack.pop();

  const QValueVector<AIElement> aval = elem.toElementArray();

  double ka = getDoubleValue();
  double k = getDoubleValue();
  double r = getDoubleValue();
  double rf = getDoubleValue();
  double angle = getDoubleValue();
  double sy = getDoubleValue();
  double sx = getDoubleValue();
  double py = getDoubleValue();
  double px = getDoubleValue();

  AIElement elem2 (m_stack.top());
  m_stack.pop();

  const QString &name = elem2.toString();
  if (m_gstateHandler) m_gstateHandler->gotFillPattern (name.latin1(), px, py, sx, sy, angle, rf, r, k, ka, aval);
} */

/*Ai88*/ /* void AIParserBase::_handleSetStrokePattern()
{
  AIElement elem (m_stack.top());
  m_stack.pop();

  const QValueVector<AIElement> aval = elem.toElementArray();

  double ka = getDoubleValue();
  double k = getDoubleValue();
  double r = getDoubleValue();
  double rf = getDoubleValue();
  double angle = getDoubleValue();
  double sy = getDoubleValue();
  double sx = getDoubleValue();
  double py = getDoubleValue();
  double px = getDoubleValue();

  AIElement elem2 (m_stack.top());
  m_stack.pop();

  const QString &name = elem2.toString();
  if (m_gstateHandler) m_gstateHandler->gotStrokePattern (name.latin1(), px, py, sx, sy, angle, rf, r, k, ka, aval);
} */

/*Ai88*/ /* void AIParserBase::_handleSetStrokeColorCMYK()
{
  double k = getDoubleValue();
  double y = getDoubleValue();
  double m = getDoubleValue();
  double c = getDoubleValue();
  if (m_debug) qDebug ("values 2 are %f %f %f %f",c,m,y,k);

  AIColor color (c,m,y,k);

  if (m_gstateHandler) m_gstateHandler->gotStrokeColor (color);
} */

/*Ai88*/ /* void AIParserBase::_handleSetFillColorGray()
{
  double g = getDoubleValue();
  if (m_debug) qDebug ("values 3 are %f",g);

  AIColor color (g);

  if (m_gstateHandler) m_gstateHandler->gotFillColor (color);
} */

/*Ai88*/ /* void AIParserBase::_handleSetStrokeColorGray()
{
  double g = getDoubleValue();
  if (m_debug) qDebug ("values 4 are %f",g);

  AIColor color (g);

  if (m_gstateHandler) m_gstateHandler->gotStrokeColor (color);
} */

/*Ai88*/ /* void AIParserBase::_handleSetFillColorCustom()
{
  double g = getDoubleValue();
  const QString &name = getStringValue();
  double k = getDoubleValue();
  double y = getDoubleValue();
  double m = getDoubleValue();
  double c = getDoubleValue();
  if (m_debug) qDebug ("values 5 are %f %f %f %f",c,m,y,k);

  AIColor color (c,m,y,k,name.latin1(),g);

  if (m_gstateHandler) m_gstateHandler->gotFillColor (color);
} */

void AIParserBase::_handlePSGet() {
  m_stack.pop();
  m_stack.pop();

  QString name ("xxx");
  AIElement ref (name,AIElement::Reference);
  m_stack.push (ref);
}

void AIParserBase::_handlePSExec() {
  m_stack.pop();
}

void AIParserBase::_handlePSString() {
  m_stack.pop();

  QString name ("stringval");
  AIElement ref (name,AIElement::Reference);
  m_stack.push (ref);
}

void AIParserBase::_handlePSBind() {
  m_stack.pop();

  QString name ("bindentry");
  AIElement ref (name,AIElement::Reference);
  m_stack.push (ref);
}

void AIParserBase::_handlePSUserdict() {
  QString name ("userdict");
  AIElement ref (name,AIElement::Reference);
  m_stack.push (ref);
}

void AIParserBase::_handlePSDict() {
  m_stack.pop();
  m_stack.pop();
  m_stack.pop();

  QString name ("dict");
  AIElement ref (name,AIElement::Reference);
  m_stack.push (ref);
}

void AIParserBase::_handlePSDup() {
  AIElement &tos = m_stack.top();

  AIElement copy (tos);
  m_stack.push (copy);
}

void AIParserBase::_handlePSBegin() {
  m_stack.pop();

  QString name ("dictionary begin");
  AIElement ref (name,AIElement::Reference);
  m_stack.push (ref);
}

void AIParserBase::_handlePSPut() {
  m_stack.pop();
  m_stack.pop();
}

/*Ai88*/ /* void AIParserBase::_handlePatternDefinition()
{
  AIElement elem (m_stack.top());
  m_stack.pop();

  const QValueVector<AIElement> aval = elem.toElementArray();

  double ury = getDoubleValue();
  double urx = getDoubleValue();
  double lly = getDoubleValue();
  double llx = getDoubleValue();

  AIElement elem2 (m_stack.top());
  m_stack.pop();

  const QString &name = elem2.toString();

  if (m_documentHandler) m_documentHandler->gotPatternDefinition (name.latin1(), aval, llx, lly, urx, ury);
} */

void AIParserBase::_handlePSDef() {
  // name ref
  m_stack.pop();

  // impl
  m_stack.pop();
}

/*Ai88*/ /* void AIParserBase::_handleSetStrokeColorCustom()
{
  double g = getDoubleValue();
  const QString &name = getStringValue();
  double k = getDoubleValue();
  double y = getDoubleValue();
  double m = getDoubleValue();
  double c = getDoubleValue();
  if (m_debug) qDebug ("values 6 are %f %f %f %f",c,m,y,k);

  AIColor color (c,m,y,k,name.latin1(),g);

  if (m_gstateHandler) m_gstateHandler->gotStrokeColor (color);
} */

void AIParserBase::_handleDocumentFonts(const char *) {
}

void AIParserBase::_handleDocumentFiles(const char *) {
}

void AIParserBase::_handleDocumentCustomColors(const char *) {
}

void AIParserBase::_handleDocumentNeededResources(const char *data) {
  if (!data) return;
  QStringList items = QStringList::split (' ', data);

  QString itemType = items[1];
  QString name = items[2];
  QString version = items[3];
  QString release = items[4];
}

void AIParserBase::_handleIncludeResource(const char *data) {
  if (!data) return;
  QStringList items = QStringList::split (' ', data);

  QString itemType = items[1];
  QString name = items[2];
  QString version = items[3];
  QString release = items[4];

  m_modules.push_back (name);
}

void AIParserBase::_handleDocumentProcessColors(const char *data) {
  if (!data) return;

  int colorSet = 0;
  QString tmp (data);

  signed int index;

  index = tmp.find ("Cyan");
  if (index > 0) colorSet |= PC_Cyan;

  index = tmp.find ("Magenta");
  if (index > 0) colorSet |= PC_Magenta;

  index = tmp.find ("Yellow");
  if (index > 0) colorSet |= PC_Yellow;

  index = tmp.find ("Black");
  if (index > 0) colorSet |= PC_Black;

  if (m_documentHandler) m_documentHandler->gotProcessColors (colorSet);
}

void AIParserBase::_handleCMYKCustomColor(const char *) {
}

/*Ai88*/ /* void AIParserBase::_handleGsaveIncludeDocument() {
  AIElement elem2 (m_stack.top());
  m_stack.pop();

  const QString &name = elem2.toString();

  int ury = getIntValue();
  int urx = getIntValue();
  int lly = getIntValue();
  int llx = getIntValue();

  AIElement elem (m_stack.top());
  m_stack.pop();

  const QValueVector<AIElement> aval = elem.toElementArray();

  if (m_embeddedHandler) m_embeddedHandler->gotGsaveIncludeDocument (aval, llx,lly,urx,ury,name.latin1());
} */

/*Ai88*/ /* void AIParserBase::_handleSetCurrentText() {
  int iAlign = getIntValue();
  TextAlign ta = TA_HLeft;

  switch (iAlign)
  {
    case 0 : ta = TA_HLeft; break;
    case 1 : ta = TA_HCenter; break;
    case 2 : ta = TA_HRight; break;
    case 3:  ta = TA_VTop; break;
    case 4 : ta = TA_VCenter; break;
    case 5 : ta = TA_VBottom; break;
  }

  double kerning = getDoubleValue();
  double leading = getDoubleValue();
  double size = getDoubleValue();

  AIElement elem2 (m_stack.top());
  m_stack.pop();

  const QString &fontname = elem2.toReference();

  if (m_textHandler) m_textHandler->gotFontDefinition (fontname.latin1(), size, leading, kerning, ta);
} */

/*Ai88*/ /* void AIParserBase::_handleTextBlock (TextOperation to) {
  AIElement elem (m_stack.top());
  qDebug ("to element is (%s)",elem.typeName());
  m_stack.pop();

  const QValueVector<AIElement> aval = elem.toElementArray();

  if (m_textHandler) m_textHandler->gotTextBlockBegin (aval, to);
} */

/*Ai88*/ /* void AIParserBase::_handleTextOutput () {
  AIElement elem (m_stack.top());
  m_stack.pop();

  const QString &text = elem.toString();

  int length = -1;

  if (m_stack.empty())
  {
    AIElement elem2 (m_stack.top());
    if (elem2.type() == AIElement::Int)
    {
      length = elem2.asInt();
      m_stack.pop();
    }
  }
  if (m_textHandler) m_textHandler->gotTextOutput (text.latin1(), length);
} */

void AIParserBase::_handleCreationDate (const char *data)
{
  if (!data) return;

  QRegExp test ("\\((.+)\\) \\((.+)\\)");
  if (test.search (data))
  {
    QString val1 = test.cap(1);
    QString val2 = test.cap(2);

   if (m_documentHandler) m_documentHandler->gotCreationDate (val1.latin1(),val2.latin1());
  }
}

void AIParserBase::gotToken (const char *value) {
  if (m_debug) qDebug ("got token");

  if (m_ignoring) return;
  if (m_debug) qDebug ("token: %s", value);

  if (m_sink == DS_Array)
  {
    if (m_debug) qDebug ("token in array");
    QString op (value);
    AIElement realElement (op, AIElement::Operator);
    handleElement (realElement);

    return;
  }
  if (m_sink == DS_Block)
  {
    if (m_debug) qDebug ("token in block");
    QString op (value);
    AIElement realElement (op, AIElement::Operator);
    handleElement (realElement);

    return;
  }

  if (m_debug) qDebug ("get ai operation");

  AIOperation op = getAIOperation (value);
//  PathElement pathElement;
//  double fval;
//  int ival;

   bool handled = false;

   handled = m_ai88Handler->handleAIOperation (op);
   if (!handled) handled = m_ai3Handler->handleAIOperation (op);

   if (!handled)
   {
      if (m_sink == DS_Other)
      {
        if (handlePS (value)) return;
      }
      qWarning ( "unknown operator: %s", value );

      QString string(value);

      if (m_modules.findIndex(string) != -1)
      {
        AIElement element (string, AIElement::Reference);
        handleElement (element);
        return;
      }

      if (m_debug) stacktoa (m_stack);
      qWarning ( "pushing %s to stack", value );
      AIElement element (string, AIElement::Operator);
      handleElement (element);
  }

  if (m_debug) qDebug ("/got token value");
}

bool AIParserBase::handlePS (const char *operand){
  if (m_ignoring) return false;

  PSOperation psop = getPSOperation (operand);

  switch (psop)
  {
    case PSO_Get :
      _handlePSGet ();
      return true;
    case PSO_Exec :
      _handlePSExec ();
      return true;
    case PSO_Def :
      _handlePSDef ();
      return true;
    case PSO_String :
      _handlePSString ();
      return true;
    case PSO_Bind :
      _handlePSBind ();
      return true;
    case PSO_Userdict :
      _handlePSUserdict ();
      return true;
    case PSO_Dict :
      _handlePSDict ();
      return true;
    case PSO_Dup :
      _handlePSDup ();
      return true;
    case PSO_Begin :
      _handlePSBegin ();
      return true;
    case PSO_Put :
      _handlePSPut ();
      return true;
    default: break;
  }
  return false;
}

const double AIParserBase::getDoubleValue(void) {
  const AIElement &elem = m_stack.pop();

  return elem.toDouble();
}

const int AIParserBase::getIntValue(void) {
  const AIElement &elem = m_stack.pop();

  return elem.toInt();
}

const bool AIParserBase::getBoolValue(void) {
  return getIntValue() == 1;
}

const QString AIParserBase::getStringValue(void) {
  const AIElement &elem = m_stack.pop();

  return elem.toString();
}

const QString AIParserBase::getOperatorValue(void) {
  const AIElement &elem = m_stack.pop();

  return elem.toOperator();
}

AIOperation AIParserBase::getAIOperation (const char *operand)
{
  int i=0;
  QString cmpValue (operand);

  for(;;) {    AIOperationMapping map = aiMappings[i];
    if (map.op == NULL) return AIO_Other;
    if (cmpValue.compare (map.op) == 0) return map.action;

    i++;
  }
}

PSOperation AIParserBase::getPSOperation (const char *operand)
{
  int i=0;
  QString cmpValue (operand);

  for(;;) {    PSOperationMapping map = psMappings[i];
    if (map.op == NULL) return PSO_Other;
    if (cmpValue.compare (map.op) == 0) return map.action;

    i++;
  }
}

CommentOperation AIParserBase::getCommentOperation (const char *command) {
  QString data (command);

  signed int index;

  int i=0;

  for(;;) {
    CommentOperationMapping map = commentMappings[i];
    if (map.op == NULL) return CO_Other;
    index = data.find (map.op);
    if (index >= 0) return map.action;
    i++;
  }
}

void GStateHandlerBase::gotFillPattern (const char *pname, double px, double py, double sx, double sy, double angle, double rf, double r, double k, double ka, const Q3ValueVector<AIElement>& transformData) {
  qDebug ( "got fill pattern %s %f %f %f %f %f %f %f %f %f", pname, px, py, sx, sy, angle, rf, r, k, ka);
  arraytoa (transformData);
  qDebug ("/got fill pattern");
}

void GStateHandlerBase::gotStrokePattern (const char *pname, double px, double py, double sx, double sy, double angle, double rf, double r, double k, double ka, const Q3ValueVector<AIElement>& transformData) {
  qDebug ( "got stroke pattern %s %f %f %f %f %f %f %f %f %f", pname, px, py, sx, sy, angle, rf, r, k, ka);
  arraytoa (transformData);
  qDebug ("/got stroke pattern");
}

const char *AIParserBase::getValue (const char *input) {
  QString data(input);

  signed int index = data.find (':');
  if (index < 0) return "";
  index++;
  while (data.at(index) == ' ') index++;
  return data.mid(index).latin1();
}

bool AIParserBase::getRectangle (const char* input, int &llx, int &lly, int &urx, int &ury) {
  if (input == NULL) return false;

  QString s(input);
  if (s.contains ("(atend)")) return false;
  QStringList values = QStringList::split (" ", input);
  if (values.size() < 5) return false;
  llx = values[1].toInt();
  lly = values[2].toInt();
  urx = values[3].toInt();
  ury = values[4].toInt();

  return true;
}

bool AIParserBase::getPoint (const char* input, int &x, int &y) {
  if (input == NULL) return false;

  QString s(input);
  QStringList values = QStringList::split (" ", input);

  if (values.size() < 3) return false;

  x = values[1].toInt();
  y = values[2].toInt();

  return true;
}

void AIParserBase::cleanupArrays()
{
  if (m_sink == DS_Array) qDebug ("unclosed array(s).");
  while (m_sink == DS_Array) gotArrayEnd ();
  stacktoa (m_stack);
}

/*Ai88*/ /* void AIParserBase::_handleFontEncoding()
{
  while (m_stack.top().type() != AIElement::Reference) {
    m_stack.pop();
  }

  AIElement elem (m_stack.top());
  m_stack.pop();
  const QString &oldFont = elem.toReference();

  AIElement elem2 (m_stack.top());
  m_stack.pop();
  const QString &newFont = elem2.toReference();

  AIElement elem3 (m_stack.top());
  m_stack.pop();
  const QValueVector<AIElement> encodingData = elem3.toElementArray();

  if (m_textHandler) m_textHandler->gotFontEncoding (encodingData, oldFont.latin1(), newFont.latin1());
} */

void TextHandlerBase::gotFontEncoding (const Q3ValueVector<AIElement>& encodingData, const char*oldFontName, const char*newFontName)
{
  qDebug ("font encoding %s to %s",oldFontName, newFontName);
  arraytoa (encodingData);
  qDebug ("/font encoding");
}

void TextHandlerBase::gotFontDefinition (const char*fontName, double size, double leading, double kerning, TextAlign align)
{
  qDebug ("font definition: name %s size %f leading %f kerning %f align %d", fontName, size, leading, kerning, align);
}

void TextHandlerBase::gotTextBlockBegin (const Q3ValueVector<AIElement>& transData, TextOperation mode)
{
  qDebug ("text block begin %d",mode);
  arraytoa (transData);
  qDebug ("/text block begin");
}

void TextHandlerBase::gotTextOutput (const char*text, int length)
{
  qDebug ("text output (%s) %d",text,length);
}

void TextHandlerBase::gotTextBlockEnd ()
{
  qDebug ("text block end");
}

const void elementtoa (const AIElement &/*data*/)
{
/*  AIElement::Type type = data.type();
  qDebug ("type: %s", AIElement::typeToName (type));

  switch (type)
  {
    case AIElement::String :
    case AIElement::CString :
    case AIElement::Int :
    case AIElement::UInt :
    case AIElement::Double :
      qDebug ("string value : %s",data.toString().latin1());
      break;
    case AIElement::Reference :
      qDebug ("string value : %s",data.toReference().latin1());
      break;
    case AIElement::Operator :
      qDebug ("string value : %s",data.toOperator().latin1());
      break;
    case AIElement::ElementArray :
      arraytoa (data.toElementArray());
      break;
    case AIElement::Block :
      arraytoa (data.toBlock());
      break;

    default :
      qDebug ("could not fetch data");
  } */
}

const void arraytoa (const Q3ValueVector<AIElement> &/*data*/)
{
/*  qDebug ("array size is %d ",data.size());
  if (data.size() > 0)
  {
    qDebug ("[[[[[[[[[[[[[[[[[[[[");
    for (uint i=0; i< data.size(); i++)
    {
      elementtoa (data[i]);
    }
    qDebug ("]]]]]]]]]]]]]]]]]]]]");
  } */
}

const void stacktoa (const Q3ValueStack<AIElement> &/*data*/)
{
/*  qDebug ("stack size is %d",data.size());
  if (data.size() > 0)
  {
    qDebug ("<<<<<<<<<<<<<<<<<<");
    for (uint i=0; i< data.size(); i++)
    {
      elementtoa (data[i]);
    }
  }
  qDebug (">>>>>>>>>>>>>>>>>>"); */
}

const void stacktoa2 (const Q3ValueStack<Q3ValueVector<AIElement> >&/*data*/)
{
/*  qDebug ("stack size is %d",data.size());

  if (data.size() > 0)
  {
    qDebug ("(((((((((((((((((((((((");
    for (uint i=0; i< data.size(); i++)
    {
      arraytoa (data[i]);
    }
    qDebug (")))))))))))))))))))))))");
  } */
}

const void aiotoa (AIOperation &data)
{
  switch (data)
  {
    case AIO_SetFillColorCMYK : qDebug ("AIO_SetFillColorCMYK"); break;
    case AIO_SetStrokeColorCMYK : qDebug ("AIO_SetStrokeColorCMYK"); break;
    case AIO_SetFillColorGray : qDebug ("AIO_SetFillColorGray"); break;
    case AIO_SetStrokeColorGray : qDebug ("AIO_SetStrokeColorGray"); break;
    case AIO_SetFillColorCustom : qDebug ("AIO_SetFillColorCustom"); break;
    case AIO_SetStrokeColorCustom : qDebug ("AIO_SetStrokeColorCustom"); break;
    case AIO_SetFillPattern : qDebug ("AIO_SetFillPattern"); break;
    case AIO_SetStrokePattern : qDebug ("AIO_SetStrokePattern"); break;
    case AIO_SetFillOverprinting : qDebug ("AIO_SetFillOverprinting"); break;
    case AIO_SetStrokeOverprinting : qDebug ("AIO_SetStrokeOverprinting"); break;
    case AIO_SetFlatness : qDebug ("AIO_SetFlatness"); break;
    case AIO_SetLineCap : qDebug ("AIO_SetLineCap"); break;
    case AIO_SetLineJoin : qDebug ("AIO_SetLineJoin"); break;
    case AIO_SetLineWidth : qDebug ("AIO_SetLineWidth"); break;
    case AIO_SetMiterLimit : qDebug ("AIO_SetMiterLimit"); break;
    case AIO_SetDash : qDebug ("AIO_SetDash"); break;
    case AIO_BeginGroupClip : qDebug ("AIO_BeginGroupClip"); break;
    case AIO_EndGroupClip : qDebug ("AIO_EndGroupClip"); break;
    case AIO_MoveTo : qDebug ("AIO_MoveTo"); break;
    case AIO_LineToCorner : qDebug ("AIO_LineToCorner"); break;
    case AIO_LineToSmooth : qDebug ("AIO_LineToSmooth"); break;
    case AIO_CurveToSmooth : qDebug ("AIO_CurveToSmooth"); break;
    case AIO_CurveToCorner : qDebug ("AIO_CurveToCorner"); break;
    case AIO_CurveToOmitC1Smooth : qDebug ("AIO_CurveToOmitC1Smooth"); break;
    case AIO_CurveToOmitC1Corner : qDebug ("AIO_CurveToOmitC1Corner"); break;
    case AIO_CurveToOmitC2Smooth : qDebug ("AIO_CurveToOmitC2Smooth"); break;
    case AIO_CurveToOmitC2Corner : qDebug ("AIO_CurveToOmitC2Corner"); break;
    case AIO_PathIgnoreNoReset : qDebug ("AIO_PathIgnoreNoReset"); break;
    case AIO_PathIgnoreNoResetClose : qDebug ("AIO_PathIgnoreNoResetClose"); break;
    case AIO_PathClipPath : qDebug ("AIO_PathClipPath"); break;
    case AIO_PathIgnoreReset : qDebug ("AIO_PathIgnoreReset"); break;
    case AIO_PathIgnoreResetClose : qDebug ("AIO_PathIgnoreResetClose"); break;
    case AIO_PathFillNonZero : qDebug ("AIO_PathFillNonZero"); break;
    case AIO_PathFillNonZeroClose : qDebug ("AIO_PathFillNonZeroClose"); break;
    case AIO_PathStroke : qDebug ("AIO_PathStroke"); break;
    case AIO_PathStrokeClose : qDebug ("AIO_PathStrokeClose"); break;
    case AIO_PathFillNoReset : qDebug ("AIO_PathFillNoReset"); break;
    case AIO_PathFillNoResetClose : qDebug ("AIO_PathFillNoResetClose"); break;
    case AIO_FontEncoding : qDebug ("AIO_FontEncoding"); break;
    case AIO_PatternDefinition : qDebug ("AIO_PatternDefinition"); break;
    case AIO_SetCurrentText : qDebug ("AIO_SetCurrentText"); break;
    case AIO_TextBlockFillStroke : qDebug ("AIO_TextBlockFillStroke"); break;
    case AIO_TextBlockFill : qDebug ("AIO_TextBlockFill"); break;
    case AIO_TextBlockAppend : qDebug ("AIO_TextBlockAppend"); break;
    case AIO_TextBlockIgnore : qDebug ("AIO_TextBlockIgnore"); break;
    case AIO_TextBlockStroke : qDebug ("AIO_TextBlockStroke"); break;
    case AIO_TextOutput : qDebug ("AIO_TextOutput"); break;
    case AIO_TextBlockEnd : qDebug ("AIO_TextBlockEnd"); break;
    case AIO_GsaveIncludeDocument : qDebug ("AIO_GsaveIncludeDocument"); break;
    case AIO_Grestore : qDebug ("AIO_Grestore"); break;
    case AIO_LockElement : qDebug ("AIO_LockElement"); break;
    case AIO_SetWindingOrder : qDebug ("AIO_SetWindingOrder"); break;
    default : qDebug ("unknown");
  }
}

const void sttoa (SectionType &data, bool begin)
{
  switch (data)
  {
    case ST_Setup : begin ? qDebug ("start setup") : qDebug ("end setup"); break;
    case ST_Prolog : begin ? qDebug ("start prolog") : qDebug ("end prolog"); break;
    case ST_ProcSet : begin ? qDebug ("start procset") : qDebug ("end procset"); break;
    case ST_Encoding : begin ? qDebug ("start encoding") : qDebug ("end encoding"); break;
    case ST_Pattern : begin ? qDebug ("start pattern") : qDebug ("end pattern"); break;
    case ST_Document : begin ? qDebug ("start document") : qDebug ("end document"); break;
    case ST_BrushPattern : begin ? qDebug ("start brush pattern") : qDebug ("end brush pattern"); break;
    case ST_Gradient : begin ? qDebug ("start gradient") : qDebug ("end gradient"); break;
    case ST_Palette : begin ? qDebug ("start palette") : qDebug ("end palette"); break;
    case ST_Resource : begin ? qDebug ("start resource") : qDebug ("end resouce"); break;

    default : begin ? qDebug ("unknown") : qDebug ("end unknown");
  }
}

