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

#include "ktables.h"
 
int main( int argc, char* argv[] )
{ 
  KOMApplication app( argc, argv, "Ktables" );

  // TODO: KoApplication should be used.
  // TODO: Restoring and loading of files is included in KoApplication
 
  if( app.isRestored() )
  { 
    RESTORE( KtablesApp );
  }
  else 
  {
    KtablesApp* ktables = new KtablesApp;
    ktables->show();
    if( argc > 1 )
    {
      ktables->openDocumentFile( argv[1] );
    }
  }
  
  app.exec();

  return 0;
}
  
