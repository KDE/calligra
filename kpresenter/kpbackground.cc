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
/* Module: Background                                             */
/******************************************************************/

#include "kpbackground.h"
#include "kpbackground.moc"

/******************************************************************/
/* Class: KPBackGround                                            */
/******************************************************************/

/*===================== constructor ==============================*/
KPBackGround::KPBackGround(KPPixmapCollection *_pixmapCollection,KPGradientCollection *_gradientCollection)
  : QObject(), backPixFilename(), backClipFilename(), backClip(), pixSize(orig_size), data()
{
  backType = BT_COLOR; 
  backView = BV_CENTER;
  backColor1 = white;
  backColor2 = white;
  bcType = BCT_PLAIN;
  pageEffect = PEF_NONE;

  pixmapCollection = _pixmapCollection;
  gradientCollection = _gradientCollection;
  backPix = 0;
  gradient = 0;
}

/*======================== set backpix filename ==================*/
void KPBackGround::setBackPixFilename(QString _filename)
{
  if (_filename.isEmpty() || _filename == backPixFilename) return;

  if (!backPixFilename.isEmpty())
    pixmapCollection->removeRef(backPixFilename,pixSize);

  backPixFilename = _filename;

  switch (backView)
    {
    case BV_ZOOM: pixSize = QSize(ext.width(),ext.height());
      break;
    case BV_TILED: case BV_CENTER: pixSize = orig_size;
      break;
    }

  backPix = pixmapCollection->getPixmap(backPixFilename,pixSize,data);
  pixSize = backPix->size();
}

/*========================== set backpix  ========================*/
void KPBackGround::setBackPix(QString _filename,QString _data)
{
  if (_filename.isEmpty() || _data.isEmpty() || _filename == backPixFilename) return;

  if (!backPixFilename.isEmpty())
    pixmapCollection->removeRef(backPixFilename,pixSize);

  backPixFilename = _filename;

  switch (backView)
    {
    case BV_ZOOM: pixSize = QSize(ext.width(),ext.height());
      break;
    case BV_TILED: case BV_CENTER: pixSize = orig_size;
      break;
    }

  data = _data;
  backPix = pixmapCollection->getPixmap(backPixFilename,_data,pixSize);
  pixSize = backPix->size();
}

/*========================= set backclip filename ================*/
void KPBackGround::setBackClipFilename(QString _filename)
{ 
  if (_filename.isEmpty()) return;

  backClipFilename = _filename;
  backClip.setClipartName(_filename);
}

/*========================== draw ================================*/
void KPBackGround::draw(QPainter *_painter,QPoint _offset,bool _drawBorders)
{
  _painter->save();
  QRect r = _painter->viewport();

  switch (backType)
    {
    case BT_COLOR:
      {
	_painter->setViewport(_offset.x(),_offset.y(),r.width(),r.height());
	drawBackColor(_painter);
      } break;
    case BT_PICTURE:
      {
	_painter->setViewport(_offset.x(),_offset.y(),r.width(),r.height());
	if (backView == BV_CENTER) drawBackColor(_painter);
	drawBackPix(_painter);
      } break;
    case BT_CLIPART:
      {
	_painter->setViewport(_offset.x(),_offset.y(),r.width(),r.height());
	drawBackColor(_painter);
	_painter->setViewport(r);
	_painter->save();
	_painter->setViewport(_offset.x(),_offset.y(),ext.width(),ext.height());
	drawBackClip(_painter);
	_painter->restore();
      } break;
    }

  if (_drawBorders)
    {
      _painter->setViewport(_offset.x(),_offset.y(),r.width(),r.height());
      drawBorders(_painter);
    }

  _painter->setViewport(r);
  _painter->restore();
}

