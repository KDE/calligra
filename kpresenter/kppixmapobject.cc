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
/* Module: pixmap object                                          */
/******************************************************************/

#include "kppixmapobject.h"
#include "kppixmapobject.moc"

/******************************************************************/
/* Class: KPPixmapObject                                          */
/******************************************************************/

/*================ default constructor ===========================*/
KPPixmapObject::KPPixmapObject(KPPixmapCollection *_pixmapCollection)
  : KPObject(), filename(), data()
{
  pixmapCollection = _pixmapCollection;
  pixmap = 0;
}

/*================== overloaded constructor ======================*/
KPPixmapObject::KPPixmapObject(KPPixmapCollection *_pixmapCollection,QString _filename)
  : KPObject(), data()
{
  pixmapCollection = _pixmapCollection;
  pixmap = 0;
  ext = orig_size;
  setFileName(_filename);
}

/*======================= set size ===============================*/
void KPPixmapObject::setSize(int _width,int _height)
{
  KPObject::setSize(_width,_height);
  if (move) return;

  if (!filename.isEmpty() && !data.isEmpty())
    {
      if (pixmap)
	{
	  pixmap = pixmapCollection->getPixmap(filename,data,pixSize,true,false);
	  QString _data = data;
	  KSize _pixSize = pixSize;
	  pixSize = ext;
	  pixmap = pixmapCollection->getPixmap(filename,data,pixmap,pixSize);
	  pixmapCollection->removeRef(filename,_data,_pixSize);
	}
      else
	{
	  pixSize = ext;
	  pixmap = pixmapCollection->getPixmap(filename,data,pixSize);
	}
    }
}

/*======================= set size ===============================*/
void KPPixmapObject::resizeBy(int _dx,int _dy)
{
  KPObject::resizeBy(_dx,_dy);
  if (move) return;

  if (!filename.isEmpty() && !data.isEmpty())
    {
      if (pixmap)
	{
	  pixmap = pixmapCollection->getPixmap(filename,data,pixSize,true,false);
	  QString _data = data;
	  KSize _pixSize = pixSize;
	  pixSize = ext;
	  pixmap = pixmapCollection->getPixmap(filename,data,pixmap,pixSize);
	  pixmapCollection->removeRef(filename,_data,_pixSize);
	}
      else
	{
	  pixSize = ext;
	  pixmap = pixmapCollection->getPixmap(filename,data,pixSize);
	}
    }
}

/*======================== set backpix filename ==================*/
void KPPixmapObject::setFileName(QString _filename)
{
  if (_filename.isEmpty() || _filename == filename) return;

  if (!filename.isEmpty())
    pixmapCollection->removeRef(filename,pixSize);

  filename = _filename;

  pixSize = ext;

  pixmap = pixmapCollection->getPixmap(filename,pixSize,data);
  ext = pixmap->size();
  pixSize = ext;
}

/*========================== set backpix  ========================*/
void KPPixmapObject::setPixmap(QString _filename,QString _data)
{
  if (!filename.isEmpty() || _filename == filename)
    pixmapCollection->removeRef(filename,pixSize);

  filename = _filename;
  data = _data;

  pixSize = ext;
  
  pixmap = pixmapCollection->getPixmap(filename,data,pixSize);
}

/*========================= save =================================*/
void KPPixmapObject::save(ostream& out)
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

  QString _data;
  pixmapCollection->getPixmap(filename,pixSize,_data);
  pixmapCollection->removeRef(filename,pixSize);

  out << indent << "<PIXMAP filename=\"" << filename << "\" data=\"" 
      << _data << "\"/>" << endl;
}

