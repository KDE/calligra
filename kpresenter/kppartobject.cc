/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Part Object                                            */
/******************************************************************/

#include "kppartobject.h"
#include "kppartobject.moc"
#include "kpresenter_doc.h"
#include "kpresenter_view.h"

/******************************************************************/
/* Class: KPPartObject                                            */
/******************************************************************/

/*======================== constructor ===========================*/
KPPartObject::KPPartObject(KPresenterChild *_child)
  : KPObject()
{
  child = _child;
  view = 0L;
  brush = NoBrush;
  gradient = 0;
  fillType = FT_BRUSH;
  gType = BCT_GHORZ;
  redrawPix = false;
  pen = QPen(black,1,NoPen);
  gColor1 = red;
  gColor2 = green;
}

/*======================== draw ==================================*/
void KPPartObject::draw(QPainter *_painter,int _diffx,int _diffy)
{
  if (move) 
    {
      KPObject::draw(_painter,_diffx,_diffy);
      return;
    }

  int ox = orig.x() - _diffx;
  int oy = orig.y() - _diffy;
  int ow = ext.width();
  int oh = ext.height();
  KRect r;

  _painter->save();

  r = _painter->viewport();
  _painter->setViewport(ox,oy,r.width(),r.height());
  
  if (angle == 0)
    paint(_painter);
  else
    {
      KRect br = KRect(0,0,ow,oh);
      int pw = br.width();
      int ph = br.height();
      KRect rr = br;
      int yPos = -rr.y();
      int xPos = -rr.x();
      rr.moveTopLeft(KPoint(-rr.width() / 2,-rr.height() / 2));
      
      QWMatrix m,mtx,m2;
      mtx.rotate(angle);
      m.translate(pw / 2,ph / 2);
      m2.translate(rr.left() + xPos,rr.top() + yPos);
      m = m2 * mtx * m;
      
      _painter->setWorldMatrix(m);
      paint(_painter);
    }

  _painter->setViewport(r);
  
  _painter->restore();

  KPObject::draw(_painter,_diffx,_diffy);
}

/*================================================================*/
void KPPartObject::paint(QPainter *_painter)
{
  QPicture* pic;
  pic = child->draw();

  _painter->setPen(pen);
  _painter->setBrush(brush);

  int pw = pen.width();

  if (pic && !pic->isNull())
    {
      if (fillType == FT_BRUSH || !gradient)
	{
	  _painter->drawRect(pw,pw,ext.width() - 2 * pw,ext.height() - 2 * pw);
	  _painter->drawPicture(*pic);
	}
      else
	{
	  int ow = ext.width();
	  int oh = ext.height();
	  
	  if (angle == 0)
	    _painter->drawPixmap(pw,pw,*gradient->getGradient(),0,0,ow - 2 * pw,oh - 2 * pw);
	  else
	    {
	      QPixmap pix(ow - 2 * pw,oh - 2 * pw);
	      QPainter p;
	      p.begin(&pix);
	      p.drawPixmap(0,0,*gradient->getGradient());
	      p.end();
	      
	      _painter->drawPixmap(pw,pw,pix);
	    }
	  
	  _painter->setPen(pen);
	  _painter->setBrush(NoBrush);
	  _painter->drawRect(pw,pw,ow - 2 * pw,oh - 2 * pw);
	  _painter->drawPicture(*pic);
	}
    }
}

/*================================================================*/
void KPPartObject::activate(QWidget *_widget,int diffx,int diffy)
{
  view->setGeometry(orig.x() - diffx + 20,orig.y() - diffy + 20,ext.width(),ext.height());
  view->show();
  view->view()->mainWindow()->setActivePart(view->view()->id());
}

/*================================================================*/
void KPPartObject::deactivate()
{
  view->hide();
  view->view()->mainWindow()->setActivePart(parentID);
}

/*================================================================*/
void KPPartObject::setSize(int _width,int _height)
{
  KPObject::setSize(_width,_height);
  child->setGeometry(QRect(orig.x(),orig.y(),ext.width(),ext.height()));

  if (move) return;

  if (fillType == FT_GRADIENT && gradient)
    {
      gradient->setSize(getSize());
      redrawPix = true;
      pix.resize(getSize());
    }
}

/*================================================================*/
void KPPartObject::resizeBy(int _dx,int _dy)
{
  KPObject::resizeBy(_dx,_dy);
  child->setGeometry(QRect(orig.x(),orig.y(),ext.width(),ext.height()));

  if (move) return;

  if (fillType == FT_GRADIENT && gradient)
    {
      gradient->setSize(getSize());
      redrawPix = true;
      pix.resize(getSize());
    }
}

/*================================================================*/
void KPPartObject::setOrig(int _x,int _y)
{
  KPObject::setOrig(_x,_y);
  child->setGeometry(QRect(orig.x(),orig.y(),ext.width(),ext.height()));
}

/*================================================================*/
void KPPartObject::moveBy(int _dx,int _dy)
{
  KPObject::moveBy(_dx,_dy);
  child->setGeometry(QRect(orig.x(),orig.y(),ext.width(),ext.height()));
}

/*================================================================*/
void KPPartObject::setFillType(FillType _fillType)
{ 
  fillType = _fillType; 

  if (fillType == FT_BRUSH && gradient) 
    {
      delete gradient;
      gradient = 0;
    }
  if (fillType == FT_GRADIENT && !gradient) 
    {
      gradient = new KPGradient(gColor1,gColor2,gType,getSize());
      redrawPix = true;
      pix.resize(getSize());
    }
}

