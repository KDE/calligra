/***************************************************************************
                          main.cpp  -  description                              
                             -------------------                                         
    begin                : Mið Júl  7 17:04:49 CEST 1999
                                           
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

#include <opAutoLoader.h>

#include <koFactory.h>
#include <koDocument.h>
#include <koQueryTypes.h>
#include <koApplication.h>

#include "ktables_doc.h"
#include "ktables_main.h"
#include "ktables_shell.h"
#include "ktables.h"

KOFFICE_DOCUMENT_FACTORY( KtablesDoc, KtablesFactory, Ktables::DocumentFactory_skel )
typedef OPAutoLoader<KtablesFactory> KtablesAutoLoader;

int main(int argc, char* argv[]) {
  KtablesAutoLoader loader( "IDL:Ktables/DocumentFactory:1.0","Ktables" );

  KtablesMain app(argc,argv);

  app.exec();

  return 0;
}  
 

























