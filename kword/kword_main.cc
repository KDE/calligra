/******************************************************************/ 
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Main                                                   */
/******************************************************************/

#include "kword_main.h"
#include "kword_doc.h"
#include <string.h>
#include <koFactory.h>
#include <koScanParts.h>
#include <koIMR.h>
#include <koDocument.h>
#include <opAutoLoader.h>
#include "kword_main.moc"

#include <list>

bool g_bWithGUI = true;

list<string> g_openFiles;

KOFFICE_DOCUMENT_FACTORY( KWordDocument, KWordFactory )
typedef OPAutoLoader<KWordFactory> KWordAutoLoader;


/******************************************************************/
/* Class: KWordApp                                                */
/******************************************************************/

/*================================================================*/
KWordApp::KWordApp(int &argc,char** argv) 
  : OPApplication(argc,argv,"kword")
{
  getLocale()->insertCatalogue("koffice");
}

/*================================================================*/
KWordApp::~KWordApp()
{
}

/*================================================================*/
void KWordApp::start()
{
  // Are we going to create a GUI ?
  if ( g_bWithGUI )
  {
    imr_init();
    koScanParts();

    if ( g_openFiles.size() == 0 )
    {
      KWordShell* m_pShell = new KWordShell;
      m_pShell->show();
      m_pShell->newDocument();
    }
    else
    {
      list<string>::iterator it = g_openFiles.begin();
      for( ; it != g_openFiles.end(); ++it )
      {
	KWordShell* m_pShell = new KWordShell;
	m_pShell->show();
	m_pShell->openDocument( it->c_str(), "" );
      }
    }
  }
}

/*================================================================*/
int main(int argc,char **argv)
{
  FormatManager *formatMngr;
  formatMngr = new FormatManager();
  
  // Publish our factory
  KWordAutoLoader loader("IDL:KOffice/DocumentFactory:1.0");

  // Lets rock
  KWordApp app(argc,argv);

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
