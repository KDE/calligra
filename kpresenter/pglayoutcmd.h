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
/* Module: Set PageLayout Command (header)                        */
/******************************************************************/

#ifndef pglayoutcmd_h
#define pglayoutcmd_h

#include <koPageLayoutDia.h>

#include "command.h"
#include "kpbackground.h"

class KPresenterView;

/******************************************************************/
/* Class: PgLayoutCmd                                             */
/******************************************************************/

class PgLayoutCmd : public Command
{
  Q_OBJECT

public:
  PgLayoutCmd(QString _name,KoPageLayout _layout,KoPageLayout _oldLayout,
	      KPresenterView *_view);

  virtual void execute();
  virtual void unexecute();

protected:
  PgLayoutCmd()
    {;}

  KoPageLayout layout,oldLayout;
  KPresenterView *view;

};

#endif
