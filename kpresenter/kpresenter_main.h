/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998                   */
/* Version: 0.1.0                                                 */
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

#include <koApplication.h>

/******************************************************************/
/* class KPresenterApp - KPresenter Application                   */
/******************************************************************/
class KPresenterApp : public KoApplication
{
  Q_OBJECT

public:

  // constructor - destructor
  KPresenterApp(int &argc,char** argv);
  ~KPresenterApp();
  
  // start application
  virtual void start();
};

#endif
