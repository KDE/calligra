/* This file is part of the KDE project
   Copyright (C) 2002 Igor Janssen <rm@linux.ru.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "koPainterTest.h"

#include <kapplication.h>
#include <kdebug.h>

#include <koPainter.h>

KoPainterTest::KoPainterTest(QWidget *widget, const char *name):
QWidget( widget, name )
{
  setFixedSize(500,300);
  p = new KoPainter(500, 300);
}

KoPainterTest::~KoPainterTest()
{
}

int main(int argc, char **argv)
{
  KApplication *a = new KApplication(argc, argv, "kopaintertest");
  KoPainterTest *t = new KoPainterTest();
  a->setMainWidget(t);
  t->show();
  return a->exec();
}

#include "koPainterTest.moc"
