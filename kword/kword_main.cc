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
#include "kword_main.moc"
#include "kword_doc.h"
#include "kword_shell.h"
#include "formats.h"

#include <koFactory.h>
#include <koQueryTypes.h>
#include <koIMR.h>
#include <koDocument.h>
#include <opAutoLoader.h>
#include <koApplication.h>

#include <list>

KOFFICE_DOCUMENT_FACTORY( KWordDocument, KWordFactory, KWord::DocumentFactory_skel )
typedef OPAutoLoader<KWordFactory> KWordAutoLoader;

/******************************************************************/
/* Class: KWordApp                                                */
/******************************************************************/

/*================================================================*/
KWordApp::KWordApp( int &argc, char** argv )
    : KoApplication( argc, argv, "kword" )
{
}

/*================================================================*/
KWordApp::~KWordApp()
{
}

/*================================================================*/
int main( int argc, char **argv )
{
    FormatManager *formatMngr;
    formatMngr = new FormatManager();

    // Publish our factory
    KWordAutoLoader loader( "IDL:KWord/DocumentFactory:1.0", "KWord" );

    // Lets rock
    KWordApp app( argc, argv );

    app.exec();

    return 0;
}
