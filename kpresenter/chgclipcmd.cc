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
/* Module: Change Clipart Command                                 */
/******************************************************************/

#include "kpresenter_doc.h"
#include "chgclipcmd.h"
#include "chgclipcmd.moc"

/******************************************************************/
/* Class: ChgClipCmd                                              */
/******************************************************************/

/*======================== constructor ===========================*/
ChgClipCmd::ChgClipCmd(QString _name,KPClipartObject *_object,QString _oldName,QString _newName,KPresenterDoc *_doc)
  : Command(_name), oldName(_oldName), newName(_newName)
{
  object = _object;
  doc = _doc;
  object->incCmdRef();
}

/*======================== destructor ============================*/
ChgClipCmd::~ChgClipCmd()
{
  object->decCmdRef();
}

/*======================== execute ===============================*/
void ChgClipCmd::execute()
{
  object->setFileName(newName);
  doc->repaint(object);
}

/*====================== unexecute ===============================*/
void ChgClipCmd::unexecute()
{
  object->setFileName(oldName);
  doc->repaint(object);
}

