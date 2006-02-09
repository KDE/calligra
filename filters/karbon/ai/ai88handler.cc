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

#include <kglobal.h>
#include "ai88handler.h"

AI88Handler::AI88Handler(AIParserBase *delegate){
  m_delegate = delegate;
}
AI88Handler::~AI88Handler(){
}

bool AI88Handler::handleAIOperation (AIOperation op) {
  PathElement pathElement;
  double fval;
  int ival;

  switch (op) {
    case AIO_SetFillColorCMYK :
      _handleSetFillColorCMYK();
      return true;
      break;
    case AIO_SetStrokeColorCMYK :
      _handleSetStrokeColorCMYK();
      return true;
      break;
    case AIO_SetFillColorGray :
      _handleSetFillColorGray();
      return true;
      break;
    case AIO_SetStrokeColorGray :
      _handleSetStrokeColorGray();
      return true;
      break;
    case AIO_SetFillColorCustom :
      _handleSetFillColorCustom();
      return true;
      break;
    case AIO_SetStrokeColorCustom :
      _handleSetStrokeColorCustom();
      return true;
      break;
    case AIO_SetFillPattern :
      _handleSetFillPattern();
      return true;
      break;
    case AIO_SetStrokePattern :
      _handleSetStrokePattern();
      return true;
      break;
    case AIO_SetFillOverprinting :
      if (m_delegate->m_miscGStateHandler) m_delegate->m_miscGStateHandler->gotFillOverprinting (m_delegate->getBoolValue());
      return true;
      break;
    case AIO_SetStrokeOverprinting :
      if (m_delegate->m_miscGStateHandler) m_delegate->m_miscGStateHandler->gotStrokeOverprinting (m_delegate->getBoolValue());
      return true;
      break;
    case AIO_LockElement :
      if (m_delegate->m_miscGStateHandler) m_delegate->m_miscGStateHandler->gotLockNextObject (m_delegate->getBoolValue());
      return true;
      break;
    case AIO_SetFlatness :
      fval = m_delegate->getDoubleValue();
      if (m_delegate->m_gstateHandler) m_delegate->m_gstateHandler->gotFlatness (fval);
      return true;
      break;
    case AIO_SetLineCap :
      ival = m_delegate->getIntValue();
      if (m_delegate->m_gstateHandler) m_delegate->m_gstateHandler->gotLineCaps (ival);
      return true;
      break;
    case AIO_SetLineJoin :
      ival = m_delegate->getIntValue();
      if (m_delegate->m_gstateHandler) m_delegate->m_gstateHandler->gotLineJoin (ival);
      return true;
      break;
    case AIO_SetLineWidth :
      fval = kMax(0.2, m_delegate->getDoubleValue()); // Use thinnest pen stroke possible for 0 (Rob)
      if (m_delegate->m_gstateHandler) m_delegate->m_gstateHandler->gotLineWidth (fval);
      return true;
      break;
    case AIO_SetMiterLimit :
      fval = m_delegate->getDoubleValue();
      if (m_delegate->m_gstateHandler) m_delegate->m_gstateHandler->gotMiterLimit (fval);
      return true;
      break;
    case AIO_SetDash :
      _handleSetDash();
      return true;
      break;
    case AIO_BeginGroupClip :
      if (m_delegate->m_structureHandler) m_delegate->m_structureHandler->gotBeginGroup (true);
      return true;
      break;
    case AIO_EndGroupClip :
      if (m_delegate->m_debug) qDebug ("got end group clip");
      if (m_delegate->m_structureHandler) m_delegate->m_structureHandler->gotEndGroup (true);
      if (m_delegate->m_debug) qDebug ("/got end group clip");
      return true;

      break;
    case AIO_MoveTo :
      pathElement.petype = PET_MoveTo;
      pathElement.pttype = PT_Corner;
      pathElement.pevalue.pointdata.y = m_delegate->getDoubleValue();
      pathElement.pevalue.pointdata.x = m_delegate->getDoubleValue();
      if (m_delegate->m_pathHandler) m_delegate->m_pathHandler->gotPathElement (pathElement);
      return true;
      break;
    case AIO_LineToCorner :
      pathElement.petype = PET_LineTo;
      pathElement.pttype = PT_Corner;
      pathElement.pevalue.pointdata.y = m_delegate->getDoubleValue();
      pathElement.pevalue.pointdata.x = m_delegate->getDoubleValue();
      if (m_delegate->m_pathHandler) m_delegate->m_pathHandler->gotPathElement (pathElement);
      return true;
      break;
    case AIO_LineToSmooth :
      pathElement.petype = PET_LineTo;
      pathElement.pttype = PT_Smooth;
      pathElement.pevalue.pointdata.y = m_delegate->getDoubleValue();
      pathElement.pevalue.pointdata.x = m_delegate->getDoubleValue();
      if (m_delegate->m_pathHandler) m_delegate->m_pathHandler->gotPathElement (pathElement);
      return true;
      break;
    case AIO_CurveToCorner :
      pathElement.petype = PET_CurveTo;
      pathElement.pttype = PT_Corner;
      pathElement.pevalue.bezierdata.y3 = m_delegate->getDoubleValue();
      pathElement.pevalue.bezierdata.x3 = m_delegate->getDoubleValue();
      pathElement.pevalue.bezierdata.y2 = m_delegate->getDoubleValue();
      pathElement.pevalue.bezierdata.x2 = m_delegate->getDoubleValue();
      pathElement.pevalue.bezierdata.y1 = m_delegate->getDoubleValue();
      pathElement.pevalue.bezierdata.x1 = m_delegate->getDoubleValue();
      if (m_delegate->m_pathHandler) m_delegate->m_pathHandler->gotPathElement (pathElement);
      return true;
      break;
    case AIO_CurveToSmooth :
      pathElement.petype = PET_CurveTo;
      pathElement.pttype = PT_Smooth;
      pathElement.pevalue.bezierdata.y3 = m_delegate->getDoubleValue();
      pathElement.pevalue.bezierdata.x3 = m_delegate->getDoubleValue();
      pathElement.pevalue.bezierdata.y2 = m_delegate->getDoubleValue();
      pathElement.pevalue.bezierdata.x2 = m_delegate->getDoubleValue();
      pathElement.pevalue.bezierdata.y1 = m_delegate->getDoubleValue();
      pathElement.pevalue.bezierdata.x1 = m_delegate->getDoubleValue();
      if (m_delegate->m_pathHandler) m_delegate->m_pathHandler->gotPathElement (pathElement);
      return true;
      break;
    case AIO_CurveToOmitC1Corner :
      pathElement.petype = PET_CurveTo;
      pathElement.pttype = PT_Corner;
      pathElement.pevalue.bezierdata.y3 = m_delegate->getDoubleValue();
      pathElement.pevalue.bezierdata.x3 = m_delegate->getDoubleValue();
      pathElement.pevalue.bezierdata.y2 = m_delegate->getDoubleValue();
      pathElement.pevalue.bezierdata.x2 = m_delegate->getDoubleValue();
      if (m_delegate->m_pathHandler) m_delegate->m_pathHandler->gotPathElement (pathElement);
      return true;
      break;
    case AIO_CurveToOmitC1Smooth :
      pathElement.petype = PET_CurveTo;
      pathElement.pttype = PT_Smooth;
      pathElement.pevalue.bezierdata.y3 = m_delegate->getDoubleValue();
      pathElement.pevalue.bezierdata.x3 = m_delegate->getDoubleValue();
      pathElement.pevalue.bezierdata.y2 = m_delegate->getDoubleValue();
      pathElement.pevalue.bezierdata.x2 = m_delegate->getDoubleValue();
      if (m_delegate->m_pathHandler) m_delegate->m_pathHandler->gotPathElement (pathElement);
      return true;
      break;
    case AIO_CurveToOmitC2Corner :
      pathElement.petype = PET_CurveTo;
      pathElement.pttype = PT_Corner;
      pathElement.pevalue.bezierdata.y3 = m_delegate->getDoubleValue();
      pathElement.pevalue.bezierdata.x3 = m_delegate->getDoubleValue();
      pathElement.pevalue.bezierdata.y1 = m_delegate->getDoubleValue();
      pathElement.pevalue.bezierdata.x1 = m_delegate->getDoubleValue();
      if (m_delegate->m_pathHandler) m_delegate->m_pathHandler->gotPathElement (pathElement);
      return true;
      break;
    case AIO_CurveToOmitC2Smooth :
      pathElement.petype = PET_CurveTo;
      pathElement.pttype = PT_Smooth;
      pathElement.pevalue.bezierdata.y3 = m_delegate->getDoubleValue();
      pathElement.pevalue.bezierdata.x3 = m_delegate->getDoubleValue();
      pathElement.pevalue.bezierdata.y1 = m_delegate->getDoubleValue();
      pathElement.pevalue.bezierdata.x1 = m_delegate->getDoubleValue();
      if (m_delegate->m_pathHandler) m_delegate->m_pathHandler->gotPathElement (pathElement);
      return true;
      break;

    case AIO_PathIgnoreReset :
      if (m_delegate->m_pathHandler) m_delegate->m_pathHandler->gotIgnorePath(false, true);
      return true;
      break;
    case AIO_PathIgnoreResetClose :
      if (m_delegate->m_pathHandler) m_delegate->m_pathHandler->gotIgnorePath(true, true);
      return true;
      break;
    case AIO_PathIgnoreNoReset :
      if (m_delegate->m_pathHandler) m_delegate->m_pathHandler->gotIgnorePath(false, false);
      return true;
      break;
    case AIO_PathIgnoreNoResetClose :
      if (m_delegate->m_pathHandler) m_delegate->m_pathHandler->gotIgnorePath(true, false);
      return true;
      break;
    case AIO_PathClipPath :
      if (m_delegate->m_pathHandler) m_delegate->m_pathHandler->gotClipPath(false);
      return true;
      break;
    case AIO_PathFillNonZero :
      if (m_delegate->m_pathHandler)
      {
        m_delegate->m_pathHandler->gotFillMode (FM_NonZero);
        m_delegate->m_pathHandler->gotFillPath(false, true);
      }
      return true;
      break;
    case AIO_PathFillNonZeroClose :
      if (m_delegate->m_pathHandler) m_delegate->m_pathHandler->gotFillPath(true, true);
      return true;
      break;
    case AIO_PathFillNoReset :
      if (m_delegate->m_pathHandler) m_delegate->m_pathHandler->gotFillPath(false, false);
      return true;
      break;
    case AIO_PathFillNoResetClose :
      if (m_delegate->m_pathHandler) m_delegate->m_pathHandler->gotFillPath(true, false);
      return true;
      break;
    case AIO_PathStroke :
      if (m_delegate->m_pathHandler) m_delegate->m_pathHandler->gotStrokePath(false);
      return true;
      break;
    case AIO_PathStrokeClose :
      if (m_delegate->m_pathHandler) m_delegate->m_pathHandler->gotStrokePath(true);
      return true;
      break;
    case AIO_PatternDefinition :
      _handlePatternDefinition();
      return true;
      break;
    case AIO_GsaveIncludeDocument :
      _handleGsaveIncludeDocument();
      return true;
      break;
    case AIO_Grestore :
      if (m_delegate->m_embeddedHandler) m_delegate->m_embeddedHandler->gotGrestore();
      return true;
      break;
    case AIO_FontEncoding :
      _handleFontEncoding();
      return true;
      break;
    case AIO_SetCurrentText :
      _handleSetCurrentText();
      return true;
      break;
    case AIO_TextBlockFillStroke :
      _handleTextBlock (TO_FillStroke);
      return true;
      break;
    case AIO_TextBlockFill :
      _handleTextBlock (TO_Fill);
      return true;
      break;
    case AIO_TextBlockAppend :
      _handleTextBlock (TO_Append);
      return true;
      break;
    case AIO_TextBlockIgnore :
      _handleTextBlock (TO_Ignore);
      return true;
      break;
    case AIO_TextBlockStroke :
      _handleTextBlock (TO_Stroke);
      return true;
      break;
    case AIO_TextOutput :
      _handleTextOutput ();
      return true;
      break;
    case AIO_TextBlockEnd :
      if (m_delegate->m_textHandler) m_delegate->m_textHandler->gotTextBlockEnd();
      return true;
      break;
    default :
      return false;
  }
  return false;
}


