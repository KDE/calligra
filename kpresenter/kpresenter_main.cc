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
/* Module: KPresenter Application                                 */
/******************************************************************/

#include "kpresenter_main.h"
#include "kpresenter_main.moc"
#include "kpresenter_doc.h"
#include <string.h>
#include <koScanParts.h>
#include <koFactory.h>
#include <koDocument.h>
#include <opAutoLoader.h>
#include "kpresenter_shell.h"
#include <koIMR.h>
#include "formats.h"

bool g_bWithGUI = true;

list<string> g_openFiles;

KOFFICE_DOCUMENT_FACTORY( KPresenterDoc, KPresenterFactory )
typedef OPAutoLoader<KPresenterFactory> KPresenterAutoLoader;

/******************************************************************/
/* class KPresenterApp - KPresenter Application                   */
/******************************************************************/

/*====================== constrcutor =============================*/
KPresenterApp::KPresenterApp( int &argc, char** argv )
  : KoApplication(argc,argv,"kpresenter")
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
    imr_init();
    koScanParts();

    if ( g_openFiles.size() == 0 )
    {
      KPresenterShell* shell = new KPresenterShell;
      shell->show();
      shell->newDocument();
    }
    else
    {
      list<string>::iterator it = g_openFiles.begin();
      for( ; it != g_openFiles.end(); ++it )
      {
	KPresenterShell* shell = new KPresenterShell;
	shell->show();
	shell->openDocument( it->c_str(), "" );
      }
    }
  }
}

/*======================== main ==================================*/
int main(int argc,char **argv)
{
  KPresenterAutoLoader loader("IDL:KOffice/DocumentFactory:1.0", "KPresenter" );

  KPresenterApp app(argc,argv);

  FormatManager *formatMngr;
  formatMngr = new FormatManager();

  int i = 1;
  if ( strcmp( argv[i], "-s" ) == 0 || strcmp( argv[i], "--server" ) == 0 )
  {
    i++;
    g_bWithGUI = false;
  }

  for( ; i < argc; i++ )
    g_openFiles.push_back( (const char*)argv[i] );

  app.exec();

  return 0;
}

