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
/* Module: text object (header)                                   */
/******************************************************************/

#ifndef kptextobject_h
#define kptextobject_h

#include <qwidget.h>
#include <qpicture.h>

#include "kpobject.h"
#include "ktextobject.h"

/******************************************************************/
/* Class: KPTextObject                                            */
/******************************************************************/

class KPTextObject : public KPObject
{
  Q_OBJECT

public:
  KPTextObject();

  virtual void setSize(int _width,int _height);
  virtual void resizeBy(int _dx,int _dy);

  virtual ObjType getType()
    { return OT_TEXT; }
  virtual int getSubPresSteps()
    { return ktextobject.paragraphs() - 1; }
  virtual KTextObject* getKTextObject()
    { return &ktextobject; }

  virtual void save(ostream& out);
  virtual void load(KOMLParser& parser,vector<KOMLAttrib>& lst);

  virtual void draw(QPainter *_painter,int _diffx,int _diffy);

  virtual void activate(QWidget *_widget,int diffx,int diffy);
  virtual void deactivate();

  virtual void zoom(float _fakt);
  virtual void zoomOrig();

protected:
  virtual void saveKTextObject(ostream& out);
  virtual void loadKTextObject(KOMLParser& parser,vector<KOMLAttrib>& lst);

  KTextObject ktextobject;

};

#endif
