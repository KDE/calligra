/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998                   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* KPresenter is under GNU GPL                                    */
/******************************************************************/
/* Module: KPresenter Application (header)                        */
/******************************************************************/

#ifndef __kpresenter_main_h__
#define __kpresenter_main_h__

class PartShell_impl;
class KPresenterApp;
class KPresenterDocument_impl;

#include <op_app.h>
#include <part_shell_impl.h>
#include <parts.h>

#include "kpresenter_doc.h"
#include "kpresenter_shell.h"
#include "formats.h"

/******************************************************************/
/* class KPresenterApp - KPresenter Application                   */
/******************************************************************/
class KPresenterApp : public OPApplication
{
  Q_OBJECT

public:

  // constructor - destructor
  KPresenterApp(int argc,char** argv);
  ~KPresenterApp();
  
  // start application
  virtual void start();
  
};

#endif
