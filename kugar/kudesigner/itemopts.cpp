/***************************************************************************
                          itemopts.cpp  -  description
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
#include <qtable.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qpushbutton.h>
#include "dlgoptions.h"
#include "itemopts.h"

//save the properties from table widget to a object
void dlgItemOptions::commitProps()
{
    for (int i = 0; i < taProps->numRows(); i++)
    {
	QString a = taProps->item(i, 0)->text();
	(*props)[a].first = taProps->item(i, 1)->text();
    }
}

dlgItemOptions::dlgItemOptions(std::map<QString, std::pair<QString, QStringList> > *p,
			       QWidget *parent, const char *name, WFlags f):
	    dlgOptions(parent, name, f)
{
    taProps->setLeftMargin(0);

    //show properties in list view
    props = p;

    std::map<QString, std::pair<QString, QStringList> >::const_iterator i;
    int j;
    for (i = props->begin(), j = 0; i != props->end(); ++i, ++j)
    {
	taProps->insertRows(taProps->numRows());
	taProps->setText(j, 0, i->first);
	taProps->setText(j, 1, i->second.first);
    }

    connect(buttonOk, SIGNAL(clicked()), this, SLOT(commitProps()));
}
