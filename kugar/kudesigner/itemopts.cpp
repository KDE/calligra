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
#include <qlineedit.h>
#include "dlgoptions.h"
#include "itemopts.h"

//save the properties from table widget to a object
void dlgItemOptions::commitProps()
{
    for (int i = 0; i < taProps->numRows(); i++)
    {
        QString a = taProps->item(i, 0)->text();
        QString result = taProps->item(i, 1)->text();
        (*props)[a].first = result;
	
	// Alexander Dymo:
	// Can't do this - property values are not integers and doubles
	// there are strings and colors (like 255,255,255) - I have bug reports
	// conversion IMHO is not nesessary ???
/*        bool ok;
        result.toDouble( &ok );
        if ( ok )
            (*props)[a].first = result;
        else
        {
            result.toInt( &ok );
            if ( ok )
                (*props)[a].first = result;
            else
                (*props)[a].first = QString::number(0);
        }*/
    }
}

dlgItemOptions::dlgItemOptions(std::map<QString, std::pair<QString, QStringList> > *p,
			       QWidget *parent, const char *name, WFlags f):
	    dlgOptions(parent, name, f)
{
    taProps->setLeftMargin(0);
    taProps->setColumnReadOnly(0, TRUE);
    connect(taProps, SIGNAL(currentChanged(int, int)), this, SLOT(showPropertyTip(int, int)));

    //show properties in list view
    props = p;

    std::map<QString, std::pair<QString, QStringList> >::const_iterator i;
    int j = 0;
    for (i = props->begin(); i != props->end(); ++i)
    {
	if (i->first.isNull()) continue;
	taProps->insertRows(taProps->numRows());
	taProps->setText(j, 0, i->first);
	taProps->setText(j, 1, i->second.first);
/*	if (i->second.second[1] == "string")
	{
	    taProps->setText(j, 0, i->first);
	    taProps->setText(j, 1, i->second.first);
	}
	if (i->second.second[1] == "int_from_list")
	{
	    taProps->setText(j, 0, i->first);
	    QStringList sl;
	    for (QStringList::const_iterator it = i->second.second.begin(); it!=i->second.second.end(); ++it)
	    {
		sl << (*it);
	    }
	    taProps->setItem(j, 1, new QComboTableItem(taProps, sl));
	    taProps->setText(j, 0, i->first);
	    taProps->setText(j, 1, i->second.first);
	}
	if (i->second.second[1] == "int")
	{
	    taProps->setText(j, 0, i->first);
	    taProps->setText(j, 1, i->second.first);
	}
	if (i->second.second[1] == "font")
	{
	    taProps->setText(j, 0, i->first);
	    taProps->setText(j, 1, i->second.first);
	}
	if (i->second.second[1] == "color")
	{
	    taProps->setText(j, 0, i->first);
	    taProps->setText(j, 1, i->second.first);
	}*/
	j++;
    }

    //delete the last empty row if exists
    if (taProps->text(taProps->numRows()-1, 0) == "")
    	taProps->removeRow(taProps->numRows()-1);

    connect(buttonOk, SIGNAL(clicked()), this, SLOT(commitProps()));
}

void dlgItemOptions::showPropertyTip(int row, int col)
{
    LineEdit1->setText((*props)[taProps->text(row, 0)].second[0]);
}
#include "itemopts.moc"
