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
/* Module: Part Object (header)                                   */
/******************************************************************/

#ifndef kppartobject_h
#define kppartobject_h

#include <qpicture.h>
#include <qwidget.h>

#include "kpobject.h"

class KPresenterChild;
class KPresenterFrame;

/******************************************************************/
/* Class: KPPartObject                                            */
/******************************************************************/

class KPPartObject : public KPObject
{
  Q_OBJECT

public:
  KPPartObject(KPresenterChild *_child);

  virtual void save(ostream& out) 
    {;}
  virtual void load(KOMLParser& parser,vector<KOMLAttrib>& lst) 
    {;}

  virtual ObjType getType()
    { return OT_PART; }

  virtual void draw(QPainter *_painter,int _diffx,int _diffy);

  virtual void activate(QWidget *_widget,int diffx,int diffy);
  virtual void deactivate();

  virtual void setSize(int _width,int _height);
  virtual void resizeBy(int _dx,int _dy);
  virtual void setOrig(int _x,int _y);
  virtual void moveBy(int _dx,int _dy);

  KPresenterFrame *getView() { return view; }
  void setView(KPresenterFrame *_view) { view = _view; }

protected:
  void paint(QPainter *_painter);

  KPresenterFrame *view;
  KPresenterChild *child;

};

#endif
