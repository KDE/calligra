/***************************************************************************
                          aiparserbase.cpp  -  description
                             -------------------
    begin                : Thu Jul 19 2001
    copyright            : (C) 2001 by Dirk Schönberger
    email                : schoenberger@signsoft.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "aiparserbase.h"

#include <qregexp.h>
#include <qstringlist.h>

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

  { NULL, AIO_Other }
};

static PSOperationMapping psMappings[] = {
  { "get", PSO_Get },
  { "exec", PSO_Exec },
  { NULL, PSO_Other },
};

static CommentOperationMapping commentMappings[] = {
  { "BeginProlog", CO_BeginProlog },
  { "BeginSetup", CO_BeginSetup },
  { "BeginProcSet", CO_BeginProcSet },
  { "BeginEncoding", CO_BeginEncoding },
  { "BeginPattern", CO_BeginPattern },
  { "Trailer", CO_Trailer },
  { "EndProlog", CO_EndProlog },
  { "EndSetup", CO_EndSetup },
  { "EndProcSet", CO_EndProcSet },
  { "EndEncoding", CO_EndEncoding },
  { "EndPattern", CO_EndPattern },

  { "Title", CO_Title },
  { "Creator", CO_Creator },

  { "EOF", CO_Ignore },
  { "Note", CO_Ignore },
  { "EndDocument", CO_Ignore },
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
  { "+",  CO_Continuation },

  { "Template",  CO_Template },
  { "PageOrigin",  CO_PageOrigin },
  { "PrinterName",  CO_PrinterName },
  { "PrinterRect",  CO_PrinterRect },
  { "Note",  CO_Note },

  { NULL, CO_Other }
};

AIParserBase::AIParserBase() : m_ignoring(false), m_sink (DS_Other), m_continuationMode(CM_None) {
}
AIParserBase::~AIParserBase(){
}
bool AIParserBase::parse (QIODevice& fin){
  return AILexer::parse (fin);
}

void AIParserBase::gotComment (const char *value) {
  int llx, lly, urx, ury;

  CommentOperation cop = getCommentOperation (value);
  switch (cop) {
    case CO_BeginSetup :
    case CO_EndSetup :
      break;
    case CO_BeginProlog :
    case CO_Trailer :
      m_ignoring = true;
      break;
    case CO_EndProlog :
      m_ignoring = false;
      break;
    case CO_Ignore :
      break;
    case CO_BoundingBox :
      if (getRectangle (value, llx, lly, urx, ury))
        gotBoundingBox (llx, lly, urx, ury);
      break;
    case CO_TemplateBox :
      if (getRectangle (value, llx, lly, urx, ury))
        gotTemplateBox (llx, lly, urx, ury);
      break;
    case CO_Margin :
      if (getRectangle (value, llx, lly, urx, ury))
        gotMargin (llx, lly, urx, ury);
      break;
    case CO_Title :
      gotTitle (getValue (value));
      break;
    case CO_Creator :
      gotCreator (getValue (value));
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
    case CO_Continuation :
      switch (m_continuationMode) {
        case CM_DocumentFonts : _handleDocumentFonts (value); break;
        case CM_DocumentFiles : _handleDocumentFiles (value); break;
        case CM_DocumentCustomColors : _handleDocumentCustomColors (value); break;
        case CM_CMYKCustomColor :  _handleCMYKCustomColor (value); break;
        default : qWarning ("unknown continuation mode %d",m_continuationMode);
      }
      break;

    default :
      qWarning( "unhandled comment: %s", value );
  }
}


void AIParserBase::handleElement (AIElement &element)
{
  if (m_sink == DS_Array)
  {
    QValueVector<AIElement> &elementArray = m_arrayStack.top();
    elementArray.push_back(element);
  }
  else
    m_stack.push (element);
}

void AIParserBase::gotIntValue (int value) {
  if (m_ignoring) return;
  AIElement element (value);
  handleElement (element);
}

void AIParserBase::gotDoubleValue (double value) {
  if (m_ignoring) return;
  AIElement element (value);
  handleElement (element);
}

void AIParserBase::gotStringValue (const char *value) {
  if (m_ignoring) return;
  if (value == NULL) value = "";
  AIElement element (value);
  handleElement (element);
}

void AIParserBase::gotReference (const char *value) {
  if (m_ignoring) return;
  if (value == NULL) value = "";
  QString string(value);
  AIElement element (string, AIElement::Reference);
  handleElement (element);
}

void AIParserBase::gotArrayStart () {
  if (m_ignoring) return;

  QValueVector<AIElement> array;
  m_arrayStack.push (array);

  m_sink = DS_Array;
}

void AIParserBase::gotArrayEnd () {
  QValueVector<AIElement> stackArray = m_arrayStack.pop();

  if (m_arrayStack.empty())
  {
    AIElement realElement (stackArray);
    m_stack.push (realElement);

    m_sink = DS_Other;
  }
  else
  {
    QValueVector<AIElement> currentTOS = m_arrayStack.top();
    currentTOS.push_back (stackArray);
  }
}

void AIParserBase::_handleSetDash()
{
//  qDebug ("found dash operation");
  double fval = getDoubleValue();

  AIElement elem (m_stack.top());
  m_stack.pop();

  const QValueVector<AIElement> aval = elem.toVector();
  gotDash (aval, fval);
//  qDebug ("dash operation finished");
}

void AIParserBase::_handleSetFillColorCMYK()
{
  double k = getDoubleValue();
  double y = getDoubleValue();
  double m = getDoubleValue();
  double c = getDoubleValue();

  AIColor color (c,m,y,k);

  gotFillColor (color);
}

void AIParserBase::_handleSetStrokeColorCMYK()
{
  double k = getDoubleValue();
  double y = getDoubleValue();
  double m = getDoubleValue();
  double c = getDoubleValue();

  AIColor color (c,m,y,k);

  gotStrokeColor (color);
}

void AIParserBase::_handleSetFillColorGray()
{
  double g = getDoubleValue();

  AIColor color (g);

  gotFillColor (color);
}

void AIParserBase::_handleSetStrokeColorGray()
{
  double g = getDoubleValue();

  AIColor color (g);

  gotStrokeColor (color);
}

void AIParserBase::_handleSetFillColorCustom()
{
  double g = getDoubleValue();
  const QString &name = getStringValue();
  double k = getDoubleValue();
  double y = getDoubleValue();
  double m = getDoubleValue();
  double c = getDoubleValue();

  AIColor color (c,m,y,k,name,g);

  gotFillColor (color);
}

void AIParserBase::_handlePSGet() {
//  qDebug ("handle PS get");
  m_stack.pop();
  m_stack.pop();

  QString name ("xxx");
  AIElement ref (name,AIElement::Reference);
  m_stack.push (ref);
}
void AIParserBase::_handlePSExec() {
//  qDebug ("handle PS exec");
  m_stack.pop();
}


void AIParserBase::_handleSetStrokeColorCustom()
{
  double g = getDoubleValue();
  const QString &name = getStringValue();
  double k = getDoubleValue();
  double y = getDoubleValue();
  double m = getDoubleValue();
  double c = getDoubleValue();

  AIColor color (c,m,y,k,name,g);

  gotStrokeColor (color);
}

void AIParserBase::_handleDocumentFonts(const char *data) {
// qDebug ("in handle document fonts");
}

void AIParserBase::_handleDocumentFiles(const char *data) {
// qDebug ("in handle document files");
}

void AIParserBase::_handleDocumentCustomColors(const char *data) {
// qDebug ("in handle document colors");
}

void AIParserBase::_handleCMYKCustomColor(const char *data) {
// qDebug ("in handle cmyk custom color");
}

void AIParserBase::gotToken (const char *value) {
  if (m_ignoring) return;

  if (m_sink == DS_Array)
  {
    QString op (value);
    AIElement realElement (op, AIElement::Operator);
    m_stack.push (realElement);

    return;
  }

  AIOperation op = getAIOperation (value);
  PathElement pathElement;
  double fval;
  int ival;

  switch (op) {
    case AIO_SetFillColorCMYK :
      _handleSetFillColorCMYK();
      break;
    case AIO_SetStrokeColorCMYK :
      _handleSetStrokeColorCMYK();
      break;
    case AIO_SetFillColorGray :
      _handleSetFillColorGray();
      break;
    case AIO_SetStrokeColorGray :
      _handleSetStrokeColorGray();
      break;
    case AIO_SetFillColorCustom :
      _handleSetFillColorCustom();
      break;
    case AIO_SetStrokeColorCustom :
      _handleSetStrokeColorCustom();
      break;
    case AIO_SetFillOverprinting :
      gotFillOverprinting (getBoolValue());
      break;
    case AIO_SetStrokeOverprinting :
      gotStrokeOverprinting (getBoolValue());
      break;
    case AIO_LockElement :
      gotLockNextObject (getBoolValue());
      break;
    case AIO_SetFlatness :
      fval = getDoubleValue();
      gotFlatness (fval);
      break;
    case AIO_SetLineCap :
      ival = getIntValue();
      gotLineCaps (ival);
      break;
    case AIO_SetLineJoin :
      ival = getIntValue();
      gotLineJoin (ival);
      break;
    case AIO_SetLineWidth :
      fval = getDoubleValue();
      gotLineWidth (fval);
      break;
    case AIO_SetMiterLimit :
      fval = getDoubleValue();
      gotMiterLimit (fval);
      break;
    case AIO_SetWindingOrder :
      ival = getIntValue();
      gotWindingOrder (ival);
      break;
    case AIO_SetDash :
      _handleSetDash();
      break;
    case AIO_BeginGroupClip :
      gotBeginGroup (true);
      break;
    case AIO_EndGroupClip :
      gotEndGroup (true);
      break;
    case AIO_MoveTo :
      pathElement.petype = PET_MoveTo;
      pathElement.pttype = PT_Corner;
      pathElement.pevalue.pointdata.y = getDoubleValue();
      pathElement.pevalue.pointdata.x = getDoubleValue();
      gotPathElement (pathElement);
      break;
    case AIO_LineToCorner :
      pathElement.petype = PET_LineTo;
      pathElement.pttype = PT_Corner;
      pathElement.pevalue.pointdata.y = getDoubleValue();
      pathElement.pevalue.pointdata.x = getDoubleValue();
      gotPathElement (pathElement);
      break;
    case AIO_LineToSmooth :
      pathElement.petype = PET_LineTo;
      pathElement.pttype = PT_Smooth;
      pathElement.pevalue.pointdata.y = getDoubleValue();
      pathElement.pevalue.pointdata.x = getDoubleValue();
      gotPathElement (pathElement);
      break;
    case AIO_CurveToCorner :
      pathElement.petype = PET_CurveTo;
      pathElement.pttype = PT_Corner;
      pathElement.pevalue.bezierdata.y3 = getDoubleValue();
      pathElement.pevalue.bezierdata.x3 = getDoubleValue();
      pathElement.pevalue.bezierdata.y2 = getDoubleValue();
      pathElement.pevalue.bezierdata.x2 = getDoubleValue();
      pathElement.pevalue.bezierdata.y1 = getDoubleValue();
      pathElement.pevalue.bezierdata.x1 = getDoubleValue();
      gotPathElement (pathElement);
      break;
    case AIO_CurveToSmooth :
      pathElement.petype = PET_CurveTo;
      pathElement.pttype = PT_Smooth;
      pathElement.pevalue.bezierdata.y3 = getDoubleValue();
      pathElement.pevalue.bezierdata.x3 = getDoubleValue();
      pathElement.pevalue.bezierdata.y2 = getDoubleValue();
      pathElement.pevalue.bezierdata.x2 = getDoubleValue();
      pathElement.pevalue.bezierdata.y1 = getDoubleValue();
      pathElement.pevalue.bezierdata.x1 = getDoubleValue();
      gotPathElement (pathElement);
      break;
    case AIO_CurveToOmitC1Corner :
      pathElement.petype = PET_CurveTo;
      pathElement.pttype = PT_Corner;
      pathElement.pevalue.bezierdata.y3 = getDoubleValue();
      pathElement.pevalue.bezierdata.x3 = getDoubleValue();
      pathElement.pevalue.bezierdata.y2 = getDoubleValue();
      pathElement.pevalue.bezierdata.x2 = getDoubleValue();
      gotPathElement (pathElement);
      break;
    case AIO_CurveToOmitC1Smooth :
      pathElement.petype = PET_CurveTo;
      pathElement.pttype = PT_Smooth;
      pathElement.pevalue.bezierdata.y3 = getDoubleValue();
      pathElement.pevalue.bezierdata.x3 = getDoubleValue();
      pathElement.pevalue.bezierdata.y2 = getDoubleValue();
      pathElement.pevalue.bezierdata.x2 = getDoubleValue();
      gotPathElement (pathElement);
      break;
    case AIO_CurveToOmitC2Corner :
      pathElement.petype = PET_CurveTo;
      pathElement.pttype = PT_Corner;
      pathElement.pevalue.bezierdata.y3 = getDoubleValue();
      pathElement.pevalue.bezierdata.x3 = getDoubleValue();
      pathElement.pevalue.bezierdata.y1 = getDoubleValue();
      pathElement.pevalue.bezierdata.x1 = getDoubleValue();
      gotPathElement (pathElement);
      break;
    case AIO_CurveToOmitC2Smooth :
      pathElement.petype = PET_CurveTo;
      pathElement.pttype = PT_Smooth;
      pathElement.pevalue.bezierdata.y3 = getDoubleValue();
      pathElement.pevalue.bezierdata.x3 = getDoubleValue();
      pathElement.pevalue.bezierdata.y1 = getDoubleValue();
      pathElement.pevalue.bezierdata.x1 = getDoubleValue();
      gotPathElement (pathElement);
      break;

    case AIO_PathIgnoreReset :
      gotIgnorePath(false, true);
      break;
    case AIO_PathIgnoreResetClose :
      gotIgnorePath(true, true);
      break;
    case AIO_PathIgnoreNoReset :
      gotIgnorePath(false, false);
      break;
    case AIO_PathIgnoreNoResetClose :
      gotIgnorePath(true, false);
      break;
    case AIO_PathClipPath :
      gotClipPath(false);
      break;
    case AIO_PathFillNonZero :
      gotFillPath(false, true, FM_NonZero);
      break;
    case AIO_PathFillNonZeroClose :
      gotFillPath(true, true);
      break;
    case AIO_PathFillNoReset :
      gotFillPath(false, false);
      break;
    case AIO_PathFillNoResetClose :
      gotFillPath(true, false);
      break;
    case AIO_PathStroke :
      gotStrokePath(false);
      break;
    case AIO_PathStrokeClose :
      gotStrokePath(true);
      break;

    default :
//      qWarning ( "unknown operator: %s", value );
      if (m_sink == DS_Other)
      {
        if (handlePS (value)) return;
      }
      qWarning ( "pushing %s to stack", value );
      QString string(value);
      AIElement element (string, AIElement::Operator);
      handleElement (element);
  }
}

bool AIParserBase::handlePS (const char *operand){
  PSOperation psop = getPSOperation (operand);

  switch (psop)
  {
    case PSO_Get :
      _handlePSGet ();
      return true;
    case PSO_Exec :
      _handlePSExec ();
      return true;
  }
  return false;
}

void AIParserBase::gotBlockStart () {
  if (m_ignoring) return;
  AILexer::gotBlockStart();
}

void AIParserBase::gotBlockEnd () {
  if (m_ignoring) return;
  AILexer::gotBlockEnd();
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


const QString& AIParserBase::getStringValue(void) {
  const AIElement &elem = m_stack.pop();

  const QString &string = elem.toString();
  return string;
}

const QString& AIParserBase::getOperatorValue(void) {
  const AIElement &elem = m_stack.pop();

  const QString &string = elem.toOperator();
  return string;
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



void AIParserBase::gotFillColor (AIColor &color) {
/*  double r, g, b;
  Color.toRGB (color, r, g, b);
  qDebug ( "got fill color: %f %f %f ", r, g, b ); */
}

