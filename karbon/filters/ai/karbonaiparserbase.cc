/***************************************************************************
                          karbonaiparserbase.cpp  -  description
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

#include "karbonaiparserbase.h"

#define MERGE_LINESEGMENTS
#define eps 0.00000001

// generic
KarbonAIParserBase::KarbonAIParserBase() : m_pot(POT_Other), m_strokeColor(), m_fillColor() {
  m_curPath.setAutoDelete( TRUE );

  // A4, 70 dpi
  m_bbox.llx = 0;
  m_bbox.lly = 0;
  m_bbox.urx = 612;
  m_bbox.ury = 792;

  m_lineWidth = 0;
  m_flatness = 0;
  m_lineCaps = 0;
  m_lineJoin = 0;
  m_miterLimit = 10;
  m_windingOrder = 0;

  m_fm = FM_NonZero;
}

// generic
KarbonAIParserBase::~KarbonAIParserBase(){
}

// generic
void KarbonAIParserBase::parsingStarted(){
  qDebug ( getHeader().latin1() );
}

// generic
void KarbonAIParserBase::parsingFinished(){
  qDebug ( getFooter().latin1() );
}

// specific
QString KarbonAIParserBase::getHeader(){
  QString data;
  data += "<!DOCTYPE DOC>\n";
  data += "<DOC mime=\"application/x-karbon\" version=\"0.1\" editor=\"aiimport 0.0.1\">\n";
  data += " <LAYER name=\"\" visible=\"1\">\n";
  return data;
}

// specific
QString KarbonAIParserBase::getFooter(){
  QString data;
  data += " </LAYER>\n";
  data += "</DOC>\n";
  return data;
}

// generic
QString KarbonAIParserBase::getParamList(Parameters& params){
  QString data("");

  Parameter *param;

  if (params.count() > 0)
  {

    for ( param=params.first(); param != 0; param=params.next() ) {
      data += " " + param->first + "=\"" + param->second + "\"";
    }
  }

  return data;
}

// generic
void KarbonAIParserBase::gotStartTag (const char *tagName, Parameters& params){
  qDebug ("<%s%s>", tagName, getParamList (params).latin1() );
}

// generic
void KarbonAIParserBase::gotEndTag (const char *tagName){
  qDebug ("</%s>", tagName );
}

// generic
void KarbonAIParserBase::gotSimpleTag (const char *tagName, Parameters& params){
  qDebug ("<%s%s/>", tagName, getParamList (params).latin1() );
}

// generic
void KarbonAIParserBase::gotPathElement (PathElement &element){
  PathElement *elem = new PathElement();
  memcpy (elem, &element, sizeof(PathElement));

  m_curPath.append (elem);
}

// generic
void KarbonAIParserBase::gotFillPath (bool closed, bool reset, FillMode fm){
  qDebug ("found fill path");
  if (!reset) qDebug ("retain filled path");

  if (!reset)
    m_pot = POT_Filled;
  else
  {
    doOutputCurrentPath (POT_Filled, closed);
    m_pot = POT_Other;
  }
  if (reset) m_curPath.clear();
}

// generic
void KarbonAIParserBase::gotIgnorePath (bool closed, bool reset){
  qDebug ("found ignore path");

  if (! reset)
    m_pot = POT_Other;
  else
  {
    doOutputCurrentPath (POT_Ignore, closed);
    m_pot = POT_Other;
  }
  if (reset) m_curPath.clear();
}


// generic
void KarbonAIParserBase::gotStrokePath (bool closed) {
  qDebug ("found stroke path");

  PathOutputType pot = POT_Stroked;
  if (m_pot != POT_Other)
  {
    pot = POT_FilledStroked;
  }

  doOutputCurrentPath (pot, closed);
  m_curPath.clear();
  m_pot = POT_Other;
}

// specific
void KarbonAIParserBase::fillColorAttributes (Parameters &parameters, AIColor &color){
  double c, m, y, k;
  color.toCMYK (c,m,y,k);

  parameters.append (new Parameter ("colorSpace", "1")); // 1 = CMYK?
  parameters.append (new Parameter ("v1", QString::number(c)));
  parameters.append (new Parameter ("v2", QString::number(m)));
  parameters.append (new Parameter ("v3", QString::number(y)));
  parameters.append (new Parameter ("v4", QString::number(k)));
}


// specific
void KarbonAIParserBase::doOutputCurrentPath(PathOutputType type, bool closed){
  if ((type != POT_Filled) && (type != POT_Stroked) && (type != POT_FilledStroked)) return;

  Parameters params;
  params.setAutoDelete( TRUE );

  params.append (new Parameter ("fillrule", QString::number(m_windingOrder)));
  gotStartTag ("PATH", params);

  if ((type == POT_FilledStroked) || (type == POT_Stroked))
  {
    params.clear();
    params.append (new Parameter ("lineWidth", QString::number(m_lineWidth)));
    params.append (new Parameter ("lineJoin", QString::number(m_lineJoin)));
    params.append (new Parameter ("lineCap", QString::number(m_lineCaps)));
    params.append (new Parameter ("miterLimit", QString::number(m_miterLimit)));
    gotStartTag ("STROKE", params);

    params.clear();
    fillColorAttributes (params, m_strokeColor);
    gotSimpleTag ("COLOR", params);

    gotEndTag ("STROKE");
  }

  if ((type == POT_FilledStroked) || (type == POT_Filled))
  {
    params.clear();
    gotStartTag ("FILL", params);

    params.clear();
    fillColorAttributes (params, m_fillColor);
    gotSimpleTag ("COLOR", params);

    gotEndTag ("FILL");
  }

  // segment data

  params.clear();
  params.append (new Parameter ("isClosed", closed ? "1" : "0"));
  gotStartTag ("SEGMENTS", params);

  bool breakPath = FALSE;

  PathElement *elem;

  double curX, curY;
  double changeX, changeY;

  for ( elem=m_curPath.first(); elem != 0; elem=m_curPath.next() )
  {
    params.clear();
    switch (elem->petype) {
      case PET_MoveTo :
        changeX = elem->pevalue.pointdata.x - curX;
        changeY = elem->pevalue.pointdata.y - curY;

#ifdef MERGE_LINESEGMENTS
        if ((!breakPath) || (changeX > eps) ||  (changeY > eps))
        {
          if (breakPath)
          {
            gotEndTag ("SEGMENTS");

            params.append (new Parameter ("isClosed", "1"));
            gotStartTag ("SEGMENTS", params);
            params.clear();
          }

          params.append (new Parameter ("x", QString::number(elem->pevalue.pointdata.x)));
          params.append (new Parameter ("y", QString::number(elem->pevalue.pointdata.y)));
          gotSimpleTag ("MOVE", params);
          breakPath = TRUE;
          curX = elem->pevalue.pointdata.x;
          curY = elem->pevalue.pointdata.y;
        }
#else
        if (breakPath)
        {
          gotEndTag ("SEGMENTS");
          gotStartTag ("SEGMENTS", params);
        }

        params.append (new Parameter ("x", QString::number(elem->pevalue.pointdata.x)));
        params.append (new Parameter ("y", QString::number(elem->pevalue.pointdata.y)));
        gotSimpleTag ("MOVE", params);
        breakPath = TRUE;
        curX = elem->pevalue.pointdata.x;
        curY = elem->pevalue.pointdata.y;
#endif

        break;
      case PET_LineTo :
        params.append (new Parameter ("x", QString::number(elem->pevalue.pointdata.x)));
        params.append (new Parameter ("y", QString::number(elem->pevalue.pointdata.y)));
        gotSimpleTag ("LINE", params);
        curX = elem->pevalue.pointdata.x;
        curY = elem->pevalue.pointdata.y;
        break;
      case PET_CurveTo :
        params.append (new Parameter ("x1", QString::number(elem->pevalue.bezierdata.x1)));
        params.append (new Parameter ("y1", QString::number(elem->pevalue.bezierdata.y1)));
        params.append (new Parameter ("x2", QString::number(elem->pevalue.bezierdata.x2)));
        params.append (new Parameter ("y2", QString::number(elem->pevalue.bezierdata.y2)));
        params.append (new Parameter ("x3", QString::number(elem->pevalue.bezierdata.x3)));
        params.append (new Parameter ("y3", QString::number(elem->pevalue.bezierdata.y3)));
        gotSimpleTag ("CURVE", params);
        curX = elem->pevalue.bezierdata.x3;
        curY = elem->pevalue.bezierdata.y3;
        break;
      case PET_CurveToOmitC1 :
        params.append (new Parameter ("x1", QString::number(curX)));
        params.append (new Parameter ("y1", QString::number(curY)));
        params.append (new Parameter ("x2", QString::number(elem->pevalue.bezierdata.x2)));
        params.append (new Parameter ("y2", QString::number(elem->pevalue.bezierdata.y2)));
        params.append (new Parameter ("x3", QString::number(elem->pevalue.bezierdata.x3)));
        params.append (new Parameter ("y3", QString::number(elem->pevalue.bezierdata.y3)));
        gotSimpleTag ("CURVE", params);
        curX = elem->pevalue.bezierdata.x3;
        curY = elem->pevalue.bezierdata.y3;
        break;
      case PET_CurveToOmitC2 :
        params.append (new Parameter ("x1", QString::number(elem->pevalue.bezierdata.x1)));
        params.append (new Parameter ("y1", QString::number(elem->pevalue.bezierdata.y1)));
        params.append (new Parameter ("x2", QString::number(elem->pevalue.bezierdata.x3)));
        params.append (new Parameter ("y2", QString::number(elem->pevalue.bezierdata.y3)));
        params.append (new Parameter ("x3", QString::number(elem->pevalue.bezierdata.x3)));
        params.append (new Parameter ("y3", QString::number(elem->pevalue.bezierdata.y3)));
        gotSimpleTag ("CURVE", params);
        curX = elem->pevalue.bezierdata.x3;
        curY = elem->pevalue.bezierdata.y3;
        break;
      default :
         qDebug ( "\ntype is %d", elem->petype );

    }
  }


  gotEndTag ("SEGMENTS");

  gotEndTag ("PATH");
}

// generic
void KarbonAIParserBase::gotClipPath (bool closed){
  doOutputCurrentPath (POT_Clip, closed);

  m_curPath.clear();
}

// generic
void KarbonAIParserBase::gotFillColor (AIColor &color){
  double r, g, b;
  color.toRGB (r,g,b);
  qDebug ("set fillcolor to %f %f %f",r,g,b);
  m_fillColor = color;
}

// generic
void KarbonAIParserBase::gotStrokeColor (AIColor &color){
  double r, g, b;
  color.toRGB (r,g,b);
  qDebug ("set strokecolor to %f %f %f",r,g,b);
  m_strokeColor = color;
}

// generic
void KarbonAIParserBase::gotBoundingBox (int llx, int lly, int urx, int ury){
  m_bbox.llx = llx;
  m_bbox.lly = lly;
  m_bbox.urx = urx;
  m_bbox.ury = ury;
}

void KarbonAIParserBase::gotLineWidth (double val){
  m_lineWidth = val;
}

void KarbonAIParserBase::gotFlatness (double val)
{
  m_flatness = val;
}

void KarbonAIParserBase::gotLineCaps (int val)
{
  m_lineCaps = val;
}

void KarbonAIParserBase::gotLineJoin (int val)
{
  m_lineJoin = val;
}

void KarbonAIParserBase::gotMiterLimit (double val)
{
  m_miterLimit = val;
}

void KarbonAIParserBase::gotWindingOrder (int val)
{
  m_windingOrder = val;
}

