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
/* Module: clipart object                                        */
/******************************************************************/

#include "kpclipartobject.h"
#include "kpclipartobject.moc"

/******************************************************************/
/* Class: KPClipartObject                                        */
/******************************************************************/

/*================ default constructor ===========================*/
KPClipartObject::KPClipartObject()
  : KPObject(), filename(), clipart()
{
}

/*================== overloaded constructor ======================*/
KPClipartObject::KPClipartObject(QString _filename)
  : KPObject(), filename(_filename), clipart(filename)
{
}

/*========================= save =================================*/
void KPClipartObject::save(ostream& out)
{
  out << indent << "<ORIG x=\"" << orig.x() << "\" y=\"" << orig.y() << "\"/>" << endl;
  out << indent << "<SIZE width=\"" << ext.width() << "\" height=\"" << ext.height() << "\"/>" << endl;
  out << indent << "<SHADOW distance=\"" << shadowDistance << "\" direction=\""
      << static_cast<int>(shadowDirection) << "\" red=\"" << shadowColor.red() << "\" green=\"" << shadowColor.green()
      << "\" blue=\"" << shadowColor.blue() << "\"/>" << endl;
  out << indent << "<EFFECTS effect=\"" << static_cast<int>(effect) << "\" effect2=\"" 
      << static_cast<int>(effect2) << "\"/>" << endl;
  out << indent << "<PRESNUM value=\"" << presNum << "\"/>" << endl;
  out << indent << "<ANGLE value=\"" << angle << "\"/>" << endl;
  out << indent << "<FILENAME filename=\"" << filename << "\"/>" << endl;
}

/*========================== load ================================*/
void KPClipartObject::load(KOMLParser& parser,vector<KOMLAttrib>& lst) 
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

      // filename
      else if (name == "FILENAME")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "filename")
		filename = (*it).m_strValue.c_str();
	      
	    }
	  setFileName(filename);
	}
      
      else
	cerr << "Unknown tag '" << tag << "' in CLIPART_OBJECT" << endl;    
      
      if (!parser.close(tag))
	{
	  cerr << "ERR: Closing Child" << endl;
	  return;
	}
    }
}

/*========================= draw =================================*/
void KPClipartObject::draw(QPainter *_painter,int _diffx,int _diffy)
{
  int ox = orig.x() - _diffx;
  int oy = orig.y() - _diffy;
  int ow = ext.width();
  int oh = ext.height();
  QRect r;

  // ********* TODO: shadow for cliparts
  
  _painter->save();
  r = _painter->viewport();
  _painter->setViewport(ox + 1,oy + 1,ext.width() - 2,ext.height() - 2);
  
  if (angle == 0)
    _painter->drawPicture(*clipart.getPic());
  else
    {
      QRect br = QRect(0,0,ow,oh);
      int pw = br.width();
      int ph = br.height();
      int yPos = -br.y();
      int xPos = -br.x();
      br.moveTopLeft(QPoint(-br.width() / 2,-br.height() / 2));

      QWMatrix m,mtx;
      mtx.rotate(angle);
      m.translate(pw / 2,ph / 2);
      m = mtx * m;
      
      QPixmap pm(pw,ph);
      pm.fill(white);
      QPainter pnt;
      pnt.begin(&pm);
      pnt.drawPicture(*clipart.getPic());
      pnt.end();
      
      _painter->setViewport(ox,oy,r.width(),r.height());
      _painter->setWorldMatrix(m);
      
      _painter->drawPixmap(br.left() + xPos,br.top() + yPos,pm);
    }

  _painter->setViewport(r);
  _painter->restore();

  KPObject::draw(_painter,_diffx,_diffy);
}




