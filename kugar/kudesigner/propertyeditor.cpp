/***************************************************************************
                          propertyeditor.cpp  -  description
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

#include <qdockwindow.h>
#include <qtable.h>
#include <qlayout.h>
 
#include "propertyeditor.h"

PropertyEditor::PropertyEditor( Place p, QWidget * parent, const char * name, WFlags f):
    QDockWindow(p, parent, name, f)
{
    /*Temporary widgets below will be replaced by the real Property Editor:
      Table with custom cell editors
    */

    setCloseMode(QDockWindow::Always);

    QWidget *container = new QWidget(this);

    QTable *table = new QTable(container);
    table->setNumRows(5);
    table->setNumCols(2);
    table->horizontalHeader()->setLabel(0, "Property");
    table->horizontalHeader()->setLabel(1, "Value");
    table->setLeftMargin(0);

    QGridLayout *gl = new QGridLayout(container);
    gl->addWidget(table, 0, 0);
    setWidget(container);
}


PropertyEditor::~PropertyEditor(){
}

#include "propertyeditor.moc"
