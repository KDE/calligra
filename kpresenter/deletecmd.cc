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
/* Module: Delete Command                                         */
/******************************************************************/

#include "kpresenter_doc.h"
#include "deletecmd.h"
#include "deletecmd.moc"

/******************************************************************/
/* Class: DeleteCmd                                               */
/******************************************************************/

/*======================== constructor ===========================*/
DeleteCmd::DeleteCmd(QString _name,QList<KPObject> &_objects,KPresenterDocument_impl *_doc)
  : Command(_name), objects(_objects)
{
  objects.setAutoDelete(false);
  doc = _doc;
  for (unsigned int i = 0;i < objects.count();i++)
    objects.at(i)->incCmdRef();
}

/*======================== destructor ============================*/
DeleteCmd::~DeleteCmd()
{
  for (unsigned int i = 0;i < objects.count();i++)
    objects.at(i)->decCmdRef();
}

/*======================== execute ===============================*/
void DeleteCmd::execute()
{
  QRect oldRect;

  for (unsigned int i = 0;i < objects.count();i++)
    {
      oldRect = objects.at(i)->getBoundingRect(0,0);
      if (doc->objectList()->findRef(objects.at(i)) != -1)
	{
	  doc->objectList()->take(doc->objectList()->findRef(objects.at(i)));
	  objects.at(i)->removeFromObjList();
	}
      doc->repaint(oldRect);
      doc->repaint(objects.at(i));
    }
}

/*====================== unexecute ===============================*/
void DeleteCmd::unexecute()
{
  for (unsigned int i = 0;i < objects.count();i++)
    {
      doc->objectList()->append(objects.at(i));
      objects.at(i)->addToObjList();
      doc->repaint(objects.at(i));
    }
}


