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
/* Module: Change Pixmap Command                                  */
/******************************************************************/

#include "kpresenter_doc.h"
#include "chgpixcmd.h"
#include "chgpixcmd.moc"

/******************************************************************/
/* Class: ChgPixCmd                                               */
/******************************************************************/

/*======================== constructor ===========================*/
ChgPixCmd::ChgPixCmd(QString _name,KPPixmapObject *_oldObject,KPPixmapObject *_newObject,
		     KPresenterDocument_impl *_doc)
  : Command(_name)
{
  oldObject = _oldObject;
  newObject = _newObject;
  doc = _doc;
  oldObject->incCmdRef();
  newObject->incCmdRef();
  newObject->setSize(oldObject->getSize());
  newObject->setOrig(oldObject->getOrig());
}

/*======================== destructor ============================*/
ChgPixCmd::~ChgPixCmd()
{
  oldObject->decCmdRef();
  newObject->decCmdRef();
}

/*======================== execute ===============================*/
void ChgPixCmd::execute()
{
  unsigned int pos = doc->objectList()->findRef(oldObject);
  doc->objectList()->take(pos);
  doc->objectList()->insert(pos,newObject);
  doc->repaint(newObject);
}

/*====================== unexecute ===============================*/
void ChgPixCmd::unexecute()
{
  unsigned int pos = doc->objectList()->findRef(newObject);
  doc->objectList()->take(pos);
  doc->objectList()->insert(pos,oldObject);
  doc->repaint(oldObject);
}

