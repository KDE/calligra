/***************************************************************************
                          itemopts.h  -  description
                             -------------------
    begin                : 07.06.2002
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
#ifndef ITEMOPTS_H
#define ITEMOPTS_H

#include <map.h>
#include "dlgoptions.h"

class QString;
class QStringList;

class dlgItemOptions: public dlgOptions{
    Q_OBJECT
public:
    dlgItemOptions(map<QString, pair<QString, QStringList> > *p,
		   QWidget *parent = 0, const char *name = 0, WFlags f = 0);
private:
    map<QString, pair<QString, QStringList> > *props;
public slots:
    void commitProps();
};

#endif
