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
/* Module: Image (header)                                         */
/******************************************************************/

#ifndef image_h
#define image_h

#include <qimage.h>
#include <qstring.h>

#include <iostream>
#include <koStream.h>

class KWordDocument;

/******************************************************************/
/* Class: KWImage                                                 */
/******************************************************************/

class KWImage : public QImage
{
public:
  KWImage(KWordDocument *_doc,QString _filename) : QImage(_filename)
    { ref = 0; doc = _doc; filename = _filename; }
  KWImage(KWordDocument *_doc,KWImage _image) : QImage((QImage)_image)
    { ref = 0; filename = _image.getFilename(); doc = _doc; }
  KWImage() : QImage()
    { ref = 0; doc = 0L; }

  void setDocument(KWordDocument *_doc)
    { doc = _doc; }

  void incRef();
  void decRef();
  int refCount()
    { return ref; }
  
  QString getFilename()
    { return filename; }

  void save(ostream &out);
  void load(KOMLParser&,vector<KOMLAttrib>&,KWordDocument*);
  
protected:
  int ref;
  KWordDocument *doc;
  QString filename;

};

#endif
