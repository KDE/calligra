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
/* Module: Pixmap                                                 */
/******************************************************************/

#include "kppixmap.h"
#include "kppixmap.moc"

/******************************************************************/
/* Class: KPPixmap                                                */
/******************************************************************/

/*===================== constructor ==============================*/
KPPixmap::KPPixmap(QString _filename,QSize _size)
  : filename(_filename), pixmap(filename), orig_pixmap(pixmap), refCount(0)
{
  QFileInfo fileInfo(filename);
	
  if (fileInfo.extension().lower() != "xpm")
    {
      pixmap.save("/tmp/kpresenter_tmp.xpm","XPM");
      data = load_pixmap_native_format("/tmp/kpresenter_tmp.xpm");
    }
  else
    data = load_pixmap_native_format(filename);

  if (_size != pixmap.size() && _size != orig_size)
    {
      QWMatrix m;
      m.scale(static_cast<float>(_size.width()) / static_cast<float>(pixmap.width()),
	      static_cast<float>(_size.height()) / static_cast<float>(pixmap.height()));
      pixmap = pixmap.xForm(m);
    }
}

/*==================== constructor ==============================*/
KPPixmap::KPPixmap(QString _filename,QString _data,QSize _size)
  : filename(_filename), data(_data), refCount(0)
{
  pixmap = native_string_to_pixmap(data);
  orig_pixmap = pixmap;

  if (_size != pixmap.size() && _size != orig_size)
    {
      QWMatrix m;
      m.scale(static_cast<float>(_size.width()) / static_cast<float>(pixmap.width()),
	      static_cast<float>(_size.height()) / static_cast<float>(pixmap.height()));
      pixmap = pixmap.xForm(m);
    }
}

/*==================== constructor ==============================*/
KPPixmap::KPPixmap(QString _filename,QString _data,QPixmap *_pixmap,QSize _size)
  : filename(_filename), data(_data), refCount(0)
{
  pixmap = QPixmap(*_pixmap);
  orig_pixmap = QPixmap(*_pixmap);

  if (_size != pixmap.size() && _size != orig_size)
    {
      QWMatrix m;
      m.scale(static_cast<float>(_size.width()) / static_cast<float>(pixmap.width()),
	      static_cast<float>(_size.height()) / static_cast<float>(pixmap.height()));
      pixmap = pixmap.xForm(m);
    }
}

/*====================== add reference ==========================*/
void KPPixmap::addRef()
{
#ifdef SHOW_INFO
  debug("Refs of '%s': %d",filename.data(),++refCount);
#else
  ++refCount;
#endif
}

/*====================== remove reference =======================*/
bool KPPixmap::removeRef()
{ 
#ifdef SHOW_INFO
  debug("Refs of '%s': %d",filename.data(),--refCount); 
  return refCount == 0; 
#else
  return (--refCount == 0);
#endif
}

/*========================== convert string to pixmap ============*/
QPixmap KPPixmap::native_string_to_pixmap(const char *_pixmap)
{
  if (_pixmap == 0L || _pixmap[0] == 0)
    return QPixmap();
  
  char* pos = const_cast<char*>(&_pixmap[0]);
  
  while (*pos)
    {
      if (*pos == 1)
	*pos = '\"';
      
      pos++;
    }
  
    QPixmap ret;
    if (!_pixmap)
      return ret;
    ret.loadFromData((unsigned const char*)_pixmap,static_cast<unsigned int>(strlen(_pixmap)));
    return ret;
}

/*= load pixmap saved in KPresenter's native format - make valid =*/
QString KPPixmap::load_pixmap_native_format(const char *_file)
{
  FILE *f = fopen(_file,"r");
  if (f == 0L)
    {
      warning(" Could not open pixmap file '%s\n",_file);
      return QString();
    }
  
  char buffer[2048];
  
  QString str( "" );
  while(!feof(f))
    {
      int i = fread(buffer,1,2047,f);
      if (i > 0)
	{
	  buffer[i] = 0;
	  char* c = &buffer[0];
	  while(*c)
	    {
	      if (*c == '\"')
		*c = 1;
	      
	      c++;
	    }
	  
	  str += buffer;
	}
    }
  
  fclose(f);
  
  return str;
}
