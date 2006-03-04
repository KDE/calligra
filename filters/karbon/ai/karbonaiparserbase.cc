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

#include "karbonaiparserbase.h"

#include <core/vcolor.h>
#include <core/vlayer.h>
#include <core/vgroup.h>
#include <core/vclipgroup.h>
#include <core/vvisitor.h>
#include "aicolor.h"
#include <qwmatrix.h>
#include <commands/vtransformcmd.h>

#include <KoPageLayout.h>

const void pottoa (PathOutputType &data);

// generic
KarbonAIParserBase::KarbonAIParserBase() : m_pot(POT_Other), m_ptt (PTT_Output), m_bbox(0,0,612,792), m_emptyFill(), m_emptyStroke()
/* , m_strokeColor(), m_fillColor() */ {

  // A4, 70 dpi
/*  m_bbox.llx = 0;
  m_bbox.lly = 0;
  m_bbox.urx = 612;
  m_bbox.ury = 792; */

/*  m_lineWidth = 0;
  m_flatness = 0;
  m_lineCaps = 0;
  m_lineJoin = 0;
  m_miterLimit = 10; */
  m_windingOrder = 0;

  m_fm = FM_NonZero;

  m_curKarbonPath = new VPath( 0L );

  m_document = new VDocument();
  m_layer = NULL;
  m_combination = NULL;

  m_emptyFill.setType (VFill::none);
  m_emptyStroke.setType (VStroke::none);

  setupHandlers();
}

// generic
KarbonAIParserBase::~KarbonAIParserBase(){
  teardownHandlers();
  delete m_curKarbonPath;

  delete m_document;
}

// generic
void KarbonAIParserBase::parsingStarted(){
//  qDebug ( getHeader().latin1() );
}

// generic
void KarbonAIParserBase::parsingFinished(){
//  qDebug ( getFooter().latin1() );

  // handle bounding box
  if (m_document)
  {
    kdDebug() << "bbox 1 is " << m_bbox << endl;

    if (m_bbox.width() > 0. )
      m_document->setWidth (m_bbox.width());
    if (m_bbox.height() > 0. )
      m_document->setHeight (m_bbox.height());

/*    QWMatrix matrix;
    matrix.translate (-m_bbox.x(),-m_bbox.y());

    VTransformNodes translator (matrix);
    m_document->accept (translator); */
    VTranslateCmd cmd (0L, -m_bbox.x(), -m_bbox.y());
    m_document->accept (cmd);
  }
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
  switch (element.petype)
  {
    case PET_MoveTo :
      m_curKarbonPath->moveTo (KoPoint (element.pevalue.pointdata.x,element.pevalue.pointdata.y));
      break;
    case PET_LineTo :
      m_curKarbonPath->lineTo (KoPoint (element.pevalue.pointdata.x,element.pevalue.pointdata.y));
      break;
    case PET_CurveTo :
      m_curKarbonPath->curveTo (KoPoint (element.pevalue.bezierdata.x1,element.pevalue.bezierdata.y1),
                             KoPoint (element.pevalue.bezierdata.x2,element.pevalue.bezierdata.y2),
                             KoPoint (element.pevalue.bezierdata.x3,element.pevalue.bezierdata.y3));
      break;
    case PET_CurveToOmitC1 :
      m_curKarbonPath->curve1To (KoPoint (element.pevalue.bezierdata.x2,element.pevalue.bezierdata.y2),
                             KoPoint (element.pevalue.bezierdata.x3,element.pevalue.bezierdata.y3));
      break;
    case PET_CurveToOmitC2 :
      m_curKarbonPath->curve2To (KoPoint (element.pevalue.bezierdata.x1,element.pevalue.bezierdata.y1),
                             KoPoint (element.pevalue.bezierdata.x3,element.pevalue.bezierdata.y3));
      break;
  }
}

// generic
void KarbonAIParserBase::gotFillPath (bool closed, bool reset, FillMode /*fm*/){
//  qDebug ("found fill path");
//  if (!reset) qDebug ("retain filled path");

  if (closed) m_curKarbonPath->close();

  if (!reset)
    m_pot = POT_Filled;
  else
  {
    doOutputCurrentPath2 (POT_Filled);
    m_pot = POT_Other;
  }
}

