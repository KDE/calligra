/******************************************************************/ 
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Image Collection (header)                              */
/******************************************************************/

#ifndef imagecollection_h
#define imagecollection_h

#include "image.h"

#include <qdict.h>
#include <qfont.h>
#include <qstring.h>
#include <qcolor.h>
#include <qsize.h>

class KWordDocument;

/******************************************************************/
/* Class: KWImageCollection                                       */
/******************************************************************/

class KWImageCollection
{
public:
  KWImageCollection(KWordDocument *_doc);
  ~KWImageCollection();

  KWImage *getImage(KWImage &_image,QString &key);
  KWImage *getImage(KWImage &_image,QString &key,QSize _imgSize);
  void removeImage(KWImage *_image);

  QString generateKey(KWImage *_image)
    { return generateKey(*_image); }
  
  KWImage *findImage(QString _key);

protected:
  QString generateKey(KWImage &_image);
  QString generateKey(KWImage &_image,QSize _imgSize);
  KWImage *insertImage(QString _key,KWImage &_format);
  KWImage *insertImage(QString _key,KWImage &_format,QSize _imgSize);

  QDict<KWImage> images;
  KWordDocument *doc;

};

#endif
