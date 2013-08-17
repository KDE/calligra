/*
 *   Copyright (C) 1997  Michael Roth <mroth@wirlweb.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */


#include <QApplication>
#include <QLayout>
#include <QWidget>
#include <QBoxLayout>

#include "kseparator.h"


int main(int argc, char **argv)
{
   QApplication app(argc, argv);
   
   QWidget toplevel;
   QBoxLayout *mainbox = new QBoxLayout(QBoxLayout::TopToBottom,&toplevel);
   mainbox->setMargin(10);
   
   KSeparator *sep1 = new KSeparator( Qt::Vertical, &toplevel );
   mainbox->addWidget(sep1);
   
   KSeparator *sep2 = new KSeparator( Qt::Horizontal, &toplevel );
   mainbox->addWidget(sep2);
   mainbox->activate();
   
   toplevel.show();
   return app.exec();
}