void AIParserBase::gotStrokeColor (AIColor &color) {
/*  double r, g, b;
  Color.toRGB (color, r, g, b);
  qDebug ( "got stroke color: %f %f %f ", r, g, b );  */
}

void AIParserBase::gotFlatness (double val) {
//  qDebug ( "got flatness: %f ", val );
}

void AIParserBase::gotLineWidth (double val) {
//  qDebug ( "got line width: %f ", val );
}

void AIParserBase::gotMiterLimit (double val) {
//  qDebug ( "got miter limit: %f ", val );
}

void AIParserBase::gotLineCaps (int val) {
//  qDebug ( "got line caps: %d ", val );
}

void AIParserBase::gotLineJoin (int val) {
//  qDebug ( "got line join: %d ", val );
}

void AIParserBase::gotBeginGroup (bool clipping) {
//  qDebug ( "got begin group: %d ", clipping );
}

void AIParserBase::gotEndGroup (bool clipping) {
//  qDebug ( "got end group: %d ", clipping );
}

void AIParserBase::gotPathElement (PathElement &element) {
/*  float x1, y1, x2, y2, x3, y3;
  char *pttype = NULL;
  switch (element.pttype)
  {
    case PT_Smooth :
      pttype =" (S)";
      break;
    case PT_Corner :
      pttype = " (C)";
      break;

  }

  switch (element.petype) {
    case PET_MoveTo :
      x1 = element.pevalue.pointdata.x;
      y1 = element.pevalue.pointdata.y;
      qDebug ( "got moveto %f %f %s", x1, y1, pttype );
      break;
    case PET_LineTo :
      x1 = element.pevalue.pointdata.x;
      y1 = element.pevalue.pointdata.y;
      qDebug ( "got lineto %f %f %s", x1, y1, pttype );
      break;
    case PET_CurveTo :
      x1 = element.pevalue.bezierdata.x1;
      y1 = element.pevalue.bezierdata.y1;
      x2 = element.pevalue.bezierdata.x2;
      y2 = element.pevalue.bezierdata.y2;
      x3 = element.pevalue.bezierdata.x3;
      y3 = element.pevalue.bezierdata.y3;
      qDebug ( "got curveto %f %f %f %f %f %f %s", x1, y1, x2, y2, x3, y3, pttype );
      break;
  } */

}