void AI88Handler::_handleSetFillColorCMYK()
{
  double k = m_delegate->getDoubleValue();
  double y = m_delegate->getDoubleValue();
  double m = m_delegate->getDoubleValue();
  double c = m_delegate->getDoubleValue();

  if (m_delegate->m_debug) qDebug ("values 1 are %f %f %f %f",c,m,y,k);
  AIColor color (c,m,y,k);

  if (m_delegate->m_gstateHandler) m_delegate->m_gstateHandler->gotFillColor (color);
}

void AI88Handler::_handleSetFillPattern()
{
  AIElement elem (m_delegate->m_stack.top());
  m_delegate->m_stack.pop();

  const QValueVector<AIElement> aval = elem.toElementArray();

  double ka = m_delegate->getDoubleValue();
  double k = m_delegate->getDoubleValue();
  double r = m_delegate->getDoubleValue();
  double rf = m_delegate->getDoubleValue();
  double angle = m_delegate->getDoubleValue();
  double sy = m_delegate->getDoubleValue();
  double sx = m_delegate->getDoubleValue();
  double py = m_delegate->getDoubleValue();
  double px = m_delegate->getDoubleValue();

  AIElement elem2 (m_delegate->m_stack.top());
  m_delegate->m_stack.pop();

  const QString &name = elem2.toString();
  if (m_delegate->m_gstateHandler) m_delegate->m_gstateHandler->gotFillPattern (name.latin1(), px, py, sx, sy, angle, rf, r, k, ka, aval);
}

