/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer 1997-1998                   */
/* based on Torben Weis' KWord                                    */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
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
#include <string.h>
#include <factory_impl.h>
#include <koScanParts.h>
#include "kword_main.moc"

bool g_bWithGUI = true;

FACTORY(KWordDocument_impl,KWord::Factory_skel,KWordFactory)
typedef AutoLoader<KWordFactory> KWordAutoLoader;

/******************************************************************/
/* Class: KWordApp                                                */
/******************************************************************/

/*================================================================*/
KWordApp::KWordApp(int argc,char** argv) 
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
  koScanParts();

  // Are we going to create a GUI ?
  if (g_bWithGUI)
    {
      KWordShell_impl* m_pShell;
      // Create a GUI
      m_pShell = new KWordShell_impl;
      // Allow status/menu/toolbars
      m_pShell->enableMenuBar();
      m_pShell->PartShell_impl::enableStatusBar();
      m_pShell->enableToolBars();
      // Display
      m_pShell->show();
    }
}

/*================================================================*/
int main(int argc,char **argv)
{
  // Parse command line parameters.
  for(int i = 1;i < argc;i++)
    {
      // Are we started as server? => Someones wants to embed us
      // => We dont create a shell
      if (strcmp(argv[i],"-s") == 0 || strcmp(argv[i],"--server") == 0)
	g_bWithGUI = false;
    }
  
  // Publish our factory
  KWordAutoLoader loader("IDL:KWord/Factory:1.0");

  // Lets rock
  KWordApp app(argc,argv);
  app.exec();
  
  return 0;
}
