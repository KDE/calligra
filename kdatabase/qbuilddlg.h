/***************************************************************************
                          qbuilddlg.h  -  description
                             -------------------
    begin                : Mon Apr 8 2002
    copyright            : (C) 2002 by Chris Machemer
    email                : machey@ceinetworks.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.        *
 *                                                                         *
 ***************************************************************************/

#ifndef QBUILDDLG_H
#define QBUILDDLG_H

#include <qwidget.h>
#include <kdatabase_qbuilder.h>

/**
  *@author root
  */

class QBuildDlg : public clsQBuilder  {
   Q_OBJECT
public: 
	QBuildDlg(QWidget *parent=0, const char *name=0);
	~QBuildDlg();
};

#endif