void AI88Handler::_handleSetStrokePattern()
{
  AIElement elem (m_delegate->m_stack.top());
  m_delegate->m_stack.pop();

  const QValueVector<AIElement> aval = elem.toElementArray();

  double ka = m_delegate->getDoubleValue();
  double k = m_delegate->getDoubleValue();
  double r = m_delegate->getDoubleValue();
  double rf = m_delegate->getDoubleValue();
  double angle = m_delegate->getDoubleValue();
  double sy = m_delegate->getDoubleValue();
  double sx = m_delegate->getDoubleValue();
  double py = m_delegate->getDoubleValue();
  double px = m_delegate->getDoubleValue();

  AIElement elem2 (m_delegate->m_stack.top());
  m_delegate->m_stack.pop();

  const QString &name = elem2.toString();
  if (m_delegate->m_gstateHandler) m_delegate->m_gstateHandler->gotStrokePattern (name.latin1(), px, py, sx, sy, angle, rf, r, k, ka, aval);
}


void AI88Handler::_handleSetStrokeColorCMYK()
{
  double k = m_delegate->getDoubleValue();
  double y = m_delegate->getDoubleValue();
  double m = m_delegate->getDoubleValue();
  double c = m_delegate->getDoubleValue();
  if (m_delegate->m_debug) qDebug ("values 2 are %f %f %f %f",c,m,y,k);

  AIColor color (c,m,y,k);

  if (m_delegate->m_gstateHandler) m_delegate->m_gstateHandler->gotStrokeColor (color);
}