// generic
void KarbonAIParserBase::gotIgnorePath (bool closed, bool reset){
//  qDebug ("found ignore path");

  if (closed) m_curKarbonPath->close();

  if (! reset)
    m_pot = POT_Other;
  else
  {
    doOutputCurrentPath2 (POT_Ignore);
    m_pot = POT_Other;
  }
}

// generic
void KarbonAIParserBase::gotStrokePath (bool closed) {
//  qDebug ("found stroke path");

  if (closed) m_curKarbonPath->close();

  PathOutputType pot = POT_Stroked;
  if (m_pot != POT_Other)
  {
    pot = POT_FilledStroked;
  }

  doOutputCurrentPath2 (pot);

  m_pot = POT_Other;
}

// generic
void KarbonAIParserBase::gotClipPath (bool /*closed*/){

  doOutputCurrentPath2 (POT_Clip);
}

// generic
void KarbonAIParserBase::gotFillColor (AIColor &color){
//  double r, g, b;
//  color.toRGB (r,g,b);
//  qDebug ("set fillcolor to %f %f %f",r,g,b);
//  m_fillColor = color;

  VColor karbonColor = toKarbonColor (color);
  m_fill.setColor (karbonColor);
}

// generic
void KarbonAIParserBase::gotStrokeColor (AIColor &color){
//  double r, g, b;
//  color.toRGB (r,g,b);
//  qDebug ("set strokecolor to %f %f %f",r,g,b);
//  m_strokeColor = color;

  VColor karbonColor = toKarbonColor (color);
  m_stroke.setColor (karbonColor);
}

// generic
void KarbonAIParserBase::gotBoundingBox (int llx, int lly, int urx, int ury){
/*  m_bbox.llx = llx;
  m_bbox.lly = lly;
  m_bbox.urx = urx;
  m_bbox.ury = ury; */
  m_bbox.setCoords(llx,lly,urx,ury);
}

void KarbonAIParserBase::gotLineWidth (double val){
//  m_lineWidth = val;
  m_stroke.setLineWidth (val);
}

void KarbonAIParserBase::gotFlatness (double /*val*/)
{
//  m_flatness = val;
//  m_stroke.setFlatness (val);
}

void KarbonAIParserBase::gotLineCaps (int val)
{
//  m_lineCaps = val;
  VStroke::VLineCap lineCap = VStroke::capButt;

  switch (val)
  {
	  case 0 : lineCap = VStroke::capButt; break;
	  case 1 : lineCap = VStroke::capRound; break;
	  case 2 : lineCap = VStroke::capSquare; break;
  }

  m_stroke.setLineCap (lineCap);
}

void KarbonAIParserBase::gotLineJoin (int val)
{
//  m_lineJoin = val;

  VStroke::VLineJoin lineJoin = VStroke::joinMiter;

  switch (val)
  {
	  case 0 : lineJoin = VStroke::joinMiter; break;
	  case 1 : lineJoin = VStroke::joinRound; break;
	  case 2 : lineJoin = VStroke::joinBevel; break;
  }

  m_stroke.setLineJoin (lineJoin);

}

void KarbonAIParserBase::gotMiterLimit (double val)
{
//  m_miterLimit = val;
  m_stroke.setMiterLimit (val);
}

void KarbonAIParserBase::gotWindingOrder (int val)
{
  m_windingOrder = val;
}

void KarbonAIParserBase::gotBeginGroup (bool clipping)
{
//  qDebug ("start begin group");
  if (clipping)
  {
    VClipGroup *group = new VClipGroup( 0L );
    m_groupStack.push (group);
  }
  else
  {
    VGroup *group = new VGroup( 0L );
    m_groupStack.push (group);
  }

//  qDebug ("end begin group");

}

