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
/* Module: Delete Command                                         */
/******************************************************************/

#include "kpresenter_doc.h"
#include "deletecmd.h"
#include "deletecmd.moc"

/******************************************************************/
/* Class: DeleteCmd                                               */
/******************************************************************/

/*======================== constructor ===========================*/
DeleteCmd::DeleteCmd(QString _name,KPObject *_object,KPresenterDocument_impl *_doc)
  : Command(_name)
{
  object = _object;
  doc = _doc;
  object->incCmdRef();
}

/*======================== destructor ============================*/
DeleteCmd::~DeleteCmd()
{
  object->decCmdRef();
}

/*======================== execute ===============================*/
void DeleteCmd::execute()
{
  QRect oldRect = object->getBoundingRect(0,0);
  if (doc->objectList()->findRef(object) != -1)
    {
      doc->objectList()->take(doc->objectList()->findRef(object));
      object->removeFromObjList();
    }
  doc->repaint(oldRect);
}

/*====================== unexecute ===============================*/
void DeleteCmd::unexecute()
{
  doc->objectList()->append(object);
  object->addToObjList();
  doc->repaint(object);
}