/*========================== restore =============================*/
void KPBackGround::restore()
{
  if (backType == BT_PICTURE)
    {
      if (backView == BV_ZOOM)
	{
	  if (!backPixFilename.isEmpty() && !data.isEmpty())
	    {
	      if (backPix)
		{
		  backPix = pixmapCollection->getPixmap(backPixFilename,data,pixSize,true,false);
		  QString _data = data;
		  QSize _pixSize = pixSize;
		  pixSize = ext;
		  backPix = pixmapCollection->getPixmap(backPixFilename,data,backPix,pixSize);
		  pixmapCollection->removeRef(backPixFilename,_data,_pixSize);
		}
	      else
		{
		  pixSize = ext;
		  backPix = pixmapCollection->getPixmap(backPixFilename,data,pixSize);
		}
	    }
	}
      else
	{
	  if (!backPixFilename.isEmpty() && !data.isEmpty())
	    {
	      if (backPix)
		{
		  QString _data = data;
		  QSize _pixSize = pixSize;
		  pixSize = orig_size;
		  backPix = pixmapCollection->getPixmap(backPixFilename,data,pixSize,true);
		  pixSize = backPix->size();
		  pixmapCollection->removeRef(backPixFilename,_data,_pixSize);
		}
	      else
		{
		  pixSize = orig_size;
		  backPix = pixmapCollection->getPixmap(backPixFilename,data,pixSize);
		  pixSize = backPix->size();
		}
	    }
	}
    }

  if (backType != BT_PICTURE && backPix)
    {
      pixmapCollection->removeRef(backPixFilename,data,pixSize);
      backPix = 0;
    }
      
  if (backType == BT_COLOR || backType == BT_CLIPART || backType == BT_PICTURE && backView == BV_CENTER)
    {
      if (gradient)
	{
	  gradientCollection->removeRef(backColor1,backColor2,bcType,ext);
	  gradient = 0;
	}
      gradient = gradientCollection->getGradient(backColor1,backColor2,bcType,ext);
    }

  if (backType == BT_PICTURE && backView != BV_CENTER && gradient)
    {
      gradientCollection->removeRef(backColor1,backColor2,bcType,ext);
      gradient = 0;
    }
}

/*========================== save ================================*/
void KPBackGround::save(ostream& out)
{
  out << indent << "<BACKTYPE value=\"" << static_cast<int>(backType) << "\"/>" << endl; 
  out << indent << "<BACKVIEW value=\"" << static_cast<int>(backView) << "\"/>" << endl; 
  out << indent << "<BACKCOLOR1 red=\"" << backColor1.red() << "\" green=\"" 
      << backColor1.green() << "\" blue=\"" << backColor1.blue() << "\"/>" << endl; 
  out << indent << "<BACKCOLOR2 red=\"" << backColor2.red() << "\" green=\"" 
      << backColor2.green() << "\" blue=\"" << backColor2.blue() << "\"/>" << endl; 
  out << indent << "<BCTYPE value=\"" << static_cast<int>(bcType) << "\"/>" << endl; 

  if (!backPixFilename.isEmpty() && backType == BT_PICTURE)
    {
      QString _data;
      pixmapCollection->getPixmap(backPixFilename,pixSize,_data);
      pixmapCollection->removeRef(backPixFilename,pixSize);

      out << indent << "<BACKPIX filename=\"" << backPixFilename << "\" data=\"" 
	  << _data << "\"/>" << endl;
    }

  if (!backClipFilename.isEmpty() && backType == BT_CLIPART)
    out << indent << "<BACKCLIP filename=\"" << backClipFilename << "\"/>" << endl; 
  
  out << indent << "<PGEFFECT value=\"" << static_cast<int>(pageEffect) << "\"/>" << endl; 
}

