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

class KWordDocument_impl;

/******************************************************************/
/* Class: KWImageCollection                                       */
/******************************************************************/

class KWImageCollection
{
public:
  KWImageCollection(KWordDocument_impl *_doc);
  ~KWImageCollection();

  KWImage *getImage(KWImage &_image,QString &key);
  void removeImage(KWImage *_image);

  QString generateKey(KWImage *_image)
    { return generateKey(*_image); }
  
  KWImage *findImage(QString _key);

protected:
  QString generateKey(KWImage &_image);
  KWImage *insertImage(QString _key,KWImage &_format);

  QDict<KWImage> images;
  KWordDocument_impl *doc;

};

#endif