void AIParserBase::gotFillPath (bool closed, bool reset, FillMode fm) {
//  qDebug ( "got fill path closed %d reset %d fillmode %d", closed, reset, fm);
}

void AIParserBase::gotStrokePath (bool closed) {
//  qDebug ( "got stroke path closed %d", closed );
}

void AIParserBase::gotClipPath (bool closed) {
//  qDebug ( "got clip path closed %d", closed );
}

void AIParserBase::gotIgnorePath (bool closed, bool reset)
{
//  qDebug ( "got ignore path closed %d reset %d", closed, reset );
}

void AIParserBase::gotWindingOrder (int val) {
//  qDebug ( "got winding order: %d", val );
}

void AIParserBase::gotDash (const QValueVector<AIElement>& dashData, double phase) {
//  qDebug ("got dash: <data> %f", phase );
//  arraytoa (dashData);
//  qDebug ("/got dash" );
}

void AIParserBase::gotBoundingBox (int llx, int lly, int urx, int ury) {
//  qDebug ("got binding box: %d %d %d %d", llx, lly, urx, ury );
}

void AIParserBase::gotTemplateBox (int llx, int lly, int urx, int ury) {
//  qDebug ("got template box: %d %d %d %d", llx, lly, urx, ury );
}

void AIParserBase::gotMargin (int llx, int lly, int urx, int ury) {
//  qDebug ("got margin box: %d %d %d %d", llx, lly, urx, ury );
}

