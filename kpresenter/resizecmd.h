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
/* Module: Resize Command (header)                                */
/******************************************************************/

#ifndef resizecmd_h
#define resizecmd_h

#include <qpoint.h>
#include <qrect.h>
#include <qsize.h>

#include "command.h"
#include "kpobject.h"

class KPresenterDocument_impl;

/******************************************************************/
/* Class: ResizeCmd                                               */
/******************************************************************/

class ResizeCmd : public Command
{
  Q_OBJECT

public:
  ResizeCmd(QString _name,QPoint _m_diff,QSize _r_diff,KPObject *_object,KPresenterDocument_impl *_doc);
  ~ResizeCmd();
  
  virtual void execute();
  virtual void unexecute();

protected:
  ResizeCmd()
    {;}

  QPoint m_diff;
  QSize r_diff;
  KPObject *object;
  KPresenterDocument_impl *doc;

};

#endif
