/***************************************************************************
                          ktablesmain.cpp  -  description                              
                             -------------------                                         
    begin                : Fri Jul 16 1999                                           
    copyright            : (C) 1999 by Ørn E. Hansen                         
    email                : hanseno@mail.bip.net                                     
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/


#include "ktables_main.h"
#include "ktables_shell.h"

KtablesMain::KtablesMain(int& argc, char **argv)
 : KoApplication(argc,argv,"KTables")
{
}

KtablesMain::~KtablesMain()
{
}

KoMainWindow *
KtablesMain::createNewShell()
{
	return new KtablesApp;
}

#include "ktables_main.moc"

