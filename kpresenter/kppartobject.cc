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
/* Module: Part Object                                            */
/******************************************************************/

#include "kppartobject.h"
#include "kppartobject.moc"
#include "kpresenter_doc.h"
#include "kpresenter_view.h"

/******************************************************************/
/* Class: KPPartObject                                            */
/******************************************************************/

/*======================== constructor ===========================*/
KPPartObject::KPPartObject(KPresenterChild *_child)
  : KPObject()
{
  child = _child;
  view = 0L;
}

/*======================== draw ==================================*/
void KPPartObject::draw(QPainter *_painter,int _diffx,int _diffy)
{
  if (move) 
    {
      KPObject::draw(_painter,_diffx,_diffy);
      return;
    }

  int ox = orig.x() - _diffx;
  int oy = orig.y() - _diffy;
  int ow = ext.width();
  int oh = ext.height();
  KRect r;

  _painter->save();

  r = _painter->viewport();
  _painter->setViewport(ox,oy,r.width(),r.height());
  
  if (angle == 0)
    paint(_painter);
  else
    {
      KRect br = KRect(0,0,ow,oh);
      int pw = br.width();
      int ph = br.height();
      KRect rr = br;
      int yPos = -rr.y();
      int xPos = -rr.x();
      rr.moveTopLeft(KPoint(-rr.width() / 2,-rr.height() / 2));
      
      QWMatrix m,mtx,m2;
      mtx.rotate(angle);
      m.translate(pw / 2,ph / 2);
      m2.translate(rr.left() + xPos,rr.top() + yPos);
      m = m2 * mtx * m;
      
      _painter->setWorldMatrix(m);
      paint(_painter);
    }

  _painter->setViewport(r);
  
  _painter->restore();

  KPObject::draw(_painter,_diffx,_diffy);
}

/*================================================================*/
void KPPartObject::paint(QPainter *_painter)
{
  QPicture* pic;
  pic = child->draw();

  if (pic && !pic->isNull())
    _painter->drawPicture(*pic);
}

/*================================================================*/
void KPPartObject::activate(QWidget *_widget,int diffx,int diffy)
{
  view->setGeometry(orig.x() - diffx + 20,orig.y() - diffy + 20,ext.width(),ext.height());
  view->show();
}

/*================================================================*/
void KPPartObject::deactivate()
{
  view->hide();
}

/*================================================================*/
void KPPartObject::setSize(int _width,int _height)
{
  KPObject::setSize(_width,_height);
  child->setGeometry(KRect(orig.x(),orig.y(),ext.width(),ext.height()));
}

/*================================================================*/
void KPPartObject::resizeBy(int _dx,int _dy)
{
  KPObject::resizeBy(_dx,_dy);
  child->setGeometry(KRect(orig.x(),orig.y(),ext.width(),ext.height()));
}

/*================================================================*/
void KPPartObject::setOrig(int _x,int _y)
{
  KPObject::setOrig(_x,_y);
  child->setGeometry(KRect(orig.x(),orig.y(),ext.width(),ext.height()));
}

/*================================================================*/
void KPPartObject::moveBy(int _dx,int _dy)
{
  KPObject::moveBy(_dx,_dy);
  child->setGeometry(KRect(orig.x(),orig.y(),ext.width(),ext.height()));
}