void KarbonAIParserBase::gotEndGroup (bool /*clipping*/)
{
//  qDebug ("start end group");

  if (m_debug) qDebug ("got end group");

  if (m_groupStack.isEmpty()) return;

  if (m_debug) qDebug ("got end group 2");

  VGroup *group = m_groupStack.pop();

  if (m_debug) qDebug ("got end group 3");

  if (m_debug)
  {
    if (!group) qDebug ("group is NULL");
  }

  if (m_groupStack.isEmpty())
  {
    if (m_debug) qDebug ("insert object");
    ensureLayer();
    m_layer->append (group);
    if (m_debug) qDebug ("/insert object");
  }
  else
  {
    if (m_debug) qDebug ("insert object to group");

    m_groupStack.top()->append (group);
    if (m_debug) qDebug ("/insert object to group");
  }

  if (m_debug) qDebug ("/got end group");

//  qDebug ("end end group");
}

void KarbonAIParserBase::gotBeginCombination () {
  m_ptt = PTT_Combine;
}

void KarbonAIParserBase::gotEndCombination () {
//  qDebug ( "got end combination" );

  m_ptt = PTT_Output;

  if (m_combination != NULL)
  {
    m_curKarbonPath = m_combination;
    doOutputCurrentPath2 (POT_Leave);
  }

  m_combination = NULL;
}


const VColor KarbonAIParserBase::toKarbonColor (const AIColor &color)
{
  AIColor temp (color);
  VColor value;

  double v1, v2, v3, v4;
  temp.toCMYK (v1, v2, v3, v4);

  float cv1 = v1;
  float cv2 = v2;
  float cv3 = v3;
  float cv4 = v4;

  value.setColorSpace (VColor::cmyk);
  value.set (cv1, cv2, cv3, cv4);

  return value;
}

void KarbonAIParserBase::doOutputCurrentPath2(PathOutputType type)
{
  ensureLayer();

  if (type != POT_Leave)
  {
//    pottoa(type);

    m_curKarbonPath->setStroke(m_emptyStroke);
    m_curKarbonPath->setFill(m_emptyFill);

    if ((type != POT_Filled) && (type != POT_Stroked) && (type != POT_FilledStroked)) return;
    if ((type == POT_Filled) || (type == POT_FilledStroked))
    {
/*      VFill fill;
      fill.setColor (toKarbonColor (m_fillColor));
      m_curKarbonPath->setFill(fill); */
//      qDebug ("set filled");
      m_curKarbonPath->setFill(m_fill);
    }

    if ((type == POT_Stroked) || (type == POT_FilledStroked))
    {
/*      VStroke stroke;
      stroke.setColor (toKarbonColor (m_strokeColor));
      m_curKarbonPath->setStroke (stroke); */
//      qDebug ("set stroked");
      m_curKarbonPath->setStroke (m_stroke);
    }
  }

  if (m_ptt == PTT_Combine)
  {
//    m_pot |= type;
    if (m_combination == NULL)
      m_combination = m_curKarbonPath;
    else
      m_combination->combine (*m_curKarbonPath);

    m_curKarbonPath = new VPath( 0L );

    return;
  }

  ensureLayer();

  if (m_groupStack.isEmpty())
  {
    m_layer->append( m_curKarbonPath );
  }
  else
  {
    m_groupStack.top()->append( m_curKarbonPath );
  }

  m_curKarbonPath = new VPath( 0L );
}

bool KarbonAIParserBase::parse (QIODevice& fin, QDomDocument &doc)
{

  bool res = AIParserBase::parse (fin);

//  qDebug ("document is %s",doc.toString().latin1());
  if (res)
  {
      qDebug ("before save document");
      doc = m_document->saveXML();
	// add paper info, we always need custom for svg (Rob)
	QDomElement paper = doc.createElement( "PAPER" );
	doc.documentElement().appendChild( paper );
	paper.setAttribute( "format", PG_CUSTOM );
	paper.setAttribute( "width", m_document->width() );
	paper.setAttribute( "height", m_document->height() );

      qDebug ("after save document");
  }
  else
  {
    QDomDocument tempDoc;
    doc = tempDoc;
  }

  return res;
}