/*========================== load ================================*/
void KPPixmapObject::load(KOMLParser& parser,vector<KOMLAttrib>& lst) 
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

      
      // pixmap
      else if (name == "PIXMAP")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  
	  bool openPic = true;
	  QString _data,_fileName;
	  
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "data")
		{
		  _data = (*it).m_strValue.c_str();
		  if (_data.isEmpty())
		    openPic = true;
		  else
		    openPic = false;
		}
	      else if ((*it).m_strName == "filename")
		{
		  _fileName = (*it).m_strValue.c_str();
		  if (!_fileName.isEmpty())
		    {
		      if (int _envVarB = _fileName.find('$') >= 0)
			{
			  int _envVarE = _fileName.find('/',_envVarB);
			  QString path = static_cast<const char*>(getenv(_fileName.mid(_envVarB,_envVarE-_envVarB).data()));
			  _fileName.replace(_envVarB-1,_envVarE-_envVarB+1,path);
			}
		    }
		}
	    }
	  
	  if (!openPic)
	    setPixmap(_fileName,_data);
	  else
	    setFileName(_fileName);
	}

      else
	cerr << "Unknown tag '" << tag << "' in PIXMAP_OBJECT" << endl;    
      
      if (!parser.close(tag))
	{
	  cerr << "ERR: Closing Child" << endl;
	  return;
	}
    }
}

/*========================= draw =================================*/
void KPPixmapObject::draw(QPainter *_painter,int _diffx,int _diffy)
{
  if (move) 
    {
      KPObject::draw(_painter,_diffx,_diffy);
      return;
    }

  if (!pixmap) return;

  int ox = orig.x() - _diffx;
  int oy = orig.y() - _diffy;
  KRect r;

  _painter->save();
  if (shadowDistance > 0)
    {
      if (angle == 0)
	{
	  int sx = ox;
	  int sy = oy;
	  getShadowCoords(sx,sy,shadowDirection,shadowDistance);

	  _painter->setPen(QPen(shadowColor));
	  _painter->setBrush(shadowColor);

	  KSize bs = pixmap->size();
	  
	  _painter->drawRect(sx,sy,bs.width(),bs.height());
	}
      else
	{
	  r = _painter->viewport();
	  _painter->setViewport(ox,oy,r.width(),r.height());
			
	  KRect br = pixmap->rect();
	  int pw = br.width();
	  int ph = br.height();
	  KRect rr = br;
	  int pixYPos = -rr.y();
	  int pixXPos = -rr.x();
	  br.moveTopLeft(KPoint(-br.width() / 2,-br.height() / 2));
	  rr.moveTopLeft(KPoint(-rr.width() / 2,-rr.height() / 2));
			
	  QWMatrix m,mtx;
	  mtx.rotate(angle);
	  m.translate(pw / 2,ph / 2);
	  m = mtx * m;
			
	  _painter->setWorldMatrix(m);
			
	  _painter->setPen(QPen(shadowColor));
	  _painter->setBrush(shadowColor);

	  KSize bs = pixmap->size();
	  int dx = 0,dy = 0;
	  getShadowCoords(dx,dy,shadowDirection,shadowDistance);
	  _painter->drawRect(rr.left() + pixXPos + dx,rr.top() + pixYPos + dy,
			     bs.width(),bs.height());
			
	  _painter->setViewport(r);
	}
    }
  _painter->restore();
  _painter->save();

  if (angle == 0)
    _painter->drawPixmap(ox,oy,*pixmap);
  else
    {
      r = _painter->viewport();
      _painter->setViewport(ox,oy,r.width(),r.height());
      
      KRect br = pixmap->rect();
      int pw = br.width();
      int ph = br.height();
      KRect rr = br;
      int pixYPos = -rr.y();
      int pixXPos = -rr.x();
      br.moveTopLeft(KPoint(-br.width() / 2,-br.height() / 2));
      rr.moveTopLeft(KPoint(-rr.width() / 2,-rr.height() / 2));
      
      QWMatrix m,mtx;
      mtx.rotate(angle);
      m.translate(pw / 2,ph / 2);
      m = mtx * m;
      
      _painter->setWorldMatrix(m);
		    
      _painter->drawPixmap(rr.left() + pixXPos,rr.top() + pixYPos,*pixmap);
      
      _painter->setViewport(r);
    }
  _painter->restore();

  KPObject::draw(_painter,_diffx,_diffy);
}




