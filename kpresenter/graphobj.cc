/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0alpha                                            */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Graphic Object                                         */
/******************************************************************/

#include "graphobj.h"
#include "graphobj.moc"

/******************************************************************/
/* class GraphObj                                                 */
/******************************************************************/

/*==================== constructor ===============================*/
GraphObj::GraphObj(QWidget* parent=0,const char* name=0,ObjType _objType=OT_LINE,QString fName=0)
  :QWidget(parent,name)
{
  QFileInfo fi(fName);

  objType = _objType;
  lineType = LT_HORZ;
  rectType = RT_NORM;
  fileName = fName;
  xRnd = 20;
  yRnd = 20;
  hide();
  if (objType == OT_AUTOFORM) 
    {
      atfInterp = new ATFInterpreter(this,fi.baseName());
      atfInterp->load(fileName);
    }
}

/*===================== destructor ===============================*/
GraphObj::~GraphObj()
{
}

/*======================= get pic ================================*/
QPicture* GraphObj::getPic(int,int,int,int)
{
  QPainter painter;
  
  if (objType == OT_CLIPART) return &clip;
  if (objType != OT_PICTURE)
    {
      painter.begin(&pic);
      painter.setClipping(true);
      paintObj(&painter);
      painter.end();
    }
  return &pic;
}

/*======================= load pixmap ============================*/
void GraphObj::loadPixmap()
{
  pix.load(fileName);
  origPix.load(fileName);
}

/*======================= load clipart ===========================*/
void GraphObj::loadClipart()
{
  wmf.load(fileName);
  wmf.paint(&clip);
}

/*======================= get Pixmap =============================*/
QPixmap GraphObj::getPix()
{
  QWMatrix m;

  if (width() != pix.width() || height() != pix.height())
    {
      m.scale((float)width()/origPix.width(),(float)height()/origPix.height());
      pix.operator=(origPix.xForm(m));
    }

  return pix;
}

/*======================== set filename ==========================*/
void GraphObj::setFileName(QString fn)
{
  fileName = fn;
  if (objType == OT_AUTOFORM) atfInterp->load(fileName);
}

/*========================== save ================================*/
void GraphObj::save(ostream& out)
{
  out << indent << "<LINETYPE value=\"" << lineType << "\"/>" << endl;
  out << indent << "<RECTTYPE value=\"" << rectType << "\"/>" << endl;
  out << indent << "<PEN red=\"" << oPen.color().red() << "\" green=\"" << oPen.color().green()
      << "\" blue=\"" << oPen.color().blue() << "\" width=\"" << oPen.width()
      << "\" style=\"" << oPen.style() << "\"/>" << endl;
  out << indent << "<BRUSH red=\"" << oBrush.color().red() << "\" green=\"" << oBrush.color().green()
      << "\" blue=\"" << oBrush.color().blue() << "\" style=\"" << oBrush.style() << "\"/>" << endl;
  out << indent << "<XRND value=\"" << xRnd << "\"/>" << endl;
  out << indent << "<YRND value=\"" << yRnd << "\"/>" << endl;
  if (objType != OT_AUTOFORM)
    out << indent << "<FILENAME value=\"" << fileName << "\"/>" << endl;
  else
    {
      QString afDir = qstrdup(KApplication::kde_datadir());
      afDir += "/kpresenter/autoforms/";
      int len = afDir.length();
      QString str = qstrdup(fileName);
      str = str.remove(0,len);
      out << indent << "<FILENAME value=\"" << str << "\"/>" << endl;
    }
}

/*============================ load ==============================*/
void GraphObj::load(KOMLParser& parser,vector<KOMLAttrib>& lst)
{
  string tag;
  string name;

  while (parser.open(0L,tag))
    {
      KOMLParser::parseTag(tag.c_str(),name,lst);
      
      // lineType
      if (name == "LINETYPE")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "value")
		lineType = (LineType)atoi((*it).m_strValue.c_str());
	    }
	}
      
      // rectType
      else if (name == "RECTTYPE")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "value")
		rectType = (RectType)atoi((*it).m_strValue.c_str());
	    }
	}
      
      // pen
      else if (name == "PEN")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "red")
		oPen.setColor(QColor(atoi((*it).m_strValue.c_str()),oPen.color().green(),oPen.color().blue()));
	      if ((*it).m_strName == "green")
		oPen.setColor(QColor(oPen.color().red(),atoi((*it).m_strValue.c_str()),oPen.color().blue()));
	      if ((*it).m_strName == "blue")
		oPen.setColor(QColor(oPen.color().red(),oPen.color().green(),atoi((*it).m_strValue.c_str())));
	      if ((*it).m_strName == "width")
		oPen.setWidth(atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "style")
		oPen.setStyle((PenStyle)atoi((*it).m_strValue.c_str()));
	    }
	  setObjPen(oPen);
	}
      
      // brush
      else if (name == "BRUSH")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "red")
		oBrush.setColor(QColor(atoi((*it).m_strValue.c_str()),oBrush.color().green(),oBrush.color().blue()));
	      if ((*it).m_strName == "green")
		oBrush.setColor(QColor(oBrush.color().red(),atoi((*it).m_strValue.c_str()),oBrush.color().blue()));
	      if ((*it).m_strName == "blue")
		oBrush.setColor(QColor(oBrush.color().red(),oBrush.color().green(),atoi((*it).m_strValue.c_str())));
	      if ((*it).m_strName == "style")
		oBrush.setStyle((BrushStyle)atoi((*it).m_strValue.c_str()));
	    }
	  setObjBrush(oBrush);
	}
      
      // xRnd
      else if (name == "XRND")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "value")
		xRnd = atoi((*it).m_strValue.c_str());
	    }
	}
      
      // yRnd
      else if (name == "YRND")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "value")
		yRnd = atoi((*it).m_strValue.c_str());
	    }
	}
      
      // filename
      else if (name == "FILENAME")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "value")
		{
		  fileName = (*it).m_strValue.c_str();
		  if (objType == OT_AUTOFORM)
		    {
		      QString afDir = qstrdup(KApplication::kde_datadir());
		      afDir += "/kpresenter/autoforms/";
		      fileName.insert(0,qstrdup(afDir));
		      QFileInfo fi(fileName);
		      atfInterp = new ATFInterpreter(this,fi.baseName());
		      atfInterp->load(fileName);
		    }
		}
	    }
	}
      
      else
	cerr << "Unknown tag '" << tag << "' in GRAPHOBJ" << endl;    
      
      if (!parser.close(tag))
	{
	  cerr << "ERR: Closing Child" << endl;
	  return;
	}
    }
}

