/***************************************************************************
                          clstbldesigner.h  -  description
                             -------------------
    begin                : Wed Apr 10 2002
    copyright            : (C) 2002 by Chris Machemer
    email                : machey@ceinetworks.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.       *
 *                                                                         *
 ***************************************************************************/

#ifndef CLSTBLDESIGNER_H
#define CLSTBLDESIGNER_H

#include <qwidget.h>
#include <kdatabase_tabledesigner.h>
#include "kdatabase_struct.h"

/**
  *@author root
  */

class clsTblDesigner : public clsTableDesigner  {
   Q_OBJECT
public: 
	clsTblDesigner(QWidget *parent=0, const char *name=0);
	~clsTblDesigner();
   bool populateTblDesigner(QString tblName);

};


#endif
