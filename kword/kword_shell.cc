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
/* Module: Shell                                                  */
/******************************************************************/

#include "kword_shell.h"
#include "kword_doc.h"
#include "kword_view.h"
#include "kword_factory.h"
#include <kstddirs.h>

KWordShell::KWordShell( QWidget* parent, const char* name )
    : KoMainWindow( parent, name )
{
    setDoPartActivation( FALSE );
    resize( 800, 600 );
}

KWordShell::~KWordShell()
{
}

QString KWordShell::configFile() const
{
    return readConfigFile( locate( "data", "kword/kword_shell.rc",
				   KWordFactory::global() ) );
}

KoDocument* KWordShell::createDoc()
{
    return new KWordDocument;
}

void KWordShell::setRootPart( Part *part )
{
    KoMainWindow::setRootPart( part );
    ( (KWordView*)rootView() )->initGui();
}

void KWordShell::slotFilePrint()
{
    ( (KWordView*)rootView() )->printDlg();
}

#include "kword_shell.moc"