/*======================= paint event ============================*/
void GraphObj::paintEvent(QPaintEvent* paintEvent)
{
  QPainter painter;
  QRect clipRect = paintEvent->rect();

  painter.begin(this);
  painter.setClipping(true);
  painter.setClipRect(clipRect);
  paintObj(&painter);
  painter.end();
} 

/*====================== paint object ============================*/
void GraphObj::paintObj(QPainter *painter)
{
  int ox = 0,oy = 0,ow = width(),oh = height();
  unsigned int pw = 0,pwOrig = 0,px,py;
  QPicture p;

  switch (objType)
    {
   case OT_LINE: /* line */
      {
	painter->setPen(oPen);
	switch (lineType)
	  {
	  case LT_HORZ:
	    painter->drawLine(ox,oy + oh / 2,ox + ow,oy + oh / 2);
	    break;
	  case LT_VERT:
	    painter->drawLine(ox + ow / 2,oy,ox + ow / 2,oy + oh);
	    break;
	  case LT_LU_RD:
	    painter->drawLine(ox,oy,ox + ow,oy + oh);
	    break;
	  case LT_LD_RU:
	    painter->drawLine(ox,oy + oh,ox + ow,oy);
	    break;
	  }
      } break;
    case OT_RECT: /* rectangle */
      {
	painter->setPen(oPen);
	pw = oPen.width();
	painter->setBrush(oBrush);
	if (rectType == RT_NORM)
	  painter->drawRect(ox + pw,oy + pw,ow - 2*pw,oh - 2*pw);
	else
	  painter->drawRoundRect(ox + pw,oy + pw,ow - 2*pw,oh - 2*pw,xRnd,yRnd);
      } break;
    case OT_CIRCLE: /* circle */
      {
	painter->setPen(oPen);
	pw = oPen.width();
	painter->setBrush(oBrush);
	painter->drawEllipse(ox + pw,oy + pw,ow - 2*pw,oh - 2*pw);
      } break;
    case OT_AUTOFORM: /* autoforms */
      {
	painter->setPen(oPen);
	pwOrig = oPen.width() + 3;
	painter->setBrush(oBrush);

	if (atfInterp)
	  {
	    QPointArray pntArray = atfInterp->getPointArray(width(),height());
	    QList<ATFInterpreter::AttribList> atrLs = atfInterp->getAttribList();
	    QPointArray pntArray2(pntArray.size());
	    for (unsigned int i=0;i < pntArray.size();i++)
	      {
		px = pntArray.at(i).x();
		py = pntArray.at(i).y();
		if (atrLs.at(i)->pwDiv > 0)
		  {
		    pw = pwOrig / atrLs.at(i)->pwDiv;
		    if (px < (unsigned int)width() / 2) px += pw;
		    if (py < (unsigned int)height() / 2) py += pw;
		    if (px > (unsigned int)width() / 2) px -= pw;
		    if (py > (unsigned int)height() / 2) py -= pw;
		  }
		pntArray2.setPoint(i,px,py);
	      }
	    painter->drawPolygon(pntArray2);
	    if (isVisible())
	      {
		// paint hot-points
	      }
	  }

      } break;
    }
}

/*====================== mouse press =============================*/
void GraphObj::mousePressEvent(QMouseEvent*)
{
  // manage hotpoints
}

/*====================== mouse release ===========================*/
void GraphObj::mouseReleaseEvent(QMouseEvent*)
{
  // manage hotpoints
}

/*====================== mouse move= =============================*/
void GraphObj::mouseMoveEvent(QMouseEvent*)
{
  // manage hotpoints
}




