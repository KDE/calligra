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
/* Module: Set PageConfig Command (header)                        */
/******************************************************************/

#ifndef pgconfcmd_h
#define pgconfcmd_h

#include "command.h"
#include "kpbackground.h"

#include "global.h"

class KPresenterDocument_impl;

/******************************************************************/
/* Class: PgConfCmd                                               */
/******************************************************************/

class PgConfCmd : public Command
{
  Q_OBJECT

public:
  PgConfCmd(QString _name,bool _manualSwitch,bool _infinitLoop,
	    PageEffect _pageEffect,PresSpeed _presSpeed,
	    bool _oldManualSwitch,bool _oldInfinitLoop,
	    PageEffect _oldPageEffect,PresSpeed _oldPresSpeed,
	    KPresenterDocument_impl *_doc,int _pgNum);
  
  virtual void execute();
  virtual void unexecute();

protected:
  PgConfCmd()
    {;}

  bool manualSwitch,oldManualSwitch;
  bool infinitLoop,oldInfinitLoop;
  PageEffect pageEffect,oldPageEffect;
  PresSpeed presSpeed,oldPresSpeed;
  int pgNum;
  KPresenterDocument_impl *doc;

};

#endif
