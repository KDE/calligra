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
#include <string.h>
#include <factory_impl.h>

bool g_bWithGUI = true;

FACTORY(KPresenterDocument_impl,KPresenter::Factory_skel,KPresenterFactory)
typedef AutoLoader<KPresenterFactory> KPresenterAutoLoader;

/******************************************************************/
/* class KPresenterApp - KPresenter Application                   */
/******************************************************************/

/*====================== constrcutor =============================*/
KPresenterApp::KPresenterApp(int argc,char** argv)
  : OPApplication(argc,argv)
{
}

/*====================== destrcutor ==============================*/
KPresenterApp::~KPresenterApp()
{
}

/*=================== start application ==========================*/
void KPresenterApp::start()
{
  if ( g_bWithGUI )
    {
      KPresenterShell_impl* m_pShell;
      m_pShell = new KPresenterShell_impl;
      m_pShell->enableMenuBar();
      m_pShell->PartShell_impl::enableStatusBar();
      m_pShell->enableToolBars();
      m_pShell->show();
    }
  printf("Started\n");
}

/*======================== main ==================================*/
int main(int argc,char **argv)
{
  FormatManager *formatMngr;
  formatMngr = new FormatManager();

  for(int i = 1;i < argc;i++)
  {
    if (strcmp(argv[i],"-s") == 0 || strcmp(argv[i],"--server") == 0)
      g_bWithGUI = false;
  }
  
  KPresenterAutoLoader loader("IDL:KPresenter/Factory:1.0");

  KPresenterApp app(argc,argv);
  app.exec();

  return 0;
}