void AIParserBase::gotTitle (const char *data) {
/*  if (data != NULL)
    qDebug ("got title: %s", data ); */
}

void AIParserBase::gotCreator (const char *data) {
/*  if (data != NULL)
    qDebug ("got creator: %s", data ); */
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

  x = values[1].toInt();
  y = values[2].toInt();

  return true;
}


void AIParserBase::gotFillOverprinting (bool value){
//  qDebug ("got fill overprinting %d", value);
}

void AIParserBase::gotStrokeOverprinting (bool value){
//  qDebug ("got stroke overprinting %d", value);
}

void AIParserBase::gotLockNextObject (bool value){
//  qDebug ("got lock next object %d", value);
}

void AIParserBase::gotPrinterRect (int llx, int lly, int urx, int ury){
//  qDebug ("got printer rect %d %d %d %d", llx, lly, urx, ury);
}

void AIParserBase::gotPrinterName (const char *data){
//  qDebug ("got printer name %s", data);
}

void AIParserBase::gotPageOrigin (int x, int y){
//  qDebug ("got page origin %d %d", x, y);
}

void AIParserBase::gotTemplate (const char *data){
//  qDebug ("got template %s", data);
}

const void elementtoa (const AIElement &data)
{
  AIElement::Type type = data.type();
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
    case AIElement::Vector :
      arraytoa (data.toVector());
      break;

    default :
      qDebug ("could not fetch data");
  }
}

const void arraytoa (const QValueVector<AIElement> &data)
{
  qDebug ("array size is %d ",data.size());
  if (data.size() > 0)
  {
    qDebug ("[[[[[[[[[[[[[[[[[[[[");
    for (uint i=0; i< data.size(); i++)
    {
      elementtoa (data[i]);
    }
    qDebug ("]]]]]]]]]]]]]]]]]]]]");
  }
}

const void stacktoa (const QValueStack<AIElement> &data)
{
  qDebug ("stack size is %d",data.size());
  if (data.size() > 0)
  {
    qDebug ("<<<<<<<<<<<<<<<<<<");
    for (uint i=0; i< data.size(); i++)
    {
      elementtoa (data[i]);
    }
  }
  qDebug (">>>>>>>>>>>>>>>>>>");
}

const void stacktoa2 (const QValueStack<QValueVector<AIElement> >&data)
{
  qDebug ("stack size is %d",data.size());

  if (data.size() > 0)
  {
    qDebug ("(((((((((((((((((((((((");
    for (uint i=0; i< data.size(); i++)
    {
      arraytoa (data[i]);
    }
    qDebug (")))))))))))))))))))))))");
  }
}

