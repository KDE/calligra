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
  brush = NoBrush;
  gradient = 0;
  fillType = FT_BRUSH;
  gType = BCT_GHORZ;
  redrawPix = false;
  pen = QPen(black,1,NoPen);
  gColor1 = red;
  gColor2 = green;
}

/*================== overloaded constructor ======================*/
KPPixmapObject::KPPixmapObject(KPPixmapCollection *_pixmapCollection,QString _filename)
  : KPObject(), data()
{
  pixmapCollection = _pixmapCollection;
  pixmap = 0;
  ext = orig_size;
  setFileName(_filename);
  brush = NoBrush;
  gradient = 0;
  fillType = FT_BRUSH;
  gType = BCT_GHORZ;
  redrawPix = false;
  pen = QPen(black,1,NoPen);
  gColor1 = red;
  gColor2 = green;
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

  if (fillType == FT_GRADIENT && gradient)
    {
      gradient->setSize(getSize());
      redrawPix = true;
      pix.resize(getSize());
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

  if (fillType == FT_GRADIENT && gradient)
    {
      gradient->setSize(getSize());
      redrawPix = true;
      pix.resize(getSize());
    }
}

/*================================================================*/
void KPPixmapObject::setFillType(FillType _fillType)
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

  if (fillType == FT_GRADIENT && gradient)
    {
      gradient->setSize(getSize());
      redrawPix = true;
      pix.resize(getSize());
    }
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

  if (fillType == FT_GRADIENT && gradient)
    {
      gradient->setSize(getSize());
      redrawPix = true;
      pix.resize(getSize());
    }
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

  out << indent << "<FILLTYPE value=\"" << static_cast<int>(fillType) << "\"/>" << endl;
  out << indent << "<GRADIENT red1=\"" << gColor1.red() << "\" green1=\"" << gColor1.green()
      << "\" blue1=\"" << gColor1.blue() << "\" red2=\"" << gColor2.red() << "\" green2=\"" 
      << gColor2.green() << "\" blue2=\"" << gColor2.blue() << "\" type=\""
      << static_cast<int>(gType) << "\"/>" << endl;
  out << indent << "<PEN red=\"" << pen.color().red() << "\" green=\"" << pen.color().green()
      << "\" blue=\"" << pen.color().blue() << "\" width=\"" << pen.width()
      << "\" style=\"" << static_cast<int>(pen.style()) << "\"/>" << endl;
  out << indent << "<BRUSH red=\"" << brush.color().red() << "\" green=\"" << brush.color().green()
      << "\" blue=\"" << brush.color().blue() << "\" style=\"" << static_cast<int>(brush.style()) << "\"/>" << endl;
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
  int ow = ext.width();
  int oh = ext.height();
  KRect r;

  _painter->save();
  r = _painter->viewport();

  _painter->setPen(pen);
  _painter->setBrush(brush);

  int pw = pen.width();

  _painter->save();
  _painter->setViewport(ox,oy,r.width(),r.height());
  if (fillType == FT_BRUSH || !gradient)
    _painter->drawRect(pw,pw,ext.width() - 2 * pw,ext.height() - 2 * pw);
  else
    {
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
    }
  _painter->setViewport(r);
  _painter->restore();

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




