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
#include "kword_main.moc"
#include "kword_doc.h"

#include <koFactory.h>
#include <koQueryTypes.h>
#include <koDocument.h>
#include <opAutoLoader.h>
#include <koApplication.h>
#include <kimgio.h>
#include <kstddirs.h>
#include <kglobal.h>

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
    // Publish our factory
    KWordAutoLoader loader( "IDL:KWord/DocumentFactory:1.0", "KWord" );

    // Lets rock
    KWordApp app( argc, argv );
    kimgioRegister();

    KGlobal::dirs()->addResourceType( "kword_template",
				      KStandardDirs::kde_default("data") + 
				      "kword/templates/" );
    KGlobal::dirs()->addResourceType( "toolbar", 
				      KStandardDirs::kde_default( "data" ) + 
				      "kformula/pics/" );
    app.exec();

    return 0;
}