void KarbonAIParserBase::ensureLayer ()
{
  if (!m_layer)
  {
    m_layer = new VLayer( 0 );
    m_document->insertLayer (m_layer);
  }
}


void KarbonAIParserBase::setupHandlers()
{
//  qDebug("setupHandler called");
  m_gstateHandler = new KarbonGStateHandler(this);
  m_structureHandler = new KarbonStructureHandler(this);
  m_pathHandler = new KarbonPathHandler(this);
  m_documentHandler = new KarbonDocumentHandler(this);

  m_textHandler = new TextHandlerBase();

}

void KarbonAIParserBase::teardownHandlers()
{
//  qDebug("teardownHandler called");
  delete m_textHandler;

  delete m_gstateHandler;
  delete m_structureHandler;
  delete m_pathHandler;
  delete m_documentHandler;
}


void KarbonDocumentHandler::gotBoundingBox (int llx, int lly, int urx, int ury)
{
  delegate->gotBoundingBox(llx,lly,urx,ury);
}

void KarbonDocumentHandler::gotCreationDate (const char */*val1*/,const char */*val2*/)
{
//  qDebug ("got creation date [%s], [%s]",val1,val2);
}

void KarbonDocumentHandler::gotProcessColors (int /*colors*/)
{
/*  if (colors && PC_Cyan) qDebug ("contains cyan");
  if (colors && PC_Magenta) qDebug ("contains magenta");
  if (colors && PC_Yellow) qDebug ("contains yellow");
  if (colors && PC_Black) qDebug ("contains black"); */
}


void KarbonGStateHandler::gotFillColor (AIColor &color)
{
  delegate->gotFillColor (color);
}

void KarbonGStateHandler::gotStrokeColor (AIColor &color)
{
  delegate->gotStrokeColor(color);
}

void KarbonGStateHandler::gotFlatness (double val)
{
  delegate->gotFlatness(val);
}

void KarbonGStateHandler::gotLineWidth (double val)
{
  delegate->gotLineWidth(val);
}

void KarbonGStateHandler::gotLineCaps (int val)
{
  delegate->gotLineCaps(val);
}

void KarbonGStateHandler::gotLineJoin (int val)
{
  delegate->gotLineJoin(val);
}

void KarbonGStateHandler::gotMiterLimit (double val)
{
  delegate->gotMiterLimit(val);
}

void KarbonGStateHandler::gotWindingOrder (int val)
{
  delegate->gotWindingOrder(val);
}

void KarbonStructureHandler::gotBeginGroup (bool clipping)
{
  delegate->gotBeginGroup(clipping);
}

void KarbonStructureHandler::gotEndGroup (bool clipping)
{
  delegate->gotEndGroup(clipping);
}

void KarbonStructureHandler::gotBeginCombination ()
{
  delegate->gotBeginCombination();
}

void KarbonStructureHandler::gotEndCombination ()
{
  delegate->gotEndCombination();
}

void KarbonPathHandler::gotPathElement (PathElement &element)
{
  delegate->gotPathElement (element);
}

void KarbonPathHandler::gotFillPath (bool closed, bool reset)
{
  delegate->gotFillPath(closed, reset, m_fm);
}

void KarbonPathHandler::gotFillMode (FillMode fm)
{
  m_fm = fm;
}

void KarbonPathHandler::gotStrokePath (bool closed)
{
  delegate->gotStrokePath(closed);
}

void KarbonPathHandler::gotIgnorePath (bool closed, bool reset)
{
  delegate->gotIgnorePath(closed, reset);
}

void KarbonPathHandler::gotClipPath (bool closed)
{
  delegate->gotClipPath(closed);
}

const void pottoa (PathOutputType &data)
{
  switch (data)
  {
    case POT_Filled : qDebug ("filled"); break;
    case POT_Stroked : qDebug ("stroked"); break;
    case POT_FilledStroked : qDebug ("filled/stroked"); break;
    case POT_Clip : qDebug ("clip"); break;
    case POT_Ignore : qDebug ("ignore"); break;
    case POT_Leave : qDebug ("leave"); break;
    default : qDebug ("unknown");
  }
}

