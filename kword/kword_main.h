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
/* Module: Main (header)                                          */
/******************************************************************/

#ifndef kword_main_h
#define kword_main_h

#include <koApplication.h>

class KoMainWindow;

#include "kword_shell.h"

/******************************************************************/
/* Class: KWordApp                                                */
/******************************************************************/

class KWordApp : public KoApplication
{
    Q_OBJECT

public:
    KWordApp( int &argc, char** argv );
    ~KWordApp();

	KoMainWindow* createNewShell() { return new KWordShell; }
};

#endif