/*========================== load ================================*/
void KPBackGround::load(KOMLParser& parser,vector<KOMLAttrib>& lst)
{
  string name;
  string tag;


  while (parser.open(0L,tag))
    {
      KOMLParser::parseTag(tag.c_str(),name,lst);
      
      // backtype
      if (name == "BACKTYPE")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "value")
		setBackType((BackType)atoi((*it).m_strValue.c_str()));
	    }
	}
      
      // pageEffect
      else if (name == "PGEFFECT")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "value")
		setPageEffect((PageEffect)atoi((*it).m_strValue.c_str()));
	    }
	}
      
      // backview
      else if (name == "BACKVIEW")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "value")
		setBackView((BackView)atoi((*it).m_strValue.c_str()));
	    }
	}
      
      // backcolor 1
      else if (name == "BACKCOLOR1")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "red")
		setBackColor1(QColor(atoi((*it).m_strValue.c_str()),
				     backColor1.green(),backColor1.blue()));
	      if ((*it).m_strName == "green")
		setBackColor1(QColor(backColor1.red(),
				    atoi((*it).m_strValue.c_str()),backColor1.blue()));
	      if ((*it).m_strName == "blue")
		setBackColor1(QColor(backColor1.red(),backColor1.green(),
				     atoi((*it).m_strValue.c_str())));
	    }
	}
      
      // backcolor 2
      else if (name == "BACKCOLOR2")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "red")
		setBackColor2(QColor(atoi((*it).m_strValue.c_str()),
				    backColor2.green(),backColor2.blue()));
	      if ((*it).m_strName == "green")
		setBackColor2(QColor(backColor2.red(),
				     atoi((*it).m_strValue.c_str()),backColor2.blue()));
	      if ((*it).m_strName == "blue")
		setBackColor2(QColor(backColor2.red(),backColor2.green(),
				     atoi((*it).m_strValue.c_str())));
	    }
	}
      
      // backColorType
      else if (name == "BCTYPE")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "value")
		setBackColorType((BCType)atoi((*it).m_strValue.c_str()));
	    }
	}
      
      // backpic
      else if (name == "BACKPIX")
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
	    setBackPix(_fileName,_data);
	  else
	    setBackPixFilename(_fileName);
	}
      
      // backclip
      else if (name == "BACKCLIP")
	{
	  KOMLParser::parseTag(tag.c_str(),name,lst);
	  vector<KOMLAttrib>::const_iterator it = lst.begin();
	  for(;it != lst.end();it++)
	    {
	      if ((*it).m_strName == "filename")
		{
		  QString _fileName = (*it).m_strValue.c_str();
		  if (!_fileName.isEmpty())
		    {
		      if (int _envVarB = _fileName.find('$') >= 0)
			{
			  int _envVarE = _fileName.find('/',_envVarB);
			  QString path = static_cast<const char*>(getenv(_fileName.mid(_envVarB,_envVarE-_envVarB).data()));
			  _fileName.replace(_envVarB-1,_envVarE-_envVarB+1,path);
			}
		    }
		  setBackClipFilename(_fileName);
		}
	    }
	}
      
      else
	cerr << "Unknown tag '" << tag << "' in PAGE" << endl;    
      
      if (!parser.close(tag))
	{
	  cerr << "ERR: Closing Child" << endl;
	  return;
	}
    }
}

/*========================== draw ================================*/
void KPBackGround::drawBackColor(QPainter *_painter)
{
  if (gradient)
    _painter->drawPixmap(0,0,*gradient);
}

/*========================== draw ================================*/
void KPBackGround::drawBackPix(QPainter *_painter)
{
  if (backPix)
    {  
      switch (backView)
	{
	case BV_ZOOM:
	  _painter->drawPixmap(0,0,*backPix);
	  break;
	case BV_TILED:
	  _painter->drawTiledPixmap(0,0,ext.width(),ext.height(),*backPix);
	  break;
	case BV_CENTER:
	  {
	    QPixmap *pix = new QPixmap(ext.width(),ext.height());
	    bool delPix = true;
	    int _x = 0,_y = 0;

	    if (backPix->width() > pix->width() && backPix->height() > pix->height())
	      bitBlt(pix,0,0,backPix,backPix->width() - pix->width(),backPix->height() - pix->height(),
		     pix->width(),pix->height());
	    else if (backPix->width() > pix->width())
	      {
		bitBlt(pix,0,0,backPix,backPix->width() - pix->width(),0,
		       pix->width(),backPix->height());
		_y = (pix->height() - backPix->height()) / 2;
	      }
	    else if (backPix->height() > pix->height())
	      {	      
		bitBlt(pix,0,0,backPix,0,backPix->height() - pix->height(),
		       backPix->width(),pix->height());
		_x = (pix->width() - backPix->width()) / 2;
	      }
	    else
	      {
		_x = (pix->width() - backPix->width()) / 2;
		_y = (pix->height() - backPix->height()) / 2;
		delPix = false;
		delete pix;
		pix = backPix;
	      }

	    _painter->drawPixmap(_x,_y,*pix);
	    if (delPix) delete pix;
	  }
	  break;
	}
    }
}

/*========================== draw ================================*/
void KPBackGround::drawBackClip(QPainter *_painter)
{
  _painter->drawPicture(*backClip.getPic());
}

/*========================= draw borders =========================*/
void KPBackGround::drawBorders(QPainter *_painter)
{
  QPen pen(red,1);
  QBrush brush(NoBrush);
  
  _painter->setPen(pen);
  _painter->setBrush(brush);
  _painter->drawRect(0,0,ext.width() + 1,ext.height() + 1);
}

/*======================== remove gradient =======================*/
void KPBackGround::removeGradient()
{
  if (gradient)
    {
      gradientCollection->removeRef(backColor1,backColor2,bcType,ext);
      gradient = 0;
    }
}
