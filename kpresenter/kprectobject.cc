/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: rect object                                            */
/******************************************************************/

#include "kprectobject.h"
#include "kprectobject.moc"

/******************************************************************/
/* Class: KPRectObject                                            */
/******************************************************************/

/*================ default constructor ===========================*/
KPRectObject::KPRectObject()
  : KPObject(), pen(), brush(), gColor1(red), gColor2(green)
{
  rectType = RT_NORM;
  xRnd = 20;
  yRnd = 20;
  gradient = 0;
  fillType = FT_BRUSH;
  gType = BCT_GHORZ;
}

/*================== overloaded constructor ======================*/
KPRectObject::KPRectObject(QPen _pen,QBrush _brush,RectType _rectType,FillType _fillType,
			   QColor _gColor1,QColor _gColor2,BCType _gType,int _xRnd,int _yRnd)
  : KPObject(), pen(_pen), brush(_brush), gColor1(_gColor1), gColor2(_gColor2)
{
  rectType = _rectType;
  xRnd = _xRnd;
  yRnd = _yRnd;
  gType = _gType;
  fillType = _fillType;

  if (fillType == FT_GRADIENT)
    gradient = new KPGradient(gColor1,gColor2,gType,QSize(1,1));
  else
    gradient = 0;
}

/*================================================================*/
void KPRectObject::setSize(int _width,int _height)
{
  KPObject::setSize(_width,_height);
  if (fillType == FT_GRADIENT && gradient)
    gradient->setSize(getSize());
}

/*================================================================*/
void KPRectObject::resizeBy(int _dx,int _dy)
{
  KPObject::resizeBy(_dx,_dy);
  if (fillType == FT_GRADIENT && gradient)
    gradient->setSize(getSize());
}

/*================================================================*/
void KPRectObject::setFillType(FillType _fillType)
{ 
  fillType = _fillType; 

  if (fillType == FT_BRUSH && gradient)
    {
      delete gradient;
      gradient = 0;
    }
  if (fillType == FT_GRADIENT && !gradient) gradient = new KPGradient(gColor1,gColor2,gType,getSize());
}

/*========================= save =================================*/
void KPRectObject::save(ostream& out)
{
  out << indent << "<ORIG x=\"" << orig.x() << "\" y=\"" << orig.y() << "\"/>" << endl;
  out << indent << "<SIZE width=\"" << ext.width() << "\" height=\"" << ext.height() << "\"/>" << endl;
  out << indent << "<SHADOW distance=\"" << shadowDistance << "\" direction=\""
      << shadowDirection << "\" red=\"" << shadowColor.red() << "\" green=\"" << shadowColor.green()
      << "\" blue=\"" << shadowColor.blue() << "\"/>" << endl;
  out << indent << "<EFFECTS effect=\"" << static_cast<int>(effect) << "\" effect2=\"" 
      << static_cast<int>(effect2) << "\"/>" << endl;
  out << indent << "<PEN red=\"" << pen.color().red() << "\" green=\"" << pen.color().green()
      << "\" blue=\"" << pen.color().blue() << "\" width=\"" << pen.width()
      << "\" style=\"" << pen.style() << "\"/>" << endl;
  out << indent << "<BRUSH red=\"" << brush.color().red() << "\" green=\"" << brush.color().green()
      << "\" blue=\"" << brush.color().blue() << "\" style=\"" << brush.style() << "\"/>" << endl;
  out << indent << "<RECTTYPE value=\"" << rectType << "\"/>" << endl;
  out << indent << "<PRESNUM value=\"" << presNum << "\"/>" << endl;
  out << indent << "<ANGLE value=\"" << angle << "\"/>" << endl;
}