/*================================================================*/
void KPPartObject::save(ostream& out)
{
  out << indent << "<EFFECTS effect=\"" << static_cast<int>(effect) << "\" effect2=\"" 
      << static_cast<int>(effect2) << "\"/>" << endl;
  out << indent << "<PEN red=\"" << pen.color().red() << "\" green=\"" << pen.color().green()
      << "\" blue=\"" << pen.color().blue() << "\" width=\"" << pen.width()
      << "\" style=\"" << static_cast<int>(pen.style()) << "\"/>" << endl;
  out << indent << "<BRUSH red=\"" << brush.color().red() << "\" green=\"" << brush.color().green()
      << "\" blue=\"" << brush.color().blue() << "\" style=\"" << static_cast<int>(brush.style()) << "\"/>" << endl;
  out << indent << "<PRESNUM value=\"" << presNum << "\"/>" << endl;
  out << indent << "<ANGLE value=\"" << angle << "\"/>" << endl;
  out << indent << "<FILLTYPE value=\"" << static_cast<int>(fillType) << "\"/>" << endl;
  out << indent << "<GRADIENT red1=\"" << gColor1.red() << "\" green1=\"" << gColor1.green()
      << "\" blue1=\"" << gColor1.blue() << "\" red2=\"" << gColor2.red() << "\" green2=\"" 
      << gColor2.green() << "\" blue2=\"" << gColor2.blue() << "\" type=\""
      << static_cast<int>(gType) << "\"/>" << endl;
}

/*========================== load ================================*/
void KPPartObject::load(KOMLParser& parser,vector<KOMLAttrib>& lst) 
{
  string tag;
  string name;

  while (parser.open(0L,tag))
    {
      KOMLParser::parseTag(tag.c_str(),name,lst);
	      
      // effects
      if (name == "EFFECTS")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "effect")
		  effect = (Effect)atoi((*it).m_strValue.c_str());
	      if ((*it).m_strName == "effect2")
		  effect2 = (Effect2)atoi((*it).m_strValue.c_str());
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
		pen.setColor(QColor(atoi((*it).m_strValue.c_str()),pen.color().green(),pen.color().blue()));
	      if ((*it).m_strName == "green")
		pen.setColor(QColor(pen.color().red(),atoi((*it).m_strValue.c_str()),pen.color().blue()));
	      if ((*it).m_strName == "blue")
		pen.setColor(QColor(pen.color().red(),pen.color().green(),atoi((*it).m_strValue.c_str())));
	      if ((*it).m_strName == "width")
		pen.setWidth(atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "style")
		pen.setStyle((PenStyle)atoi((*it).m_strValue.c_str()));
	    }
	  setPen(pen);
	}
      
      // brush
      else if (name == "BRUSH")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "red")
		brush.setColor(QColor(atoi((*it).m_strValue.c_str()),brush.color().green(),brush.color().blue()));
	      if ((*it).m_strName == "green")
		brush.setColor(QColor(brush.color().red(),atoi((*it).m_strValue.c_str()),brush.color().blue()));
	      if ((*it).m_strName == "blue")
		brush.setColor(QColor(brush.color().red(),brush.color().green(),atoi((*it).m_strValue.c_str())));
	      if ((*it).m_strName == "style")
		brush.setStyle((BrushStyle)atoi((*it).m_strValue.c_str()));
	    }
	  setBrush(brush);
	}
      
      // angle
      else if (name == "ANGLE")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "value")
		angle = atof((*it).m_strValue.c_str());
	    }
	}

      // presNum
      else if (name == "PRESNUM")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "value")
		presNum = atoi((*it).m_strValue.c_str());
	    }
	}

      // fillType
      else if (name == "FILLTYPE")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "value")
		fillType = static_cast<FillType>(atoi((*it).m_strValue.c_str()));
	    }
	  setFillType(fillType);
	}

      // gradient
      else if (name == "GRADIENT")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "red1")
		gColor1 = QColor(atoi((*it).m_strValue.c_str()),gColor1.green(),gColor1.blue());
	      if ((*it).m_strName == "green1")
		gColor1 = QColor(gColor1.red(),atoi((*it).m_strValue.c_str()),gColor1.blue());
	      if ((*it).m_strName == "blue1")
		gColor1 = QColor(gColor1.red(),gColor1.green(),atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "red2")
		gColor2 = QColor(atoi((*it).m_strValue.c_str()),gColor2.green(),gColor2.blue());
	      if ((*it).m_strName == "green2")
		gColor2 = QColor(gColor2.red(),atoi((*it).m_strValue.c_str()),gColor2.blue());
	      if ((*it).m_strName == "blue2")
		gColor2 = QColor(gColor2.red(),gColor2.green(),atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "type")
		gType = static_cast<BCType>(atoi((*it).m_strValue.c_str()));
	    }
	  setGColor1(gColor1);
	  setGColor2(gColor2);
	  setGType(gType);
	}

      else
	cerr << "Unknown tag '" << tag << "' in PART_OBJECT" << endl;    
      
      if (!parser.close(tag))
	{
	  cerr << "ERR: Closing Child" << endl;
	  return;
	}
    }
}
