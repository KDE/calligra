/***************************************************************************
                          propertyeditor.h  -  description
                             -------------------
    begin                : 08.12.2002
    copyright            : (C) 2002 by Alexander Dymo
    email                : cloudtemple@mksat.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <qdockwindow.h>

/**
  *@author Alexander Dymo
  Docked window that contains property editor - QTable with customized editors
  Unfinished
  */

class PropertyEditor : public QDockWindow  {
   Q_OBJECT
public: 
    PropertyEditor( Place p = InDock, QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
    ~PropertyEditor();
};

#endif