/*========================== load ================================*/
void KPRectObject::load(KOMLParser& parser,vector<KOMLAttrib>& lst) 
{
  string tag;
  string name;

  while (parser.open(0L,tag))
    {
      KOMLParser::parseTag(tag.c_str(),name,lst);
	      
      // orig
      if (name == "ORIG")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "x")
		orig.setX(atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "y")
		orig.setY(atoi((*it).m_strValue.c_str()));
	    }
	}

      // size
      else if (name == "SIZE")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "width")
		ext.setWidth(atoi((*it).m_strValue.c_str()));
	      if ((*it).m_strName == "height")
		ext.setHeight(atoi((*it).m_strValue.c_str()));
	    }
	}

      // shadow
      else if (name == "SHADOW")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "distance")
		shadowDistance = atoi((*it).m_strValue.c_str());
	      if ((*it).m_strName == "direction")
		shadowDirection = (ShadowDirection)atoi((*it).m_strValue.c_str());
	      if ((*it).m_strName == "red")
		shadowColor.setRgb(atoi((*it).m_strValue.c_str()),
				   shadowColor.green(),shadowColor.blue());
	      if ((*it).m_strName == "green")
		shadowColor.setRgb(shadowColor.red(),atoi((*it).m_strValue.c_str()),
				   shadowColor.blue());
	      if ((*it).m_strName == "blue")
		shadowColor.setRgb(shadowColor.red(),shadowColor.green(),
				   atoi((*it).m_strValue.c_str()));
	    }
	}

      // effects
      else if (name == "EFFECTS")
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
      else
	cerr << "Unknown tag '" << tag << "' in RECT_OBJECT" << endl;    
      
      if (!parser.close(tag))
	{
	  cerr << "ERR: Closing Child" << endl;
	  return;
	}
    }
}

/*========================= draw =================================*/
void KPRectObject::draw(QPainter *_painter,int _diffx,int _diffy)
{
  int ox = orig.x() - _diffx;
  int oy = orig.y() - _diffy;
  int ow = ext.width();
  int oh = ext.height();
  QRect r;

  _painter->save();
  
  if (shadowDistance > 0)
    {
      QPen tmpPen(pen);
      pen.setColor(shadowColor);
      QBrush tmpBrush(brush);
      brush.setColor(shadowColor);
      r = _painter->viewport();
      
      if (angle == 0)
	{
	  int sx = ox;
	  int sy = oy;
	  getShadowCoords(sx,sy,shadowDirection,shadowDistance);
			
	  _painter->setViewport(sx,sy,r.width(),r.height());
	  paint(_painter);
	}
      else
	{
	  _painter->setViewport(ox,oy,r.width(),r.height());

	  QRect br = QRect(0,0,ow,oh);
	  int pw = br.width();
	  int ph = br.height();
	  QRect rr = br;
	  int yPos = -rr.y();
	  int xPos = -rr.x();
	  rr.moveTopLeft(QPoint(-rr.width() / 2,-rr.height() / 2));
	  
	  int sx = 0;
	  int sy = 0;
	  getShadowCoords(sx,sy,shadowDirection,shadowDistance);

	  QWMatrix m,mtx,m2;
	  mtx.rotate(angle);
	  m.translate(pw / 2,ph / 2);
	  m2.translate(rr.left() + xPos + sx,rr.top() + yPos + sy);
	  m = m2 * mtx * m;
			
	  _painter->setWorldMatrix(m);
	  paint(_painter);
	}
      
      _painter->setViewport(r);
      pen = tmpPen;
      brush = tmpBrush;
    }
  
  _painter->restore();
  _painter->save();

  r = _painter->viewport();
  _painter->setViewport(ox,oy,r.width(),r.height());
  
  if (angle == 0)
    paint(_painter);
  else
    {
      QRect br = QRect(0,0,ow,oh);
      int pw = br.width();
      int ph = br.height();
      QRect rr = br;
      int yPos = -rr.y();
      int xPos = -rr.x();
      rr.moveTopLeft(QPoint(-rr.width() / 2,-rr.height() / 2));
      
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

/*======================== paint =================================*/
void KPRectObject::paint(QPainter* _painter)
{
  int ow = ext.width();
  int oh = ext.height();

  _painter->setPen(pen);
  int pw = pen.width();
  _painter->setBrush(brush);
  if (rectType == RT_NORM)
    _painter->drawRect(pw,pw,ow - 2 * pw,oh - 2 * pw);
  else
    _painter->drawRoundRect(pw,pw,ow - 2 * pw,oh - 2 * pw,xRnd,yRnd);
}




