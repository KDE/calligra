#include <qprinter.h>
#include <koQueryTypes.h>
#include <koIMR.h>
#include <koFactory.h>
#include <koDocument.h>
#include <opAutoLoader.h>
#include <koApplication.h>

#include "kformula_shell.h"
#include "kformula_doc.h"
#include "kformula_main.h"

// DEBUG
#include <iostream>

KOFFICE_DOCUMENT_FACTORY( KFormulaDoc, KFormulaFactory, KFormula::DocumentFactory_skel )
typedef OPAutoLoader<KFormulaFactory> KFormulaAutoLoader;

KFormulaApp::KFormulaApp( int &argc, char** argv ) : 
  KoApplication( argc, argv, "kformula" )
{
}

KFormulaApp::~KFormulaApp()
{
}

int main( int argc, char **argv )
{
  KFormulaAutoLoader loader( "IDL:KFormula/DocumentFactory:1.0", "KFormula" );

  KFormulaApp app( argc, argv );

  app.exec();

  cerr << "============ BACK from event loop ===========" << endl;

  return 0;
}

#include "kformula_main.moc"
