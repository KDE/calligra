/***************************************************************************
                          ktablesmain.h  -  description                              
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


#ifndef KTABLESMAIN_H
#define KTABLESMAIN_H

#include <koApplication.h>

class KtablesApp;

/**
  *@author Ørn E. Hansen
  */

class KtablesMain : public KoApplication  {
	Q_OBJECT
	
public: 
	KtablesMain(int&, char **);
	~KtablesMain();
	
	virtual KoMainWindow *createNewShell();
};

#endif


