/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998                   */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* KPresenter is under GNU GPL                                    */
/******************************************************************/
/* Module: pixmap collection                                      */
/******************************************************************/

#include "kppixmapcollection.h"
#include "kppixmapcollection.moc"

/******************************************************************/
/* Class: KPPixmapCollection                                      */
/******************************************************************/

/*======================= get Pixmap =============================*/
QPixmap* KPPixmapCollection::getPixmap(QString _filename,QSize _size,QString &_data,bool orig = false,bool addref = true)
{
  int num = inPixmapList(_filename,_size);

  if (num == -1)
    {
      KPPixmap *kppixmap = new KPPixmap(_filename,_size);
      pixmapList.append(kppixmap);
      _data = kppixmap->getPixDataNative();

#ifdef SHOW_INFO
      debug("-------------");
      debug("add Pixmap '%s'",_filename.data());
      debug("PixmapCollection count: %d",pixmapList.count());
      debug("%d: ",pixmapList.count() - 1);
#endif

      if (addref)
	kppixmap->addRef();

#ifdef SHOW_INFO
      debug("-------------");
#endif

      if (orig)
	return kppixmap->getOrigPixmap();
      else
	return kppixmap->getPixmap();
    }
  else 
    {

#ifdef SHOW_INFO
      debug("-------------");
      debug("%d: ",num);
#endif

      if (addref)
	pixmapList.at(num)->addRef();

#ifdef SHOW_INFO
      debug("-------------");
#endif

      _data = pixmapList.at(num)->getPixDataNative();
      if (orig)
	return pixmapList.at(num)->getOrigPixmap();
      else
	return pixmapList.at(num)->getPixmap();
    }
}

/*======================= get Pixmap =============================*/
QPixmap* KPPixmapCollection::getPixmap(QString _filename,QString _data,QSize _size,bool orig = false,bool addref = true)
{
  int num = inPixmapList(_filename,_data,_size);

  if (num == -1)
    {
      KPPixmap *kppixmap = new KPPixmap(_filename,_data,_size);
      pixmapList.append(kppixmap);

#ifdef SHOW_INFO
      debug("-------------");
      debug("add Pixmap '%s'",_filename.data());
      debug("PixmapCollection count: %d",pixmapList.count());
      debug("%d: ",pixmapList.count() - 1);
#endif

      if (addref)
	kppixmap->addRef();

#ifdef SHOW_INFO
      debug("-------------");
#endif

      if (orig)
	return kppixmap->getOrigPixmap();
      else
	return kppixmap->getPixmap();
    }
  else 
    {

#ifdef SHOW_INFO
      debug("%d: ",num);
#endif

      if (addref)
	pixmapList.at(num)->addRef();
      
#ifdef SHOW_INFO
      debug("-------------");
#endif

      if (orig)
	return pixmapList.at(num)->getOrigPixmap();
      else
	return pixmapList.at(num)->getPixmap();
    }
}

/*======================= get Pixmap =============================*/
QPixmap* KPPixmapCollection::getPixmap(QString _filename,QString _data,QPixmap *_pixmap,QSize _size,bool orig = false,bool addref = true)
{
  int num = inPixmapList(_filename,_data,_size);

  if (num == -1)
    {
      KPPixmap *kppixmap = new KPPixmap(_filename,_data,_pixmap,_size);
      pixmapList.append(kppixmap);

#ifdef SHOW_INFO
      debug("-------------");
      debug("add Pixmap '%s'",_filename.data());
      debug("PixmapCollection count: %d",pixmapList.count());
      debug("%d: ",pixmapList.count() - 1);
#endif

      if (addref)
	kppixmap->addRef();
      
#ifdef SHOW_INFO
      debug("-------------");
#endif

      if (orig)
	return kppixmap->getOrigPixmap();
      else
	return kppixmap->getPixmap();
    }
  else 
    {

#ifdef SHOW_INFO
      debug("%d: ",num);
#endif

      if (addref)
	pixmapList.at(num)->addRef();
      
#ifdef SHOW_INFO
      debug("-------------");
#endif

      if (orig)
	return pixmapList.at(num)->getOrigPixmap();
      else
	return pixmapList.at(num)->getPixmap();
    }
}

/*====================== remove ref =============================*/
void KPPixmapCollection::removeRef(QString _filename,QSize _size)
{
  int num = inPixmapList(_filename,_size);

  if (num != -1)
    {
      if (pixmapList.at(num)->removeRef())
	{
	  pixmapList.remove(num);

#ifdef SHOW_INFO
	  debug("remove Pixmap '%s'",_filename.data());
	  debug("PixmapCollection count: %d\n",pixmapList.count());
#endif

	}
    }
}

/*====================== remove ref =============================*/
void KPPixmapCollection::removeRef(QString _filename,QString _data,QSize _size)
{
  int num = inPixmapList(_filename,_data,_size);

  if (num != -1)
    {
      if (pixmapList.at(num)->removeRef())
	{
	  pixmapList.remove(num);

#ifdef SHOW_INFO
	  debug("remove Pixmap '%s'",_filename.data());
	  debug("PixmapCollection count: %d\n",pixmapList.count());
#endif

	}
    }
}

/*========================== in pixmap list? ====================*/
int KPPixmapCollection::inPixmapList(QString _filename,QSize _size)
{
  if (!pixmapList.isEmpty())
    {
      KPPixmap *kppixmap = 0;
      for (int i = 0;i < static_cast<int>(pixmapList.count());i++)
	{
	  kppixmap = pixmapList.at(i);
	  if (kppixmap->getFilename() == _filename && (kppixmap->getSize() == _size ||
						       _size == orig_size && 
						       kppixmap->getSize() == kppixmap->getOrigSize()))
	    return i;
	}
      return -1;
    }
  else return -1;
}

/*========================== in pixmap list? ====================*/
int KPPixmapCollection::inPixmapList(QString _filename,QString _data,QSize _size)
{
  if (!pixmapList.isEmpty())
    {
      KPPixmap *kppixmap = 0;
      for (int i = 0;i < static_cast<int>(pixmapList.count());i++)
	{
	  kppixmap = pixmapList.at(i);
	  if (kppixmap->getFilename() == _filename && (kppixmap->getPixData() == _data || kppixmap->getPixDataNative() == _data ) && 
	      (kppixmap->getSize() == _size || _size == orig_size && kppixmap->getSize() == kppixmap->getOrigSize()))
	    return i;
	}
      return -1;
    }
  else return -1;
}




