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
/* Module: Set options Command (header)                           */
/******************************************************************/

#ifndef setoptionscmd_h
#define setoptionscmd_h

#include <qpoint.h>
#include <qrect.h>
#include <qsize.h>
#include <qcolor.h>

#include "command.h"
#include "kpobject.h"

class KPresenterDocument_impl;

/******************************************************************/
/* Class: SetOptionsCmd                                           */
/******************************************************************/

class SetOptionsCmd : public Command
{
  Q_OBJECT

public:
  SetOptionsCmd(QString _name,QList<QPoint> &_diffs,QList<KPObject> &_objects,int _xRnd,int _yRnd,
		int _rastX,int _rastY,int _oxRnd,int _oyRnd,int _orastX,int _orastY,
		QColor _txtBackCol,QColor _txtSelCol,QColor _otxtBackCol,QColor _otxtSelCol,KPresenterDocument_impl *_doc);
  ~SetOptionsCmd();
  
  virtual void execute();
  virtual void unexecute();

protected:
  SetOptionsCmd()
    {;}

  QList<QPoint> diffs;
  QList<KPObject> objects;
  int xRnd,yRnd,rastX,rastY;
  int oxRnd,oyRnd,orastX,orastY;
  KPresenterDocument_impl *doc;
  QColor txtBackCol;
  QColor txtSelCol;
  QColor otxtBackCol;
  QColor otxtSelCol;

};

#endif
