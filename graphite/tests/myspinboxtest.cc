/* This file is part of the KDE project
   Copyright (C) 1999 by Dirk A. Mueller <dmuell@gmx.net>
   Copyright (C) 2001 Werner Trobin <trobin@kde.org>

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

#include <qlayout.h>
#include <qvgroupbox.h>

#include <kapp.h>

#include <myspinbox.h>
#include <myspinboxtest.h>


TopLevel::TopLevel(QWidget *parent, const char *name) : QWidget(parent, name) {

    setCaption(QString::fromLatin1("MySpinBox test application"));

    QBoxLayout* l = new QHBoxLayout(this, 10);
    QGroupBox* b1 = new QVGroupBox(QString::fromLatin1("MySpinBox"), this);

    sbox = new MySpinBox(b1, "spinbox1");
    sbox->setValue(42.0);
    l->addWidget(b1);
}

int main(int argc, char ** argv) {

    KApplication *a = new KApplication ( argc, argv, "MySpinBoxTest" );
    TopLevel *toplevel = new TopLevel(0, "myspinboxtest");
    a->setMainWidget(toplevel);
    toplevel->show();
    a->exec();
}

#include <myspinboxtest.moc>
