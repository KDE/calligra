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
/* Module: Insert Command                                         */
/******************************************************************/

#include "kpresenter_doc.h"
#include "insertcmd.h"
#include "insertcmd.moc"

/******************************************************************/
/* Class: InsertCmd                                               */
/******************************************************************/

/*======================== constructor ===========================*/
InsertCmd::InsertCmd(QString _name,KPObject *_object,KPresenterDocument_impl *_doc)
  : Command(_name)
{
  object = _object;
  doc = _doc;
  object->incCmdRef();
}

/*======================== destructor ============================*/
InsertCmd::~InsertCmd()
{
  object->decCmdRef();
}

/*====================== execute =================================*/
void InsertCmd::execute()
{
  doc->objectList()->append(object);
  object->addToObjList();
  doc->repaint(object);
}

/*====================== unexecute ===============================*/
void InsertCmd::unexecute()
{
  QRect oldRect = object->getBoundingRect(0,0);
  if (doc->objectList()->findRef(object) != -1)
    {
      doc->objectList()->take(doc->objectList()->findRef(object));
      object->removeFromObjList();
    }
  doc->repaint(oldRect);
}