void AI88Handler::_handleSetFillColorGray()
{
  double g = m_delegate->getDoubleValue();
  if (m_delegate->m_debug) qDebug ("values 3 are %f",g);

  AIColor color (g);

  if (m_delegate->m_gstateHandler) m_delegate->m_gstateHandler->gotFillColor (color);
}

void AI88Handler::_handleSetStrokeColorGray()
{
  double g = m_delegate->getDoubleValue();
  if (m_delegate->m_debug) qDebug ("values 4 are %f",g);

  AIColor color (g);

  if (m_delegate->m_gstateHandler) m_delegate->m_gstateHandler->gotStrokeColor (color);
}

void AI88Handler::_handleSetFillColorCustom()
{
  double g = m_delegate->getDoubleValue();
  const QString name = m_delegate->getStringValue();
  double k = m_delegate->getDoubleValue();
  double y = m_delegate->getDoubleValue();
  double m = m_delegate->getDoubleValue();
  double c = m_delegate->getDoubleValue();
  if (m_delegate->m_debug) qDebug ("values 5 are %f %f %f %f",c,m,y,k);

  AIColor color (c,m,y,k,name.latin1(),g);

  if (m_delegate->m_gstateHandler) m_delegate->m_gstateHandler->gotFillColor (color);
}

void AI88Handler::_handleSetDash()
{
//  qDebug ("found dash operation");
  double fval = m_delegate->getDoubleValue();

  AIElement elem (m_delegate->m_stack.top());
  m_delegate->m_stack.pop();

  const QValueVector<AIElement> aval = elem.toElementArray();
  if (m_delegate->m_gstateHandler) m_delegate->m_gstateHandler->gotDash (aval, fval);
//  qDebug ("dash operation finished");
}

