/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Set Options Command                                    */
/******************************************************************/

#include "kpresenter_doc.h"
#include "setoptionscmd.h"
#include "setoptionscmd.moc"

/******************************************************************/
/* Class: SetOptionsCmd                                           */
/******************************************************************/

/*======================== constructor ===========================*/
SetOptionsCmd::SetOptionsCmd(QString _name,QList<QPoint> &_diffs,QList<KPObject> &_objects,int _xRnd,int _yRnd,
			     int _rastX,int _rastY,int _oxRnd,int _oyRnd,int _orastX,int _orastY,
			     QColor _txtBackCol,QColor _txtSelCol,QColor _otxtBackCol,QColor _otxtSelCol,KPresenterDocument_impl *_doc)
  : Command(_name), diffs(_diffs), objects(_objects), txtBackCol(_txtBackCol), txtSelCol(_txtSelCol),
    otxtBackCol(_otxtBackCol), otxtSelCol(_otxtSelCol)
{
  xRnd = _xRnd;
  yRnd = _yRnd;
  rastX = _rastX;
  rastY = _rastY;
  oxRnd = _oxRnd;
  oyRnd = _oyRnd;
  orastX = _orastX;
  orastY = _orastY;
  doc = _doc;
  for (unsigned int i = 0;i < objects.count();i++)
    objects.at(i)->incCmdRef();
}

/*======================== destructor ============================*/
SetOptionsCmd::~SetOptionsCmd()
{
  for (unsigned int i = 0;i < objects.count();i++)
    objects.at(i)->decCmdRef();
}

/*====================== execute =================================*/
void SetOptionsCmd::execute()
{
  for (unsigned int i = 0;i < objects.count();i++)
    {
      objects.at(i)->moveBy(*diffs.at(i));

      if (objects.at(i)->getType() == OT_RECT && dynamic_cast<KPRectObject*>(objects.at(i))->getRectType() == RT_ROUND)
	dynamic_cast<KPRectObject*>(objects.at(i))->setRnds(xRnd,yRnd);
    }

  doc->setRasters(rastX,rastY,false);
  doc->setRnds(xRnd,yRnd,false);
  doc->setTxtBackCol(txtBackCol);
  doc->setTxtSelCol(txtSelCol);
  doc->repaint(false);
}

/*====================== unexecute ===============================*/
void SetOptionsCmd::unexecute()
{
  for (unsigned int i = 0;i < objects.count();i++)
    {
      objects.at(i)->moveBy(-diffs.at(i)->x(),-diffs.at(i)->y());
 
      if (objects.at(i)->getType() == OT_RECT && dynamic_cast<KPRectObject*>(objects.at(i))->getRectType() == RT_ROUND)
	dynamic_cast<KPRectObject*>(objects.at(i))->setRnds(oxRnd,oyRnd);
    }

  doc->setRasters(orastX,orastY,false);
  doc->setRnds(oxRnd,oyRnd,false);
  doc->setTxtBackCol(otxtBackCol);
  doc->setTxtSelCol(otxtSelCol);
  doc->repaint(false);
}
