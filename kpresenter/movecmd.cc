/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Move Command                                           */
/******************************************************************/

#include "kpresenter_doc.h"
#include "movecmd.h"
#include "movecmd.moc"

/******************************************************************/
/* Class: MoveByCmd                                               */
/******************************************************************/

/*======================== constructor ===========================*/
MoveByCmd::MoveByCmd(QString _name,QPoint _diff,QList<KPObject> &_objects,KPresenterDocument_impl *_doc)
  : Command(_name), diff(_diff), objects(_objects)
{
  objects.setAutoDelete(false);
  doc = _doc;
  for (unsigned int i = 0;i < objects.count();i++)
    objects.at(i)->incCmdRef();
}

/*======================== destructor ============================*/
MoveByCmd::~MoveByCmd()
{
  for (unsigned int i = 0;i < objects.count();i++)
    objects.at(i)->decCmdRef();
}

/*====================== execute =================================*/
void MoveByCmd::execute()
{
  QRect oldRect;

  for (unsigned int i = 0;i < objects.count();i++)
    {
      oldRect = objects.at(i)->getBoundingRect(0,0);
      objects.at(i)->moveBy(diff);
      doc->repaint(oldRect);
      doc->repaint(objects.at(i));
    }
}

/*====================== unexecute ===============================*/
void MoveByCmd::unexecute()
{
  QRect oldRect;

  for (unsigned int i = 0;i < objects.count();i++)
    {
      oldRect = objects.at(i)->getBoundingRect(0,0);
      objects.at(i)->moveBy(-diff.x(),-diff.y());
      doc->repaint(oldRect);
      doc->repaint(objects.at(i));
    }
}

/******************************************************************/
/* Class: MoveByCmd2                                              */
/******************************************************************/

/*======================== constructor ===========================*/
MoveByCmd2::MoveByCmd2(QString _name,QList<QPoint> &_diffs,QList<KPObject> &_objects,KPresenterDocument_impl *_doc)
  : Command(_name), diffs(_diffs), objects(_objects)
{
  objects.setAutoDelete(false);
  diffs.setAutoDelete(true);
  doc = _doc;
  for (unsigned int i = 0;i < objects.count();i++)
    objects.at(i)->incCmdRef();
}

/*======================== destructor ============================*/
MoveByCmd2::~MoveByCmd2()
{
  for (unsigned int i = 0;i < objects.count();i++)
    objects.at(i)->decCmdRef();

  diffs.clear();
}

/*====================== execute =================================*/
void MoveByCmd2::execute()
{
  QRect oldRect;

  for (unsigned int i = 0;i < objects.count();i++)
    {
      oldRect = objects.at(i)->getBoundingRect(0,0);
      objects.at(i)->moveBy(*diffs.at(i));
      doc->repaint(oldRect);
      doc->repaint(objects.at(i));
    }
}

/*====================== unexecute ===============================*/
void MoveByCmd2::unexecute()
{
  QRect oldRect;

  for (unsigned int i = 0;i < objects.count();i++)
    {
      oldRect = objects.at(i)->getBoundingRect(0,0);
      objects.at(i)->moveBy(-diffs.at(i)->x(),-diffs.at(i)->y());
      doc->repaint(oldRect);
      doc->repaint(objects.at(i));
    }
}