void AI88Handler::_handlePatternDefinition()
{
  AIElement elem (m_delegate->m_stack.top());
  m_delegate->m_stack.pop();

  const QValueVector<AIElement> aval = elem.toElementArray();

  double ury = m_delegate->getDoubleValue();
  double urx = m_delegate->getDoubleValue();
  double lly = m_delegate->getDoubleValue();
  double llx = m_delegate->getDoubleValue();

  AIElement elem2 (m_delegate->m_stack.top());
  m_delegate->m_stack.pop();

  const QString &name = elem2.toString();

  if (m_delegate->m_documentHandler) m_delegate->m_documentHandler->gotPatternDefinition (name.latin1(), aval, llx, lly, urx, ury);
}

void AI88Handler::_handleGsaveIncludeDocument() {
  AIElement elem2 (m_delegate->m_stack.top());
  m_delegate->m_stack.pop();

  const QString &name = elem2.toString();

  int ury = m_delegate->getIntValue();
  int urx = m_delegate->getIntValue();
  int lly = m_delegate->getIntValue();
  int llx = m_delegate->getIntValue();

  AIElement elem (m_delegate->m_stack.top());
  m_delegate->m_stack.pop();

  const QValueVector<AIElement> aval = elem.toElementArray();

  if (m_delegate->m_embeddedHandler) m_delegate->m_embeddedHandler->gotGsaveIncludeDocument (aval, llx,lly,urx,ury,name.latin1());
}

void AI88Handler::_handleSetCurrentText() {
  int iAlign = m_delegate->getIntValue();
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

  double kerning = m_delegate->getDoubleValue();
  double leading = m_delegate->getDoubleValue();
  double size = m_delegate->getDoubleValue();

  AIElement elem2 (m_delegate->m_stack.top());
  m_delegate->m_stack.pop();

  const QString &fontname = elem2.toReference();

  if (m_delegate->m_textHandler) m_delegate->m_textHandler->gotFontDefinition (fontname.latin1(), size, leading, kerning, ta);

}

void AI88Handler::_handleTextBlock (TextOperation to) {
  AIElement elem (m_delegate->m_stack.top());
  qDebug ("to element is (%s)",elem.typeName());
  m_delegate->m_stack.pop();

  const QValueVector<AIElement> aval = elem.toElementArray();

  if (m_delegate->m_textHandler) m_delegate->m_textHandler->gotTextBlockBegin (aval, to);
}

void AI88Handler::_handleTextOutput () {
  AIElement elem (m_delegate->m_stack.top());
  m_delegate->m_stack.pop();

  const QString &text = elem.toString();

  int length = -1;

  if (m_delegate->m_stack.empty())
  {
    AIElement elem2 (m_delegate->m_stack.top());
    if (elem2.type() == AIElement::Int)
    {
      length = elem2.asInt();
      m_delegate->m_stack.pop();
    }
  }
  if (m_delegate->m_textHandler) m_delegate->m_textHandler->gotTextOutput (text.latin1(), length);
}

void AI88Handler::_handleFontEncoding()
{
  while (m_delegate->m_stack.top().type() != AIElement::Reference) {
    m_delegate->m_stack.pop();
  }

  AIElement elem (m_delegate->m_stack.top());
  m_delegate->m_stack.pop();
  const QString &oldFont = elem.toReference();

  AIElement elem2 (m_delegate->m_stack.top());
  m_delegate->m_stack.pop();
  const QString &newFont = elem2.toReference();

  AIElement elem3 (m_delegate->m_stack.top());
  m_delegate->m_stack.pop();
  const QValueVector<AIElement> encodingData = elem3.toElementArray();

  if (m_delegate->m_textHandler) m_delegate->m_textHandler->gotFontEncoding (encodingData, oldFont.latin1(), newFont.latin1());
}

void AI88Handler::_handleSetStrokeColorCustom()
{
  double g = m_delegate->getDoubleValue();
  const QString name = m_delegate->getStringValue();
  double k = m_delegate->getDoubleValue();
  double y = m_delegate->getDoubleValue();
  double m = m_delegate->getDoubleValue();
  double c = m_delegate->getDoubleValue();
  if (m_delegate->m_debug) qDebug ("values 6 are %f %f %f %f",c,m,y,k);

  AIColor color (c,m,y,k,name.latin1(),g);

  if (m_delegate->m_gstateHandler) m_delegate->m_gstateHandler->gotStrokeColor (color);
}
