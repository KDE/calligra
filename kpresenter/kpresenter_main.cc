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
/* Module: KPresenter Application                                 */
/******************************************************************/

#include "kpresenter_main.h"
#include "kpresenter_main.moc"

/******************************************************************/
/* class KPresenterApp - KPresenter Application                   */
/******************************************************************/

/*====================== constrcutor =============================*/
KPresenterApp::KPresenterApp(int argc,char** argv)
  : OPApplication(argc,argv)
{
  m_pShell = 0L;
}

/*====================== destrcutor ==============================*/
KPresenterApp::~KPresenterApp()
{
}

/*=================== start application ==========================*/
void KPresenterApp::start()
{
  printf("Started\n");
  
  m_pShell = new KPresenterShell_impl;
  m_pShell->enableMenuBar();
  m_pShell->PartShell_impl::enableStatusBar();
  m_pShell->enableToolBars();
  m_pShell->show();
}

/*======================== main ==================================*/
int main(int argc,char **argv)
{
  FormatManager *formatMngr;
  formatMngr = new FormatManager();

  KPresenterApp app(argc,argv);
  app.exec();

  return 0;
}
