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
/* Module: Pie Value Command                                      */
/******************************************************************/

#include "kpresenter_doc.h"
#include "pievaluecmd.h"
#include "pievaluecmd.moc"

/******************************************************************/
/* Class: PieValueCmd                                             */
/******************************************************************/

/*======================== constructor ===========================*/
PieValueCmd:: PieValueCmd(QString _name,QList<PieValues> &_oldValues,PieValues _newValues,
			  QList<KPObject> &_objects,KPresenterDoc *_doc)
  : Command(_name), oldValues(_oldValues), objects(_objects)
{
  objects.setAutoDelete(false);
  oldValues.setAutoDelete(false);
  doc = _doc;
  newValues = _newValues;

  for (unsigned int i = 0;i < objects.count();i++)
    objects.at(i)->incCmdRef();
}

/*======================== destructor ============================*/
PieValueCmd::~PieValueCmd()
{
  for (unsigned int i = 0;i < objects.count();i++)
    objects.at(i)->decCmdRef();
  oldValues.setAutoDelete(true);
  oldValues.clear();
}

/*====================== execute =================================*/
void PieValueCmd::execute()
{
  for (unsigned int i = 0;i < objects.count();i++)
    {
      dynamic_cast<KPPieObject*>(objects.at(i))->setPieType(newValues.pieType);
      dynamic_cast<KPPieObject*>(objects.at(i))->setPieAngle(newValues.pieAngle);
      dynamic_cast<KPPieObject*>(objects.at(i))->setPieLength(newValues.pieLength);
    }
  doc->repaint(false);
}

/*====================== unexecute ===============================*/
void PieValueCmd::unexecute()
{
  for (unsigned int i = 0;i < objects.count();i++)
    {
      dynamic_cast<KPPieObject*>(objects.at(i))->setPieType(oldValues.at(i)->pieType);
      dynamic_cast<KPPieObject*>(objects.at(i))->setPieAngle(oldValues.at(i)->pieAngle);
      dynamic_cast<KPPieObject*>(objects.at(i))->setPieLength(oldValues.at(i)->pieLength);
    }
  doc->repaint(false);
}
