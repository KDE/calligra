/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Autoform Editor                                                */
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
/* Module: Mainwidget (header)                                    */
/******************************************************************/

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <kapp.h>
#include <kmenubar.h>
#include <ktoolbar.h>
#include <kstatusbar.h>
#include <ktopwidget.h>
#include <knewpanner.h>
#include <klocale.h>
#include <koAboutDia.h>

#include <qpopmenu.h>
#include <qtimer.h>
#include <qkeycode.h>
#include <qlist.h>
#include <qstring.h>
#include <qstrlist.h>
#include <qmsgbox.h>

#include "drawWidget.h"
#include "editWidget.h"
#include "atfinterpreter.h"
#include "pntInsDia.h"
#include "afchoose.h"
#include "saveAsDia.h"

/******************************************************************/
/* Class: AEditWin                                                */
/******************************************************************/
class AEditWin : public KTopLevelWidget
{
  Q_OBJECT
    
public:

  // constructor - destructor
  AEditWin(const char *name=0);
  ~AEditWin();
  
protected:
  
  // setup GUI
  void setupMenu();
  void setupToolbar1();
  void setupToolbar2();
  void setupStatusbar();
  void setupPanner();

  // indexes of the menuitems/toobaritems
  int M_FILE,M_EDIT,M_EXTRA,M_HELP;
  int M_FNEW,M_FOPEN,M_FSAVE,M_FSAS,M_FWIN,M_FCLOSE,M_FEXIT;
  int M_ECUT,M_ECOPY,M_EPASTE,M_EDELETE;
  int M_EINSPNT,M_E121;
  int M_HABOUTKPA,M_HABOUTKOFFICE,M_HABOUTKDE,M_HHELP;
  static const int ST_INFO  = 1;
  static const int T_RELATION = 1;

  // ********** variables **********

  // pointers to GUI-elements
  KMenuBar *menu;
  KToolBar *toolbar1;
  KStatusBar *statBar;
  QPopupMenu *file;
  QPopupMenu *edit;
  QPopupMenu *extra;
  QPopupMenu *help;
  QTimer *timer;
  KNewPanner *panner;
  DrawWidget *drawWid;
  EditWidget *editWid;

  // autoform drawn in 1:1 or not
  bool relation;

  // dialogs
  ATFInterpreter *atfInterpret;
  PntInsDia *pntInsDia;
  AFChoose *afChoose;
  QString fileName,groupName;
  SaveAsDia* saveAsDia;

private slots:

  // slots of menuitems
  void fileNew();
  void fileOpen();
  void fileSave();
  void fileSaveAs();
  void fileNewWin();
  void fileClose();
  void fileQuit();
  void editCut();
  void editCopy();
  void editPaste();
  void editDelete();
  void editSelAll();
  void extraInsertPoint();
  void extraOne2One();
  void helpHelp();
  void helpAbout();
  void helpAboutKOffice();
  void helpAboutKDE();

  // slots for statusbartext
  void setInfoText(int);
  void checkMenu();
  void clearInfoText();

  // slots vor variable/point changes
  void sendSource();
  void sendPntArry(int,int);
  void setPenWidth(int);
  void chnVar(int,int,int,QString);
  void insPntOk(int,bool);
  void delPoint(int);

  // slots for dialogs
  void afChooseOk(const char*);
  void saveAsDiaOk(const char*,const char*);

};

#endif


