/***************************************************************************
                          clstbldesigner2.h  -  description
                             -------------------
    begin                : Mon Apr 29 2002
    copyright            : (C) 2002 by root
    email                : root@machlinux
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CLSTBLDESIGNER2_H
#define CLSTBLDESIGNER2_H

#include <kdialog.h>
#include <qtable.h>

/**
  *@author root
  */

class clsTblDesigner2 : public KDialog  {
         Q_OBJECT
public: 
	clsTblDesigner2();
	~clsTblDesigner2();
   bool populateTblDesigner(QString tblName);
private:
   QTable* myQTable;
};

#endif
